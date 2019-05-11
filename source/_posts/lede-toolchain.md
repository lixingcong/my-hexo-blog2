title: 获得LEDE的工具链
date: 2017-02-01 16:25:49
tags: openwrt
categories: 网络
---
工具链(toolchain)用于交叉编译，每个工具链对应一种CPU指令集。LEDE的工具链可以编译特定的Package。没有它的话，将无法编译Package，当然也无法编译固件。
<!-- more -->
## 工具链简介

它通常有一个固定的前缀(prefix)，比如

	arm-openwrt-linux-uclibcgnueabi

那么这个工具链里面，很多的二进制编译器链接器调试器就这样命名

	arm-openwrt-linux-uclibcgnueabi-gcc
	arm-openwrt-linux-uclibcgnueabi-ar
	arm-openwrt-linux-uclibcgnueabi-gdb

默认情况下，当我们git克隆了openwrt/LEDE的源码后，并没有获得工具链的二进制文件。我们可以有两种方法获得工具链。

下文的target system为MT7620N，对应的prefix为

	mipsel-openwrt-linux

对应的工具链名称为

	toolchain-mipsel_24kc_gcc

## 法一：源码编译

这种方法优点是能获得最新的工具链，但是在低端电脑上面编译耗时间较长。

执行make menuconfig后，选择好target system后，回到主菜单中有如下一项，勾选以编译SDK（即工具链）

![](/images/lede-tc/build-the-openwrt-sdk.png)

这样保存退出后，编译

	# 双线程编译
	make -j2

我的i5 3317u编译了差不多半小时。获得target固件在bin目录下。SDK的位置在staging_dir目录下。

## 法二：使用预编译工具链

这种方法优点最方便，但是获得的工具链的版本可能较老。

执行make menuconfig后，不勾选编译SDK

如果此时我们选择一个package编译，例如aria2

	make package/aria2/compile V=99

就会出现找不到工具链的错误。这就证明了没有工具链是无法编译package的。

此时在lede源码目录的staging_dir中会生成一个新的文件夹，名称如下（后面版本号也许有差异）

	toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.16

解决思路：我们从LEDE官网中下载工具链的预编译二进制文件，建立符号链接到该自动生成的toolchain目录，让LEDE认为工具链已经就绪。

从官网下载合适target的工具链，解压缩，比如我的是MT7620N的工具链。

	# 下载目录
	cd /home/ubuntu/toolchain

	wget https://downloads.lede-project.org/releases/17.01.0-rc1/targets/ramips/mt7620/lede-sdk-17.01.0-rc1-ramips-mt7620_gcc-5.4.0_musl-1.1.15.Linux-x86_64.tar.xz

	# 解压
	tar xf *.tar.xz

在LEDE源码目录建立符号链接即可

	cd /home/ubuntu/lede

	# 删掉原来的无效toolchain文件夹
	rm -rf staging_dir/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.16

将下载的toochain符号链接到原来的无效toolchain文件夹，注意使用绝对路径比较保险

	ln -s /home/ubuntu/toolchain/lede-sdk-17.01.0-rc1-ramips-mt7620_gcc-5.4.0_musl-1.1.15.Linux-x86_64/staging_dir/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.15 \
          /home/ubuntu/lede/staging_dir/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.16

替换原来的host文件夹

	# 删掉原来的无效host文件夹
	rm -rf staging_dir/host

将下载的host符号链接到原来的无效host文件夹

	ln -s /home/ubuntu/toolchain/lede-sdk-17.01.0-rc1-ramips-mt7620_gcc-5.4.0_musl-1.1.15.Linux-x86_64/staging_dir/host
	      /home/ubuntu/lede/staging_dir/host

## 多个工具链并存

比如我有MT7620N和AR71xx两个工具链，那么host文件夹无需重复替换。我认为host文件夹与target无关。

多个工具链并存的好处就是可以在make menuconfig选择不同的target编译出不同CPU架构的二进制package，比较方便实现一份源码编译出不同target的二进制。