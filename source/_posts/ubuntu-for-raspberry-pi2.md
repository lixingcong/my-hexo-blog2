title: ubuntu 14.04 树莓派2
date: 2016-04-21 00:41:35
tags: [ubuntu, 树莓派]
categories: 编程
---
个人并不太喜欢原配的raspbian系统，占用空间太大（4GB+），太多杂七杂八的应用。如果在树莓派上面使用ubuntu社区版本，就会令我觉得更轻量（无X-desktop），更通用（与桌面版命令一致）。因此记下自己折腾ubuntu的过程。
<!-- more -->
## 与原系统性能差别

评测：[Pi 2安装ubuntu和respbain的性能测评](http://www.finnie.org/2015/02/21/raspberry-pi-2-ubuntu-raspbian-benchmarks/)

结论：由于ubuntu内核默认开启了LSM系统防护（AppArmor）模块，导致性能略逊于raspbian(它默认没有该模块)，如果能关掉LSM那ubuntu性能明显强于后者。

参考ubuntu的AppArmor说明文档

https://help.ubuntu.com/community/AppArmor
http://ask.xmodulo.com/disable-particular-apparmor-profile-ubuntu.html

## 安装ubuntu

内存卡至少2GB，参考[ubuntu 14.04 社区版 博客 安装指引](http://www.finnie.org/2015/02/14/ubuntu-14-04-trusty-tahr-on-the-raspberry-pi-2/)，下载该文件解压。

	sudo apt-get install bmap-tools
	sudo bmaptool copy --bmap 2015-04-06-ubuntu-trusty.bmap 2015-04-06-ubuntu-trusty.img /dev/sdX

不建议dd或者ddrescure恢复。会丢启动信息导致ubuntu halt

## 开启ssh

进入树莓派的ubuntu使用网线连接路由，安装ssh服务器

	sudo apt-get update
	sudo apt-get install openssh-server 

小设置

	sudo vi /etc/ssh/sshd_config
    # 监听
	ListenAddress 0.0.0.0
    # Authentication:
    # 允许root登陆
    PermitRootLogin yes
    # 关闭严格模式
    #StrictModes yes
    # 重定义失败次数
    MaxAuthTries 6
    #MaxSessions 10

重启sshd

	sudo /etc/init.d/ssh restart

## 开启无线连接

我愿意开启root用户。先改root密码

    sudo passwd root
    
切换root

    su

安装wpa支持无线加密

	apt-get install wpasupplicant

插入无线网卡（RTL 8188CU/8192CU），启动接口

    ifup wlan0
    wpa_passphrase <SSID> <WIFI-PASSWORD> > /etc/wpa_supplicant.conf

开启连接

	wpa_supplicant -B -iwlan0 -c/etc/wpa_supplicant.conf -Dwext
	# 可能出现三行错误，忽略它
	dhclient wlan0

测试一下是否连接成功，看wlan0的ip：
	
    ifconfig

然后写入到/etc/rc.local作为启动脚本,开机即可使用无线网

	vi /etc/rc.local
    # 增加到exit 0前
	wpa_supplicant -B -iwlan0 -c/etc/wpa_supplicant.conf -Dwext
	dhclient wlan0
    exit 0

## 交叉编译

工具链（树莓派2：BCM2836 ARMv7）

	sudo apt-get install gcc-arm-linux-gnueabihf
	arm-linux-gnueabihf-gcc -march=armv7 -o main main.c 
    
然后scp到树莓派看看能否运行。

	scp ./main root@192.168.1.101:/tmp

使用GPIO库（C语言）（默认是动态库，可以选择编译静态库）

	git clone git://git.drogon.net/wiringPi
	cd wiringPi && sudo ./build

使用GPIO库(Python)

	http://sourceforge.net/p/raspberry-gpio-python/wiki/Home/

## 其他 

### 安装中文语言

	sudo apt-get -y install language-pack-zh-hans language-pack-zh-hans-base


### 应用案例

[知乎：有哪些对树莓派 (Raspberry Pi) 的有趣改造、扩展应用？](https://www.zhihu.com/question/20697024)
[不可小觑！树莓派16个酷炫玩法](http://www.ithome.com/html/digi/121079.htm)
[我拿树莓派来做什么](http://www.geekfan.net/2135/)



