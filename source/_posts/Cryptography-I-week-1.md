title: Cryptography解题报告：Two time pad
date: 2016-01-31 21:16:20
tags: [python, 密码学]
categories: [编程]
---
Coursera公开课《加密学》的一道课后习题，花了整整一下午写程序解出来了，参考了很多资料。
深刻体会One time pad必须一次性使用的重要性。
<!-- more -->
## 理解

题目链接：[博客](http://www.terence-yang.com/?p=169)
建议先观看视频，该题目是原型是该公开课视频 【2-3】 的内容：

> Two-time pad is insecure!
> Never use stream cipher key more than once!

有了解题思路，做出这道题，核心点就是理解hint中的提示：

> XOR the ciphertexts together, and consider what happens when a space is XORed with a character in [a-z A-Z].

实际上字母表与空格进行xor后，实现了大小写转换。依据这点可以猜出很多Plaintext的内容。
基本原理就是：

> (m1 xor k) xor (m2 xor m2) = m1 xor m2
> A xor k xor k =A


## 源码
我的程序思路就是：让第十一个字符串（下文称为target_string）的每一个字符与上面10个字符串进行异或，若异或遇到空格，就可以得出target_string每一位的内容。但是得出的是大小写颠倒的，需要转换一次

    #!/usr/bin/env python
    # -*- coding: utf-8 -*-
    # Time-stamp: < 1.py 2016年01月31日 21:11:58 >
    str_many = ['32510ba9a7b2bba9b8005d43a304b5714cc0bb0c8a34884dd91304b8ad40b62b07df44ba6e9d8a2368e51d04e0e7b207b70b9b8261112bacb6c866a232dfe257527dc29398f5f3251a0d47e503c66e935de81230b59b7afb5f41afa8d661cb', '234c02ecbbfbafa3ed18510abd11fa724fcda2018a1a8342cf064bbde548b12b07df44ba7191d9606ef4081ffde5ad46a5069d9f7f543bedb9c861bf29c7e205132eda9382b0bc2c5c4b45f919cf3a9f1cb74151f6d551f4480c82b2cb24cc5b028aa76eb7b4ab24171ab3cdadb8356f', '32510ba9a7b2bba9b8005d43a304b5714cc0bb0c8a34884dd91304b8ad40b62b07df44ba6e9d8a2368e51d04e0e7b207b70b9b8261112bacb6c866a232dfe257527dc29398f5f3251a0d47e503c66e935de81230b59b7afb5f41afa8d661cb', '32510ba9aab2a8a4fd06414fb517b5605cc0aa0dc91a8908c2064ba8ad5ea06a029056f47a8ad3306ef5021eafe1ac01a81197847a5c68a1b78769a37bc8f4575432c198ccb4ef63590256e305cd3a9544ee4160ead45aef520489e7da7d835402bca670bda8eb775200b8dabbba246b130f040d8ec6447e2c767f3d30ed81ea2e4c1404e1315a1010e7229be6636aaa', '3f561ba9adb4b6ebec54424ba317b564418fac0dd35f8c08d31a1fe9e24fe56808c213f17c81d9607cee021dafe1e001b21ade877a5e68bea88d61b93ac5ee0d562e8e9582f5ef375f0a4ae20ed86e935de81230b59b73fb4302cd95d770c65b40aaa065f2a5e33a5a0bb5dcaba43722130f042f8ec85b7c2070', '32510bfbacfbb9befd54415da243e1695ecabd58c519cd4bd2061bbde24eb76a19d84aba34d8de287be84d07e7e9a30ee714979c7e1123a8bd9822a33ecaf512472e8e8f8db3f9635c1949e640c621854eba0d79eccf52ff111284b4cc61d11902aebc66f2b2e436434eacc0aba938220b084800c2ca4e693522643573b2c4ce35050b0cf774201f0fe52ac9f26d71b6cf61a711cc229f77ace7aa88a2f19983122b11be87a59c355d25f8e4', '32510bfbacfbb9befd54415da243e1695ecabd58c519cd4bd90f1fa6ea5ba47b01c909ba7696cf606ef40c04afe1ac0aa8148dd066592ded9f8774b529c7ea125d298e8883f5e9305f4b44f915cb2bd05af51373fd9b4af511039fa2d96f83414aaaf261bda2e97b170fb5cce2a53e675c154c0d9681596934777e2275b381ce2e40582afe67650b13e72287ff2270abcf73bb028932836fbdecfecee0a3b894473c1bbeb6b4913a536ce4f9b13f1efff71ea313c8661dd9a4ce', '315c4eeaa8b5f8bffd11155ea506b56041c6a00c8a08854dd21a4bbde54ce56801d943ba708b8a3574f40c00fff9e00fa1439fd0654327a3bfc860b92f89ee04132ecb9298f5fd2d5e4b45e40ecc3b9d59e9417df7c95bba410e9aa2ca24c5474da2f276baa3ac325918b2daada43d6712150441c2e04f6565517f317da9d3', '271946f9bbb2aeadec111841a81abc300ecaa01bd8069d5cc91005e9fe4aad6e04d513e96d99de2569bc5e50eeeca709b50a8a987f4264edb6896fb537d0a716132ddc938fb0f836480e06ed0fcd6e9759f40462f9cf57f4564186a2c1778f1543efa270bda5e933421cbe88a4a52222190f471e9bd15f652b653b7071aec59a2705081ffe72651d08f822c9ed6d76e48b63ab15d0208573a7eef027', '466d06ece998b7a2fb1d464fed2ced7641ddaa3cc31c9941cf110abbf409ed39598005b3399ccfafb61d0315fca0a314be138a9f32503bedac8067f03adbf3575c3b8edc9ba7f537530541ab0f9f3cd04ff50d66f1d559ba520e89a2cb2a83']
    str_target = '32510ba9babebbbefd001547a810e67149caee11d945cd7fc81a05e9f85aac650e9052ba6a8cd8257bf14d13e6f0a803b54fde9e77472dbff89d71b57bddef121336cb85ccb8f3315f4b52e301d16e9f52f904'

    def strxor(a, b):    # xor two strings of different lengths
        if len(a) > len(b):
            return "".join([chr(ord(x) ^ ord(y)) for (x, y) in zip(a[:len(b)], b)])
        else:
            return "".join([chr(ord(x) ^ ord(y)) for (x, y) in zip(a, b[:len(a)])])

    def show_msgs(str_target1):
        msg = 'The secret message is: When using a stream cipher, never use the key more than once'
        xor_key = strxor(str_target1, msg)
        for i in str_decoded:
            print strxor(i, xor_key)
        print msg

    if __name__ == '__main__':
        str_decoded = []
        # 编码为hex
        for i in str_many:
            str_decoded.append(i.decode('hex'))
        str_target = str_target.decode('hex')
        # 取得长度
        l = len(str_decoded)
        target_len = len(str_target)
        result = []
        # 对target每一个字符处理
        for i in range(target_len):
            temp = []    # target的每一位存放一个list，对应可能的字符
            for j in range(l):
                len_of_another_str = len(str_decoded[j])
                if i >= len_of_another_str:continue    # 长度超长，跳到下个长串
                a_char = (chr(ord(str_decoded[j][i:i + 1]) ^ ord(str_target[i:i + 1])))
                if a_char >= 'A' and a_char <= 'z':
                    new_char = chr(ord(a_char) ^ 0x20)    # 跟空格xor一次，还原真正的大小写
                    if new_char not in temp:
                        temp.append(new_char)
            if temp == []:
                temp.append('*')    # 占位符，表示任意字符
            result.append(temp)
        # 打印可能的字符，标点符号注意意译处理
        for i in result:
            print i

        is_show = raw_input('Do you want to see the full msgs?(y/n):')
        if is_show == 'y':
            show_msgs(str_target)

## 运行结果

把输出结果连起来读一遍，大致意思就是课程的题目主旨：

> The secret message is: When using a stream cipher, never use the key more than once

注意标点符号没有进行处理，实际还需要英文意译一下添上标点。

    ['T']
    ['h']
    ['e', 'm']
    ['e', 'r', 'c', 'p']
    ['s']
    ['e']
    ['c']
    ['u']
    ['e']
    ['t']
    ['h', 'd', 't', 'w', 's']
    ['m']
    ['e']
    ['s']
    ['s', 'z']
    ['a']
    ['g']
    ['e']
    ['u', 'l', 'd', 'b', 's', 'n']
    ['i']
    ['s']
    ['Q', '\x7f', '|', 'm', 'c', 'y']
    ['e', 'n', 'd', 'a', 'h', 'p', 't']
    ['W']
    ['*']
    ['*']
    ['n']
    ['q', 't', 'a', 'o', 'r']
    ['u']
    ['s']
    ['i', 'a']
    ['n', '|']
    ['w']
    ['o', 'r', 'h', 'y', 'i', 'e']
    ['a', '{', 'w']
    ['n', 'k', 's']
    ['~', 's']
    ['t']
    ['r']
    ['e']
    ['*']
    ['m']
    ['p', 'e', 'o', 'a', 'n']
    ['c']
    ['i']
    ['p']
    ['h']
    ['e']
    ['r']
    ['d', 'i', '~', 'u', '{', 'e', '|']
    ['e', 'c', 'i', 's', 'a', 't']
    ['n']
    ['e']
    ['v']
    ['e', 'i']
    ['r']
    ['n', 'a', 'o', 'p', 'e', 'G', 'g', 't']
    ['u']
    ['s']
    ['e']
    ['i', 'r', 'a', 'e', 't', 'l']
    ['t']
    ['h']
    ['e']
    ['a', 'g', 'e', 't', 'n', 'o']
    ['k']
    ['e']
    ['y']
    ['t', 'n', 'a', 'o', 'c', 'w']
    ['m']
    ['o']
    ['r']
    ['e']
    ['f', 'i', 'a', 'r', 'e', 'n']
    ['t']
    ['h']
    ['a']
    ['n']
    ['t', 'o', 'e', 'u', 'r']
    ['o']
    ['n']
    ['n', 'c']
    ['e']

## 参考资料

[密码学many time pad](http://www.terence-yang.com/?p=169)
[Weiran Liu的渣技术小专栏](http://blog.csdn.net/liuweiran900217/article/details/19933549)
[python hex to dec](https://www.daniweb.com/programming/software-development/code/216638/hexadecimal-to-decimal-python)
[ASCII code table](http://www.ascii-code.com/)