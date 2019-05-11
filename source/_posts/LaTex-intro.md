title: LaTex数学公式语法
date: 2016-04-04 15:05:54
tags: [html, LaTex]
mathjax: true
categories: 网络
---
如果用写代码的方式创造公式的话，那就可以装逼了！本文是我翻译自[LaTex数学符号手册](https://www.artofproblemsolving.com/wiki/index.php/LaTeX:Symbols)
<!-- more -->
在浏览器中使用Latex，使用Mathjax库

> MathJax是一套开源js库，它并不是将公式转化为一个图片，而是在浏览器端通过位置控制、字体、大小来显示一个公式。支持LaTeX数学公式。

对于hexo博客框架：首先更改next主题中的mathjax库的URL，MathJax2.4版有些许bug：显示尾随的竖线。

	vi themes/next/layout/_scripts/third-party/mathjax.swig
	从
    if config.language === 'zh-Hans'
    到配对的endif标签
    替换为固定的js链接：
    <script type="text/javascript" src="https://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML"></script>

## 符号定义

### 操作符

#### 加减乘除

|加|语法|减|语法|乘|语法|除|语法|
|---|---|---|---|---|---|---|---|
|$+$|+|$-$|-|$\times$|\times|$\div$|\div|

#### 幂运算

|符号|语法|符号|语法|符号|语法|符号|语法|
|---|---|---|---|---|---|---|---|
|$a^x$|a^x|$a^{xyz}$|a^{xyz}|$\sqrt{x}$|\sqrt{x}|$\sqrt[n]{x}$|\sqrt[n]{x}|

#### 逻辑运算

|异或|语法|或|语法|与|语法|
|---|---|---|---|---|---|
|$\oplus$|\oplus|$\vee$|\vee|$\wedge$|\wedge|

#### 绝对值

直接插入竖线|即可，使用\left和\right标签指定竖线的垂直长度。

$\left| \sum_i \vec{v}_i \left|\Delta t_i\right|  \right|$

	代码：\left| \sum_i \vec{v}_i \left|\Delta t_i\right|  \right|

#### 分式

参考本文下方的“分式”一节，这里简单说说语法

$(\frac{a}{x} )^2$  语法：(\frac{a}{x} )^2

上面的表达式效果是：右边的上标没有对齐，因此可以改进一下：

$\left(\frac{a}{x} \right)^2$ 语法：\left(\frac{a}{x} \right)^2

#### 其他运算
详见下文操作

|求和|语法|积分|语法|积分|语法|求积|语法|
|---|---|---|---|---|---|---|---|
|$\sum$|\sum|$\int$|\int|$\oint$|\oint|$\prod$|\prod|

### 关系

#### 判等

|符号|语法|符号|语法|符号|语法|符号|语法|
|---|---|---|---|---|---|---|---|
|$=$|=|$\not=$|\not=|$\approx$|\approx|$\equiv$|\equiv|
|$<$|<|$>$|>|$\le$|\le|$\ge$|\ge|
|$\ll$|\ll|$\gg$|\gg|

#### 集合

|符号|语法|符号|语法|
|---|---|---|---|
|$\in$|\in|$\ni$|\ni|
|$\subset$|\subset|$\supset$|\supset|
|$\subseteq$|\subseteq|$\supseteq$|\supseteq|

#### 存在

|符号|语法|符号|语法|
|---|---|---|---|
|$\exists$|\exists|$\forall$|\forall|

### 希腊字母

直接根据拼写输入即可，小写共有22个，大写8个：大写的是小写语法首字母大写。不全部列出了。

||符号|语法|符号|语法|符号|语法|
|-|---|---|---|---|---|---|
|小写|$\alpha$|\alpha|$\beta$|\beta|$\gamma$|\gamma|
|大写|$\Phi$|\Phi|$\Omega$|\Omega|$\Delta$|\Delta|

### 箭头

|符号|语法|符号|语法|符号|语法|符号|语法|符号|语法|
|---|---|---|---|---|---|---|---|---|---|
|$\gets$|\gets|$\to$|\to|$\Leftarrow$|\Leftarrow|$\Rightarrow$|\Reftarrow|$\Leftrightarrow$|\Leftrightarrow|

### 省略号

|符号|语法|符号|语法|
|---|---|---|---|
|$\dots$|\dots|$\cdots$|\cdots|

### 头顶符号

|符号|语法|符号|语法|符号|语法|
|---|---|---|---|---|---|
|$\hat{x}$|\hat{x}|$\bar{x}$|\bar{x}|$\vec{x}$|\vec{x}|
|$\dot{x}$|\dot{x}|$\ddot{x}$|\ddot{x}|

### 括号

#### 标准括号

|符号|语法|符号|语法|符号|语法|符号|语法|
|---|---|---|---|---|---|---|---|
|$($|(|$)$|)|$[$|[|$]$|]|


#### 取整函数

|符号|语法|符号|语法|
|---|---|---|---|
|$\lfloor$|\lfloor|$\rfloor$|\rfloor|
|$\lceil$|\lceil|$\rceil$|\rceil|

### 空格
LaTex默认是省略空格，要输入空格就得自己输入命令，mu是一个数学单位。

|效果|说明|语法|
|--|--|--|
|$a \quad b$|空格宽度是当前字宽(18mu)|\quad|
|$a \, b$|空格宽度是3mu|\,|
|$a \: b$|空格宽度是4mu|\:|
|$a \; b$|空格宽度是5mu|\;|
|$a \! b$|空格宽度是-3mu(向左缩)|\\!|
|$a \ b$|空格宽度是标准空格键效果|在\后面敲一个空格|
|$a \qquad b$|空格宽度是36mu|\qquad|


### 其他符号

|符号|语法|符号|语法|符号|语法|
|---|---|---|---|---|---|
|$\infty$|\infty|$\triangle$|\triangle|$\angle$|\angle|
|$\checkmark$|\checkmark|$\nabla$|\nabla|||

## 命令

### 上标与下标

使用{..}来告诉MathJax上下表的位置在哪，是谁是大括号

|符号|语法|符号|语法|
|---|---|---|---|
|$2^2$|2^2|$a_i$|a_i|
|$2^{23}$|2^{23}|$n_{i-1}$|n_{i-1}|
|$a^{i+1}_3$|a^{i+1}_3|$x^{3^2}$|x^{3^2}|
|$2^{a_i}$|2^{a_i}|$2^a_i$|2^a_i|

### 上划线下划线

|上|语法|下|语法|
|---|---|---|---|
|$\overline{a+bi}$|\overline{a+bi}|$\underline{431}$|\underline{431}|

### 分式

有两种尺寸：frac和drac

|尺寸较小|因此适合打印|尺寸适中|用于显示器展示|
|---|---|---|---|
|符号|语法|符号|语法|
|$\frac{1}{2}$|\frac{1}{2}|$\dfrac{1}{2}$|\dfrac{1}{2}|
|$\frac{2}{x+2}$|\frac{2}{x+2}|$\dfrac{2}{x+2}$|\dfrac{2}{x+2}|
|$\frac{1+\frac{1}{x}}{3x + 2}$|\frac{1+\frac{1}{x}}{3x + 2}|$\dfrac{1+\frac{1}{x}}{3x + 2}$|\dfrac{1+\frac{1}{x}}{3x + 2}|

连续（嵌套）使用：cfrac

|符号|语法|
|---|---|
|$\cfrac{2}{1+\cfrac{2}{1+\cfrac{2}{1+\cfrac{2}{1}}}}$|\cfrac{2}{1+\cfrac{2}{1+\cfrac{2}{1+\cfrac{2}{1}}}}|

### 根式

|符号|语法|符号|语法|
|---|---|---|---|
|$\sqrt(2)$|\sqrt(2)|$\sqrt{2}$|\sqrt{2}|
|$\sqrt{x+y}$|\sqrt{x+y}|$\sqrt{x+\frac{1}{2}}$|\sqrt{x+\frac{1}{2}}|
|$\sqrt[3]{3}$|\sqrt[3]{3}|$\sqrt[n]{x}$|\sqrt[n]{x}|


### 求和 求积 求极限

|符号|语法|
|---|---|
|$\sum_{i=1}^{\infty}\frac{1}{i}$|\sum_{i=1}^{\infty}\frac{1}{i}|
|$\prod_{n=1}^5\frac{n}{n-1}$|\prod_{n=1}^5\frac{n}{n-1}|
|$\lim_{x\to\infty}\frac{1}{x}$|\lim_{x\to\infty}\frac{1}{x}|

### 求微积分 偏导

|符号|语法|
|---|---|
|$\frac{d}{dx}\left(x^2\right) = 2x$|\frac{d}{dx}\left(x^2\right) = 2x|
|$\int 2x\,dx = x^2+C$|\int 2x\,dx = x^2+C|
|$\frac{\partial^2U}{\partial x^2} + \frac{\partial^2U}{\partial y^2}$|\frac{\partial^2U}{\partial x^2} + \frac{\partial^2U}{\partial y^2}|


### 求模

$9\equiv 3 \pmod{6}$   —— 语法：9\equiv 3 \pmod{6}

### 三角函数

直接输入正常书写的符号即可，不全列举

|余弦|语法|反余弦|语法|双曲余弦|语法|
|---|---|---|---|---|---|
|$\cos$|\cos|$\arccos$|\arccos|$\cosh$|\cosh|

|符号|语法|
|---|---|
|$\cos^2 x +\sin^2 x = 1$|\cos^2 x +\sin^2 x = 1|
|$\cos 90^\circ = 0$|\cos 90^\circ = 0|
