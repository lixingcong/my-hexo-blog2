title: 不编译shadowsocks的文档
date: 2016-07-20 23:35:25
tags: shadowsocks
categories: 网络
---
shadowsocks-libev从2.4.8版本开始增加了asciidoc样式的帮助文档生成，个人感觉意义不太大，手册这玩意再漂亮，谁也不会天天看，要不只看一次就会用了，下次不会用再查。
<!-- more -->

```2018-06-11```友情提示：本文较老旧，可能对新版ss-libev不再适用。

该man page依赖的asciidoc，安装后体积高达1GB，真没必要啊！我的宝贵的搬瓦工这个破3GB磁盘，绝对没空间放下这么庞然大物！

下文以shadowsocks-libev v2.6.2 为例，版本为

	commit 520290349ed62129579013eac72bcc4d0119fcd1
	Author: Max Lv <max.c.lv@gmail.com>
	Date:   Thu Jan 19 12:14:26 2017 +0800

		Update configure.ac

其它版本也许不太一样。但是思路是类似的。

首先得更新submodules，按照README.md文档步骤来。

	git clone https://github.com/shadowsocks/shadowsocks-libev.git
	cd shadowsocks-libev
	git submodule update --init --recursive

编译主要分为下面两种情况：Make编译和build-deb包

## 方式一 make

make是直接产生二进制可执行文件。

编译时候默认是编译帮助文档的，由于依赖包体积过大可以选择不编译手册(documentation)

	./autogen.sh
	./configure --disable-documentation
	
	# 四线程编译并安装
	make -j4 install

## 方式二 build-deb

主要是debian/ubuntu用户，方便打包成debian package在多台机子上安装。这样不需要每台机子都执行make

删掉检查依赖asciidoc xmlto

	vi debian/control
	# 删build-depends内的asciidoc和xmlto

不安装Man手册

	vi debian/shadowsocks-libev.install
	# 删usr/share/man/

向configure传递参数：禁用编译文档

	vi debian/rules
	# 找到
	override_dh_auto_configure:
	# 添加这句
	--disable-documentation \

编译

	./autogen.sh && dpkg-buildpackage -b -us -uc -i
	ls .. | grep shadowsocks

安装前可以备份一份/etc/shadowsocks-libev/config.json。以便恢复旧版本的配置。

卸载干净旧版本（可选步骤）

	dpkg -r shadowsocks-libev 
	dpkg -P shadowsocks-libev

最后安装deb，并解决软件包依赖错误

	dpkg -i shadowsocks-libev_2.6.2-1_amd64.deb
	apt install -f

