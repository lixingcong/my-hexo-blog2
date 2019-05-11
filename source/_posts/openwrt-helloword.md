title: openwrt编译出helloword.ipk
date: 2016-05-03 12:47:57
tags: openwrt
categories: 编程
---
openwrt开发的第一步就是生成一个package让其在路由器上运行！
<!-- more -->
## 前戏

### 获取openwrt内核树

以一个非root用户登陆ubuntu 16.04 x64，这里的用户名叫test。

新建一个1.sh文件

	#! /bin/bash
	cd ~
	sudo apt-get install git build-essential libssl-dev libncurses5-dev unzip subversion mercurial gawk
	git clone http://git.openwrt.org/15.05/openwrt.git
	cd openwrt && ./scripts/feeds update -a && ./scripts/feeds install -a && make defconfig && make prereq
	
运行1.sh，实现了下载openwrt 15.05的源码并自动配置依赖关系

### 获取工具链

以MT7620为例

	wget http://downloads.openwrt.org/chaos_calmer/15.05.1/ramips/mt7620/OpenWrt-SDK-15.05.1-ramips-mt7620_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64.tar.bz2
	tar xjf OpenWrt-SDK-15.05-ar71xx-generic*.bz2
	cd OpenWrt-SDK-15.05-ar71xx-generic*
	cp -R staging_dir ~/openwrt
	
	
## 简单的print

所以记得开发的原则是：先确保在本机运行正常，再尝试移植到openwrt！

### 本机的hellowolrd

	cd ~/openwrt/package
	mkdir hello && mkdir hello/src
	cd hello/src
	

main.c主程序
	
	vi main.c
	# ---
	#include <stdio.h>
	int main(){
		printf("hello world!\n");
		return 0;
	}

	
Makefile也要写一个。

	vi Makefile
	# ---
	all:hello
	LIBS+=-lsodium
	hello:main.o
		$(CC) main.o -o hello $(LDFLAGS) $(LIBS) 
	main.o:main.c
		$(CC) -c main.c $(CFLAGS)	
	clean:
		rm hello
		rm -rf *.o
	# install is required for openwrt if you'd like to build *.ipk
	install:
		cp hello /tmp/
	
测试一下Make是否成功

	make && ./hello
	# 结果：hello world!
	make install
	# 结果：成功拷贝/tmp
	make clean
	# 结果：成功清除垃圾啊中间文件
	
	
### 编写openwrt的Makefile

参考[openwrt官方教程](https://wiki.openwrt.org/doc/devel/packages)

openwrt的package Makefile是移植程序的好帮手，非常容易掌握，格式很固定，我是根据clowwindy的配置修改的

	cd ~/openwrt/package/hello
	vi Makefile
	# ---
	include $(TOPDIR)/rules.mk
	# 包信息
	PKG_NAME:=hello
	PKG_VERSION:=0.1
	PKG_RELEASE:=1
	
	# build目录
	PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

	# 使用install=1生成安装包
	PKG_INSTALL:=1
	PKG_FIXUP:=autoreconf
	PKG_USE_MIPS16:=0
	PKG_BUILD_PARALLEL:=1
	include $(INCLUDE_DIR)/package.mk
	
	# 准备编译前先拷贝源码build_dir
	define Build/Prepare
		cp -R src/* $(PKG_BUILD_DIR)/
	endef

	# 定义包的menuconfig分类目录、包的依赖等
	define Package/hello
		SECTION:=net
		CATEGORY:=Network
		TITLE:=hello world
		URL:=https://github.com/clowwindy/shadowvpn
	endef

	# 定义安装到openwrt的路经，其中$(1)等常量的解释详见官方文档
	define Package/hello/install
		$(INSTALL_DIR) $(1)/usr/bin
		$(INSTALL_BIN) /tmp/hello $(1)/usr/bin
	endef

	$(eval $(call BuildPackage,hello))
	
开始配置编译

	cd ~/openwrt
	make menuconfig
	# 选择Network->hello (这个可以修改，在上面的Makefile)
	# 使用 M 标记为编译单独的package
	make package/hello/compile V=99
	
确认无误后查看编译的ipk

	find bin | grep hello
	
拷贝到路由器安装，测试一下！！

## 编译带有第三方库的print

在上面的例子中没有引入第三方的lib，日常开发中不可能自造轮子而不调用api，所以试一下第三方包[libsodium](https://github.com/jedisct1/libsodium)，当然，你的电脑必须先安装libsodium。

### 本机编译helloworld

src/main.c内容修改为

	#include <stdio.h>
	#include <sodium.h>
	int main(){
		if (sodium_init() == -1) return 1; 
		printf("hello!\n");
		return 0;
	}


src/Makefile中加入一句

	LIBS += -lsodium
	
测试一下

	make && ./hello
	make install
	make clean
	
没有问题就直接移植吧！

### 编写openwrt的Makefile

修改Makefile，在 *'define Package/hello'*一段中加入依赖

	DEPENDS := +libsodium

我的feed中的Libsodium源码地址有问题，要加上old

	vi package/feeds/packages/libsodium/Makefile
	# 加上/old
	PKG_SOURCE_URL:=https://download.libsodium.org/libsodium/releases/old
	
开始配置编译，要记得menuconfig一下！

	cd ~/openwrt
	make menuconfig
	make package/hello/compile V=99
	
编译后同样拷贝ipk测试一下

## 发布package

上面的步骤都是在本地修改源码并编译，确认版本无误后可以选择发布，即让Makefile从指定的URL下载源码编译

将hello/src文件夹打包

	cd ~/openwrt/package/hello
	tar -cvzf /tmp/0.1.tar.gz src/1.c src/Makefile
	
然后把/tmp/0.1.tar.gz上传到某网站，例如github release page。这里我随意指定个地址示意

	vi ~/openwrt/hello/Makefile
	# ---
	# 增加几个信息，可以选择加入md5sum校验
	PKG_SOURCE:=$(PKG_VERSION).tar.gz
	PKG_SOURCE_URL:=https://github.com/$(PKG_VERSION)
	PKG_MD5SUM:=xxxxxxxxxxxxx
	
	# 因为打包的是src目录结构，所以改一下 PKG_BUILD_DIR指向src
	PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)/src
	
	# 顺便删掉define build/prepare整块，不需要再拷贝源码了
	
这样就能删掉package/hello/src整个文件夹。然后打包Makefile发布，给别人编译。

	rm -rf package/hello/src
	tar -cvzf /tmp/hello.tar.gz package/hello
	
别人拿到你的hello.tar.gz直接在openwrt目录解压，然后他就能编译了

	cd ~/openwrt && tar -zxf hello.tar.gz
	# 然后就是耳熟能详的make menuconfig 和 make/hello/compile
	
## 后记

这篇文章是我参考很多英文材料总结得来的，经过我的实践确认，算是半原创了，参考了aa65535和clowwindy的Makefile，所以码农的一大特点就是：会搜集，会抄会改！