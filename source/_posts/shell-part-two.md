title: Shell学习：基本语法
date: 2016-01-17 14:00:26
tags: [ubuntu, shell]
categories: 读书笔记
---
在学习和工作中shell的使用是相当普遍。为了跟上开源时代的步伐，我也得抽时间进行基础学习。
<!-- more -->

|书名|作者|下载地址|
|--|--|--|
|Classic Shell Scripting|Arnold R. & Nelson H.F.B|[暂无]()|
|Advanced Bash-Scripting Guide|Mendel Cooper|[PDF](http://www.tldp.org/LDP/abs/abs-guide.pdf)|

shell主流版本有几个：sh bash csh ksh等，有一定的区别，我使用ubuntu默认就是bash，可以在脚本第一行加上这个"shebang"指示shell用到哪个版本

	#~ /bin/bash
    
使用 chmod a+x this.sh 加上可执行属性，就可以 ./this.sh 运行该脚本。

Unix特殊字符有如下：使用其时要进行转义

 	( ; $ ? & * () [] ` ‘ “ +　


## 变量

### POSIX内置的shell变量

|名称|说明|
|---|---|
|#|目前进程的参数个数|
|@|传递给当前进程的命令行参数，若置于双引号内，会展开为个别的参数|
|*|当前进程的的命令行参数。若置于双引号内，会展开为一个单独的参数|
|-|(连字号) 在引用时给予shell的选项|
|?|前一命令的退出状态|
|$|shell的进程编号|
|0|Shell程序的名称|
|!|最后一个后台命令的进程编号|
|IFS|内部的字段分隔器，例如读取/etc/passwd可以用|
|PWD|当前工作目录|

### 变量的赋值与修改
shell的默认赋值是字符串赋值。比如：

    var=1 
    var=$var+1 
    echo $var 
    
打印出来的不是2而是1＋1。为了达到我们想要的效果有以下几种表达方式：

	let "var+=1" #let表示数学运算
	var=$[$var+1] #$[]将中括号内的表达式作为数学运算先计算结果再输出
	var=`expr $var + 1` #expr用于整数值运算，注意加号两边的空格，否则还是按照字符串的方式赋值。 

若要修改字符串

    is_valid="False"
    is_valid=$"True"
    echo is_valid

### expr运算符

expr运算符设计得不是那么严谨，而且很难用，主要用于以下四则混合算术运算：
*加 减 乘 除 取mod*
expr也是一个手工命令行计数器。

    echo $(expr 30 / 3 / 2) #注意运算符左右都有空格 ，如果没有空格表示是字符串连接
    echo $(expr 1500 + 900)
    echo $(expr 30 \* 3) #使用乘号时，必须用反斜线转义
    
将输入数据转成数字

    read a
    a=`expr $a`

### 字符串替换

替换运算符：

|名称|说明|常用场合|
|---|---|---|
|${varname:-word}|若varname存在且不是null，返回值，否则返回word|若count未被定义，则${count:-0}返回0|
|${varname:=name}|若varname存在且不是null，返回他的值，否则设置为word，返回该值|若count未被定义，则${count:=0}设置为0|
|${varname:?message}|若varname存在且非null，返回他的值，否则显示message，并退出脚本。|捕捉由于未定义变量所导致的错误 ${count:?"undefined!"}显示错误|
|${varname:+word}|若varname存在且非null，返回word，否则返回null|测试变量存在:若count已定义，${count:+1}|

### 模式匹配运算符
使用wildcard通配符进行匹配。
假设测试字符串为：/home/tolstoy/mem/long.file.name

|名称|说明|
|--|--|
|${variable#pattern}|若匹配变量值开头处，则删掉匹配最短部分。返回剩下的部分|
|${varialbe##pattern}|若模式匹配变量的开头处，则删除匹配最长部分，返回剩下部分|
|${variable%pattern}|若模式匹配变量结尾处，则删掉匹配最短部分。返回剩下的部分|
|${variable%%pattern}|若模式匹配变量结尾处，则删除匹配最长部分，返回剩下部分|
	
    测试：${path#/*/} 结果/tolstoy/mem/long.file.name
    测试：${path##/*/} 结果long.file.name
    测试：${path%.*} 结果/home/tolstoy/mem/long.file
    测试：${path%.*} 结果/home/tolstoy/mem/long
    
模式匹配记忆方法：
\# 是去掉左边(在键盘上#在$之左边)
% 是去掉右边(在键盘上%在$之右边)
\#和%中的单一符号是最小匹配，两个相同符号是最大匹配。

### 删除
删除变量
unset -v variable
删除函数
unset -f function

## 逻辑语句
查看文档man test确定更复杂的表达式

### for语句
1~9循环：

    for ((i=1;i<10;++i))
    do
        echo $i
    done

打印每一行：

    for i in read*.xml
    do
        echo $i
        mv $i /tmp
    done
    
嵌套循环中，break 命令后面还可以跟一个整数，表示跳出第几层循环

	break 2 # jump out of loop 2
    
### while语句

类似接下来的if语句，循环1~9

    i=1
    while [ $i -lt 10 ]
    do
        echo $i
        ((i++))
    done

与while循环相反的是until，只要conditon尚未满足，一直循环

    #等待某人登陆
    until who | grep "$user" > /dev/null
    do
        sleep 20
    done

### if语句
考虑到兼容性，所有变量都应该用引号括起来。

    if [ -f "$file" ] && [ -w "$file" ]
    # 上面的&&要是否需要紧挨者方括号?
    then
        # $file存在且为一般文件，但不可写入
        echo $0: $file is not writable >&2
    elif [ -f "$file_1" ]
    then
    	echo bad
    else
    	echo exit.
    fi
    
上面的>&2为输出到文件描述符，
0:stdin
1:stdout
2:stderr

很奇葩的i+1

    i=1
    if [ $i -le $((i+1)) ]; then
        echo hello
    fi

### case语句
每次总是漏掉最后的esac

    case $1 in
    -f) 
        # do something
        ;;
    -d)
        # do something
        ;;
    *)
        echo unknown
        exit 1
    esac

其中;;不是必须的，不过是习惯,也方便编辑器的自动缩进。

## 输入输出
read可以读取多个变量

    x=abc
    printf "x is '%s' now, Enter a new value:" $x
    read x
    echo $x

使用重定向读取username和uid

    while IFS=: read user pass uid gid fullname homedir shell
    do
        echo "${user}"\'s uid is "${uid}"
    done < /etc/passwd

上面使用了Shell变量IFS，能进行自动切割。使用read读取多个值，然后打印出来

一个拷贝目录结构的例子，对应这篇文章的新建目录

    find /tmp/test -type d -print |
    sed 's;/tmp/test/;/home/test/;' |
    while read newdir
    do
        mkdir ${newdir}
    done


读取磁盘使用量，发送邮件给耗资源大户

    cd /home
    du -s * | # 获取磁盘使用量
    sort -nr | # 按数字排列
    sed 10q | #排序前十名
    while read amount name
    do
        mail -s "Disk Waring" ${name} <<- EOF  # 这里的EOF前面有横线
             pleas empty your disk space!
             EOF
    done

注意EOF前面有个-符号，可以自动删掉前导的TAB字符，但无法删掉空格。
若去掉-，则实际文本中含有前导空格。

## 命令替换
反引号backticks（就是esc按键下面的的那个符号），表示返回计算值给上级
当出现多级嵌套很混乱。通常使用$(COMMANDS)代替

比较以下命令的异同

    echo seq 1 10
    echo `seq 1 10`
    echo $(seq 1 10) # 建议使用这个


比较两个文件新旧版本的异同

    for i in $(cd /tmp/old; echo *.c)
    do
        diff -c /tmp/old/$i $i | less
    done


将less移到外部，试试跟上面执行结果有什么不一样

    for i in $(cd /tmp/old; echo *.c)
    do
        diff -c /tmp/old/$i $i
    done | less

## 括号总结
### 圆括号

单小括号 ()
- 命令组。括号中的命令将会新开一个子shell顺序执行，所以括号中的变量不能够被脚本余下的部分使用。括号中多个命令之间用分号隔开，最后一个命令可以没有分号，各命令和括号之间不必有空格。
- 命令替换。等同于\`cmd\`，shell扫描一遍命令行，发现了$(cmd)结构，便将$(cmd)中的cmd执行一次，得到其标准输出，再将此输出放到原来命令。有些shell不支持，如tcsh。
- 用于初始化数组,空格隔开。如：array=(a b c d)

双小括号 (( ))
- 整数扩展。这种扩展计算是整数型的计算，不支持浮点型。((exp))结构扩展并计算一个算术表达式的值，如果表达式的结果为0，那么返回的退出状态码为1，或者 是"假"，而一个非零值的表达式所返回的退出状态码将为0，或者是"true"。若是逻辑判断，表达式exp为真则为1,假则为0。
- 只要括号中的运算符、表达式符合C语言运算规则，都可用在$((exp))中，甚至是三目运算符。作不同进位(如二进制、八进制、十六进制)运算时，输出结果全都自动转化成了十进制。如：echo $((16#5f)) 结果为95 (16进位转十进制)
- 单纯用 (( )) 也可重定义变量值，比如 a=5; ((a++)) 可实现c语言中的a\+\+
- 常用于算术运算比较，双括号中的变量可以不使用$符号前缀。括号内支持多个表达式用逗号分开。 只要括号中的表达式符合C语言运算规则,比如可以直接使用for((i=0;i<5;i++)), 如果不使用双括号, 则为for i in \`seq 0 4\`或者for i in {0..4}。再如可以直接使用if (($i<5)), 如果不使用双括号, 则为if [ $i -lt 5 ]。

例子

    for i in $(seq 0 4);do echo $i;done    
    for i in `seq 0 4`;do echo $i;done    
    for ((i=0;i<5;i++));do echo $i;done    
    for i in {0..4};do echo $i;done   

### 方括号
单中括号 []
- bash 的内部命令，左括号[和test是等同的，右中括号]是关闭条件判断
- Test和[]中可用的比较运算符只有==和!=，两者都是用于字符串比较的，不可用于整数比较，整数比较只能使用-eq，-gt这种形式。无论是字符串比较还是整数比较都不支持大于号小于号。如果实在想用，对于字符串比较可以使用转义形式，如果比较"ab"和"bc"：[ ab \< bc ]，结果为真，也就是返回状态为0。[ ]中的逻辑与和逻辑或使用-a 和-o 表示。
- 字符范围。用作正则表达式的一部分，描述一个匹配的字符范围。作为test用途的中括号内不能使用正则。
- 在一个array 结构的上下文中，中括号用来引用数组中每个元素的编号。${array[3]}

双中括号[[ ]]
- [[是 bash 程序语言的关键字。并不是一个命令，[[ ]] 结构比[ ]结构更加安全，但是有的shell不支持。在[[和]]之间所有的字符都不会发生文件名扩展或者单词分割，但是会发生参数扩展和命令替换。
- 支持字符串的模式匹配，使用=~操作符时甚至支持shell的正则表达式。字符串比较时可以把右边的作为一个模式，而不仅仅是一个字符串，比如[[ hello == hell? ]]，结果为真。[[ ]] 中匹配字符串或通配符，不需要引号。
- 使用[[ ... ]]条件判断结构，而不是[ ... ]，能够防止脚本中的许多逻辑错误。比如，&&、||、<和> 操作符能够正常存在于[[ ]]条件判断结构中，但是如果出现在[ ]结构中的话，会报错。比如可以直接使用if [[ $a != 1 && $a != 2 ]], 如果不适用双括号, 则为if [ $a -ne 1] && [ $a != 2 ]或者if [ $a -ne 1 -a $a != 2 ]。
- bash把双中括号中的表达式看作一个单独的元素，并返回一个退出状态码。

例子

    if ($i<5)    
    if [ $i -lt 5 ]    
    if [ $a -ne 1 -a $a != 2 ]    
    if [ $a -ne 1] && [ $a != 2 ]    
    if [[ $a != 1 && $a != 2 ]]    


### 大括号（花括号）
一、{}常规用法
- 大括号拓展。(通配(globbing))将对大括号中的文件名做扩展。在大括号中，不允许有空白，除非这个空白被引用或转义。第一种：对大括号中的以逗号分割的文件列表进行拓展。如 touch {a,b}.txt 结果为a.txt b.txt。第二种：对大括号中以点点（..）分割的顺序文件列表起拓展作用，如：touch {a..d}.txt 结果为a.txt b.txt c.txt d.txt
- 代码块，又被称为内部组，这个结构事实上创建了一个匿名函数 。与小括号中的命令不同，大括号内的命令不会新开一个子shell运行，即脚本余下部分仍可使用括号内变量。括号内的命令间用分号隔开，最后一个也必须有分号。{}的第一个命令和左括号之间必须要有一个空格。

二、特殊的替换（上面“变量”一节提到）

	${var:-string},${var:+string},${var:=string},${var:?string}
    
三、模式匹配（上面“变量”一节提到）

     ${var%pattern},${var%%pattern},${var#pattern},${var##pattern}

四、字符串提取和替换

|名称|说明|
|---|---|
|${var:num}|在var中提取第num个字符到末尾的所有字符。若num为正数，从左开始；若num为负数，从右开始|
|${var:pos:len}|pos是位置，len是长度。表示从$var字符串的第$pos个位置开始提取长度len子串|
|${var/pattern1/pattern2}|var字符串的第一个匹配的pattern1替换为pattern2|
|${var//pattern/pattern}|var字符串的所有匹配的pattern1替换为pattern2|

### 符号$后的括号
常见几种
- ${a} 变量a的值, 在不引起歧义的情况下可以省略大括号
- $(cmd) 命令替换，和\`cmd\`效果相同，某些Shell版本不支持$(cmd)
- $((expression)) 和\`expr expression\`效果相同, 计算数学表达式exp的数值, 其中exp只要符合C语言的运算规则即可, 甚至三目运算符和逻辑表达式都可以计算。

### 括号多命令执行
（1）单小括号，(cmd1;cmd2;cmd3) 新开一个子shell顺序执行命令cmd1,cmd2,cmd3, 各命令之间用分号隔开, 最后一个命令后可以没有分号。
（2）单大括号，{ cmd1;cmd2;cmd3;} 在当前shell顺序执行命令cmd1,cmd2,cmd3, 各命令之间用分号隔开, 最后一个命令后必须有分号, 第一条命令和左括号之间必须用空格隔开。
（3）对{}和()而言, 括号中的重定向符只影响该条命令， 而括号外的重定向符影响到括号中的所有命令。

## 执行顺序与eval

这部分需要参考书籍《Classic Shell Scripting》了，无法用Markdown写成日志。
大致就是这个意思：将字符串转成shell执行的正常解析命令

例子：

	listfile="ls | less"
    $listfile
	# 结果： 
    # ls: 无法访问|: 没有那个文件或目录
    # ls: 无法访问less: 没有那个文件或目录
    
说明shell执行字符串时候把“|” 和 “less” 看成ls的参数了，使得ls尝试在目录寻找 | 和 less 文件。。。
使用eval执行顺序可以重新解析成shell命令。

	eval $listpage
    
## 其他

getopts shift $# 提供处理命令行的工具：自行查阅相关文档