title: GRUB2的rescue模式
date: 2019-07-14 10:11:14
tags: ubuntu
categories: 编程
---
一台不支持UEFI启动的老爷机，已安装了grub2，某日正常使用手贱给/boot分区前面的分区重新分区。

分区后，MBR分区的/dev/sdaX序号改变了，因此重启后进入grub找不到/boot分区，停在了GRUB rescue模式
<!-- more -->
## 救援模式

![](/images/grub-rescue/boot.jpg)

rescue模式支持非常少的命令，而且没TAB触发自动补全，更坑爹是没有方向Up和Down导航上一条/下一条命令。。。

所以很多命令都要一次输入完整和正确，否则又要重新输入一次，真麻烦。

|命令|功能|
|--|--|
|set|不带参数则查看所有变量，或者带参数设置这个变量|
|ls|不带参数则查看所有分区，或者带参数列出分区或者路径下文件|
|insmod|加载模块(*.mod)|

就是得通过上面几个命令来在rescue模式下，加载正常的GRUB，再重新进入Linux修复GRUB

## 进入正常GRUB

像我这种是之前就有正确的/boot分区，可以在rescue模式下直接指定/boot/grub启动：

查看当前的变量

    set

从输出结果看出当前的root变量和prefix变量都是不正确的。比如我这边显示的是hd0,msdos7，那么根据分区情况可以推导正确的/boot可能在msdos6或者msdos8，反正接下来逐一尝试。

设置root变量，然后ls显示root下面的文件。

    set root=(hd0,msdos6)
    ls /

逐一尝试msdos6和msdos8，直到找到正确boot分区的设备，那么此时正确的root变量设置好了，就得设置正确的prefix变量

    set prefix=(hd0,msdos6)/boot/grub

加载normal.mod模块

    insmod /boot/grub/i386-pc/normal.mod

进入正常的GRUB2界面。

    normal

以上操作步骤可以拓展成更通用的情况：
1. 使用一个U盘，拷贝x86_x64-efi或者i386-pc架构的完整boot文件夹到U盘根目录
2. 在rescue模式下加载linux和initrd

## 引导Linux

既然进入了正常的Grub2，那么支持的命令就非常丰富了，还有自动补全功能。

引导Linux通用的操作先加载linux内核，再指定initrd内存盘。如下

    linux /boot/vmlinuz-4.15.0-46-generic
    initrd /boot/initrd.img-4.15.0-46-generic
    boot

因为本机上已有正常的ubuntu系统，直接从老的配置回车启动就可以。

也可以手工指定一个grub.cfg配置文件

    configfile (hd1,msdos1)/mygrub.cfg

## 进入Linux重新安装grub

安装到/dev/sda主引导记录（别照抄，根据个人实际情况修改）

    sudo grub-install
    sudo update-grub

如果是EFI平台，则另外安装efi的grub（当然这老爷机根本不支持的...）

    sudo apt install grub-efi
    sudo grub-install /dev/sda --target=x86_64-efi
    sudo update-grub
