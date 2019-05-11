title: 反汇编apk
date: 2016-03-27 13:53:36
tags: Android
categories: 编程
---
反汇编用于分析某个apk的程序思路。逆向工程有风险，举报需谨慎。以下为ubuntu系统操作。
<!-- more -->
## apk转成jar
首先安装JDK环境。必须的

	sudo apt-get install default-jdk

使用[dex2jar](https://github.com/pxb1988/dex2jar)

首先使用zip解压出classes.dex，执行

	sh ./d2j-dex2jar.sh /tmp/classes.dex
    mv classes-dex2jar.jar /tmp/
    
## apk转成smali

使用[apktool](https://github.com/iBotPeaches/Apktool)

	java -jar apktool.jar test.apk
    
输出文件夹为.out结尾的。

## jar转成java

使用[procyon](https://bitbucket.org/mstrobel/procyon/)

	java -jar ./procyon-decompiler.jar /tmp/test.jar -o /tmp/java_src
    
即可把/tmp/test.jar反汇编成java源代码。放在/tmp/java_src下。
配合jd-gui-eclipse-plugin即可查看。

如果是windows系统，可以直接使用[jd-gui](https://github.com/java-decompiler/jd-gui)打开jar即可看到源代码。
## 打包

	TODO
    
## 签名

	TODO