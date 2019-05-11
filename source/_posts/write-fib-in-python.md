title: 用Python实现Fibonacci函数
date: 2015-09-03 19:45:06
tags: [python, 转载]
categories: 编程
---
不同的角度，不同的玩法
用九种不同的风格写出一个Fibonacci函数。
要求很简单，输入n，输出第n个Fibonacci数，n为正整数
<!-- more -->
下面是这九种不同的风格：

## 第一次写程序的Python程序员

    def fib(n):
         return nth fibonacci number
         
说明：
第一次写程序的人往往遵循人类语言的语法而不是编程语言的语法，就拿我一个编程很猛的哥们来说，他写的第一个判断闰年的程序，里面直接是这么写的：如果year是闰年，输出year是闰年，否则year不是闰年。

## 刚学Python不久的的C程序员
 
    def fib(n):
        if n<=2:
            return 1
        else:
            return fib(n-1)+fib(n-2)

 
说明：
在刚接触Python时，用缩进而非大括号的方式来划分程序块这种方式我是很不适应的，而且每个语句后面没有结束符，所以每次写完一个Python函数之后干的第一件事一般就是一边注释大括号，一边添加漏掉的冒号。
 
 

## 懒散的Python程序员
 
    def fib(n):
        return 1 and n<=2 or fib(n-1)+fib(n-2)
 
说明：
看了Learning Python之后，才知道Python没有三元操作符？，不过鉴于Python里bool值比较特殊（有点像C，非零即真，非空即真），再加上Python的逻辑语句也是支持短路求值（Short-Circuit Evaluation）的，这就可以写出一个仿？语句出来。
 
 

## 更懒的Python程序员
 
    fib=lambda n:1 if n<=2 else fib(n-1)+fib(n-2)
 
说明：
lambda关键字我曾在C#和Scheme里面用过，Python里面的lambda比C#里简便，并很像Scheme里的用法，所以很快就适应了。在用Python Shell声明一些小函数时经常用这种写法。
 
 

## 刚学完数据结构的Python程序员
 
    def fib(n):
        x,y=0,1
        while(n):
            x,y,n=y,x+y,n-1
        return x
 
说明：
前面的Fibonacci函数都是树形递归的实现，哪怕是学一点算法就应该知道这种递归的低效了。在这里从树形递归改为对应的迭代可以把效率提升不少。
Python的元组赋值特性是我很喜欢的一个东东，这玩意可以把代码简化不少。举个例子，以前的tmp=a;a=b;b=tmp;可以直接用一句a,b=b,a实现，既简洁又明了。
 
 

## 正在修SICP课程的Python程序员
 
    def fib(n):
        def fib_iter(n,x,y):
            if n==0 : return x
            else : return fib_iter(n-1,y,x+y)
        return fib_iter(n,0,1)
 
说明：
在这里我使用了Scheme语言中很常见的尾递归（Tail-recursion）写法。Scheme里面没有迭代，但可以用不变量和尾递归来模拟迭代，从而实现相同的效果。不过我还不清楚Python有没有对尾递归做相应的优化，回头查一查。
PS：看过SICP的同学，一眼就能看出，这个程序其实就是SICP第一章里的一个例子。
 
 

## 好耍小聪明的Python程序员
 
    fib=lambda n,x=0,y=1:x if not n else f(n-1,y,x+y)
 
说明：
基本的逻辑和上面的例子一样，都是尾递归写法。主要的区别就是利用了Python提供的默认参数和三元操作符，从而把代码简化至一行。至于默认参数，学过C++的同学都知道这玩意，至于C#4.0也引入了这东东。
 
 

## 刚修完线性代数的Python程序员
 
    def fib(n):
        def m1(a,b):
            m=[[],[]]
            m[0].append(a[0][0]*b[0][0]+a[0][1]*b[1][0])
            m[0].append(a[0][0]*b[0][1]+a[0][1]*b[1][1])
            m[1].append(a[1][0]*b[0][0]+a[1][1]*b[1][0])
            m[1].append(a[1][0]*b[1][0]+a[1][1]*b[1][1])
            return m
        def m2(a,b):
            m=[]
            m.append(a[0][0]*b[0][0]+a[0][1]*b[1][0])
            m.append(a[1][0]*b[0][0]+a[1][1]*b[1][0])
            return m
        return m2(reduce(m1,[[[0,1],[1,1]] for i in range(n)]),[[0],[1]])[0]
 
说明：
这段代码就不像之前的代码那样清晰了，所以先介绍下原理（需要一点线性代数知识）：
首先看一下之前的迭代版本的Fibonacci函数，很容易可以发现存在一个变换：

    y->x, x+y->y
    
换一个角度，就是

	[x,y]->[y,x+y]
    
    
在这里，我声明一个二元向量[x,y]T，它通过一个变换得到[y,x+y]T，可以很容易得到变换矩阵是[[1,0],[1,1]]，也就是说：

	[[1,0],[1,1]]*[x,y]T=[y,x+y]T
    
令二元矩阵A=[[1,0],[1,1]]，二元向量x=[0,1]T，容易知道Ax的结果就是下一个Fibonacci数值，即：

	Ax=[fib(1),fib(2)]T
    
亦有：

	Ax=[fib(2),fib(3)]T
	......

以此类推，可以得到：

	Aⁿx=[fib(n),fib(n-1)]T
    
也就是说可以通过对二元向量[0,1]T进行n次A变换，从而得到[fib(n),fib(n+1)]T，从而得到fib(n)。
 
在这里我定义了一个二元矩阵的相乘函数m1，以及一个在二元向量上的变换m2，然后利用reduce操作完成一个连乘操作得到Aⁿx，最后得到fib(n)。
 
 

## 准备参加ACM比赛的Python程序员
 
    def fib(n):
        lhm=[[0,1],[1,1]]
        rhm=[[0],[1]]
        em=[[1,0],[0,1]]
        #multiply two matrixes
        def matrix_mul(lhm,rhm):
            #initialize an empty matrix filled with zero
            result=[[0 for i in range(len(rhm[0]))] for j in range(len(rhm))]
            #multiply loop
            for i in range(len(lhm)):
                for j in range(len(rhm[0])):
                    for k in range(len(rhm)):
                        result[i][j]+=lhm[i][k]*rhm[k][j]
            return result

        def matrix_square(mat):
            return matrix_mul(mat,mat)
        #quick transform
        def fib_iter(mat,n):
            if not n:
                return em
            elif(n%2):
                return matrix_mul(mat,fib_iter(mat,n-1))
            else:
                return matrix_square(fib_iter(mat,n/2))
        return matrix_mul(fib_iter(lhm,n),rhm)[0][0]
 
说明：
看过上一个fib函数就比较容易理解这一个版本了，这个版本同样采用了二元变换的方式求fib(n)。不过区别在于这个版本的复杂度是lgn，而上一个版本则是线性的。
这个版本的不同之处在于，它定义了一个矩阵的快速求幂操作fib_iter，原理很简单，可以类比自然数的快速求幂方法，所以这里就不多说了。
 
Python是最好的初学编程入门语言，没有之一。所以它可以取代Scheme成为MIT的计算机编程入门语言。

[原文地址](http://www.cnblogs.com/figure9/archive/2010/08/30/1812927.html)