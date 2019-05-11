---
title: 树莓派2上安装ArchLinux
date: 2017-10-16 20:58:02
tags: [树莓派, ubuntu]
categories: 编程
---
在树莓派上安装体积相对较小的ArchLinux，告别体积超大的Debian/ubuntu。本文在ubuntu 16.04下操作。
<!-- more -->
Archlinux对树莓派提供两种镜像：

一种是在windows下的Win32DiskImager写入的，文件为img。另一种是tar.gz的压缩文件，里面包含着Linux根分区的文件。

前者解压后img文件体积较大，需要TF卡容量至少为img文件的大小，分区不灵活，非常固定，需要后期调整分区。

而tar.gz解压后，可以在写入镜像前进行分区，分区分多少，分多少个分区，全由自己掌握。

|ArchLinux|优缺点|地址|
|---|---|---|
|img版|傻瓜化操作，方便|[下载](https://sourceforge.net/projects/archlinux-rpi2)|
|tar.gz版|自定义|[下载](http://os.archlinuxarm.org/os/ArchLinuxARM-rpi-2-latest.tar.gz)|

下面使用tar.gz版本在ubuntu下进行写入到树莓派TF卡。

## 编译libarchive

bsdtar版本需要更新到3.3以上，老版本解压tar.gz时候会出错：

	bsdtar: Ignoring malformed pax extended attribute
	bsdtar: Error exit delayed from previous errors.

直接在git上面获取最新版本，这里以3.3.2为例。需要cmake作为编译工具

	wget https://github.com/libarchive/libarchive/archive/v3.3.2.tar.gz
	tar xf v3.3.2.tar.gz
	cd libarchive-3.3.2
	cmake .
	make -j2

编译成功后，libarchive-3.3.2下的bin目录有可执行文件bsdtar，备用。（我没有直接安装Make install）

## 写入镜像到TF卡

参考ArchLinux安装的[官方教程](https://archlinuxarm.org/platforms/armv7/broadcom/raspberry-pi-2)，应该不会有错的。注意使用root用户而不是sudo。

在运行bsdtar命令时候指定一下刚编译出来的bsdtar绝对路径就可以了。

进行文件操作时候，最后记得sync，将文件完整地写入到TF卡里面。

## 登陆到ArchLinux

默认用户alarm，密码alarm。

默认root密码为root