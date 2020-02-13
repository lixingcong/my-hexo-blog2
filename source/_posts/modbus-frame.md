---
title: Modbus协议帧
date: 2020-02-12 14:46:03
tags: [翻译, 协议]
categories: [网络]
---
人工翻译网上找到的Modbus协议文档，讲述不同封包的格式RTU/ASCII/TCP，大端存储，功能码。附加自己在工作中对协议的理解。
<!-- more -->
Modbus协议是通讯协议，广泛应用在设备之间的主从通讯。主站发送requset，从站作为response。Modbus协议与底层物理层无关。 其底层物理层常是RS232，RS422或RS485实现。

> 主站为master或者poll，从站为slave。

在标准的modbus系统中，只有一个master设备，和最多247个slave设备（[信息来源](https://www.picotech.com/library/oscilloscopes/modbus-serial-protocol-decoding)）

每个slave设备有一个唯一的地址，用一个字节表示，0表示广播地址，其余地址(1~247，最大支持247个slave设备)为其它设备所用。地址将出现在modbus帧中，用于区分本帧是发给哪个slave设备，地址有时候被称为slave ID，下文用slave ID来表示这个值。

## 大端存储

Modbus官方[协议文档](http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b3.pdf)中有一段话：

> Data Encoding: Modbus uses a big-Endian representation for addresses and data items. This means
that when a numerical quantity larger than a single byte is transmitted, the most
significant byte is sent first.

在Modbus数据帧中，存储格式是大端，即0x1234和0x12345678这两个数据，对应char数组应该是

```
u8 d1[2]={0x12, 0x34};
u8 d2[4]={0x12, 0x34, 0x56, 0x78};
```

我用C艹写了的转换函数，[点击我下载源码](/attachments/modbus-frame/modbus-big-endian.cpp)，主要有以下几个函数实现转换。使用了CPP的模板，目的是为了同时让unsigned和signed都编译通过。

```
T bufferToU16(const void* data); // 从buffer拿出来
T bufferToU32(const void* data);

void u32ToBuffer(T input, void* output); // 写入到buffer
void u16ToBuffer(T input, void* output);
```

在下文[数据传输模式](#数据传输模式)内提到的封包步骤，Modbus的起始地址、寄存器个数等官方字段，需要遵循这个大端存储的规定。而数据字段可以根据实际应用场合，选用适合的字节序，但仍强烈建议使用官方钦定的大端存储。

## PDU

Protocol Data Units简称PDU，是Modbus协议帧最小单元，由“功能码”+“数据”两个字段组成。

### PDU与APU关系

PDU封包完成后，对PDU进行更高一层的封包叫APU，APU直接发送给目标设备。

他们两者关系是：
- PDU = Function code + Data
- ADU(ASCII或RTU) = Slave ID + PDU + Error check
- ADU(TCP) = MBAP + PDU

### 请求帧PDU

请求帧PDU的“功能码”字段，用于指示从设备要执行哪种操作。不同的功能码，“数据”字段有不同定义

以03H功能码为例，请求帧PDU的“数据”字段需要包含以下信息：
- 起始寄存器地址
- 要读多少个寄存器

### 回应帧PDU

以03H功能码为例

如果从站能正确处理请求，response的“功能码”字段与requset功能码一致，回应帧PDU“数据”字段需要包含以下信息：

- 有几个寄存器
- 这几个寄存器的内容

如果slave不能处理请求，或者发生错误，则slave会将错误代码替代原来的功能码回复给master端。错误代码的存在意义是让master端确认消息有效。

回应帧PDU的“功能码”字段错误码为request功能号的最高bit置1得到。因此大于0x80的功能号都是错误功能号。“数据”字段需要包含以下信息

- 错误代码，参见[Modbus Execptions](http://www.simplymodbus.ca/exceptions.htm)

常见的错误代码有：不支持的功能号（01H），错误的地址（02H）

## 数据传输模式

常见的三种传输模式（transmission mode）为ASCII、RTU、TCP

### ASCII

数据每个字节(8bit)会被编码成两个字节(16bit)的ASCII字符。使用LRC作为校验和。

|有效码元|hex值|
|--|--|
|'0'-'9'|30H-31H|
|'A'-'F'|41H-46H|

该传输模式的主要优点是易于人类阅读究竟传输了什么字节，便于调试。

采用ASCII编码的APU帧结构如下表

|位置|起始|slave ID|功能码|数据N字节|LRC|末尾|
|--|--|--|--|--|--|--|
|帧内容|':'|2个ASCII|2个ASCII|2N个ASCII|2个ASCII|'\r\n'|

每一帧以冒号（:）字符（3AH）开头，并以回车换行符（CRLF）作为结束（0DH和0AH）。

封包步骤

1. 封装好PDU，即功能码加数据
2. 将slaveID插到PDU首部，得到的二进制数据（称为A流）
3. A流作为LRC的输入，得到LRC。[点击下载LRC实现](/attachments/modbus-frame/modbus-crc-lrc.cpp)
4. 将LRC追加到A流末尾，得到B流
5. 将B流逐字节编码为ASCII字符，得到C流
6. 将冒号(:)和CRLF(0x0D0A)分别加到C流头部尾部，完成封包

### RTU

数据直接为二进制内容，报文必须以连续流的形式发送。使用CRC作为校验和。

采用RTU编码的APU帧结构如下表

|位置|起始|slave ID|功能码|数据N字节|CRC|末尾|
|--|--|--|--|--|--|--|
|帧内容|3.5字符时间|1字节|1字节|N字节|2字节|3.5字符时间|

上表提到的字符时间对于串口传输有要求
- 整个消息帧必须作为连续流传输。 如果在完成帧之前发生超过1.5个字符时间的静默间隔，则接收设备将丢弃未完成的消息，并假定下一个字节将是新消息的地址字段。
- 如果新消息在前一条消息之后的3.5个字符时间之前开始，则接收设备将认为它是前一条消息的延续。会返回modbus错误。

封包步骤

1. 封装好PDU，即功能码加数据
2. 将slaveID插到PDU首部，得到的二进制数据（称为A流）
3. A流作为CRC的输入，得到CRC。[点击下载CRC实现](/attachments/modbus-frame/modbus-crc-lrc.cpp)
4. 将CRC追加到A流末尾，完成封包

### TCP

基于以太网TCP/IP的TCP传输模式，适用于TCP或者UDP连接，默认端口为502。

要学习ModbusTCP，需要先了解MBAP包头，MBAP是ModbusTCP帧前7个字节。

MBAP主要有以下几个字段

|字段|字节数|功能
|--|--|--|
|Transaction ID|2|事务ID，默认值0，通常在于并发通讯中，区分不同的事务。常用于master在某时刻并发发送多个请求，而不必每次请求都要按顺序阻塞等待slave回复的场合|
|Protocol ID|2|协议ID，默认值0，用于区分自定义的协议，这个字段很像HTTP1.0、1.1、2.0的区别，用户可以根据值不同，自行实现特定的协议，如：<br>值0表示标准的Modbus TCP协议，类似HTTP1.0<br>值1表示用户实现加长版的数据帧，支持每帧10000字节传输，类似HTTP1.1<br>值2表示用户实现的一帧实现5个非连续的寄存器读取，类似HTTP2.0|
|Length|2|表示从长度字段后开始计算，本帧有多少字节|
|Unit ID|1|默认值0，类似于RTU中的Slave ID，用于非TCP/IP协议栈的场合，如桥接网卡下的区分对端。在标准TCP/IP协议下，此字段的值可以忽略，因为TCP/IP模型默认已经是一对一通讯，不需要区分对端。|

Modbus TCP数据帧实际上是PDU加上7字节的MBAP而成。如下图，展示了Modbus TCP与PDU的关系

![](/images/modbus-frame/modbus-tcp-apu.png)


帧格式

|位置|Tran. ID|Prot. ID|Length|Unit ID|功能码|数据N字节|
|--|--|--|--|--|--|--|
|帧内容|2字节|2字节|2字节|1字节|1字节|N字节|

封包步骤

1. 封装好PDU，即功能码加数据
2. 封装好MBAP，其字段Transaction ID可以根据实际需要进行计数器自增
3. MBAP放在PDU前，得到二进制流，完成封包

备注
- Modbus TCP中PDU的编码方式，没有特别标明为ASCII还是RTU。为了提高传输效率，默认情况下为RTU，即直接二进制发送，而不经过复杂ASCII编码。
- Modbus TCP协议有几种变种，如Modbus RTU over TCP/IP，其定义为Modbus TCP加上CRC末尾。不同变种可以参考[维基百科](https://en.wikipedia.org/wiki/Modbus#Protocol_versions)

### 实践

以功能号04H为例，假设欲读取的slave ID为16，起始寄存器地址0，读取个数为2。

使用三种传输格式，均为大端存储，封包完成的二进制流对比：

|字段|RTU|ASCII|TCP|
|--|--|--|--|
|头部||':'|MBAP|
|Slave ID|0x0F|"0F"|0x0F|
|功能号|0x04|"04"|0x04|
|起始地址H|0x00|"00"|0x00|
|起始地址L|0x00|"00"|0x04|
|寄存器个数H|0x00|"00"|0x00|
|寄存器个数L|0x02|"02"|0x02|
|校验和H|0x71|"F9"||
|校验和L|0xCB||
|尾部||CRLF||

上面表格来源：[Function 04 (04hex) Read Input Registers](https://www.modbustools.com/modbus.html)


## 常见功能码

功能码基本上是根据PLC术语定义的。

|PLC英文|术语|理解|数据类型|
|--|--|--|--|
|Coil|线圈|开关输出信号，可读写|布尔型|
|Discrete Input|离散量|输入信号，不能被写入|布尔型|
|Input Register|输入寄存器|只能读，不能被写入|WORD,2字节|
|Holding Register|保持寄存器|可读写|WORD,2字节|

我习惯按读取数据类型分类功能号。参考[常见的功能号]([http://www.simplymodbus.ca/FC01.htm](http://www.simplymodbus.ca/FC01.htm))

下表是读写布尔型（点击中文，电梯直达）：

|功能十进制|十六进制|英文|中文|最小数据单位|
|--|--|--|--|--|
|01|01|Read Coils|[读多个线圈](#读多个线圈)|bit（布尔值）|
|05|05|Write Single Coil|[写单个线圈](#写单个线圈)|bit（布尔值）|
|15|0F|Write Multiple Coils|[写多个线圈](#写多个线圈)|bit（布尔值）|
|02|02|Read Discrete Inputs|[读多个离散输入](读多个离散输入)|bit（布尔值）|

下表是读写16bit数据（点击中文，电梯直达）：

|功能十进制|十六进制|英文|中文|最小数据单位|
|--|--|--|--|--|
|04|04|Read Input Registers|[读多个输入寄存器](#读多个输入寄存器)|16bit|
|03|03|Read Holding Registers|[读多个保持寄存器](#读多个保持寄存器)|16bit|
|06|06|Write Single Register|[写单个保持寄存器](#写单个保持寄存器)|16bit|
|16|10|Write Multiple Registers|[写多个保持寄存器](#写多个保持寄存器)|16bit|

这些各种骚操作都是在PDU层，即只考虑“功能码”+“数据”，与ASCII和RTU等传输模式无关。

当PDU构造好后，才进行对应的ASCII、RTU、TCP等通讯模式封包。

### 读写布尔型

#### 读多个线圈

读取从20到56号的线圈状态

master请求帧PDU

    01 0014 0025

- 01: 功能号01H
- 0014: 14H是20号，作起始地址
- 0025: 25H是十进制37，20到56共有37个线圈

slave回应帧PDU

    01 05 CD6BB20E1B

- 01: 功能号01H
- 05: 接下来有5个字节(37/8=5bytes)
- CD: 线圈 27 - 20 (1100 1101) 高位为线圈27，低位为线圈20
- 6B: 线圈 35 - 28 (0110 1011)
- B2: 线圈 43 - 36 (1011 0010)
- 0E: 线圈 51 - 44 (0000 1110) 
- 1B: 线圈 56 - 52 (0001 1011) 最后一字节仅提供低5位信息，其余位用0填充

返回的线圈每字节的高位比特，表示线圈数较大的布尔值；低位比特，表示线圈较小的布尔值。如上回复中，线圈43为高电平，线圈36为低电平。

#### 写单个线圈

写线圈173号，设置为高电平（ON）

master请求帧PDU

    05 00AD FF00

- 05: 功能号05H
- 00AD: ADH对应十进制173，作起始地址
- FF00: 高电平，( FF00 = ON,  0000 = OFF )

若slave回应正常，则返回与master请求的相同PDU给master

    05 00AD FF00

#### 写多个线圈

将线圈20到29写入新的值

master请求帧PDU

    0F 0014 000A 02 CD01

- 0F: 功能号0FH
- 0014: 14H对应十进制20，作起始地址
- 000A: 0AH对应十进制10，写入10个线圈（从20到29）
- 02: 接下来有2个字节(10/8=2bytes)
- CD: 线圈 27 - 20 (1100 1101) 高位为线圈27，低位为线圈20
- 01: 线圈 29 - 28 (0000 0001) 最后一字节仅提供低2位信息，其余位用0填充

返回的线圈每字节的高位比特，表示线圈数较大的布尔值；低位比特，表示线圈较小的布尔值。如上请求中，线圈20为高电平，线圈21为低电平。

slave回应帧PDU

    0F 0014 000A

- 0F: 功能号0FH
- 0014: 14H对应十进制20，作起始地址
- 000A: 0AH对应十进制10，写入10个线圈（从20到29）

#### 读多个离散输入

读取从196到217离散输入。

master请求帧PDU

    02 00C4 0016

- 02: 功能号02H
- 00C4: C4H对应十进制196，表示离散输入的起点
- 0016: 16H对应十进制22，表示读取22个离散输入（从196到217）

slave回应帧PDU

    02 03 ACDB35

- 02: 功能号02H
- 03: 接下来有2个字节(22/8=3 bytes)
- AC: 离散输入 203 - 196 (1010 1100) 高位为203，低位为196
- DB: 离散输入 211 - 204 (1101 1011)
- 35: 离散输入 217 - 212 (0011 0101) 最后一字节仅提供低6位信息，其余位用0填充

返回的线圈每字节的高位比特，表示线圈数较大的布尔值；低位比特，表示线圈较小的布尔值。如上回应中，离散203为高电平，离散196为低电平。

### 读写16位

#### 读多个输入寄存器

读取2个输入寄存器的值，从8开始。

master请求帧PDU

    04 0008 0002

- 04: 功能号04H
- 0008: 8H对应十进制8，作起始地址
- 0002: 读取2个寄存器

slave回应帧PDU

    04 04 000A0003

- 04: 功能号04H
- 04: 接下来有4个字节，对应2个输入寄存器（16bit x 2 = 32bit）
- 000A: 输入寄存器8的值
- 0003: 输入寄存器9的值

#### 读多个保持寄存器

功能号为03H，与[读多个输入寄存器](#读多个输入寄存器)操作一致。

#### 写单个保持寄存器

向保持寄存器1写入新的值

master请求帧PDU

    06 0001 0003

06: 功能号06H
0001: 1H对应十进制1，作起始地址
0003: 新的值

若slave回应正常，则返回与master请求的相同PDU给master

    06 0001 0003

#### 写多个保持寄存器

向PLC保持寄存器1和2写入新的值。

master请求帧PDU

    10 0001 0002 04 000A 0102

10: 功能号10H
0001: 1H对应十进制1，作起始地址
0002: 寄存器个数=2
04: 接下来有4个字节 (2寄存器 x 2字节 = 4)
000A: 寄存器 1 的新值
0102: 寄存器 2 的新值

slave回应帧PDU

    10 0001 0002

10: 功能号10H
0001: 1H对应十进制1，作起始地址
0002: 寄存器个数=2

## 后记

- 在PLC中，大部份地址1表示第一个线圈或者寄存器，而在PDU封包过程中，地址0表示第一个。因此实际可能有一定的偏移值，都是可以自定义的。
- 如果整个主从设备都是自己设计，就无所谓什么读保持寄存器还是写保持寄存器，可能只用到两个功能号（03H和10H），如果是主从设备其中一个是别人做的，大家就必须要遵守Modbus通用协议了。
- 在PDU的“数据”字段中，对8bit以上的数据类型（如short, int、int64、float，double），可以不使用Modbus钦定的大端存储，选择小端、交换序都可以，只要master和slave使用相同的数据存储方式，就能保证通讯正常。常见的几种字节序：

|字节序|对四字节数据ABCD存放|
|--|--|
|Big-endian|AB CD|
|Little-endian|CD AB|
|Big-endian byte swap|BA DC|
|Little-endian byte swap|DC BA|

- 使用03H功能码，读取非整数倍16bit的数据时，注意最后一个字节是否越界。因为Modbus都是按16bit对齐的，忽然冒出一个8bit可能会对齐期间越界。

## 参考文章

[MODBUS PROTOCOL](http://www.modbus.org/specs.php)

[Modbus interface tutorial](https://www.lammertbies.nl/comm/info/modbus)

[PDF - INTRODUCTION TO MODBUS TCP/IP](https://www.prosoft-technology.com/kb/assets/intro_modbustcp.pdf)