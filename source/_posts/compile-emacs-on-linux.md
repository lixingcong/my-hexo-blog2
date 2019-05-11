title: 记一次编译Emacs的过程
date: 2016-04-19 22:56:43
tags: 转载
categories: 编程
---
由于ubuntu的APT源emacs版本太老，到2016年居然还是24.3的，没法用上最新的功能，例如“Shifit block缩进”，决定自行从官方源码编译一个。操作平台是VPS ubuntu 14.04 x64
<!-- more -->
[原文链接](http://blog.chinaunix.net/uid-28977986-id-3852467.html)

```2018-06-11```友情提示：本文提到的emacs版本较老，可能Emacs最新版不再适用。

于是自己下载了一个emacs-24.5.tar.gz的压缩包，[（官方地址）](http://ftp.gnu.org/gnu/emacs/)，解压
	
    tar xf emacs-24.5.tar.xz 

一看，发现里面全是源文件，要自己编译。
于是先阅读了一下里面的README文件，得知编译与安装步骤都在INSTALL文件中有。于是我一下的步骤都是参考的INSTALL文件。

先安装基本的编译库

	apt-get install build-essential
	apt-get install libncurses-dev

终端，cd到刚才解压的目录，运行./configure命令，运行了一阵子，我正感觉良好呢，它却报错了，如下：

> You seem to be running X, but no X development libraries were found. You should install the relevant development files for X
> and for the toolkit you want, such as Gtk+, Lesstif or Motif. Also make sure you have development files for image handling, i.e.
> tiff, gif, jpeg, png and xpm. If you are sure you want Emacs compiled without X window support, pass \-\-without-x
> to configure.

看来编译这个并不是那么容易的，上面这段错误提示的大意是没有装GTK+等图形开发库，如果不想在图形界面下使用Emacs，可以使用--without-x选项进行编译（这不废话吗，能用图形界面却不用，难道要回到原始的字符界面？）

那就装GTK+吧，命令如下（注意是GTK用于开发的开发库）：

	sudo apt-get install libgtk-3-dev texinfo

因为我是在vps上面编译：使用如下命令开启编译gtk3图形界面

	./configure --with-x-toolkit=gtk3

装好后，再运行./configure，没想到又报错了，如下：

> configure: error: The following required libraries were not found:
> libXpm libjpeg libgif/libungif libtiff
> Maybe some development libraries/packages are missing?If you don't want to link 
> with them give\-\-with-xpm=no -
-with-jpeg=no \-\-with-gif=no \-\-with-tiff=no as options to configure


好吧，几个图像开发库也要装。。
注意一个一个命令敲，若一次性全装会产生依赖问题。

	sudo apt-get install libgif-dev libxpm-dev
    sudo apt-get install libtiff4-dev

我需要libxml2，用于markdown-mode

	sudo apt install libxml2-dev

然后再运行./configure，这下终于可以正常地跑到结束了，看输出的信息，该生成的文件都生成了。于是，运行make，耗时15分钟编译完成。(这是搬瓦工的垃圾VPS，我的笔记本5min就编译了)

我把服务器上面打包回本地：

	tar -cvzf /tmp/emacs_x64.tar.gz emacs_24.5

然后从本地scp拷贝回来

	scp -P 82 username@xxx.xxx.xx.xx:/tmp/emacs_x64.tar.gz /tmp
    # 同样装上libgtk2
    sudo apt-get install libgtk-3-dev

本地解压，再运行sudo make install，但中间报了两个error，Cannot open load file: case-table。在网上查阅资料，说是Emacs源文件不能放在中文目录下。而我的恰好放在中文目录下，改变目录后，因为前面编译过，所以运行make clean，make distclean来清理一下，再重新运行./configure，make，sudo make install，这次是彻底没错误了，貌似中间连一个warning都没看到。

多线程编译

	make -j 4

新建一个快捷方式emcas.desktop

    #!/usr/bin/env xdg-open
    [Desktop Entry]
    Version=1.0
    Name=GNU Emacs 24.5
    GenericName=Text Editor
    Comment=View and edit files
    MimeType=text/english;text/plain;text/x-makefile;text/x-c++hdr;text/x-c++src;text/x-chdr;text/x-csrc;text/x-java;text/x-moc;text/x-pascal;text/x-tcl;text/x-tex;application/x-shellscript;text/x-c;text/x-c++;
    Exec="/usr/local/bin/emacs" %f
    Icon=/usr/local/share/icons/hicolor/scalable/apps/emacs.svg
    Type=Application
    Terminal=false
    Categories=Utility;Development;TextEditor;
    StartupWMClass=Emacs24

保存，设为权限可执行

	chmod a+x emacs.desktop
    
双击快捷方式，这位编辑器的大神终于露出了他的真容，不容易啊。
