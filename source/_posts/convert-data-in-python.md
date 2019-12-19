title: python整数、字符串、字节串相互转换
date: 2016-03-06 19:52:25
tags: [python, 转载]
categories: 编程
---
数据解析时，python可以相互转换各种数据类型。最近在斯坦福公开课《密码学》网站上面做题发现，我对数据转换很不熟悉，写下日志记下用法。
<!-- more -->

导航

| |数字|字符串|字节码|
|-|---|-----|-----|
|到数字|[进制转换](#进制转换)|[字符转整数](#字符to整数)|[字节串转整数](#字节串to整数)|
|到字符串|str()|[字符串编码解码](#字节串to字符串)|decode('hex')|
|到字节码|[数字转字符串](#整数to字节串)|[字符串转字节串](#字符串to字节串)|🈚|

还有常见的单个字符转换

|函数|功能|记忆口诀|例子|备注|
|--|--|--|--|--|
|chr|数字转成对应的ascii字符|chr长得很像char，因此转成char|```chr(65) ==> A```|输入参数有效范围为0~255|
|ord|单个字符转对应ascii序号|d为digit最后一个字母，因此转成数字|```ord('A') ==> 65```||

### 进制转换
10进制转16进制: 

	hex(16)  ==>  0x10
    
16进制转10进制: 

int(STRING,BASE)将字符串STRING转成十进制int，其中STRING的基是base。该函数的第一个参数是字符串
	
    int('0x10', 16)  ==>  16
    
类似的还有八进制oct()， 二进制bin()

16进制字符串转成二进制

	hex_str='00fe'
	bin(int('1'+hex_str, 16))[3:]  #含有前导0
	# 结果 '0000000011111110'
	bin(int(hex_str, 16))[2:]   #忽略前导0
	# 结果 '11111110'
	
二进制字符串转成16进制字符串
	
	bin_str='0b0111000011001100'
	hex(int(bin_str,2))
	# 结果 '0x70cc'


### 字符to整数
10进制字符串: 

	int('10')  ==>  10

16进制字符串: 

	int('10', 16)  ==>  16
    # 或者
	int('0x10', 16)  ==>  16
	

### 字节串to整数
使用网络数据包常用的struct，兼容C语言的数据结构
struct中支持的格式如下表

|Format|C-Type|Python-Type|字节数|备注|
|--|--|--|--|--|
|x|pad byte|no value|1| |
|c|char|string of length 1|1| |
|b|signed char|integer|1| |
|B|unsigned char|integer|1| |
|?|_Bool|bool|1| |
|h|short|integer|2| |
|H|unsigned short|integer|2| |
|i|int|integer|4| |
|I|unsigned int|integer or long|4| |
|l|long|integer|4| |
|L|unsigned long|long|4| |
|q|long long|long|8|仅支持64bit机器|
|Q|unsigned long long|long|8|仅支持64bit机器|
|f|float|float|4| |
|d|double|float|8| |
|s|char[]|string|1| |
|p|char[]|string|1(与机器有关)|作为指针|
|P|void *|long|4|作为指针|

对齐方式：放在第一个fmt位置

|CHARACTER|BYTE ORDER|SIZE|ALIGNMENT|
|----|----|----|----|
|@|native|native|native|
|=|native|standard|none|
|<|little-endian|standard|none|
|>|big-endian|standard|none|
|!|network (= big-endian)|standard|none|

转义为short型整数:

	struct.unpack('<hh', bytes(b'\x01\x00\x00\x00'))  ==>  (1, 0)
    
转义为long型整数: 

	struct.unpack('<L', bytes(b'\x01\x00\x00\x00'))  ==>  (1,)

### 整数to字节串
转为两个字节: 

	struct.pack('<HH', 1,2)  ==>  b'\x01\x00\x02\x00'

转为四个字节: 

	struct.pack('<LL', 1,2)  ==>  b'\x01\x00\x00\x00\x02\x00\x00\x00'
    
### 整数to字符串

直接用函数

	str(100)


### 字符串to字节串
[我用c++实现的encode(hex)和decode(hex)](#CPP实现encode)

**decode和encode区别**

decode函数是重新解码，把CT字符串所显示的69dda8455c7dd425【每隔两个字符】解码成十六进制字符\x69\xdd\xa8\x45\x5c\x7d\xd4\x25

    CT='69dda8455c7dd425'
    print "%r"%CT.decode('hex')
    # 输出结果
    'i\xdd\xa8E\\}\xd4%'

encode函数是重新编码，把CT字符串所显示的69dda8455c7dd425【每个字符】编码成acsii值，ascii值为十六进制显示，占两位。执行下列结果显示36396464613834353563376464343235等价于将CT第一个字符'6'编码为0x36h 第二个字符'9'编码为0x39h

    CT='69dda8455c7dd425'
    print "%r"%CT.encode('hex')
    # 输出结果
    '36396464613834353563376464343235'
    
> 可以理解为：decode解码，字符串变短一半，encode编码，字符串变为两倍长度

decode('ascii')解码为字符串Unicode格式。输出带有'u'
encode('ascii')，编码为Unicode格式，其实python默认处理字符串存储就是Unicode，输出结果估计和原来的字符串一样。

字符串编码为字节码: 

	'12abc'.encode('ascii')  ==>  b'12abc'

数字或字符数组: 

	bytes([1,2, ord('1'),ord('2')])  ==>  b'\x01\x0212'

16进制字符串:

	bytes().fromhex('010210')  ==>  b'\x01\x02\x10'

16进制字符串: 

	bytes(map(ord, '\x01\x02\x31\x32'))  ==>  b'\x01\x0212'

16进制数组: 

	bytes([0x01,0x02,0x31,0x32])  ==>  b'\x01\x0212'


### 字节串to字符串
字节码解码为字符串: 

	bytes(b'\x31\x32\x61\x62').decode('ascii')  ==>  12ab
    
字节串转16进制表示,夹带ascii: 

	str(bytes(b'\x01\x0212'))[2:-1]  ==>  \x01\x0212

字节串转16进制表示,固定两个字符表示: 

	str(binascii.b2a_hex(b'\x01\x0212'))[2:-1]  ==>  01023132

字节串转16进制数组: 

	[hex(x) for x in bytes(b'\x01\x0212')]  ==>  ['0x1', '0x2', '0x31', '0x32']

问题：什么时候字符串前面加上'r'、'b'、'r'，其实官方文档有写。我认为在Python2中，r和b是等效的。

The Python 2.x documentation:

> A prefix of 'b' or 'B' is ignored in Python 2; it indicates that the literal should become a bytes literal in Python 3 (e.g. when code is automatically converted with 2to3). A 'u' or 'b' prefix may be followed by an 'r' prefix.
> 'b'字符加在字符串前面，对于python2会被忽略。加上'b'目的仅仅为了兼容python3，让python3以bytes数据类型(0~255)存放这个字符、字符串。
    
The Python 3.3 documentation states:

> Bytes literals are always prefixed with 'b' or 'B'; they produce an instance of the bytes type instead of the str type. They may only contain ASCII characters; bytes with a numeric value of 128 or greater must be expressed with escapes.
> 数据类型byte总是以'b'为前缀，该数据类型仅为ascii。

下面是stackflow上面一个回答。我觉得不错，拿出来跟大家分享

In Python 2.x
> Pre-3.0 versions of Python lacked this kind of distinction between text and binary data. Instead, there was:

> - unicode = u'...' literals = sequence of Unicode characters = 3.x str
> - str = '...' literals = sequences of confounded bytes/characters
Usually text, encoded in some unspecified encoding.
But also used to represent binary data like struct.pack output.


Python 3.x makes a clear distinction between the types:

> - str = '...' literals = a sequence of Unicode characters (UTF-16 or UTF-32, depending on how Python was compiled)
> - bytes = b'...' literals = a sequence of octets (integers between 0 and 255)

### CPP实现encode
就是做个笔记，毕竟在做题Cryptography时候用c++写字符串的处理很蛋疼！为了防止再次造轮子，记下来。

	#include <cstring> //用到strlen函数
    static unsigned char ByteMap[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8','9', 'a', 'b', 'c', 'd', 'e', 'f' };

    unsigned char hex_2_dec(unsigned char c){
        if(c >= '0' && c <= '9') return c - '0';
        if(c >= 'a' && c <= 'f') return c - 'a' + 10;
    }

    void str_encode(unsigned char *src, unsigned char *dest, int len_of_src) {
        // 使用注意：dest_len >= 2*len_src +1，最后一位是存放'\0'。
        int t1;
        for (int i = 0; i < len_of_src; ++i) {
            t1 = (int) src[i];
            dest[2 * i] = ByteMap[t1 / 16];
            dest[2 * i + 1] = ByteMap[t1 % 16];
        }
        dest[2 * len_of_src] = 0; //必须填充最后一个为'\0'
    }

    void str_decode(unsigned char *src,unsigned char *dest){
        int len_of_src=strlen((char *)src);
        unsigned char t1;
        for(int i=1;i<=len_of_src;i+=2){
            t1=hex_2_dec(src[i-1]);
            t1= 16*t1 + hex_2_dec(src[i]);
            dest[i/2]=t1;
        }
    }

鸣谢
本文转载自csdn博客的[《python常用的十进制、16进制、字符串、字节串之间的转换》](http://blog.csdn.net/crylearner/article/details/38521685)。