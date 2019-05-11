title: 朋友圈小视频替换大法
date: 2016-03-28 15:26:52
tags: Android
categories:	读书笔记
---
由于微信用户群体太大了，因此他有骨气、有自信、有政府支持才可以做得越来越封闭，以前跟支付宝闹过矛盾互相封杀链接，现在不但封杀各种第三方链接，还把上传小视频的来源限定为微信软件拍摄，无法选定系统中视频上传。
<!-- more -->
微信如此“牛逼”，目的无非就是方便某些政治“维稳”目的——不是你亲眼拍摄的视频就无法扩散传播。对“谣言”抑制传播起到一定作用。但是对于一个技术宅来说，受到软件上的约束真是受不了，赶紧把这个限制自己突破！

实测微信Android版本6.20.52_r1162382可以替换自己拍摄的视频文件并发送到朋友圈，但是，更高的版本就不能简单地进行替换了。微信将视频md5存档到数据库EnMicroMsg.db中了，而且还是加密存放。

```2018-06-11```友情提示：微信从某个版本开始不能替换字段了，本文仅作参考，没有什么实际使用价值。


2015年末已经有人开发了一个小程序来实现新版微信的替换视频软件：[朋友圈发送任意小视频工具V2.0](http://download.csdn.net/detail/pygyh/9435049)，为了了解一下该软件工作原理，我特意使用apktool和jd-gui进行反汇编。通过反汇编研究作者的思路：操作sqlcipher API实现替换字段，我记下来并自己实现一遍，学到不少东西。

## 方法

### 记下视频文件名

在微信随意拍摄一个小视频，选择临时保存小视频。记下如下目录中的视频文件名。文件名是128bit的MD5格式。（实际上不是视频的md5值），其中没有后缀的是视频文件。有thumb后缀的是缩略图，缩略图可以替换。

	/sdcard/tencent/MicroMsg/xxxxxxxxxxxxxxxx/draft/

PS：上面xxxxxxxxxxxxxxxx每个账号变化不一样，一般是128bit的长度（32字节）。比如我的就是748a453710c416e0bd000e011706f2c4

### 拷贝数据库

拷贝操作要求root手机，否则打不开data文件夹，听说未root的机器可以开发者模式把data文件夹弄出来，我没试过。

拷贝下面两个文件到电脑同一个文件夹中

	/data/data/com.tencent.mm/MicroMsg/xxxxxxxxxxxxxxxx/EnMicroMsg.db
	/data/data/com.tencent.mm/shared_prefs/system_config_prefs.xml

### python解密sql

下载我写的这个python脚本，这是我参考[How To Decrypt WeChat EnMicroMsg.db Database][0]文章中的脚本的，我添加了加密数据库的功能，方便打包返回微信data文件夹。关键代码仅作部分删除工作

下载地址：[fwd_wetchatdecipher.py](/attachments/wechat_decrypt_database/fmd_wechatdecipher.py)

python脚本依赖第三方库：[sqlcipher](https://www.zetetic.net/sqlcipher/)，首先安装它。
运行fmd_wetchatdecipher.py，输入你的IMEI号码，可以拨号输入*#06#获得。脚本的功能是先解密EnMicroMsg.db得到EnMicroMsg-decrypted.db

### SQL修改数据库

然后使用数据库软件比如[SQLiteStudio](http://sqlitestudio.pl/)打开这个解密的数据库。修改表SightDraftInfo，将已经拍摄到的小视频正确的文件对应的md5数据替换为自己想要发送的视频md5。md5的获取可以使用流行的md5校验软件。

### python加密sql

运行fmd_wetchatdecipher.py，输入你的IMEI号码，选择加密，则生成EnMicroMsg-encrypted-NEW.db，把它改名替换原先的EnMicroMsg.db，记得删掉手机上的EnMicroMsg.db.ini，该文件是对数据库的md5校验。

### 装逼时刻

然后重新打开微信，在最近的14天小视频中就可以看到你替换后的视频。
正是因为多了一步加密解密数据库，微信6.20.52版本后自定义视频就显得更麻烦了。因此我也准备开发同样一个插件，方便各位装逼的小伙伴发送自定义视频。

## 原理

解密原理我是参考[How To Decrypt WeChat EnMicroMsg.db Database][0]，本人仅作翻译

### EnMicroMsg.db与SQLCipher

EnMicroMsg.db是微信的单个账号数据库，存放各种聊天记录，联系人信息，甚至是腾讯在后台偷偷上传的信息，该数据库使用SQLCipher加密(AES-256)

该EnMicroMsg.db的加密操作API为

    PRAGMA key = KEY; # 这是key
    PRAGMA cipher_use_hmac = off; # 禁用HMAC验证，以兼容sqlcipher 1.1.x数据库
    PRAGMA cipher_page_size = 1024;
    PRAGMA kdf_iter = 4000; # 迭代次数，需要指定。否则新版sqlcipher无法解密

参考[SQLcipher API](https://www.zetetic.net/sqlcipher/sqlcipher-api)，获取更详细的操作方法。

### key的获取

key的生成公式：

	KEY = MD5( IMEI + UIN ) [0:7]

因此我们可以导入IMEI和UIN实现python解密数据库。
    
![](/images/wechat_decrypt_database/key-generate.png)

![](/images/wechat_decrypt_database/key-generate2.png)

### 微信安全性启示

> 要先批评一下微信，居然用开源的数据库加密方式，这不是一破解一个准吗？

国外机构对微信的安全分析：[A look at WeChat security](http://blog.emaze.net/2013/09/a-look-at-wechat-security.html)

看了上面这篇英文的文章，对微信真是无言了，2013年的文章说是这样加密方式，到今天2016年还是那套老套路，依旧是md5 password，不怕被攻击吗？！

## 参考资料

[How To Decrypt WeChat EnMicroMsg.db Database][0]
[利用Android手机破解微信加密数据库EnMicroMsg.db][1]
[Android微信数据导出][2]
[微信聊天记录分析][3]

[0]:https://articles.forensicfocus.com/2014/10/01/decrypt-wechat-enmicromsgdb-database
[1]:http://blog.csdn.net/yuanbohx/article/details/41674949
[2]:https://maskray.me/blog/2014-10-14-wechat-export
[3]:https://blog.slinuxer.com/2015/10/%E5%BE%AE%E4%BF%A1%E8%81%8A%E5%A4%A9%E8%AE%B0%E5%BD%95
