title: Cryptography解题报告：Compute Discrete Log
date: 2016-03-17 15:57:32
tags: [python, 密码学]
categories: [编程]
---
第一次看这个题目时候，感觉挺简单的，但是考虑使用python或者Java会很慢，决定用C++解题，没想到跳进了大坑！哎，c++这样的底层的操作语言，实现高层脚本语言（python）功能的确是非常麻烦——不断造轮子，轮子破破烂烂的，可是C++性能确实摆在那里，老司机开车比人家快嘛！
<!-- more -->
题目就是让你在2^20复杂度下计算出一个大素数p的Zp*集合中一个元素x：

	h = g^x in p
    
视频[10-5 Intractable problems](https://www.youtube.com/watch?v=EeV_gd8dSQI)定位到9:45,教授说道，计算DLOG比较困难，但是对某些数字位数比较小的key，可以两端夹击（Meet in The Middle）破解。本次作业就是模拟这个过程。思路很简单，就是用c++实现太麻烦了！看看人家Java写的代码不超过80行。python更是50行代码就搞定了。。。

这个公式就是算法核心：
![](/images/crypro_coursera_I/week5.png)

## 思路

教授给出的思路如下：
1. 令B=2^20
2. for x1 in [0,B]: 计算出左侧公式的值记为a
3. 将每一个hash(a)的值存入哈希表
4. for x0 in [0,B]: 计算出右侧公式的值记为b，计算hash(b)看看是否在哈希表中
5. 输出x0 x1，计算最后的结果 x = x0*B + x1

## 遇到的坑

1. python语言中的encode(hex)和decode(hex)方法实现过程中，对unsigned char和char数据类型不了解，导致将u8的哈希值转成s8字符串出错。还有在函数参数进行强制类型转换时，注意u8可以转成s8类型，反过来不可以。编译器提示丢失精度指的是丢失正负号。
2. 使用第三方的库还是第一次，在编译链接时候耗费太多时间调试Makefile，教训：先一句一句地进行终端的G++编译通过了，再写进Makefile，最后才是对Makefile进行优化：什么隐式推倒自动化变量之类的。
3. 使用libsodium遇到SHA-256对两个msg（msg1!=msg2)生成两个摘要时，发现总是hash(msg1)=hash(msg2)，最后折腾一番才发现是MSG_LEN没有指定，正确做法：strlen(msg)
4. 第三条中，strlen(msg)又是一个坑，当msg为unsigned char数组时，无法正确获得长度，所以传入str_to_hex参数中我将strlen(hash_raw_result)替换成常量crypto_hash_sha256_BYTES，成功将unsigned char 转成hex编码的字符串。
5. 又是那个可恶的unsigned char：不能使用strlen就够了，连strcmp和strcpy都不能用！最后只能使用memcpy和memcmp进行比较字符串大小和赋值字符串。
6. 因为好久没有接触c++了（至少一年半吧），对new动态分配内存不了解，以后记得对于无法预测数据统统使用new来动态分配内存，这次算是学到不少内存管理的经验。
7. set容器要指定一个排序的方法，因为使用的红黑树，插入即完成排序。还有在排序时候不能使用等号（>= 和 <=），否则有元素重复！
8. 进行内存回收，防止内存泄露，这是最基本的，记得free或者delete。

自己按照要求实现了一下，第一次是失败了，输出了错误的x1和x0，后来参考了一下[他的java代码](http://groglogs.blogspot.com/2013/12/java-compute-discrete-logarithm.html)，通过不停打印和对比中间结果，我发现自己思维上走了歪路。最后重构耗时一天半结果出来了。运行时间还能接受，我C++跑了24秒，而他的java代码跑了两分钟。。。可以看出两种语言的性能差异。

## 源码
使用[libsodium](https://github.com/jedisct1/libsodium)和[libgmp](https://gmplib.org/)这两个库，前者是大多数梯子的crypto库首选，后者是流行的GNU高精度运算库。
C++源码：

    #include <iostream>
    #include <gmp.h>
    #include <cstring>
    #include <time.h>
    #include <sodium.h>
    #include <set>

    using namespace std;
    static unsigned char ByteMap[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8','9', 'a', 'b', 'c', 'd', 'e', 'f' };
    static char p_str[] =
            "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084171";
    static char g_str[] =
            "11717829880366207009516117596335367088558084999998952205599979459063929499736583746670572176471460312928594829675428279466566527115212748467589894601965568";
    static char h_str[] =
            "3239475104050450443565264378728065788649097520952449527834792452971981976143292558073856937958553180532878928001494706097394108577585732452307673444020333";

    static unsigned long int pow_2_over_20 = 1048576; // 常量2^20
    void str_encode(unsigned char *src, unsigned char *dest, int len_of_src) {
        // python的str.encode('hex')方法实现
        // 使用注意：dest_len >= 2*len_src +1，最后一位是存放'\0'。
        int t1;
        for (int i = 0; i < len_of_src; ++i) {
            t1 = (int) src[i];
            dest[2 * i] = ByteMap[t1 / 16];
            dest[2 * i + 1] = ByteMap[t1 % 16];
        }
        dest[2 * len_of_src] = 0; //必须填充最后一个为'\0'
    }

    // 含hash 和 序号x1 的结构体，插入到set容器
    struct my_pair {
        unsigned char hash[65];
        unsigned long int number;
    };

    // “仿函数"。重载运算符'<'指定排序规则
    class my_pair_sort {
    public:
        bool operator()(const my_pair &a, const my_pair &b) const {
            /*unsigned类型数组不能使用strcmp函数*/
            if (memcmp(a.hash, b.hash, sizeof(a.hash)) < 0) // 刚开始写了"<="，后果是插进去的元素有重复
                return true;
            else return false;
        }
    };

    void show_the_answer(unsigned long int x0, unsigned long int x1,const mpz_t p) {
        mpz_t x0_, x1_;
        char result[190];
        mpz_init(x0_);
        mpz_init(x1_);
        mpz_set_ui(x0_, x0);
        mpz_mul_ui(x1_, x0_, pow_2_over_20); // 求x0*(2^20),存入x1_
        mpz_add_ui(x0_, x1_, x1);		 // 求x1+x0*(2^20)，存入x0_
        mpz_mod(x1_, x0_, p);		 // 求x1+x0*(2^20)对P的模，存入x1_
        mpz_get_str(result, 10, x1_);	 // 转成字符串
        cout << result << endl;
        mpz_clear(x0_);
        mpz_clear(x1_);
    }

    int main() {
        time_t start_time, end_time;
        time(&start_time);			// 计时开始
        if (sodium_init() == -1) return 1; // 加载crypto库
        unsigned char hash_raw_out[crypto_hash_sha256_BYTES], hash_encoded[40]; // 存放哈希有关的数据
        char str_a[220], str_b[220];										// 临时字符串
        mpz_t mpz_t_g, mpz_t_h, mpz_t_p, mpz_t_temp, mpz_t_g_pow_B, mpz_t_inv;// 大数变量
        mpz_init_set_str(mpz_t_g, g_str, 10);
        mpz_init_set_str(mpz_t_p, p_str, 10);
        mpz_init_set_str(mpz_t_h, h_str, 10);
        mpz_init(mpz_t_temp);
        mpz_init(mpz_t_inv);

        set<my_pair, my_pair_sort> my_pair_set; // 红黑树set存放所有哈希结果

        for (unsigned long int x1 = 0; x1 <= pow_2_over_20; x1++) {
            mpz_powm_ui(mpz_t_temp, mpz_t_g, x1, mpz_t_p); // 求((g^x1) mod p)存入mpz_t_temp
            mpz_invert(mpz_t_inv, mpz_t_temp, mpz_t_p);	// 求inverse(g^x1)存入mpt_t_inv
            mpz_mul(mpz_t_temp, mpz_t_inv, mpz_t_h);// 求(h * inv(g^x1))存入mpz_t_temp
            mpz_mod(mpz_t_inv, mpz_t_temp, mpz_t_p);		  // 求模p，存入mpt_t_inv

            mpz_get_str(str_b, 10, mpz_t_inv); // 结果送字符串b
            crypto_hash_sha256(hash_raw_out, (unsigned char*) str_b, strlen(str_b)); // hash
            str_encode(hash_raw_out, hash_encoded, crypto_hash_sha256_BYTES); // 因为hash_raw是unsigned表示数字，而不是字符，所以hash_raw最后1byte不是'\0'，不能使用strlen函数求出hash_raw_out长度

            my_pair *good_pair = new (my_pair); // 动态内存分配，如果使用静态内存分配很蛋疼。
            memcpy(good_pair->hash, hash_encoded, 2 * crypto_hash_sha256_BYTES + 1); // 不能使用strcpy，因为是unsigned数据类型，遇到负值很变态。
            good_pair->number = x1;
            my_pair_set.insert(*good_pair); // 插入到set，因为规定了排序规则，插进去就是完成排序了
            delete good_pair;
        }
        cout << "build x1 hash table done.\n";

        mpz_init(mpz_t_g_pow_B);
        mpz_powm_ui(mpz_t_g_pow_B, mpz_t_g, pow_2_over_20, mpz_t_p); // 计算出g^B这个常量

        my_pair *hash_find_target = new (my_pair);
        set<my_pair, my_pair_sort>::iterator iter;

        bool is_found = false;
        unsigned long int x0_good = 0, x1_good = 0;
        for (unsigned long int x0 = 0; x0 <= pow_2_over_20; x0++) { // 开始搜索 x0
            mpz_powm_ui(mpz_t_temp, mpz_t_g_pow_B, x0, mpz_t_p); // 求出(pow(g^B,x0) mod p)存入mpz_t_temp
            mpz_get_str(str_a, 10, mpz_t_temp); // pow(g^B,x0)存入字符串a
            crypto_hash_sha256(hash_raw_out, (unsigned char*) str_a, strlen(str_a)); // 计算hash
            str_encode(hash_raw_out, hash_encoded, crypto_hash_sha256_BYTES);

            memcpy(hash_find_target->hash, hash_encoded,
                    2 * crypto_hash_sha256_BYTES + 1);
            iter = my_pair_set.find(*hash_find_target);		// 搜索
            if (iter != my_pair_set.end()) {
                x0_good = x0;
                x1_good = (*iter).number;
                is_found = true;
                break;
            }
        }
        cout << "search table done.\n";

        mpz_clear(mpz_t_g_pow_B);
        mpz_clear(mpz_t_inv);
        mpz_clear(mpz_t_g);
        mpz_clear(mpz_t_h);
        mpz_clear(mpz_t_temp);
        my_pair_set.clear();

        if (is_found) {
            cout << "bingo!\n";
            cout << "x1= " << x1_good << endl;
            cout << "x0= " << x0_good << endl;
            show_the_answer(x0_good, x1_good, mpz_t_p);
        } else
            cout << "Not found.\n";

        mpz_clear(mpz_t_p);
        time(&end_time);
        double dif = difftime(end_time, start_time);
        cout << "Cost time: " << dif << " seconds" << endl;
        return 0;
    }
    
还有这个配套的Makefile：

    CC=g++
    # link static lib: gmp
    LIBS=-lgmp -lsodium
    SRC=main.cpp
    OBJS=main.o 
    all:$(OBJS)
        $(CC) $^ $(LIBS) -o main
    $(OBJS):$(SRC)
        $(CC) -c $(LIBS) $^ -o $@
    .PHONY:clean
    clean:
        rm *.o
        rm main

## 运行结果

    li@ubuntu:~/tmp$ make && ./main
    build x1 hash table done.
    search table done.
    bingo!
    x1 = 787046
    x0 = 357984
    375374217830
    Cost time: 25 seconds
    
可见答案是375374217830