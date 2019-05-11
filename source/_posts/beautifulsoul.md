title: BeautifulSoul笔记
date: 2016-04-12 23:01:12
tags: python
categories: 编程
---
Beautiful Soup,一个HTML/XML的解析器，处理不规范标记并生成剖析树(parse tree)。 它提供简单又常用的导航（navigating），搜索以及修改剖析树的操作。它可以节省你做爬虫的编程时间，无需掌握正则表达式。
<!-- more -->
soup 就是BeautifulSoup处理格式化后的字符串，soup.title 得到的是title标签，soup.p  得到的是文档中的第一个p标签，要想得到所有标签，得用find_all函数。find_all 函数返回的是一个序列，可以对它进行循环，依次得到想到的东西.

get_text() 是返回文本,这个对每一个BeautifulSoup处理后的对象得到的标签都是生效的。你可以试试 print soup.p.get_text()

其实是可以获得标签的其他属性的，比如我要获得a标签的href属性的值，可以使用 print soup.a['href'],类似的其他属性，比如class也是可以这么得到的（soup.a['class']）。

特别的，一些特殊的标签，比如head标签，是可以通过soup.head 得到，其实前面也已经说了。

如何获得标签的内容数组？使用contents 属性就可以 比如使用 print soup.head.contents，就获得了head下的所有子孩子，以列表的形式返回结果，

可以使用 [num]  的形式获得 ,获得标签，使用.name 就可以。

获取标签的孩子，也可以使用children，但是不能print soup.head.children 没有返回列表，返回的是 
	
    <listiterator object at 0x108e6d150>

不过使用list可以将其转化为列表。当然可以使用for 语句遍历里面的孩子。

关于string属性，如果超过一个标签的话，那么就会返回None，否则就返回具体的字符串print soup.title.string 就返回了 The Dormouse's story

超过一个标签的话，可以试用strings

向上查找可以用parent函数，如果查找所有的，那么可以使用parents函数

查找下一个兄弟使用next_sibling,查找上一个兄弟节点使用previous_sibling,如果是查找所有的，那么在对应的函数后面加s就可以


## 遍历树

方法一、使用find_all 函数

	find_all(name, attrs, recursive, text, limit, **kwargs)

举例说明：

    print soup.find_all('title')
    print soup.find_all('p','title')
    print soup.find_all('a')
    print soup.find_all(id="link2")
    print soup.find_all('ul', {'class':'infoList teachinList'})
    
方法二、使用find 函数

find_all() 方法将返回文档中符合条件的所有tag,尽管有时候我们只想得到一个结果.比如文档中只有一个<body>标签,那么使用 find_all() 方法来查找<body>标签就不太合适, 使用 find_all 方法并设置 limit=1 参数不如直接使用 find() 方法.下面两行代码是等价的:

    soup.find_all('title', limit=1)
    soup.find('title')
    
唯一的区别是 find_all() 方法的返回结果是值包含一个元素的列表,而 find() 方法直接返回结果.
find_all() 方法没有找到目标是返回空列表, find() 方法找不到目标时,返回 None .

例如find方法（我理解和findChild是一样的），就是以当前节点为起始，遍历整个子树，找到后返回。

而这些方法的复数形式，会找到所有符合要求的tag，以list的方式放回。类比一下，他们的对应关系是
  
    find->findall
    findParent->findParents
    findNextSibling->findNextSiblings

输出搜到的字符串

.strings 和 stripped_strings

    for string in soup.strings:
        print(repr(string))
    for string in soup.stripped_strings:
    	print(repr(string))
    



## 对象的种类

### Tag

Tag 对象与XML或HTML原生文档中的tag相同:

    soup = BeautifulSoup('<b class="boldest">Extremely bold</b>')
    tag = soup.b
    type(tag)
    # <class 'bs4.element.Tag'>

tag对象中最重要的属性（attr）: name和attributes

#### name

每个tag都有自己的名字,通过 .name 来获取:

	tag.name
	# u'b'
    
如果改变了tag的name,那将影响所有通过当前Beautiful Soup对象生成的HTML文档:

    tag.name = "blockquote"
    tag
    # <blockquote class="boldest">Extremely bold</blockquote>


操作文档树最简单的方法就是告诉它你想获取的tag的name.如果想获取 <head> 标签,只要用 soup.head :

    soup.head
    soup.title

tag的 .contents 属性可以将tag的子节点以列表的方式输出:

    head_tag = soup.head
    print head_tag
    # <head><title>The Dormouse's story</title></head>
    print head_tag.contents
    [<title>The Dormouse's story</title>]
    title_tag = head_tag.contents[0]
    print title_tag
    # <title>The Dormouse's story</title>
    print title_tag.contents
    # [u'The Dormouse's story']

#### attributes

一个tag可能有很多个属性. tag <b class="boldest"> 有一个 “class” 的属性,值为 “boldest” . tag的属性的操作方法与字典相同:

    tag['class']
    # u'boldest'
    
也可以直接”点”取属性, 比如: .attrs :

    tag.attrs
    # {u'class': u'boldest'}
    
tag的属性可以被添加,删除或修改. 再说一次, tag的属性操作方法与字典一样

    tag['class'] = 'verybold'
    tag['id'] = 1
    tag
    # <blockquote class="verybold" id="1">Extremely bold</blockquote>

    del tag['class']
    del tag['id']
    tag
    # <blockquote>Extremely bold</blockquote>

    tag['class']
    # KeyError: 'class'
    print(tag.get('class'))
    # None

### NavigableString

翻译为“可以遍历的字符串”。字符串常被包含在tag内.Beautiful Soup用 NavigableString 类来包装tag中的字符串:

### BeautifulSoup

BeautifulSoup 对象表示的是一个文档的全部内容.大部分时候,可以把它当作 Tag 对象,它支持 遍历文档树 和 搜索文档树 中描述的大部分的方法

### Comments

获取注释部分

    markup = "<b><!--Hey, buddy. Want to buy a used parser?--></b>"
    soup = BeautifulSoup(markup)
    comment = soup.b.string
    type(comment)
    # <class 'bs4.element.Comment'>

## 指定文档解析器

Beautiful Soup为不同的解析器提供了相同的接口,但解析器本身时有区别的.同一篇文档被不同的解析器解析后可能会生成不同结构的树型文档.

	BeautifulSoup(text,'html.parser')
    BeautifulSoup(text,'lxml')

## 后记

这篇文章很乱，最好的方法是参考中文手册，非常好的例子
- css选择器
- 猜测文本编码
- 智能引号
- 格式化输出，压缩输出:prettify('utf8')
- 寻找父节点，兄弟节点
- 配合正则表达式

文章来源：
[官方手册：简体中文](https://www.crummy.com/software/BeautifulSoup/bs4/doc.zh)
[python 中BeautifulSoup入门](http://www.cnblogs.com/yupeng/p/3362031.html)
[用python的BeautifulSoup分析html](http://www.cnblogs.com/twinsclover/archive/2012/04/26/2471704.html)
