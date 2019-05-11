title: 测试VPS真实内存
date: 2016-08-28 09:59:25
tags: shell
categories: 网络
---
测试商家是否超售内存，但是不能测出其它超售参数
<!-- more -->

## 修改shm分区大小

默认tmpfs挂载点是/dev/shm。大小是系统RAM的一半，要将其改为跟内存一样大小

查看分区

	df -h /dev/shm

修改大小，size为商家的内存大小

	vi /etc/fstab 
	# 增加
	tmpfs /dev/shm tmpfs defaults,size=4096M 0 0
	
重新挂载 

	umount /dev/shm && mount /dev/shm

## 写入文件

切换到/dev/shm进行暴力测试

	cd /dev/shm
	dd if=/dev/zero of=./memtest bs=1M count=100
	
意思就是以1M为单位块写入count=100块，共100M，多测几次的count可以知道真实的内存。

直到提示```dd: error writing './memtest': No space left on device```，自己作个简单的减法即可知道奸商的超售内存实际上是多少。

实测搬瓦工6美元套餐128MB内存没有超售。

测完建议还原为正常的```/etc/fstab```内容