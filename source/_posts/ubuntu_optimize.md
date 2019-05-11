title: ubuntu通用优化设置
date: 2015-10-28 22:15:46
tags: ubuntu
categories: 编程
---
自己在反复重装过程总结出的经验。可以供大家参考

## 笔记本开机亮度

每次都是来那个瞎眼的亮度，我也是醉了，14.04居然还没有修复这个bug。
<!-- more -->
跟黑苹果一个吊样。囧

控制亮度的文件位置：
- 对于MBR格式安装的ubuntu，控制亮度在/sys/class/backlight/acpi_video1/brightness
- 对于GPT格式安装的，控制在/sys/class/backlight/intel_backlight/brightness

可以测一下当前的亮度，先手动调节到合适的亮度，打印出当前的亮度值

	cat /sys/class/backlight/intel_backlight/brightness

比如我的是GPT，当前亮度值为480，可以将脚本写入/etc/rc.local进行加载亮度

	vi /etc/rc.local
	echo 480 > cat /sys/class/backlight/intel_backlight/brightness
    
重启看看效果

## 时钟每次比CMOS快8小时

这种状况主要是出现在双系统（win + linux）上，因为win默认是以cmos时间为utc+8后的时间，而ubuntu每次读取cmos时间都是以cmos时间为utc+0，解决方法：[原文链接](http://docs.slackware.com/howtos:hardware:syncing_hardware_clock_and_system_local_time)

解决方法

	vi /etc/default/rcS //将UTC改为no，保存退出
	sudo rm /etc/localtime
	sudo ln -s /usr/share/zoneinfo/Asia/Shanghai /etc/localtime
	sudo hwclock --systohc --localtime    //将sys-clock与cmos同步

检查是否设置正常：硬件和系统时间一致，且时区为CST。

	# 显示cmos中的时间
	sudo hwclock --show
	
	# 显示当前时间
	date

网上看到另一种方法：手动设置时区

	dpkg-reconfigure tzdata
    
## 开机自动挂载ntfs
目的是开机自动挂载某个ntfs卷。首先安装gparted看看你的卷是/dev/sdxx多少。然后

	mkdir /media/USER/NTFS
	sudo vi /etc/fstab
	#加入挂载命令
	/dev/sda6 /media/USER/NTFS ntfs defaults,locale=zh_CN.UTF-8 0 0 
	
这样得到的默认权限是600，即用拥有者只能读写。其他人都不能读写。
如果想要更大的权限，比如755，可以添加umask为022，如下句

	/dev/sda6 /media/USER/NTFS ntfs defaults,user,uid=1000,gid=1000,umask=022,locale=zh_CN.UTF-8 0 0
	
如果想禁止自动挂载，每次手动挂载。可以添加noauto标志，只要执行mount优先执行fstab里面的命令。(以600权限为例)

	/dev/sda6 /media/USER/NTFS ntfs defaults,noauto,locale=zh_CN.UTF-8 0 0

若挂载ext4分区，（建议使用ext4！），可以将fstab中的/home那一行拷贝过来，改成合适的uuid，改变挂载点的拥有者为当前用户。

	sudo mkdir /mydir
	sudo chmod username:username /mydir
	# sudo vi /etc/fstab
	# 增加一行类似/home的挂载
	UUID=54d8be7f-6bae-4512-aaef-6385c5fb6d13 /mydir ext4 defaults 0 2

## 转移chrome缓存

最好的就是关机自动清除缓存，懒人必备，利用ramdisk实现，vi /etc/fstab有一句是将部份内存挂载到/dev/shm

	tmpfs /dev/shm tmpfs defaults,size=512M 0 0
    
然后退出chrome，建立一个bash脚本存为~/.chrome-cache.sh，运行这个脚本测试一下。

	#! /bin/bash

	CACHE_DIR=~/.cache/google-chrome/Default
	MEMORY_DIR=/dev/shm/chrome-cache

	if [ ! -L $MEMORY_DIR ];then
		echo "creating $MEMORY_DIR"
		rm -rf $CACHE_DIR
		mkdir -p $MEMORY_DIR
		ln -s $MEMORY_DIR $CACHE_DIR
	fi
	
测试没问题就将脚本写到~/.zprofile里面，当进入X11桌面时，自动执行该脚本

	bash ~/.chrome-cache.sh


## 创建快捷方式

以AS为例，这是自己创建的Android Studio.desktop文件内容

	[Desktop Entry]
	Version=1.0
	Type=Application
	Name=Android Studio
	Exec="/home/ubuntu/programs/android-studio/bin/studio.sh" %f
	Icon=/home/ubuntu/programs/android-studio/bin/studio.png
	Categories=Development;IDE;
	Terminal=false
	StartupNotify=true
	# 这个WMclass暂时不太清楚是什么东西
	StartupWMClass=jetbrains-android-studio
	Name[en_GB]=android-studio.desktop

可以根据实际，修改图标和程序执行位置。
更改为可执行权限

	chmod a+x 'Android Studio.desktop'
    
对于GNOME3桌面环境，可将其放置在系统托盘的快捷方式中，将Android Studio.desktop复制到*/usr/share/applications*下面即可。

## 其他

主要是某些小程序安装

gedit disable auto-backup：

	编辑->设置->编辑器 取消选中“保存前创建备份文件”

安装温度监控：

	sudo apt-get install lm-sensors
    sudo sensors-detect
    sudo apt-get install xsensors //或者ksensors(KDE中推荐使用ksensors)
    
滚动条模式：

	使用经典滚动条命令：
	gsettings set com.canonical.desktop.interface scrollbar-mode normal
	恢复overlay scoller滚动条：
	gsettings reset com.canonical.desktop.interface scrollbar-mode
    
修改默认的session for 14.04

	cat /usr/share/xsessions //记下需要设置的session名字
	sudo gedit /usr/share/lightdm/lightdm.conf.d/50-ubuntu.conf
	把user-session改为适合。
    
网速指示 for gnome:

	sudo add-apt-repository ppa:nilarimogard/webupd8
	sudo apt-get update
	sudo apt-get install indicator-netspeed
    
cpu表格指示 for gnome:

	sudo apt-get install indicator-multiload
    
安装星际词典：

	sudo apt-get install stardict
   
然后到[地址1](http://download.huzheng.org/)或者[地址2](http://blog.sciencenet.cn/blog-203570-722938.html)下载一个字典包，解压到一个文件夹，比如/home/ubuntu/dicts/stardict-oxford-gb-formated-2.4.2，解压到一个文件夹内，允许有词典子目录。创建链接到词典文件夹

	 sudo ln -s ~/programs/Stardict_dicts /usr/share/stardict/dic
     
打开星际词典，设置词典后就可以查字典了。我用的词典主要有三个

	# 朗道英汉词典5.0 词汇量435468
	langdao-ec-gb-2.4.2
	
	# 牛津高阶词典 英英 词汇量 78075	
	Oxford_Advanced_Learner_s_Dictionary-8th

	# cedict汉英词典 词汇量31992
	stardict-cedict-gb-2.4.2


安装字体：
YaHei.Consolas.1.12.ttf是混排字体，适合写程序。

	mkdir ~/.fonts
	mv YaHei.Consolas.1.12.ttf ~/.fonts
	cd ~/.fonts
	chmod 744 *
	sudo mkfontscale
	sudo mkfontdir
	sudo fc-cache -f -v
    
注销即可使用新字体

RIME输入法(APT仓库支持)

ibus-rime输入法，设置横排，用网上的教材是无效的，需要使用ibus覆盖rime设置

	gsettings set org.freedesktop.ibus.panel lookup-table-orientation 0
	
某些设置可以在运行ibus-setup设置里面看到
运行im-config配置多输入环境。

连自定义字体都需要命令修改

	gsettings set org.freedesktop.ibus.panel use-custom-font true

任务栏指示当前的运行情况（cpu.ram.hdisk.net）使用[gnome-shell-system-monitor-applet](https://github.com/paradoxxxzero/gnome-shell-system-monitor-applet)

添加shell extension的方法：[Youtube教程](https://www.youtube.com/watch?v=etAneQtlTLQ)

屏幕录像

	sudo add-apt-repository ppa:maarten-baert/simplescreenrecorder
	sudo apt-get update
	sudo apt-get install simplescreenrecorder
	
SMPLAYER播放器

	sudo add-apt-repository ppa:rvm/smplayer 
	sudo apt-get update 
	sudo apt-get install smplayer smplayer-themes smplayer-skins

Android Studio杂项

初次运行提示appt返回一个非零的值，是缺库：

	sudo apt install lib32stdc++6

输入法：fcitx支持
	
	vi android studio/bin/studio.sh
	# 文件上方加入
	export XMODIFIERS=”@im=fcitx” 
	export GTK_IM_MODULE=”fcitx” 
	export QT_IM_MODULE=”fcitx”
	
可以支持fcitx输入法框架，但是AS仍然无法跟踪光标。

KDE环境快捷键修改

终端：修改复制按键为ctrl+insert：配置->设置快捷键

全局按win窗口快捷键：
进行左右侧snap：系统设置->快捷键->全局快捷键->组件KWin->将窗口快速铺到左（右）侧，最大化，最小化
显示桌面：系统设置->快捷键->全局快捷键->组件KWin->显示桌面

多媒体（音量增减）:
快捷键->全局快捷键->自定义快捷键->新建如下三个动作代表静音、加、减音量

	amixer set 'Master' 0
	amixer set 'Master' 10%+
	amixer set 'Master' 10%-

安装差异比较器（类似windows的beyond compare）

	sudo apt install meld

串口设备读写权限

	sudo vi /etc/udev/rules.d/99-ttyUSB.rules
	KERNEL=="ttyUSB[0-9]*",MODE="0666"

效果是：不需要使用sudo 来进行open串口

fcitx在zsh环境下的环境变量。解决部份窗口无法切换输入法，和无法自启动的问题。
    
    # ~/.zprofile
    
    export XIM=fcitx  
    export XIM_PROGRAM=fcitx
    export GTK_IM_MODULE=fcitx
    export QT_IM_MODULE=fcitx
    export XMODIFIERS="@im=fcitx"
    
    # auto start fcitx
    exec fcitx-autostart &


