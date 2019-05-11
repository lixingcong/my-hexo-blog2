title: Cryptography解题报告：Padding Oracle Attack
date: 2016-03-13 20:46:07
tags: [python, 密码学]
categories: [编程]
---
这次的编程题目[(题目链接)](http://blog.csdn.net/csh1989/article/details/38457377)难度系数较高，感觉到智商碾压啊，题目要求对Dan Boneh搭建的服务器进行攻击，破译出CBC加密明文，最坑爹的地方就是他搭建在GAE上，国内访问绝对被墙。只好用手上的VPS运行python进行实验。做完题目感觉很爽！
<!-- more -->
解题思路就是视频[7-6 CBC padding attacks.mp4](https://www.youtube.com/watch?v=evrgQkULQ5U)中定位到(6:00)的讲解，一开始很不理解，我就照自己的思路写了一个程序，然后万般无奈调试得出的答案是错误的无意义的printable string，最后参考了很多代码才写出来的。囧rz

基本原理就是“选择密文攻击”，假设Attacker手上有了两个block尺寸的密文c[0]和c[1]，他可以篡改c[0]的末尾几个字节的值达到强行修改明文m[1]的末尾字节，这个是由于CBC工作原理决定的。

![](/images/crypro_coursera_I/cbc_oracle_attack_1.png)
![](/images/crypro_coursera_I/cbc_oracle_attack_2.png)

## 选择密文攻击

假设Attacker要解密出m[1]最后一个字节的数据，他只需要猜0～255，假设他猜的值为guess

1. 选择guess值，将c[0]最后一个字节的与guess异或。

		guess = new_guess in 0...255
		data' = xor(c[0], guess)
        
2. 进行padding，因为只需要最后一个字节，根据padding规则将最后一个字节异或0x01

		data_with_pad' = xor(data', 0x01)

	分析：
    - 若guess==data，则在step 1 得到的结果是0x00，进行step 2 则会将m[1]最后一个字节的数据变成0x01，这样服务器会成功解密，由于padding引起明文发生变化，传入网站参数不存在，返回404错误。
    - 若guess!=data，则padding不合法，服务端解密失败，返回403拒绝Attacker的连接。
    - 若很偶然的猜对，有一定概率返回200代码，需要特殊处理。

3. 将修改后的data_with_pad与c[1]发给服务器。观察返回的错误代码，若是404错误，可以记下来该值，就是m[1]的最后一个字节的值。
4. 将guess值另存为新的变量，例如bingo,嵌入到c[0]（使用异或的方式），留给下一轮（破解第二字节）使用。

		c[0]' = xor(c[0], [00...00] || bingo)
    
5. 继续破解第二个字节：选择新的guess in (0~255)，与data异或

        guess = new_guess in 0...255
        guess = [00...00] || guess || [00]  # 将guess移位到倒数第二个字节处
        data' = xor(c[0]', guess)
    
6. 进行padding，注意，因为是想解出第二个字节，故最后两个字节都要进行padding操作

        data_with_pad' = xor(data', 0x0202)
    
7. 同样data_with_pad和c[1]发送给服务器观察返回错误代码，同样步骤记下正确的bingo值。

		c[0]'' = xor(c[0]', [00...00] || bingo || [00] )
        
8. 继续选择第三字节穷举，然后padding 0x030303，得到正确值记下来供下一轮。。如此循环下去直到第16字节解出来。
		
        guess = new_guess in 0...255
        guess = [00...00] || guess || [0000]  # 将guess移位到倒数第三个字节处
        data'' = xor(c[0]'', guess)
		data_with_pad'' = xor(data', 0x030303)
		c[0]''' = xor(c[0]', [00...00] || bingo || [0000] )

其他要点：

> 1、解出每一个字节都要存放下来，否则影响后面字节的padding。造成“只能破解一个字节”而无法进行下去。
> 2、异或操作需要一步一步进行，便于查错。
> 3、对最后一个block进行猜时候，留意【padding_number=0x01和guess=0x01同时满足】的情况：xor(pad, guess)=0x00，则穷举不会奏效，没有达到强行修改m[1]末尾的效果。这是巨坑，我纠结了好久，写出一个检查padding有效性的检测方法，大家可以看看
> 4、遇到200代码情况要注意了，也许是遇到了padding_num=guess的情况，导致解密成功
> 5、实际上，每次查询不需要全部发送ciphertext过去，只需要发送c[0]和[1]，即：只需要2个block即可解出一个block
> 6、什么encode和decode用法需要铭记于心，贯穿整个课程。还有注意代码的可读性，使用模块化设计，方便调试和阅读。


## 源代码

参考了[@chavaone](https://github.com/chavaone/coursera-crypto-scripts/blob/master/Problems4/oracle.py)的代码（他的代码中，解第三个block的方法很牵强，没有说服力，居然是靠猜的！），我增加了几个功能，耗时一天，终于完成我的代码了：庆祝！

    #!/usr/bin/env python
    # -*- coding: utf-8 -*-
    import requests
    host_crypto = 'http://crypto-class.appspot.com/po?er='
    ct = 'f20bdba6ff29eed7b046d1df9fb7000058b1ffb4210a580f748b4ac714c001bd4a61044426fb515dad3f21f18aa577c0bdf302936266926ff37dbf7035d5eeb4'
    printable_chars = range(32, 128)
    padding_chars = range(1, 17)

    def query(q):
        url = host_crypto + q
        req = requests.get(url)
        if req.status_code == 404 :
            return True   # good padding
        return False   # bad padding

    def int2hex(i):
        '''整数转成十六进制，返回字符串形式，xx，这种方法要记下来放到日志'''
        return hex(i)[2:] if len(hex(i)[2:]) == 2 else '0' + hex(i)[2:]

    def exor_pad(i):
        '''输入一个1~16整数，返回一个长度16Byte，前导0，后面用i进行padding的字符串'''
        assert(i > 0)
        assert(i <= 16)
        return  '00' * (16 - i) + int2hex(i) * i

    def exor_g(g, pos):
        '''输入guess值，还有需要异或的位置(0~15)，返回一个长度16的前导0、后缀0、中间某一Byte为guess值的字符串'''
        assert(pos >= 0)
        assert(pos < 16)
        return '00' * (15 - pos) + int2hex(g) + '00' * pos

    def refill_zero(s):
        '''填充前导0，返回长度32的字符串'''
        return '0' * (32 - len(s)) + s

    def strxor(a, b):
        '''xor two strings of different lengths'''
        if len(a) > len(b):
            return "".join([chr(ord(x) ^ ord(y)) for (x, y) in zip(a[:len(b)], b)])
        else:
            return "".join([chr(ord(x) ^ ord(y)) for (x, y) in zip(a, b[:len(a)])])

    def hexexor(s1, s2):
        '''输入的是两个字符串，输出他们异或后的字符串'''
        # 先decode，将字符串转成Byte数据类型，再异或，异或结果后重新编码为字符串
        return strxor(s1.decode("hex"), s2.decode("hex")).encode("hex")

    def test_a_byte(found_msg, pos, dictinary_, iv, ct, is_padding = False):
        # 功能：穷举一个字节，破解得到明文
        # 输入found_msg为之前已经找到的字符串
        # pos为需要穷举的字节位置：从后开始计数1~16
        # dicitionay为穷举字典：int型范围0~255
        # iv为需要进行异或的字符串，ct为待解密的密文
        # is_padding是检查密文的Padding有效性时候用到，默认false
        pad = exor_pad(pos)
        lastmsg = refill_zero(found_msg.encode("hex"))
        getletter = False
        possible_padding = []
        # 字典破解，我有两个字典：ascii字符和padding集合
        for guess in dictinary_:
            gpad = exor_g(guess, pos - 1)
            # 是把猜想值和lastmsg做异或运算，能否破解成功依赖Lastmsg的正确性。
            if query(hexexor(lastmsg, hexexor(iv, hexexor(gpad, pad))) + ct):
                getletter = True
                new_msg = int2hex(guess).decode("hex") + found_msg
                if is_padding:
                    possible_padding.append(guess)
                else:
                    return new_msg
        if is_padding:
            return possible_padding
        if getletter == False:
            return None

    if __name__ == '__main__':
        blocks = ()   # 含四个元素，每个元素是长度32的字符串，使用tuple的目的是“不可变”的特性
        while ct:
            blocks = blocks + (ct[:32],)
            ct = ct[32:]

        b = input("input block number to crack:\n#(1~3)")
        iv = blocks[b - 1]   # 截取待破解的前一个block作为IV，其他block都可以丢弃了
        block = blocks[b]

        # 测试最后字节的Padding是否有效
        is_last_block = False
        if b == 3:
            is_last_block = True
        if is_last_block:
            possible_paddings = test_a_byte('', 1, padding_chars, iv, block, True)
            # 测试经过第一轮筛选处理的padding是否有效
            for i in possible_paddings:
                print "possible padding size is:", i
                msg = chr(i) * i
                start_byte = i + 1
                if test_a_byte(msg, start_byte, printable_chars, iv, block) != None:
                    print "good padding size is:", i
                    break
        else:
            msg = ''
            start_byte = 1

        # 对选定的block进行16字节的逐个字节破解
        for pos in range(start_byte, 17):
            is_found = test_a_byte(msg, pos, printable_chars, iv, block)
            if is_found:
                msg = is_found
                print "%r" % msg
            else:
                print "can't found the last #%d byte" % pos
                exit(0)
        if is_last_block and msg:
            print "After cutting padding off, the last block is:\n%r" % msg[:-(start_byte - 1)]

答案

	The Magic Words are Squeamish Ossifrage
    
## 参考文章

[CBC-Padding-Oracle-Attacks Standford Coursera](http://seffyvon.github.io/cryptography/2014/08/20/CBC-Padding-Oracle-Attacks)
[MOOC总结密码学assignment](http://mooc.guokr.com/note/9413/)