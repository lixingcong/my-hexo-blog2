title: Cryptography解题报告：Message Integrity
date: 2016-03-11 23:35:59
tags: [python, 密码学]
categories: [编程]
---
这次的题目就比较简单，只涉及到SHA-256的使用。装个Pycrypto库就能用了。解题思路就是从后面的block往前面block添加Hash值，复杂度O(n)。
<!-- more -->
我花的时间主要在怎么读取文件，还有各种解码encode decode，哎，基础不扎实，该用的时候只能现场查资料学习。
题目指向的两个stream文件就是视频文件：

- 6 - 1 - Introduction (11 min).mp4
- 6 - 2 - Generic birthday attack (16 min).mp4

下载这两个视频文件，先测试一下后面那个视频文件是否输出H0为教授给出的hash值：

	03c08f4ee0b576fe319338139c045c89c3e8e9409633bea29442e21425006ea8

我写的代码，其中视频文件是video_2.mp4

    #!/usr/bin/env python
    # -*- coding: utf-8 -*-
    
    from Crypto.Hash import SHA256
    BLOCK_SIZE = 1024
    hash_stream = ''

    def calc_sha(f):
        global hash_stream
        h = SHA256.new()
        h.update(f)
        hash_stream = h.digest()

    with open('/tmp/vedio_2.mp4', 'rb') as f:
        len_ = len(f.read())
        amount_of_blocks = len_ / BLOCK_SIZE
        len_of_last_block = len_ % BLOCK_SIZE

        f.seek(-len_of_last_block, 2) # 最后一个block
        calc_sha(f.read(len_of_last_block))
        i = amount_of_blocks - 1  # 倒数第二个block

        while i >= 0:
            f.seek(i * BLOCK_SIZE, 0)
            merge = f.read(BLOCK_SIZE) + hash_stream
            calc_sha(merge)
            i -= 1

        print hash_stream.encode('hex')

执行结果与hash值一致，那么自己将open函数中的视频文件名改为video_1.mp4即可得出正确答案。