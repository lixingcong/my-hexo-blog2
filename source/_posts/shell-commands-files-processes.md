title: shell命令：文件与进程
date: 2016-01-22 15:26:18
tags: [shell, ubuntu]
categories: 读书笔记
---
我的读书笔记，学习shell的强大的文件进程管理命令。

|书名|作者|下载地址|
|--|--|--|
|Classic Shell Scripting|Arnold R. & Nelson H.F.B|[暂无]()|
|Advanced Bash-Scripting Guide|Mendel Cooper|[PDF](http://www.tldp.org/LDP/abs/abs-guide.pdf)|

<!-- more -->
## ls

列出文件列表，等价于windows的dir
ll命令列出当前的文件属性

    [root@www ~]# ls -l
    -rw-r--r--	  1		root	users	6845 Jun 25 08:53 install.log
    # 输出结果格式化一下，如下表：
    
|\-rw\-r\-\-r-\-|1|root|users|6845|Jun 25 08:53|install.log|
|----------|-|----|-----|----|------------|-------|
|0123456789|link|owner|group|size|access date|filename|


注释
0：文件类型：
- d:目录
- \-:文件
- l:链接
- b:ram设备
- c:串行设备

1～9:拥有者权限，所属群组权限，其他人权限
权限掩码，[看这里](#umask)

link：文件硬链接数或目录子目录数，在[这里](#ln)看看

chgrp:改变文件所属群组
chowm:改变文件拥有者
chmod:改变文件权限（suid,sgid,sbit）
以上三个命令都要求改变后的用户或群存在于系统中/etc/passwd或group

chmod 可以使用数字形式或者符号形式：

    chmod 777 filename
    chmod a+x filename

其中a+x格式化一下：

|a|+|x|
|--|--|--|
|1|2|3|

注释
- 1:可选u g o a分别对应拥有者(user)，组(group)，其他用户(others)，全部(all)
- 2:可选+ - = 分别对应加入，除去，设定
- 3:可选r w x分别对应读写执行

显示当前的文件夹名称：

	pwd

显示当前文件夹绝对路径，加上-P 参数

pwd对应系统变量PWD

@ruanyf的奇淫技巧：有关显示目录的命令：

- 只显示普通目录，不显示隐藏目录

	ls -d */
    ls -F | grep /
    ls -l | grep ^d

- 只显示隐藏目录，不显示普通目录

	ls -d .*/
    
- 显示所有目录，不显示文件

	find -maxdepth 1 -type d

## ln

实现文件符号链接，硬链接。相当于windows的mklink

	ln [-sf] 来源文件(夹)src 目标文件(夹)dest
    
选项与参数：
-s  ：通常采用符号链接，如果不加任何参数就进行连结，那就是hard link，-s 就是symbolic link，
-f  ：如果 目标文件 存在时，就主动的将目标文件直接移除后再创建！

注意【SRC】和【DEST】需要写全绝对路径，否则提示

	Too many levels of symbolic links!

建立软链接就是建立了一个新文件。当访问链接文件时，系统就会发现他是个链接文件，它读取链接文件找到真正要访问的文件。建立硬链接会获得与原文件等同的地位。

博客上面ls命令提到的第二个字段link的含义如下：

一、如果是一个普通文件：

那么这一字段表示,这个文件所具有的硬链接数,即这个文件总共有多少个文件名.查看第一个文件:

	-rw-r--r-- 1 root root 1581 11月 24 18:14 aaa.cfg

第2字段的值为1,说明这个文件只有aaa.cfg这一个文件名.即只有一个指向该链接的硬链接. 
如果我用ln,做一个指向该文件的硬链接再查看该文件,该搜索文件的第2字段就会变成2:

    [root@ubuntu]# ln aaa.cfg aaa.cfg.hardlink 
    [root@ubuntu]# ls -l 
    总用量 4071 
    -rw-r--r-- 2 root root 1581 11月 24 18:14 aaa.cfg 
    -rw-r--r-- 2 root root 1581 11月 24 18:14 aaa.cfg.hardlink
    
此时,aaa.cfg 和aaa.cfg.hardlink 称为互为硬链接.他们指向同一个文件,无论是修改哪一个文件,另一个里也做相应的变化,因为实际上他们指向同一个文件.
可以查看它的文件节点(inode) 

	ls -i aaa.cfg

互为硬链接的文件具有相同的文件节点. 以下是验证实验:

    [root@ubuntu]# ls -i aaa.cfg 
    18102 aaa.cfg 
    [root@ubuntu]# ls -i aaa.cfg.hardlink 
    18102 aaa.cfg.hardlink

可以看到,这两个文件具有相同的文件节点号:18102
如果你知道一个文件有多个文件名,如何查找他的其他文件名分布在什么地方呢?
可以先用ls -i 获得它的节点号,然后用find查找，若具有多个文件名,我要查找与它互为硬链接的文件:

    [root@gucuiwen devices]# ls -i /etc/sysconfig/networking/devices/ifcfg-eth0 
    147181 /etc/sysconfig/networking/devices/ifcfg-eth0

得到它的节点号为 147181，再用find查找:

    [root@ubuntu]# find /etc -inum 147181 
    /etc/sysconfig/networking/devices/ifcfg-eth0 
    /etc/sysconfig/networking/profiles/default/ifcfg-eth0
    
这样就得到了同一个文件的不同文件名的位置.

二、如果是一个目录：

link字段表示该目录所含子目录的个数. 
新建一个空目录,这个目录的第二字段就是2,表示该目录下有两个子目录.有一个指向它本身的子目录"." 和指向它上级目录的子目录"..",这两个默认子目录是隐藏的.用ls -a可以看到.
每次在目录下新建一个子目录,该目录第2字段的值就增1,但是新建一个普通文件该字段值不增加.

## cd 

经典的命令，(Change Dir)

建议配合使用压栈出栈的方式cd：

	cd ~
	pushd .ssh/
	# 这样打开了.ssh文件夹，并把上一次的位置入栈
	popd
	# 返回到～，出栈了

## find

搜索当前文件夹，并以ascii顺序排序结果：

	find | LC_ALL=C sort

搜索文件夹，使用ls的风格显示结果，以第11个字段【文件名】进行排序

	find -ls | sort -k11
    
寻找"o"开头的文件，排除目录：

	find 'o' -type f

搜索我的目录下大于1MB的文件:

	find /home/ubuntu -size +1024k
    
搜索大小在1k~1M的文件：-a代表AND逻辑与，同样-o代表OR

	find . -size +1k -a -size -1024k
    
搜索空文件：

	find . -size 0
    
搜索符号链接

	find -L /var/www/ -xtype l
    
其他选项：-ctime是inode变更时间，-atime访问时间，-mtime修改时间。
    
寻找文件，类似的命令 locate，支持通配符搜索

还有个很实用的命令：type 能搜索到当前命令的别名，类型，还有指向位置

	type cd && type gcc && type ll && type foorbar
	cd is a shell builtin
    gcc is /usr/bin/gcc
    ll is alias of ‘ls -alF’
    sh: type: foorbar: could not found

还有个常用的命令：file，判断文件类型

	file /etc/passwd
    # /etc/passwd: ASCII text
    file /tmp/sogou-qimpanel-cell
    # /tmp/sogou-qimpanel-cell: socket
    file /dev/sda
    # /dev/sda: block special
    
在文件中查找字符串：(Finds string in files)

	grep -rnw '/home/ubuntu' -e "pattern_to_find"
    
## xargs

防止A命令的结果为空时，作为参数输入到B命令出错。
例如下列命令，当搜索不到结果时会使得grep报错：

	grep POSIX /dev/null $(find /usr/include -type f)
    
使用xargs防止这种错误发生，遇到空结果时候不执行grep：

	find /usr/include -type f | xargs grep POSIX /dev/null
    
## df
是Disk Free的缩写。查看磁盘分区用量

使用-h (human-readable)查看简洁的摘要：

	df -h
    #结果
    文件系统        容量  已用  可用 已用% 挂载点
    udev            3.9G  4.0K  3.9G    1% /dev
    tmpfs           788M  1.3M  786M    1% /run
    /dev/sdb5       4.8G 1020M  3.6G   22% /
    none            4.0K     0  4.0K    0% /sys/fs/cgroup
    none            5.0M     0  5.0M    0% /run/lock
    tmpfs           512M  128M  385M   25% /run/shm

类似的命令是du (Disk Usage)，告诉你的磁盘用量

	du /tmp
    # 结果
    4	/tmp/.ICE-unix
    52	/tmp/.org.chromium.Chromium.ucrnZv/html
    12	/tmp/.org.chromium.Chromium.ucrnZv/css/viewer-toc
    16	/tmp/.org.chromium.Chromium.ucrnZv/css/column
    
同样支持 -h (huaman-readable)选项。
du的原理就是深入查找文件，将每个文件空间求和输出。

## diff

比较文件，git里面也是集成了一个diff，感觉比系统自带的好用，因为有彩色显示。这里就不介绍git diff了。

使用diff的惯例：把旧文件作为参数第一个。

    # 1.txt文件
    hello world
    i am tony
    i live in China

    # 2.txt文件
    hello world
    add a line
    I am Tony
    Now I am sleepy

执行比较

	diff 1.txt 2.txt
    # 结果
    2,3c2,4     # 表示旧文件的2~3行发生变化(c符号)，变成新文件的2~4行
    < i am tony  # 左箭头表示旧文件
    < i live in China.
    ---          # 分隔新旧文件
    > add a line   # 右箭头代表新文件
    > I am Tony
    > Now I am sleepy

c代表change a代表add d代表delete。

建议使用-u参数，接近git diff的输出风格：

	diff -u 1.txt 2.txt
    # 结果
    --- 1.txt	2016-01-22 19:12:27.838666220 +0800
    +++ 2.txt	2016-01-22 19:12:41.914666098 +0800
    @@ -1,3 +1,4 @@  # 原文件的1~3行在输出结果前面3行
                     # 新文件的1~4行为输出结果是结果除去掉减号内容
     hello world
    -i am tony
    -i live in China
    +add a line
    +I am Tony
    +Now I am sleepy

## patch

执行由diff命令比较结果的改动，自动完成差异更改。通常用于程序员之间交流代码，很方便的对同一份代码进行比较修改。

	diff -c old.txt new.txt > /tmp/patch.dif
    cp old.txt /tmp
    cd /tmp
    patch < patch.dif # 这个patch直接对源文件生效，改动覆盖原文件
    cat old.txt
    
patch尽可能地套用不同之处，然后报告失败的地方，由你手动处理

## pgp

通常下载完毕，首先检验md5：
	
    md5sum ubuntu.iso
    
但是并不能保证是发布者原封不动的发布的，当黑客改动文件，并把改动后的MD5发布到网站上，也能进行欺骗，所以引入GPG签名保证文件是签名是正确的。

在更新ubutnu软件包时，开发者首先公布他的pgp公钥，然后让用户下载。

	wget -O- http://shadowsocks.org/debian/1D27208A.gpg | sudo apt-key add -
    
pgp这部分暂时没资料，不过感觉和openssl一块用进行加密是完全可以的。

## kill

PID：进程id
PPID：父进程id
相关命令： pgrep pkill trap

	pgrep netscape # 寻找netspace的PID
    pkill -HUP netspace
    pkill -TERM netspace
    pkill -KILL netspace
    # 上面三个命令不建议，因为进程名称不是唯一的。误杀。

使用kill杀死进程 pid

## trap

假设有个脚本，模拟后台运行：trap语句被触发：当后面的信号到达时

	# loop.sh
    #! /bin/sh
    trap 'echo ignoring HUP' HUP
    trap 'echo Terminating on USR1; exit 1' USR1
    
    while true; do
    	sleep 2
        date > /dev/null
    done
    
执行：

	sh ./boop.sh &
    # 24444
    kill -HUP 24444
    # Ingoring HUP
    kill -USR1
    # Terminating on USR1
    [1] + Done(1)
    
试试向他发出其他信号：

	sh ./loop.sh &
    # 22222
    kill -CHLD 22222
    jobs
    [1] + Running sh ./loop.sh &
    kill -PIPE 22222
    [1] + Broken Pipe ./loop.sh &
    
原来一个broken pipe信号就杀死他了。。
试一下默认的kill信号：

	sh ./loop.sh &
    # 11111
    kill 11111   # 默认是TERM信号
    [1] + Done sh ./loop.sh &
    

经过我的无数次实验，发现使用trap被触发后，调用trap前的一条命令会被退出，然后执行trap所在那行的命令：

	# 1.sh内容
    #! /bin/bash
    trap 'echo in trap..; sleep 5; exit(0)' 2   # 信号2代表SIGINT就是Ctrl+C
    2.sh

	# 2.sh内容
    while true; do echo from 2.sh!; sleep 1; done
   

实验过程：
在A终端执行1.sh，屏幕不断输出from 2.sh，另外开一个终端B

	ps aux | grep 2.sh
    
发现有2行输出（2.sh 和 grep auto 2.sh），在A终端按Ctrl+C，触发trap命令。接下来5秒时间内，可以在终端B执行上面这句ps aux发现已经没有2.sh的进程了。证明了我的猜想。


## umask

umask值一共有4组数字，其中第1组数字用于定义特殊权限，我们一般不予考虑，与一般权限有关的是后3组数字。

默认情况下，对于目录，用户所能拥有的最大权限是777；对于文件，用户所能拥有的最大权限是目录的最大权限去掉执行权限，即666。因为x执行权限对于目录是必须的，没有执行权限就无法进入目录，而对于文件则不必默认赋予x执行权限。

对于root用户，他的umask值是022。

当root用户创建目录时，默认的权限就是用最大权限777去掉相应位置的umask值权限，即对于所有者不必去掉任何权限，对于所属组要去掉w权限，对于其他用户也要去掉w权限，所以目录的默认权限就是755

当root用户创建文件时，默认的权限则是用最大权限666去掉相应8421权重的umask值，即文件的默认权限是644

Q:如果将umask值设为0003，那么此时创建的目录或文件的默认权限是多少？

A:正确的结果应该是：目录的默认权限是774，文件的默认权限是664。在计算默认权限时，不是用十进制的最大权限直接减去umask值，要以二进制考虑。将umask值所对应的相应位置的权限去掉，这样才能得到正确的结果。
