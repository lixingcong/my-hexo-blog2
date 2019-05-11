title: C/C++笔记
date: 2016-01-03 15:44:00
tags: cpp
categories: 编程
---
菜鸟笔记，私房菜。
<!-- more -->

### 外部变量
原帖：[外部变量](http://blog.csdn.net/traceorigin/article/details/8870576)
#### 方法一

实现从test2访问test1的变量

    //test1.c
    int COMMON_VALUE = 100;

    //test2.c
    extern int COMMON_VALUE;

如果test1.c中定义为静态变量，则在test2.c是无法访问


    //test1.c
    static int COMMON_VALUE = 100;
    
#### 方法二

假设有这样几个文件commonDef.h, test1.c, test2.c
其中test1.c和test2.c包含公用的头文件commonDef.h

    //commonDef.h
    #ifndef __COMMON_DEF_H
    #define __COMMON_DEF_H
    static int COMMON_VALUE = 100;
    #endif
    
static关键字保证了包含该头文件的编译单元分别拥有该变量的独立实体，如同在每一个源文件分别定义一次；若不加static，则会造成“redifinition”。
这时候在test1.c和test2.c直接#include"commonDef.h"即可。

#### 方法三

    //commonDef.h
    #ifndef __COMMON_DEF_H
    #define __COMMON_DEF_H
    extern int COMMON_VALUE;
    #endif

    //test1.c
    #include "commonDef.h"
    int COMMON_VALUE = 100;

    //test2.c
    #include "commonDef.h"

也即在公用头文件中，使用extern关键字。然后再某一个源文件中定义一次，再在每一个使用它的编译单元中包含该头文件即可。
