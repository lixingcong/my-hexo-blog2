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

    // ------ main.cpp ----------
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

    // ------ main.cpp ----------
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


# 静态或全局字符串变量

上文提到tr()函数用法的特点是：在运行期间动态返回的国际化后字符串。如：

    QString getHelloString()
    {
        return QObject::tr("Hello");
    }

在国际化翻译过程中，还有两种特例在使用tr()函数需要特别注意：
1. 静态变量（编译期间确定字符串，可以是类成员变量，也可以是函数内static变量）
2. 全局变量（定义在全局作用域的变量，此时无上下文）
3. const常量（运行期间不能被修改）

既然是编译期间确定的变量，在运行期间就不能随意改变它们内存中的数据，应该怎么样返回国际化后的字符串呢？Qt提供两个宏```QT_TR_NOOP```和```QT_TRANSLATE_NOOP```，对应的UTF-8编码的宏为```QT_TR_NOOP_UTF8```和```QT_TRANSLATE_NOOP_UTF8```

宏```QT_TR_NOOP```和```QT_TRANSLATE_NOOP```的区别就是是否有context上下文。若字符串定义在某个类里面，就是有上下文，上下文为类名字，可以直接用```QT_TR_NOOP```，对于定义在全局作用域，或者某个函数内部的static变量，就是没有context，需要宏```QT_TRANSLATE_NOOP```手动指定上下文，这样才能在Qlinguist看到待翻译的文本。运行过程中，用QApplication::translate()函数取出国际化后的语言。

## 具有上下文，使用QT_TR_NOOP

考虑以下错误的代码片段

    // ------ main.cpp ----------
    #include <QApplication>
    #include "MyButton.h"
    #include <QTextCodec>
    #include <QTranslator>

    int main(int argc, char *argv[])
    {
        QApplication a(argc, argv);

        #if QT_VERSION < 0x050000
        QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
        #endif

        QTranslator translator;
        if(translator.load("zh","/tmp/untitled-lang2")){
            qDebug("load ok");
            QApplication::installTranslator(&translator);
        }

        MyButton* btn=new MyButton;
        btn->show();
        return a.exec();
    }


    // ------ MyButton.h ----------
    #ifndef MYBUTTON_H
    #define MYBUTTON_H
    #include <QPushButton>
    #include <QStringList>

    class MyButton : public QPushButton
    {
        Q_OBJECT
    public:
        MyButton(QWidget *parent = 0);
    protected slots:
        void onClicked();
    private:
        static const QStringList names; // 静态变量编译期间确定
    };
    #endif // MYBUTTON_H


    // ------ MyButton.cpp ----------
    #include "MyButton.h"
    const QStringList MyButton::names={
        QObject::trUtf8("按钮1"),
        QObject::trUtf8("按钮2")
    };

    MyButton::MyButton(QWidget *parent):QPushButton(parent)
    {
        setText("Click Me!");
        setFixedWidth(120);
        connect(this,SIGNAL(clicked()),this,SLOT(onClicked()));
    }

    void MyButton::onClicked()
    {
        static int i=0;
        int nameIndex=(i++)%2;
        setText(names.at(nameIndex));
    }

按照上文步骤翻译好语言，生成qm文件，编译运行测试，点击按钮会改变文本。

现象：运行的时候却不能动态的从names数组中取出国际化语言。因为QStringList数组names是在编译期间确定的，不能动态地返回tr()的结果。因此从names取出的字符串是固定的值。

要实现动态翻译，可以把代码稍作修改：把names声明为char数组而不是QStringList，该char数组每一个元素使用宏QT_TR_NOOP_UTF8包裹着字符串。在程序运行时，取出char字符串后作为tr()函数的参数，在运行时动态返回tr()结果。

    // ------ MyButton.h ----------
    // 修改names数组的定义
    private:
        static const char* names[];
    
    // ------ MyButton.cpp ----------
    // 修改names数组的初始化
    const char* MyButton::names[]={
        QT_TR_NOOP_UTF8("按钮1"),
        QT_TR_NOOP_UTF8("按钮2")
    };

    // 修改点击后取出的字符串
    void MyButton::onClicked()
    {
        static int i=0;
        int nameIndex=(i++)%2;
        setText(tr(names[nameIndex]));
    }

QT_TR_NOOP宏的作用不会修改char数组字符串，仅仅是为了让lupdate命令识别出这是一个待翻译的字符串。其context为该类的类名（类名的字符串）。

## 无上下文，使用QT_TRANSLATE_NOOP

如下，我们指定了一个上下文，为"MyContext"。使用translate()函数翻译出结果。

    // ------ main.cpp ----------
    #include <QApplication>
    #include <QTextCodec>
    #include <QTranslator>

    const char* names[]={
        QT_TRANSLATE_NOOP_UTF8("MyContext","按钮1"), // 编译期间确定
        QT_TRANSLATE_NOOP_UTF8("MyContext","按钮2")
    };

    int main(int argc, char *argv[])
    {
        QApplication a(argc, argv);

        #if QT_VERSION < 0x050000
        QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
        #endif

        QTranslator translator;
        if(translator.load("zh","/tmp/untitled-lang2")){
            qDebug("load ok");
            QApplication::installTranslator(&translator);
        }

        QPushButton* btn=new QPushButton;
        btn->setText(qApp->translate("MyContext",names[0]));
        btn->show();

        return a.exec();
    }

使用lupdate后，在Qt语言家可以看到我们上下文。编译运行可见翻译成功

![](/images/qt-tr/context.png)

QT_TRANSLATE_NOOP宏的作用同样不会修改char数组字符串，仅仅是为了让lupdate命令识别出这是一个待翻译的字符串。其context为自己指定的字符串。

当在运行过程中，需要取出国际化文本时，不使用tr()，而是QApplication::translate()并传入context和char。

# 参考资料

[Qt国际化（源码含中文时）的点滴分析](https://blog.csdn.net/dbzhang800/article/details/6334852)

[Qt官方文档：linguist中使用QT_TR_NOOP](https://doc.qt.io/archives/qt-4.8/linguist-programmers.html#using-qt-tr-noop-and-qt-translate-noop)

[Qt官方文档：QT_TR_NOOP](https://doc.qt.io/archives/qt-4.8/qtglobal.html#QT_TR_NOOP)
