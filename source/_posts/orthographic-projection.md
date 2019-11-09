---
title: 三维空间正交投影二维空间
date: 2019-11-09 16:42:15
tags: [翻译, 3D, cpp]
categories: [读书笔记]
mathjax: true
---
最近做的投影参考了这篇[博客文章](http://www.codinglabs.net/article_world_view_projection_matrix.aspx)，也给出测试样例代码：三维空间某点(x,y,z)正交投影到某个面上得到(x1,y1)矩阵相乘算法。本博客是根据自己理解做简单翻译，当作是最基础的3D入门demo。
<!-- more -->

# 简介

本文让读者简单了解如何将三维空间的坐标投影到二维坐标上：使用一种连续把几个变换矩阵相乘的方法。本文是完全面向3D初学者，预备知识：假设读者已经对矩阵运算有一定的了解。

本文安排如下：先展示的是变换矩阵与向量空间的关系，然后举例使用多个变换矩阵相乘的结果展示Camera视角的变换，即从工件坐标系Model到世界坐标系World的变换，最后是使用相机计算出投影后的二维坐标。

# 向量空间：工件与世界

标准的右手坐标系为伸出右手，摊开五指，掌心朝向自己，大姆指指向的是X正，食指指向Y正，掌心指向Z正。如图1所示。

![figure1](/images/projection/figure1.png)

假设我们要在CAD软件内创建一个茶壶，要以茶壶内的某点作为原点进行绘图，我们可以将茶壶的中心作为(0,0,0)。那么这个新创建的坐标系称之为工件坐标系（Model Space）。如图2所示，茶壶的上某点在该工件坐标系(1,1,1)处。

![figure2](/images/projection/figure2.png)

茶壶创建好了，我们要将其放置到家里的桌子上，那么必须将茶壶放置在另一个坐标系中，世界坐标系（World Space），世界坐标系是所有物体公共的空间。

所有坐标值(x,y,z)都必须放置在特定的坐标系才有意义，如果不放置在坐标系中，则没有任何意义。我们将茶壶放置在桌子上，则需将茶壶的Model space移动或者旋转，甚至缩放到所需的位置，转成World Space。到最后，茶壶上的所有点表示，都要相对于世界坐标系。

> 移动，旋转或缩放对象，这就是我们所说的**变换**

# 变换

定义“当前坐标系（Active Space）”用作其他所有物体的参考的空间

假设当前空间是坐标系A（Space A），其中包含一个茶壶。我们要应用一种转换，将坐标系A中的所有内容移动到新位置。如果要移动坐标系A，则需要定义一个新的“当前坐标系”来表示转换后的坐标系A。让我们将新的当前坐标系称为坐标系B （见图3左）。

一旦坐标系A转换到B，此时坐标系A应该消失，不再讨论A了，或者更确切地说是如何将其重新映射到坐标系B中。应用了变换之后，所有点现在都相对于新的当前坐标系Space B（见图3右）

![figure3](/images/projection/figure3.png)

变换是可逆的，我们可以将坐标系B重新变换到原来的坐标系A。因此我们如果知道具体的变换矩阵，便可以随意将A和B来回切换。

我们可以在向量空间中使用的转换是缩放，平移和旋转。 重要的是，每个变换始终都是相对于原点的，这使得我们用来应用变换本身的顺序非常重要。如图4中，以下两种操作结果是不等价的：

- 先沿着Y向左旋转90度，然后沿着X进行平移
- 先沿着X进行平移，然后沿着Y向左旋转90度

![figure4](/images/projection/figure4.png)

缩放，平移和旋转的变换都是可逆的：以下连续操作将回到原来的坐标系

- 沿着Y向左旋转90度，然后沿着X正方向进行平移10像素
- 沿着X负方向进行平移10像素，然后沿着Y向右旋转90度

# 变换矩阵

要表示从一个3D空间到另一个空间的转换，需要4x4矩阵。以下两个矩阵都是表示变换矩阵，本质上是一样的（互为转置），使用矩阵时注意是采用行表示还是列表示的，不能混用。在OpenGL中使用列表示，不同的软件或者库有可能有不同的表示。

|方向|矩阵|
|--|--|
|行表示矩阵|$\left[ \begin{matrix} Transform\\_XAxis.x & Transform\\_XAxis.y & Transform\\_XAxis.z & 0 \\\\ Transform\\_YAxis.x & Transform\\_YAxis.y & Transform\\_YAxis.z & 0 \\\\ Transform\\_ZAxis.x & Transform\\_ZAxis.y & Transform\\_ZAxis.z & 0 \\\\ Translation.x & Translation.y & Translation.z & 1 \end{matrix} \right]$|
|列表示矩阵|$\left[ \begin{matrix} Transform\\_XAxis.x & Transform\\_YAxis.x & Transform\\_ZAxis.x & Translation.x \\\\ Transform\\_XAxis.y & Transform\\_YAxis.y & Transform\\_ZAxis.y & Translation.y \\\\ Transform\\_XAxis.z & Transform\\_YAxis.z & Transform\\_ZAxis.z & Translation.z \\\\ 0 & 0 & 0 & 1 \end{matrix} \right]$|

注意
> 本文的源码使用行表示矩阵，下面也使用行表示矩阵（原文使用列表示，现已作转置，注意差别）

> I will assume from here on a row vector notation

- Transform_XAxis是新坐标系中的XAxis方向
- Transform_YAxis是新坐标系中的YAxis方向
- Transform_ZAxis是新坐标系中的ZAxis方向
- Translation描述了新坐标系相对于当前坐标系的位置（平移）

 为了应用变换，我们必须将要变换的所有向量与变换矩阵相乘。 如果向量在坐标系A中并且变换描述的是坐标系A相对于坐标系B的新位置，则在乘法之后，所有向量都将在空间B中进行描述。

### 平移矩阵

平移，不会改变原来坐标系的XYZ方向的指向

$$\left[ 
    \begin{matrix}
    1 & 0 & 0 & 0 \\\\
    0 & 1 & 0 & 0 \\\\
    0 & 0 & 1 & 0 \\\\
    Translation.x & Translation.y & Translation.z & 1
    \end{matrix}
\right]$$

### 缩放矩阵

缩放，不会改变原来坐标系的XYZ方向的指向，但会改变该轴的单位长度

$$\left[ 
    \begin{matrix}
    Scale.x & 0 & 0 & 0 \\\\
    0 & Scale.y & 0 & 0 \\\\
    0 & 0 & Scale.z & 0 \\\\
    0 & 0 & 0 & 1
    \end{matrix}
\right]$$

### 绕X轴旋转矩阵

当$\theta=90^\circ$时，Y轴重新映射到原来Z+轴位置，Z轴重新映射到原来Y-轴的位置。

$$\left[ 
    \begin{matrix}
    1 & 0 & 0 & 0 \\\\
    0 & \cos(\theta) & \sin(\theta) & 0 \\\\
    0 & -\sin(\theta) & \cos(\theta) & 0 \\\\
    0 & 0 & 0 & 1
    \end{matrix}
\right]$$

### 绕Y轴旋转矩阵

当$\theta=90^\circ$时，X轴重新映射到原来Z-轴位置，Z轴重新映射到原来X+轴的位置。

$$\left[ 
    \begin{matrix}
    \cos(\theta) & 0 & -\sin(\theta) & 0 \\\\
    0 & 1 & 0 & 0 \\\\
    \sin(\theta) & 0 & \cos(\theta) & 0 \\\\
    0 & 0 & 0 & 1
    \end{matrix}
\right]$$

### 绕Z轴旋转矩阵

当$\theta=90^\circ$时，Y轴重新映射到原来X-轴位置，X轴重新映射到原来Y+轴的位置。


$$\left[ 
    \begin{matrix}
    \cos(\theta) & -\sin(\theta) & 0 & 0 \\\\
    \sin(\theta) & \cos(\theta) & 0 & 0 \\\\
    0 & 0 & 1 & 0 \\\\
    0 & 0 & 0 & 1
    \end{matrix}
\right]$$

### 相乘运算

 每做一次变换，则使用变换矩阵左乘原来的矩阵。
 
 注意：矩阵左乘与右乘是完全不同的，如以下操作是完全不一样的，与前面描述的变换顺序同理。
 - 沿着X旋转90度  相乘  沿着Z+平移10
 - 沿着Z+平移10  相乘  沿着X旋转90度

 因此，如果我们想绕Y轴向左旋转90°，然后沿Z轴平移10个单位，得到的变换矩阵ComposedTransformation，则该矩阵乘法为
 
```
[Translate 10 along X] x [RotateY 90°] = [ComposedTransformation]
```

 再如
 先绕Y轴旋转90°，然后绕X轴旋转180°，最后平移（1.5，1，1.5），则矩阵乘法为
```
[Translate 1.5 1 1.5] x [RotateX 180°] x [RotateY 90°] = [ComposedTransformation]
```

$$
\left[ 
    \begin{matrix}
    1 & 0 & 0 & 0 \\\\
    0 & 1 & 0 & 0 \\\\
    0 & 0 & 1 & 0 \\\\
    1.5 & 1 & 1.5 & 1
    \end{matrix}
\right]
\times
\left[ 
    \begin{matrix}
    1 & 0 & 0 & 0 \\\\
    0 & \cos(180) & \sin(\theta) & 0 \\\\
    0 & -\sin(\theta) & \cos(180) & 0 \\\\
    0 & 0 & 0 & 1
    \end{matrix}
\right]
\times
\left[ 
    \begin{matrix}
    \cos(90) & 0 & \sin(90) & 0 \\\\
    0 & 1 & 0 & 0 \\\\
    -\sin(90) & 0 & \cos(90) & 0 \\\\
    0 & 0 & 0 & 1
    \end{matrix}
\right]
=
\left[ 
    \begin{matrix}
    0 & 0 & 1 & 0 \\\\
    0 & -1 & 0 & 0 \\\\
    1 & 0 & 0 & 0 \\\\
    1.5 & 1 & 1.5& 1
    \end{matrix}
\right]
$$

只要计算出这个ComposedTransformation变换矩阵，就能将在空间任意一点，从坐标系A映射到坐标系B。

### 工件坐标映射到世界坐标

如图5，这个球以球底部作为工件坐标系原点。现在要将球的顶点(0,1,0)映射到世界坐标系中，只需相乘。球的坐标是三维，相乘需要齐次四维，因此把最后一维置为1，得到(0,1,0,1)，再与变换矩阵相乘。

使用上面的ComposedTransformation矩阵结果。

$$
\left[ 
    \begin{matrix}
    0 & 1 & 0 & 1
    \end{matrix}
\right]
\times
\left[ 
    \begin{matrix}
    0 & 0 & 1 & 0 \\\\
    0 & -1 & 0 & 0 \\\\
    1 & 0 & 0 & 0 \\\\
    1.5 & 1 & 1.5& 1
    \end{matrix}
\right]
=
\left[ 
    \begin{matrix}
    1.5 & 0 & 1.5 & 1
    \end{matrix}
\right]
$$

![figure5](/images/projection/figure5.png)

得到的点(1.5,0,1.5,1)，去掉最后一维得到(1.5,0,1.5)。
球的顶部(0,1,0)经过变换，映射到世界坐标系中的(1.5,0,1.5)，这两个坐标值虽然不同，但是实际上是同一个点。坐标值不同，正是因为用了不同的坐标系表示。

## 工件、世界、视角坐标系

现在将3个坐标系放在一起考虑。

渲染3D场景时，第一步是将所有Model放置在同一空间（世界坐标系）中。 由于每个对象在世界上都会有自己的位置和方向，因此每个对象都有一个不同的“Model到World”转换矩阵。

![figure6](/images/projection/figure6.png)

上图（图6）展示了三个茶壶，每个茶壶处于其自己的Model坐标系中

![figure7](/images/projection/figure7.png)

上图（图7）展示了三个茶壶同时在World坐标系中

现在我们需要将这三个茶壶投影到二维屏幕上（就像照相机一样），通常分两个步骤完成
1. 将所有对象移动到另一个称为“视角空间”(Camera Space或者View Space)的坐标系中
2. 使用投影矩阵执行实际投影

相机也是采用右手坐标系，Z正方向朝向自己，Z负方向朝向物体。

![figure8](/images/projection/figure8.png)

为什么我们需要一个视角空间（视角坐标系）？

视角空间是一个辅助空间，我们可以用来简化数学运算并使所有内容保持优雅并利用矩阵直接运算。我们需要渲染到摄像机，这意味着将所有顶点投影到摄像机屏幕上。我们应用的变换将所有顶点从世界坐标系映射到视角坐标系。

将每个Model进行变换到Camera的方位即可。最后让Z-轴指向照相机目标，Z+指向自己。

## 投影坐标系

投影坐标系，投影空间（Projection Space）是一个长方体，每个轴的尺寸在-1和1之间。们要做的就是将其投影到相机的假想屏幕上。为了从View Space进入Projection Space，我们需要另一个矩阵，即View to Projection矩阵，该矩阵的值取决于我们要执行哪种类型的投影。 最常用的两个投影是“ 正交投影”和“ 透视投影” 。本问仅讨论正交投影，最简单也最容易实现。透视投影可以参考其它资料，算法基本相同，矩阵定义不同。

要进行正交投影，我们必须定义相机可以看到的区域的大小。 通常使用x和y轴的宽度和高度值以及z轴的近和远z值定义该值，如下图9

![figure9](/images/projection/figure9.png)

给定这些值，我们可以创建将矩阵区域重新映射为长方体的转换矩阵。接下来的矩阵是将向量从“视图空间”转换为“正交投影空间”，如下图10

![figure10](/images/projection/figure10.png)

该投影矩阵定义为

$$\left[ 
    \begin{matrix}
    \frac{1}{width} & 0 & 0 & 0 \\\\
    0 & \frac{1}{height} & 0 & 0 \\\\
    0 & 0 & -\frac{2}{Z_far-Z_near} & 0 \\\\
    0 & 0 & -\frac{Z_far+Z_near}{Z_far-Z_near} & 1
    \end{matrix} 
\right]$$

## 开始投影

将几个矩阵连续相乘即可把三维坐标投影到二维坐标。

```
[View To Projection] x [World To View] x [Model to World] = [ModelViewProjectionMatrix]
```

- View To Projection矩阵是投影矩阵
- World To View是视角矩阵，世界到相机映射
- Model to World是将model变换成世界的矩阵

## 源代码demo

写了一个简单的C++代码测试了变换，可以给大家参考一下。

TODO：源码还在重构整理。。。。

## 参考网站

[World, View and Projection Transformation Matrices](http://www.codinglabs.net/article_world_view_projection_matrix.aspx)

[The Perspective and Orthographic Projection Matrix](https://www.scratchapixel.com/code.php?id=4&origin=/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix&src=2)

[相机矩阵(Camera Matrix)](https://blog.csdn.net/zb1165048017/article/details/71104241)

[Dissecting the Camera Matrix, Part 2: The Extrinsic Matrix](https://ksimek.github.io/2012/08/22/extrinsic/)

