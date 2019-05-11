title: 从Openwrt SDK编译出ShadowVPN包
date: 2016-01-02 13:03:38
tags: shadowsocks
categories: 网络
---
实验目的：尝试编译ShadowVPN，使其在路由器上面运行。配合Chinadns达到分流
操作平台: ubuntu 14.04 x64 (RAM 128M)
~~若用国内的主机编译的话，网速太蛋疼，下载东西老是超时~~
<!-- more -->
当然，大多数人不需要折腾了，有[预编译版本](https://github.com/aa65535/openwrt-shadowvpn)可以直接下载，我的目的是学习一下基本的SDK编译过程
## 前戏

### 安装所需工具
在root用户下

	apt-get update
	apt-get install git build-essential libssl-dev libncurses5-dev unzip subversion mercurial gawk
    
新建一个非root用户。openwrt官方文档也是这么说的。

	adduser test

注销，登陆至test用户

### 获取openwrt目录树

建议使用15.05或以上版本。其他发行版分支可以在[openwrt源码](http://git.openwrt.org/)查看。

克隆源码

    cd ~
    git clone http://git.openwrt.org/15.05/openwrt.git
    
更新、安装feed

    cd openwrt
    ./scripts/feeds update -a
    ./scripts/feeds install -a
	
检查包依赖

	make defconfig
	make prereq
    

## 处理shadowvpn源码

### 获取源码

参考这篇文章[《ShadowVPN配置记录》](/2015/12/17/shadowvpn-conf-lixingcongv2)，讲述如何处理ShadowVPN源码。


假设已经完成Shadowvpn下载，libsodium更新，并放在/tmp/ShadowVPN。并且能编译出linux服务端无误。

先清除一下

	cd /tmp/ShadowVPN
    make clean
    
### 二次打包
    
打包为tar.gz文件。目的是集成libsodium进tar文件。
	
    tar -cvzf /tmp/ShadowVPN-0.2.0.tar.gz /tmp/ShadowVPN
    
### 创建自定义patch
    
创建diff快照，用于产生patch补丁文件。

    cd /tmp/ShadowVPN
    git init
    git config --global user.name "xx"
    git config --global user.email "xx@xx.com"
    git add .
    git commit -m "Before patched"
    
按照[openwrt-shadowvpn项目](https://github.com/aa65535/openwrt-shadowvpn/blob/master/patches/000-fix-autoconf.patch)中的patch文件，修改/tmp/ShadowVPN/下面对应的四个文件：

vi Makefile.am

	找到EXTRA_DIST = \
	删掉README.md和COPYING
	找到SUBDIRS
	删掉samples

vi configure.ac

	找到AC_CONFIG_FILES([Makefile src/Makefile samples/Makefile])
	把后面的samples/Makefile删掉
	
vi libsodium/Makefile.am

	找到EXTRA_DIST = \
	删掉其下的内容，只保留autogen.sh
	找到SUBDIRS = \
	删掉其下的内容，只保留src
	
vi libsodium/configure.ac

	找到AC_CONFIG_FILES([Makefile
	删掉这些行：
	dist-build/Makefile
	libsodium-uninstalled.pc
	msvc-scripts/Makefile
	test/default/Makefile
	test/Makefile

只需根据patch删掉对应行就可以了
提交更改

	git add .
	git commit -am "patched!"
    
查看SHA散列值：记下patch前后的散列值前七位

    git log
    git diff BEFORE_ID AFTER_ID > /tmp/modify.patch
    
生成的patch文件暂时放在/tmp下面
    
### 获取shadowvpn-openwrt源码

其实并不是源码，只是借openwrt-shadowvpn这个工程（包含了很多适配脚本和init脚本）来编译含有patch后的op软件包

移动源码至dl目录。目的是制造假象：已经下载源码

	cp /tmp/ShadowVPN-0.2.0.gz /home/test/openwrt/dl/

克隆[@aa65535](https://github.com/aa65535)的工程

    cd ~/openwrt
    git clone https://github.com/aa65535/openwrt-shadowvpn.git package/shadowvpn
    cd package/shadowvpn
    
覆盖@aa65535的patch文件

	cp /tmp/modify.patch patches/000-fix-autoconf.patch

修改Makefile使其指向当前主机的源代码

    PKG_SOURCE_URL:=/home/test/openwrt/dl/${PKG_VERSION}
    
注释PKG_MD5SUM校验，防止出错。

这样效果就是不需要每次从openwrt-shadowvpn的release页面下载源码了。可以编译自定义的源码

## 编译

编译过程大同小异，对所有软件（包括shadowsocks-libev,chinadns)都是类似的。

### 获取工具链

不同平台对应不同的工具链，我的路由器是WR703n，对应AR71xx平台，如果你的路由器是MT7620或者其他的，请到[openwrt官网下载](http://downloads.openwrt.org/)对应的toolchain-gcc。

请带上你脑袋，根据实际路径和路由机型更改，不要抄袭我，跟我一样的配置！

下面所有命令以ar71xx为例。

    cd ~
    wget http://downloads.openwrt.org/chaos_calmer/15.05/ar71xx/generic/OpenWrt-SDK-15.05-ar71xx-generic_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64.tar.bz2
    tar xjf OpenWrt-SDK-15.05-ar71xx-generic*.bz2
    cd OpenWrt-SDK-15.05-ar71xx-generic*
    cp -R staging_dir ~/openwrt
    
设置系统变量

    export STAGING_DIR=/home/test/openwrt/staging_dir/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-0.9.33.2
    export PATH=$PATH:$STAGING_DIR/bin
    
测试AR71xx的gcc是否有效：

    mips-openwrt-linux-gcc
    
如果command not found需要重新检查设置变量
提示fatal error: no input files即为正确结果，可以进行下一步（配置目标）

注意：如果你的路由器是MT7620，那么

	toolchain目录是 toolchain-mipsel_24kec+dsp_gcc..
	检查MTK gcc是否生效的命令是 mipsel-openwrt-linux-gcc


### 配置目标

    cd ~/openwrt
    make menuconfig

出来蓝色窗口，由于我的是ar71xx的工具链，所以Target选ar7xxx

    ＃ Target System: Atheros AR7xxx/AR9XXX  
    ＃ Subtarget: Generic/Default
    ＃ Target Profile: (因我们只是编译包，这步可以不选)
    ＃ Network->ShadowVPN：按m设置为编译独立ipk安装包

连按esc两次，再按esc两次，然后退出保存config文件，默认回车即可

注意：MT7620的Target System是"Ralink RT288x/RT3xxx"再选subtarget MT7620

### 最后一步

编译指定的包

	make package/shadowvpn/compile V=99
    
大约一分钟，在指定的目录下找到ipk文件

    cd bin/ar71xx/packages/base
    ls | grep ShadowVPN
    
拷贝到路由器安装即可。
    
## 其他

总结思路：

在openwrt-shadowvpn目录下的Makefile中的URL改为本机目录/home/test/openwrt/dl/。
作者aa65535的思路是：每次编译都是从自己的github-releases页面下载源码。因此想要自己修改代码，必须替换下载URL，指向本机，达到自定义代码的效果。

在工程openwrt-shadowvpn中的Makefile是自动调用下载源码的命令。想把源代码直接放进去build_dir是不可能的，每次进行make都是自动下载源码会将build_dir清空。同理，将Shadowvpn源代码放进去package目录或者dl目录也是不行的。只能打包后放dl文件夹内。


