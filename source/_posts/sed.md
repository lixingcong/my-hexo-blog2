title: shell简单的文本处理
date: 2015-10-31 22:13:37
tags: [ubuntu, shell]
categories: 读书笔记
---
使用Shell脚本处理文本很方便。通常情况下，若处理文本，一句shell脚本能解决的问题，换成python要十行代码，因此在某些情况下选择shell文本处理能提高效率。

|书名|作者|下载地址|
|--|--|--|
|Classic Shell Scripting|Arnold R. & Nelson H.F.B|[暂无]()|
|Advanced Bash-Scripting Guide|Mendel Cooper|[PDF](http://www.tldp.org/LDP/abs/abs-guide.pdf)|

## grep
打印出某行。
<!-- more -->

	ifconfig | grep 127.0.0.1

使用正则表达式BRE或者ERE匹配行：

	ifconfig | grep '\([0-9]*\)\.'
    ifconfig | grep -e '[0-9]*\.'

在文件下查找含有某个字符串的文件：（find string in files）

	grep -rnw '/home/ubuntu' -e "pattern_to_find"
        
相关文章：[正则表达式学习](/2015/10/29/RE_POSIX/)

## sed

加载多个命令需要-e参数。每一个-e后面跟者一个命令
定界符可以是逗号，也可以分号，总之不要跟替换内容重复。
启用overwrite模式，将改动写入到源文件: -i 参数
启用ERE表达式 -r 参数。但并不是完整支持ERE，比如：无法使用\d匹配数字，可以用[:alnum:]代替

++sed默认只对一行进行一次匹配。++
- 若要一行内多次匹配，要加上g，比如
		
        sed 's/hello/hi/g' test.txt
        
- 若跨行搜索，使用以下命令将所有换行符替换成逗号

		sed ':a;{N;s/\n/,/};ba'
        
上面这是一种label的用法，参考[这篇文章](http://stackoverflow.com/questions/1251999/how-can-i-replace-a-newline-n-using-sed)

备份/home/ubuntu的目录结构到/tmp/backup

	mkdir /tmp/backup
    find /home/ubuntu -type d -print |
    sed 's;/home/ubuntu;/temp/backup/;' |
    sed 's;/^/mkdir /' |
    sh -x

删掉空行

	sed '/^$/d' ./1.txt # d命令是删除整行，不是删除pattern
    
从文件加载命令

	# use global replace because in a line
	echo "s/fuck/love/g" > config  
    echo "I fuck you because I love you, So I love you and fucking you" > text
    sed -f config ./text

仅显示匹配(类似grep)

	printf "one\ntwo\nthree\nfour\none\n" > text
	sed -n '/one/p' ./text

匹配特定行
- 提及两位人名：

        printf "Tom wants to sleep.\nBut Tom has to finish task.\n" > text
        sed '/Tom/ s//& and Jerry/g' ./text
    
- 指定打印哪些行

		sed -n '10,24p' /etc/passwd
    
- 仅替换范围内的行

		# 从含games那行到proxy那行进行替换
		sed '/games/,/proxy/ s/usr/ffffffffffffff/g' /etc/passwd

- 将命令应用于不匹配的某些行

        # 不匹配games的所有行进行替换操作
        sed '/games/ !s/usr/xxxxxxxxxxxxxxxxxx/g' /etc/passwd
        
        

    
小把戏：匹配NULL (由于*表示重复0~INF次)

	echo abc | sed 's/b*/1'
    结果：1abc
    
## tr
进行替换、压缩和删除。它可以将一组字符变成另一组字符。
++注意是一组字符，不是一个字符串++

选项

    -c或——complerment：取代所有不属于第一字符集的字符
    -d或——delete：删除所有属于第一字符集的字符
    -s或--squeeze-repeats：把连续重复的字符以单独一个字符表示
    -t或--truncate-set1：先删除第一字符集较第二字符集多出的字符。
    
大写转小写

	echo "HELLO WORLD" | tr 'A-Z' 'a-z'
    
'A-Z' 和 'a-z'都是集合，集合是可以自己制定的，例如：'ABD-}'、'bB.,'、'a-de-h'、'a-c0-9'都属于集合，集合里可以使用'\n'、'\t'，可以可以使用其他ASCII字符。

将制表符转换为空格

	cat text | tr '\t' ' '

将制表符转换为空格

	cat text | tr '\t' ' ' 

字符集补集，不在补集中的所有字符被删除

	echo aa.,a 1 b#$bb 2 c*/cc 3 ddd 4 | tr -d -c '0-9 \n'
    
此例中，补集中包含了数字0~9、空格和换行符\n，所以没有被删除，其他字符全部被删除了。 

用tr压缩字符，可以压缩输入中重复的字符

	echo "thissss is a text linnnnnnne." | tr -s ' sn'
    # this is a text line. 
    
删除Windows文件“造成”的'^M'字符

	cat file | tr -s "\r" "\n" > new_file 或 cat file | tr -d "\r" > new_file
    
## cut
字段提取。
取出字段：

	cut -d : -f 1,5 /etc/passwd | head
	cut -d : -f 6 /etc/passwd | tail
    
## join
连接字段
	
    printf "#Sales\njoe\t200\njane\t300\nhreman\t150\nchris\t450\n" > text1
    printf "#Quotas\njoe\t49\njane\t54\nhreman\t20\nchris\t49\n" > text2
    # 删掉注释，然后排序
    sed '/^#/d' text1 | sort > text1.sort
    sed '/^#/d' text2 | sort > text2.sort
    # 以第一个字段作结合
    join text1.sort text2.sort

## awk
分为mawk gawk 等几个版本，用法大同小异。下面以ubuntu内置的mawk为例。
注意目前的ubuntu内置mawk版本是1.3.3，是很老的版本，bug：不支持posix正则表达式字符集，比如\[:alnum:\]，因此想要用posix，就得[更新mawk](http://invisible-island.net/mawk/)。
编译安装后要更新一下软链接。

	sudo ln -sf /usr/local/bin/mawk /usr/bin/awk

awk的一般语法

	awk pattern { action } #如果模式匹配，执行action
    awk pattern            #如果模式匹配，打印它
    awk         { action } #针对每条记录，执行action

|awk变量|说明|
|--|--|
|FILENAME|当前输入文件名称|
|FNR|输入文件的记录数|
|FS|分隔符，支持正则表达式|
|NF|当前记录的字段数|
|NR|在工作job中的记录数|
|OFS|输出字段分隔符|
|ORS|输出记录分隔符|
|RS|输入记录分隔符，默认'\n'|

++awk是在文字缓冲区逐行处理++
打印非空行：

	awk 'NF > 0 {print $0}'
    
实现cut取出字段,-F指定分隔符

	awk -F: '{print "User-name:",$1,",but real name is",$5}' /etc/passwd | head
    # 改动一下，-v设置**为输出分隔符
    awk -F: -v 'OFS=**' '{print $1,$5}' /etc/passwd | head
   
将每行的回车符替换为逗号，并将所有行合并到一行

	awk '{printf"%s,",$0}'  #没有跨行概念
    
提取中国路由表，项目[chnroute](https://github.com/fivesheep/chnroutes)的源码

	# ipv4.txt 文件内容
    apnic|CN|ipv4|43.242.220.0|1024|20140904|allocated
    apnic|IN|ipv4|43.242.228.0|1024|20140904|assigned
    apnic|HK|ipv4|43.242.232.0|1024|20140904|assigned
    apnic|MN|ipv4|43.242.240.0|1024|20140905|allocated
    apnic|CN|ipv4|43.242.252.0|1024|20140905|allocated
    # 执行
	cat ipv4.txt |
    awk -F\| '/CN\|ipv4/ { printf("%s/%d\n", $4, 32-log($5)/log(2)) }'

注意：-F为重新定义默认分隔符，匹配Pattern为正则表达式CN\|ipv4，输出分隔符分隔的字段，并进行数学对数运算

在一个列表中调换第二、第三列

	# list.txt 文件
    姓名	学号	成绩
    张三	23	23
    李四	22	49
    老王	33	66
    赵爷	44	22
    # 执行：
    awk -F'\t' -v OFS='\t' '{ print $1, $3, $2 }' list.txt
    
将单换行符的文本换成双换行：

	awk '{ print $0 "\n" }' list.txt
    # 等价于 sed -e 's/$/\n/g' list.txt
    
将双换行符的文本换成单换行：

	awk 'BEGIN { RS="\n*\n" } { print }' list.txt
    
支持类似c语言的各种逻辑语句：

	awk 'BEGIN {for(x=0;x<=1.0;x+=0.05)print x}'
    
结合Shell产生随机数：

	#! /bin/bash
    for k in $(seq 1 5); do
        awk 'BEGIN{
                   srand()
                   for(k=1;k<5;k++)
                       printf("%.5f\t",rand())
                   print ""
                  }'
        sleep 1
    done

## sort
排序。
修饰符如下：

|字母|说明|字母|说明|
|--|--|--|--|
|b|忽略开头空白|g|以一般的符号数字进行比较，仅用于GNU|
|d|字典顺序|i|忽略无法打印的字符|
|f|不分大小写|n|以整数进行比较|
|r|倒序|-|-|


还是passwd文件。这东西能玩一年

	# -t分隔符 -k表示从第几个字段到第几个字段为key
    # -k2.4,5.6表示从第二个字段的第四个字符开始比较，一直比到第五字段第六字符
	sort -t: -k1,1 /etc/passwd
    
    # -k3nr修饰符表示第三字段，按数字，反向排序
    sort -t: -k3nr /etc/passwd
    
    # 先按第四个字段进行排序，若具有相同第四字段，再以第三个字段进行排序
    sort -t: -k4n -k3n /etc/passwd
    
文本块排序：
假设text为如下内容：

    # SORTKEY: Schlob, Hans Jürgen
    Hans Jürgen Schlob
    Unter den Linden 78
    D-10117 Berlin
    Germany
    
    # SORTKEY: Jones, Adrian
    Adrian Jones
    371 Montgomery Park Road
    Henley-on-Thames RG9 4AJ
    UK
    
    # SORTKEY: Brown, Kim
    Kim Brown
    1841 S Main Street
    Westchester, NY 10502
    USA

先用awk处理标识符。再处理键值，恢复原样：

	cat text | awk -v RS="" '{gsub("\n" , "^Z"); print }' |
    sort -f |
    awk -v ORS="\n\n" '{gsub("\^Z","\n");print }' |
    grep -v 'sort'
    
排序后看看是否重复：

	printf "three\none\ntwo\nthree\nfour\none\nthree\n" > text
    # 显示去掉重复的、排序后记录
    sort latin-numbers | uniq
    # 显示计数
    sort latin-numbers | uniq -c
    # 仅显示重复记录
    sort latin-numbers | uniq -d
    # 仅显示未出现重复
    sort latin-numbers | uniq -u
    
    
## fmt
打开ubuntu的默认字典，格式化段落为30个字为一行：

	sed -n '999,1020p' /usr/share/dict/words | fmt -w 30
    
    
## wc
计数，words count
	
    echo "hello world, i am the king of the world." > text
    cat text | wc -c  #字节数
    cat text | wc -l  #行数
    cat text | wc -w  #数词