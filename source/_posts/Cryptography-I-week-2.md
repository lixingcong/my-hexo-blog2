title: Cryptography解题报告：Use Block Ciphers
date: 2016-03-06 21:04:55
tags: [python, 密码学]
categories: [编程]
---
这道题目很简单，目的就是让同学们熟悉一下块加密的API。

题目中，教授Dan Boneh只给出两种解密方式，其实完成作业后，自己尝试更多加密、解密方式，也是不错的。你看shadowsocks作者都能玩遍几乎所有加密方式，每次发送数据都更新IV，非常有意思。若GFW尝试破解某条TCP流量，真是闲的蛋疼。
<!-- more -->

自己写了2份代码，给大家参考。在文章最后，给出解题答案

心得：
- 在CBC模式才需要使用padding填充末尾数据，Dan Boneh教授已经在视频中的习题说的很清楚了。
- 数据的互相转换很是头疼：hex->dec和encode->decode一大堆我花了不少时间学习。
- 在初始化CTR模式的Counter时候，网上出现的版本有lambda作为参数的，我试了一下还是不行，于是只能乖乖的使用库自带的Counter类实现IV自增。

### AES-128-CBC 

    #!/usr/bin/env python
    # -*- coding: utf-8 -*-

    from Crypto.Cipher import AES
    from Crypto import Random

    PT='你好，使用pycrypto库，可以调用各种加密算法'
    CT=''
    key_to_aes = Random.new().read(AES.block_size)
    iv_to_aes = Random.new().read(AES.block_size)

    def AES_enc(key,msg,iv):
        print "\n加密开始"
        cipher = AES.new(key, AES.MODE_CBC, iv)
        # 检查是否是16字节的整数倍
        x = len(msg) % 16
        print 'plaintext长度是： ', len(msg)
        print 'The num to padded is : ', 16-x
        # 不是整数倍 进行padding操作
        msg_pad = msg + chr(16-x) * (16 - x) # shoud be 16-x not 
        print 'After padding, the plaintext is:\n%r'%msg_pad
        # 返回的是IV+CT
        pt = iv + cipher.encrypt(msg_pad)
        return pt

    def AES_dec(key,ct):
        print "\n解密开始"
        iv_1=ct[:16]
        decipher= AES.new(key_to_aes, AES.MODE_CBC,iv_1)
        temp=decipher.decrypt(CT[16:])
        pad_len=(temp[-1:]) # 获得pad长
        pad_len_int=ord(pad_len) # 转成int，不能直接调用Int()函数
        return temp[:-pad_len_int] # 截取padding，获得明文

    if __name__ == '__main__':
        # 加密
        print "plaintext is:\n%s"%PT
        CT = AES_enc(key_to_aes,PT,iv_to_aes)
        # 解密
        print AES_dec(key_to_aes,CT)


### AES-128-CTR

    #!/usr/bin/env python
    # -*- coding: utf-8 -*-

    from Crypto.Cipher import AES
    from Crypto.Util import Counter
    from Crypto import Random

    PT='今天我过得很愉快。'
    CT=''
    key_to_aes = Random.new().read(AES.block_size)
    iv_to_aes = Random.new().read(AES.block_size)

    def AES_enc(key,msg,iv):
        print "\n加密开始"
        print "提示：CTR模式不需要padding。"
        # 注意counter初始值的要先【字节码to字符串】，再转成整数。
        IV=Counter.new(128,initial_value=int(iv.encode('hex'),16))
        cipher = AES.new(key, AES.MODE_CTR,counter= IV)
        return iv+cipher.encrypt(msg)

    def AES_dec(key,ct):
        print "\n解密开始"
        iv=ct[:16]
        ct_=ct[16:]
        IV=Counter.new(128,initial_value=int(iv.encode('hex'),16))
        decipher = AES.new(key_to_aes, AES.MODE_CTR, counter=IV)
        return decipher.decrypt(ct_)

    if __name__ == '__main__':
        print "plaintext is:\n%s"%PT
        CT=AES_enc(key_to_aes,PT,iv_to_aes)
        print AES_dec(key_to_aes,CT)

### Cryptography答案

    #!/usr/bin/env python
    # -*- coding: utf-8 -*-
    # Time-stamp: < 4.py 2016年03月06日 21:03:30 >
    """
    先安装库pycrypto，在github下载源码编译安装即可
    """ 
    from Crypto.Cipher import AES
    from Crypto.Util import Counter

    CT_cbc=['4ca00ff4c898d61e1edbf1800618fb2828a226d160dad07883d04e008a7897ee2e4b7465d5290d0c0e6c6822236e1daafb94ffe0c5da05d9476be028ad7c1d81','5b68629feb8606f9a6667670b75b38a5b4832d0f26e1ab7da33249de7d4afc48e713ac646ace36e872ad5fb8a512428a6e21364b0c374df45503473c5242a253']

    CT_ctr=['69dda8455c7dd4254bf353b773304eec0ec7702330098ce7f7520d1cbbb20fc388d1b0adb5054dbd7370849dbf0b88d393f252e764f1f5f7ad97ef79d59ce29f5f51eeca32eabedd9afa9329','770b80259ec33beb2561358a9f2dc617e46218c0a53cbeca695ae45faa8952aa0e311bde9d4e01726d3184c34451']

    key_cbc = '140b41b22a29beb4061bda66b6747e14'.decode('hex')
    key_ctr = '36f18357be4dbd77f050515c73fcf9f2'.decode('hex') # Same Key

    for CT in CT_cbc:
        print '- '*10
        ct=CT.decode('hex')
        IV=ct[:16]
        decipher= AES.new(key_cbc, AES.MODE_CBC,IV)
        temp=decipher.decrypt(ct[16:])
        pad_len=(temp[-1:]) # 获得pad长度
        pad_len_int=ord(pad_len) # 单个字符转成int，不需要调用int()函数
        print temp[:-pad_len_int] # 截取末尾的padding，获得明文

    for CT in CT_ctr:
        print '- '*10
        ct=CT[32:].decode('hex')
        IV=Counter.new(128,initial_value=int(CT[:32],16)) #注意上面的CT尚未进行decode to hex，因此截取32个作为初始值
        decrypto = AES.new(key_ctr, AES.MODE_CTR, counter=IV)
        print decrypto.decrypt(ct)


