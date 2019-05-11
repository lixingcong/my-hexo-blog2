title: Makefile读书笔记
date: 2016-01-03 14:37:34
tags: ubuntu
categories: 读书笔记
---
Makefile是帮助我们进行编译的工具，可以简化流程，易于维护，编译不必依赖IDE，当源文件数量较多时候，使用Makefile更适合管理Project的开发。在开源软件中大多采用Makefile进行管理。
<!-- more -->

花时间读了两本书，讲的很明白。做个笔记

|书名|《Managing Projects with GNU Make》|《跟我一起写makefile》|
|---|---|---|
|作者|Robert Mecklenburg|陈皓|
|地址|[暂无]()|[ubuntu社区](http://wiki.ubuntu.org.cn/%E8%B7%9F%E6%88%91%E4%B8%80%E8%B5%B7%E5%86%99Makefile)|

### 入门
每一条命令都是这样格式

    targets : prerequisites
        command
        
Makefile严格规定需要Tab缩进，否则提示

	Makefile:88: *** 遗漏分隔符 。 停止。

1.显式推导那几个文件，具有相同依赖
	
    vpath.o variabl.o : make.h config.h
    上式等价于：
    vpath.o : make.h config.h
    variale.o : make.h config.h
    
2.一条规则不必写成一句话，makefile会自动添加依赖文件
3.更复杂的情况

举例
- 让lexer.c在vpath.c之前被编译
		
        vapth.o : vpath.c
        
- 用特定的参数flags编译

		vpath.o : path.c
        	$(COMPLIE) $(RULE_FLAGS) $(OUTPUT_OPTIONS) $<
        # 这是一个双向关系：
        # 1，如果lexer.c已经被编译，则编译vpath.o
        # 2，如果想更新vpath.o，首先保证lexer.c最新

4.通配符

生成一个包含所有c文件的列表

	SRC_FILES := $(wildcard *.c)
    
生成一个中间object列表，文件名为所有c文件，后缀替换*.o

	OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
		
clean目标的编写

    clean:
    	rm -f *.o

5.伪命令

若定了make clean，不巧的是当前目录下有一个clean的同名文件，会报错。
使用.PHONY指定执行的是make clean而不是./clean

	# Tells Make that 'clean' is not a file
	.PHONY: clean
    clean:
    	rm -f *.o
        
6.空目标

与伪命令相似，是一个无输出文件，并且是需要在偶然的情况下执行的命令，也不需要任何依赖关系。

    prog: size prog.o
        gcc -o $@ prog.o

    size: prog.o
        size $<
        touch size

    prog.o: prog.c
        gcc -c $<
        
功能：每次更新prog.o时候，自动打印当前文件的hex信息，并用touch命令修改文件的修改时间。

    [root@localhost]# make
    gcc -c prog.c
    size prog.o
       text    data     bss     dec     hex filename
         58       0       0      58      3a prog.o
    touch size
    gcc -o prog prog.o
    [root@localhost]#

7.变量赋值与宏

赋值有两种
- := 不会搜寻下方的变量，直接赋值
- = 递归，搜索下方的变量

区别很简单：

	CC=gcc
    CMD := $(CC) -v
    # 结果：CMD=gcc -v
    
换位置：

	CMD := $(CC) -v
    CC=gcc
    # 结果：CMD= -v
    
### 编译器别名

|名称|说明|
|--|--|
|AR|函数库打包程序。默认命令是“ar”|
|AS|汇编语言编译程序。默认命令是“as”|
|CC|C语言编译程序。默认命令是“cc”|
|CXX|C\+\+语言编译程序。默认命令是“g\+\+”|

### 标志

|名称|说明|
|--|--|
|ARFLAGS|函数库打包程序 AR 命令的参数。默认值是“rv”|
|ASFLAGS|汇编语言编译器参数。(当明显地调用“.s”或“.S”文件时)|
|CFLAGS|C 语言编译器参数。|
|CXXFLAGS|C++语言编译器参数。|
|CPPFLAGS|C 预处理器参数。( C 和 Fortran 编译器也会用到)|
|LDFLAGS|链接器参数。(如:“ld”)|
|TARGET_ARCH|结构平台，比如armv7,mips|

不建议所有标志写在一起

	FLAGS=-I project/inc --Wall

一旦执行make FLAGS=-DEBUG会覆盖标志。所以要分门别类写标志

### 自动化变量

|名称 |  说明|
|--|--|
|$@|目的文件名|
|$<|依赖列表的第一个文件|
|$^|当前规则的整个依赖列表|
|$+|类似“$^",但它保留了依赖文件中重复出现的文件。主要用在程序链接时的库的交叉引用场合|
|$*|目标文件去掉后缀后的名称|
|$%|当规则的目标文件是一个静态文件时，$%代表静态库的一个成员名|
|$>|它和$%一样只适用于库文件，它的值是库名|
|$?|所有比目标文件新的依赖文件，以空格分隔。如果目标是静态库文件，代表的库成员|

= = = = =
若在上面每一个命令后加上'F'或者'D'，可以分别返回‘目录’或者'文件名'

|名称|说明|
|--|--|
|$(@F)|目标文件的所在目录，如果$@=dir/foo.o，则返回dir|
|$(@F)|目标文件名，如果$@=dir/foo.o，则返回foo.o|
|$(^D)|整个依赖列表的所在目录，如果$^=dir/foo.c，则返回dir|
|$(^F)|整个依赖列表文件名，如果$^=dir/foo.c，则返回foo.c|

其他自动变量表达式后面加F或D类推。

### 指定查找文件夹

文件目录展示：

	../
    |--Makefile
    |--inc/
    |  |--counter.h
    |  |--lexer.h
    |--src/
    |  |--counter_words.c
    |  |--counter.c
    |  |--lexer.l
    |--README.md
    
 若make提示
 
 	No rules to make target for 'couter.c' needed by counter_words.o, Stop.
    
即找不到文件，指定VPATH（大写）即可找到文件

	VPATH=src
    
再次make提示

	no such file or dir: 'counter.h', Error 1
    
说明c文件找到了，但是inc文件没有找到。添加变量查找include即可。

	CFLAGS = -I inc
    gcc $(CFLAGS) -c $<
    
VPATH的特性：
- 空格分隔多个目录
- 若搜索过程中出现多个结果，自动摘取第一个结果，若想精确匹配：

		VPATH %.c src
        VPATH %.h inc
        VPATH %.l src
        
        
### 静态模式

避免模糊匹配，限制匹配的范围，仅匹配自己定义的几个object。

    <TARGETS ...>: <target-pattern>: <prereq-patterns ...>
      <commands>
      
我们的target-parrtern定义成“%.o”，意味着我们的TARGETS集合中必须是以“.o”结尾的。

而我们的prereq-parrterns定义成“%.c”，意思是对target-parrtern所形成的目标集进行二次定义，其计算方法是，取target-parrtern模式中的“%”（也就是去掉了.o这个结尾），并为其加上.c这个结尾，形成的新集合。

所以，我们的“目标模式”或是“依赖模式”中都应该有“%”这个字符。

举例：假设目录下有1.c, 2.c, 3.c, foo.c, bar.c五个文件，现在只需编译几个文件即可获得可执行文件

	# 注意这里objects并不需要foo.c bar.c文件，后面会自动推导
    objects = foo.o bar.o
    all: main
    main: $(objects)
    	$(CC) %^ -o $@
    $(objects): %.o: %.c
		$(CC) -c $(CFLAGS) $< -o $@
        
我们的目标从objects中获取，%.o表明要所有以.o结尾的目标，也就是foo.o bar.o，也就是变量object集合的模式

而依赖模式%.c则取模式%.o的%，也就是foo bar，并为其加下.c的后缀

于是，我们的依赖目标就是“foo.c bar.c”。于是，上面的规则展开后等价于下面的规则：

	main: $(objects)
    	$(CC) $^ -o main
    foo.o : foo.c
    	$(CC) -c $(CFLAGS) foo.c -o foo.o
    bar.o : bar.c
    	$(CC) -c $(CFLAGS) bar.c -o bar.o
        
### 自动更正依赖关系

查看当前依赖文件的关系：

	echo "#include <stdio.h>" > 1.c
    gcc -M 1.c
    
发现依赖很多文件，所以要自动添加依赖到Makefile中。
make中可以读取每个源文件的依赖文件*.d，然后以*.d为工作目标加入依存规则，这样，当源文件被改变时，make就会知道更新该.d文件以及目标文件。
假设$(SOURCES)=main.c hello.c
    
然后使用以下规则产生依赖，参考sed的用法

	%d : %.c
        $(CC) -MM $< $(CFLAGS) > $@.$$$$; \
        sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
        rm -f $@.$$$$

在Makefile编写的依存关系后添加一句，实现把sources里面的字符串从.c转为.d字符串
include前面有个-号，用途是禁用include警告，可以去掉看看效果。

	-include $(subst .c,.d,$(SOURCES))
    

### 应用例子

#### 简单的练习

(所有文件都放在相同文件夹中)
注意提示“遗漏分隔符”，即为缺少Tab缩进，GNU规定必须使用TAB分隔而不是四个空格。
main.c中的内容：

	#include <stdio.h>
    #include "hello.h"
    main()
    {
		print_hello();
    }
    
hello.c中的内容：

	#include "hello.h"
    void print_hello()
    {
    	printf("hello_world\n");
    }

hello.h中的内容：

    #ifndef _HELLO_H_
    #define _HELLO_H_
    #include <stdio.h>
    void print_hello();
    #endif

Makefile中的内容：

    CC=gcc
    CFLAG=-I.
    CFLAGS += -Wall -Werror -Wmissing-prototypes
    SRC=hello.c main.c
    OBJS=hello.o main.o

    all:$(OBJS)
        $(CC) $(CFLAG) $^ -o main
        
    $(OBJS):$(SRC)
        $(CC) -c $^ $(CFLAG)

    .PHONY:clean
    clean:
        rm *.o
        rm main
        
#### 链接静态库的例子

首先区分一下静态库和动态库区别

Shared libraries are .so (or in Windows .dll, or in OS X .dylib) files. 

> All the code relating to the library is in this *.so file, and it is referenced by programs using it at run-time. A program using a shared library only makes reference to the code that it uses in the shared library.

Static libraries are .a (or in Windows .lib) files. 

> All the code relating to the library is in this file, and it is directly linked into the program at compile time. A program using a static library takes copies of the code that it uses from the static library and makes it part of the program. [Windows also has .lib files which are used to reference .dll files, but they act the same way as the first one].

在做到斯坦福公开课Week 5的课程编程作业时，用到了C的一个大数库GMP（就是相当于Java的BigNum类），使用Eclipse编译必须手写Makefile，所以有了下面这个例程。
注意链接静态库时什么时候使用-L什么时候使用-l

使用静态库方法

1. 使用路径，如

	g++ test.o ./libtest.a -o test.out
  
2. 使用 -L 设置文件路径，-l 代表库名，例文件为 libtest.a 则参数为 -ltest

	g++ test.o -L./ -llog -L/usr/local/lib -lboost_thread -o test.out

但是：使用 -L -l 会带来一个问题，该方式不指定链接库类型，即静态or动态（.a or .so），且优先链接动态库。
因为我电脑已经下载了libgmp的源码并且编译安装到/usr/local/lib了，如果找不到这个库地址可以临时导出linker的变量：

	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

目录结构：

    ../
    |--Makefile
    |--src/
       |--main.cpp
    
    /usr/local/lib
        |--libgmp.a
        |--libgmp.a
        |--libgmp.la
        |--libgmp.so
        |--libgmp.so.10
    
    /usr/local/include/
        |--gmp.h
        
main.cpp内容：

    #include <iostream>
    #include <gmp.h>
    using namespace std;
    int main() {
        char result[200];
        mpz_t a, b, res;
        mpz_init(res);
        mpz_init_set_str(a,"123456789123456789123456789",10);
        mpz_init_set_str(b,"100000000000000000", 10);
        mpz_sub(res, a, b);
        mpz_get_str(result, 10, res);
        cout << result << endl;
        return 0;
    }
    
Makefile内容：

    CC=g++
    VPATH = src
    LIBS = -lgmp 

    SRC=main.cpp
    OBJS=main.o 

    all:$(OBJS)
        $(CC) $^ $(LIBS) -o main

    $(OBJS):$(SRC)
        $(CC) -c $(LIBS) $^ -o $@

    .PHONY:clean
    clean:
        rm *.o
        rm main