---
title: Qt使用tr()函数中包裹中文
date: 2019-05-11 18:43:46
tags: qt
categories: 编程
---
Qt的多国语言功能强大，使用tr()函数时，一般是tr("ASCII")这样用法，然后从英文翻译得到其它语言。

但是我们作为普通人一直都很贱，非得要使用tr("中文字符")实现中文到其它语言的翻译，因此需要略添加几步指定编码的步骤。
<!-- more -->
本文所有源代码文件均为UTF-8编码。以下步骤建议按顺序完成。从中可以感受到文本文件编码的威力：为什么全世界就没法统一一种编码格式呢？

# ts文件和qm文件关系

ts文件就是一个xml文本文件，里面保存源文本和翻译结果文本，还有对应源码所在的行数，方便我们使用Qt自带Linguist（Qt语言家）进行定位。

通过lupdate命令，将工程中所有tr("xxx")的位置找出来，生成ts文件

通过lrelease命令，将ts文件编译成qm二进制文件，供QTranlator::load()函数加载语言。

# 设定源码中tr函数的编码

在Qt4中，函数QTextCodec::setCodecForTr()的用法就是针对源代码中tr()函数的编码。在Qt5中该函数被取消，如果是Qt5可以直接跳过本小节。

setCodecForTr函数的作用是，当没有加载任何的QTranslator，也就是当没有加载任何的qm文件时，tr()函数的结果。它决定了程序运行过程中是否有乱码。

若我们没有指定合适cpp源码编码时，使用tr("中文字符")，很有可能在运行时发现乱码。

设定utf-8为tr()函数的编码，可以显示tr()函数包裹的中文字符了

    // main.cpp
    int main(int argc, char *argv[])
    {
        QApplication a(argc, argv);

        #if QT_VERSION < 0x050000
        QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
        #endif

        QPushButton btn;
        btn.setText(QObject::tr("未翻译"));
        btn.show();

        return a.exec();
    }

# 设定ts文件中的编码

往工程中添加ts文件

    # demo.pro
    TRANSLATIONS += zh.ts
    CODECFORTR=utf-8

使用lupdate命令生成ts文件。

CODECFORTR这个参数指定了生成ts文件时候的编码，若未指定合适cpp源码编码，在Linguist界面看到的界面是这样的，中文变成了"???"，无法直视。

![](/images/qt-tr/linguist.png)

指定好正确的utf-8编码，就能让我们在Linguist愉快地翻译中文文本了！

# 设定qm文件中的编码

当我们确认前两步均能正确显示中文了，就只剩下最后一步生成适合的qm文件了。

使用Linguist翻译好文本后，按Ctrl+s保存，我们用文本工具打开ts文件，手工加入一行，指定编译qm文件使用的编码。

（注意每次保存都要检查是否被Linguist覆盖了，若缺这行要继续补上）

    <defaultcodec>utf-8</defaultcodec>

如图

![](/images/qt-tr/ts.png)

大功告成，使用lrelease编译生成qm文件并加载即可！

    int main(int argc, char *argv[])
    {
        QApplication a(argc, argv);

        #if QT_VERSION < 0x050000
        QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
        #endif

        QTranslator translator;
        if(translator.load("zh","/tmp/lang2")){
            qDebug("load ok");
            QApplication::installTranslator(&translator);
        }

        QPushButton btn;
        btn.setText(QObject::tr("未翻译"));
        btn.show();

        return a.exec();
    }

# 参考博客

[Qt国际化（源码含中文时）的点滴分析](https://blog.csdn.net/dbzhang800/article/details/6334852)
