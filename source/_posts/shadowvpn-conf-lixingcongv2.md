title: 我的ShadowVPN配置记录v2
date: 2015-12-17 16:41:48
tags: shadowsocks
categories: 网络
---
无论什么时候，我总希望梯子要多备一把，防止某天走出不去。
<!-- more -->
本人日常使用的是SS，发现在代理某些udp流量（例如语音流、外服游戏数据包）有点力不从心，因此尝鲜试了这个ShadowVPN，可以做到全局代理

```2018-06-11```友情提示：该shadowvpn版本较老，作者基本不再维护，而且国内ISP对UDP丢包近似疯狂状态，不建议使用。

```2018-06-10```友情提示：文档太老，本文某些内容（依赖库或者相关URL链接）或者已经发生变化

## 安装

在vps（ubuntu 14.04.3 x64）上面操作：

### 方法一：源安装

vi /etc/apt/sources.list   添加

	deb http://shadowvpn.org/debian wheezy main
    
更新源，安装

    apt-get update
	apt-get install shadowvpn
    
运行：

	/etc/init.d/shadowvpn start
    
### 方法二：编译安装

#### 获取源码

安装必要的编译工具

	apt-get install build-essential mingw-w64 autoconf libtool

获取ShadowVPN源码，以0.2.0为例（该版本发布于2015.8，从此再也没有更新了，作者被喝茶你懂的）

	cd ~
    wget https://github.com/rains31/ShadowVPN/archive/0.2.0.tar.gz
    tar -zvf 0.2.0.tar.gz
    cd ShadowVPN-0.2.0
	
0.2.0版本没有更新debian/changelog中的版本号码，编译出来的显示是0.1.7，解决方法打开debian/changelog自己手动替换版本号0.1.7为0.2.0即成为编译0.2.0（强迫症患者可以试试）。

更新[libsodium](https://github.com/jedisct1/libsodium/releases) 这个crypto库。以libsodium 1.0.10版为例。

	rm -rf libsodium
	wget https://github.com/jedisct1/libsodium/releases/download/1.0.10/libsodium-1.0.10.tar.gz
	tar -zxf libsodium-1.0.10.tar.gz
	cd libsodium-1.0.10
	./autogen.sh
	./configure
	make && make install
	ldconfig #刷新链接库
	cd ..
	mv libsodium-1.0.10 libsodium
	
进行AutoGen生成makefile脚本

	sh ./autogen.sh

#### 编译出linux版

	./configure --enable-static --sysconfdir=/etc
	make && make install
    
编译成功，可以试试运行服务端程序

	shadowvpn -c /etc/shadowvpn/server.conf -s start
	
提示：安装后建议不删shadowvpn文件夹，方便日后卸载：

	make uninstall
    
#### 编译出deb package

    apt-get install libssl-dev gawk debhelper dh-systemd init-system-helpers pkg-config
    dpkg-buildpackage -b -us -uc -i
    cd ..
    ll | grep shadowvpn
	
    
安装deb

	dpkg -i shadowvpn*.deb #根据编译出的deb确定文件名
    apt-get install -f #修复依赖关系
    
某一天不想用Shadowvpn了，可以卸载dpkg包

	dpkg -l | grep shadowvpn #理论上应该显示shadowvpn包的名称
    dpkg -r shadowvpn #仅删除程序
    dpkg -P shadowvpn #把残留的配置文件删掉
    

#### 编译出windows客户端

想偷懒的话，可以直接[下载我的预编译版本](/attachments/my_conf_of_shadowvpn/shadowvpn-win-0.2.0.tar)，不需要自行编译了。

获取源码方法跟上面linux客户端一致

	cd ~/ShadowVPN-0.2.0
	./configure --host=i686-w64-mingw32 --enable-static
	make clean
	make
    
耗时大概一分钟，然后从src目录下获得一个shadowvpn.exe，使用winscp复制到win系统备用。

#### 编译出openwrt客户端

这里假设你已经掌握了Openwrt SDK的使用方法，没有的话自行学习，比较简单，先试试路由器上移植ipk，实现在路由上面printf("hello world")，参考这篇文章：[Print Hello on Openwrt](https://www.componentality.com/res/Step-By-Step-Instruction-To-Run-Apps-On-FlexRoad-HW.en.pdf)

编译ShadowVPN openwrt客户端，请移步至[《从Openwrt SDK编译出ShadowVPN包》](/2016/01/02/compile_shadowvpn_for_openwrt)

## 配置

### 服务端

在vps上面操作：

	cd /etc/shadowvpn

这是server.conf

![](/images/shadowvpn_conf/s_vpn1.png)

这是server_up.sh

![](/images/shadowvpn_conf/s_vpn2.png)

我修改的小结：
- 打开server.conf文件
- 若要监听ipv6，修改server=::0
- 修改port=666
- 修改密码
- 添加token，方便多用户，不填就默认单用户
- 修改mtu
- 修改子网netmask(CIDR格式)

（可选）生成token的命令

	xxd -l 8 -p /dev/random

*PS 1:修改mtu这一步很重要，会直接影响vpn速度。*
详细的mtu设置讨论帖子：[帖子点击这里](https://github.com/clowwindy/ShadowVPN/issues/77)
大概意思就是如帖子里面linhua55所说的方法：

    MTU of PPPoE is 1452, But it differs between your network.
    首先用Windows插上网线
    进行正常的拨号（若是DHCP，则跳过这步，主要目的是模拟路由器拨号）
    cmd执行 ping -f -l 1452 www.baidu.com
    逐步加大或者减少该值，直到恰好不会出现DF拆包的提示。记下这个mtu
    例如，记下的mtu是1492，计算得到1492-20-8-24=1440（这是ipv4，对ipv6再减20,）
	注意：mtu减去多少，取决于你的shadowvpn版本，详细看server.conf里面的mtu注释
    那么在服务端的server.conf填入1440
    客户端也一致填写1440
    
*PS 2:设置内网netmask这一步也很重要，避免shadowvpn与isp分配到的地址冲突。*
参考[维基百科](https://en.wikipedia.org/wiki/Private_network)进行内网的设置。这里都是假设你具有基本的网络层ip知识。如果你对计算机网络ip不那么熟悉，建议保持shadowvpn默认值网关10.7.0.1。

	首先在电脑上拨号，查看isp给的是否是内网地址
    若是公网地址，那么恭喜你，保留默认的netmask即可
    若是内网地址（10、100、172开头的ip地址），需要参考维基百科的地址设置一个不冲突的内网地址
    比如我的isp分配的地址是10.9.120.2，那我可以在server.conf设netmask为为172.16.0.1/16（不唯一）
    客户端的mtu与服务端一致

### 客户端

#### linux版

##### 配置
修改配置文件与服务器配置一致：
(如：ip,port,MTU,token值,password,netmask,CIDR地址)

	vi /etc/shadowvpn/client.conf
	# net=10.7.0.2/xx
    # 计算规则是把服务端 net=10.7.0.1/xx 作为网关加上1，总之跟网关在同一个网段即可



##### 启动

修改启动模式为客户端。

	vi /etc/default/shadowvpn
    把 CONFIG 改为 /etc/shadowvpn/client.conf
    
禁止开机自启动。否则每次开机都是全局代理

    sudo update-rc.d shadowvpn disable
    
使用谷歌dns防止污染
    
    vi /etc/shadowvpn/client_up.sh
    # 在倒数第三行加上
    echo 'namesever 8.8.8.8' > /etc/reslov.conf
    
    vi /etc/shadowvpn/client_down.sh
    # 在倒数第三行加上
    echo 'namesever 114.114.114.114' > /etc/reslov.conf
    
然后重启一下进程

    /etc/init.d/shadowvpn restart

若是编译安装，直接运行重启进程

	shadowvpn -c /etc/shadowvpn/client.conf -s restart
    
##### 测试
测试一下tun通道是否成功：

	# 观察tun0的数据RX TX变化
    # 若TX有数值，而RX没有数值，说明被丢包或者配置错误
	ifconfig
    
    # 观察ping延迟
    ping [YOUR_SHADOWVPN_GATEWAY] -w 5
    
##### 停止

跟启动差不多，只需把start换成stop

#### windows版

##### 配置

先安装tun/tap隧道：[安装地址(需翻墙)](http://build.openvpn.net/downloads/releases/)
选择并下载tap-windows-xxx.exe安装

假设我们通过**无线网络连接**来进行vpn
安装后，重命名网络连接名称（共需重命名2个）

![](/images/shadowvpn_conf/s_vpn3.png)

把编译后的shadowvpn.exe放到一个目录下
然后[下载client.conf等文件](https://github.com/lixingcong/shadowVPN/tree/master/samples/windows)，放到同目录
	
    client.conf
    client_up.bat
    client_down.bat
    

改动client.conf使得跟服务端一样

若有user_token，也要照样在client.conf里填上。

**tunip=服务端的server.conf中的网关+0.0.0.1**
如果我的Shadowvpn服务端netmask设置为10.7.0.1，那么：

	tunip=10.7.0.2

改动tun接口名称（就是**改名后的tun通道**）

	intf=vpn
    
    
打开client_up.bat和client_down.bat，**同时改动两个文件里面的两个值**
改动服务端的网关

	remote_ip=10.7.0.1
    
改动“无线网络”英文名：（若使用有线，请改为以太网对应的名称）

	orig_intf="Wi-Fi"
    
注意在server_up.bat中修改与服务端一致的netmask：如255.255.255.0

##### 启动

创建一个快捷方式，输入

	shadowvpn.exe -c client.conf -s start

使用**管理员权限**运行即可
待出现 “ server_up.bat Done ” 即可上网

##### 停止

注意不能点击右上角x关闭，要按ctrl+c进行还原操作关闭

#### openwrt版
详见[openwrt-shadowvpn项目](https://github.com/aa65535/openwrt-shadowvpn/releases)，自行参考上文的配置

对于 DNS 污染，可以直接使用 Google DNS 8.8.8.8，或者使用 ChinaDNS 综合使用国内外 DNS 得到更好的解析结果。

最傻瓜的抗污染：使用Chinadns设置上游服务器114.114.114.114,8.8.8.8，然后让dns走加密隧道
路由追踪一下是否走Shadowvpn,正常来说应该能走你的vps路由。

	traceroute 8.8.8.8 

在实际使用中，发觉ChinaDNS偶尔会阻塞，具体表现为无法获得国外ip，我也搞不清是什么原因

另一种方式是使用dnsmasq（2.73以上），使用openwrt(15.05.1以上)预装的dnsmasq 2.73可以过滤DNS解析，效果等同于ChinaDNS，比ChinaDNS工作要稳定！推荐大家使用。

到aa65535的github页面[下载一份带有黑名单的默认配置](https://github.com/aa65535/openwrt-dnsmasq/blob/master/etc/dnsmasq.d/ignore-address.conf)加入到/etc/dnsmasq.conf，重启服务

	/etc/init.d/dnsmasq restart

即可达到过滤虚假dns答复的效果。如果你懂ipset+dnsmasq的使用，可以有更多的玩法（去广告、cdn加速、屏蔽虚假ip）！


## 其他

### 梯子优化
若vps是kvm或者xen可以参考以下优化：[clowwindy的优化方法](https://github.com/shadowsocks/shadowsocks/wiki/Optimizing-Shadowsocks)

	vi/etc/security/limits.conf
    * soft nofile 51200 #追加这句
	* hard nofile 51200 #追加这句
	vi /etc/profile
    ulimit -SHn 51200 #追加这句

fast_open是给tcp用的，对udp没什么卵用。

### vps上自建dns服务器

在vps上面安装pdnsd或者dnsmasq，监听5353端口的dns请求。
有个奇怪的问题：搬瓦工的装pdnsd会启动会被kill掉，内核太老了吗？我用的机子另一家KVM就能启动pdnsd正常。

假设vps的tun0网关是10.7.0.1/24，且在vps监听DNS端口5353，那么
在路由器的ChinaDNS上游dns填入

	114.114.114.114,10.7.0.1:5353
	
或者dnsmasq的上游服务器

	server=10.7.0.1#5353
	# 配合dns过滤（dnsmasq 2.73+）
	ignore-address=1.2.3.4
	
效果：vps作为dns查询服务器，vps与路由器通信走的是Shadowvpn的加密流量。

### 使用问题

目前使用ShadowVPN遇到的问题：
- 搬瓦工使用ipv6地址时候，客户端填入xxxx:xxxx:xxxx:xxxx::1会失败，填入xxxx:xxxx:xxxx:xxxx::成功。好诡异。差一个1就这样
- 加载并发多线程的网页（例如tumblr加载图片）的效率明显不如shadowsocks，而对于连续的udp数据流，则比ss流畅（例Youtube）
- 某些地区对udp包丢包严重：例如广东电信，根本连不上shadowvpn。
- 高峰期稳定性不如ss，因为对于ISP来说，udp包优先级低于tcp，受到岐视被丢包嘛！
- 偶尔Connection reset，具体表现为ssh中断，或者网页打不开，刷新就ok

### 后记

窃认为，Shadowvpn是一个比Shadowsocks更强大的工具，可惜的是，在遭受政府审查中流产了。留下给我们的，是一个早产、带有各种未知bug的版本，使用上会出现各种不方便，难以开展多用户，不适合商家售卖等诸多缺点。

因此，各位与GFW斗争的勇士应该知道，在互联网上做这样擦边球的应用，切记人身安全第一，要开小号，匿名。参考[“编程随想”博客:《如何防止跨省追捕》](https://program-think.blogspot.com/2010/04/howto-cover-your-tracks-0.html)

截止2015.8.22,作者已经停止维护ShadowVPN，并删除相关代码，因为众所周知的喝茶原因。
![](/images/shadowvpn_conf/tea.png)

最后，请仔细阅读下面的话，姑且称之为作者有话说。@clowwindy的原话：

> Shadowsocks没有办法离开去中心化的服务器。要么自己花钱买 VPS，要么用有人分享的账号，要么用有人提供的付费服务，他们各有所长，适合不同的人。所以作为开发者，保持中立，不偏袒其中任何一方，顺其自然发展下去是最好的吧。
> 
> 很多人要么一窝蜂的支持，要么一窝蜂的反对，还要把它给封禁掉，大概这种心理鲁迅先生也曾批判过。如果你们真的那么讨厌商业，那你们应该首先把你们的苹果设备给摔了，因为它就是商业社会巅峰造极的产物。我反对不喜欢一个东西就要拿出简单粗暴的制裁手段，正是这种习性成就了 GFW。
>
> 维护这个项目到现在大概总共回复过几千个问题，开始慢慢想清楚了一件事，为什么会存在 GFW。从这些提问可以看出，大部分人的自理能力都很差，只是等着别人帮他。特别是那些从 App Store 下载了 App 用着公共服务器的人，经常发来一封只有四个字的邮件：“不能用了？” 我觉得这是一个社会常识，花一分钟写的问题，不能期待一个毫无交情的陌生人花一个小时耐心地问你版本和操作步骤，模拟出你的环境来帮你分析解决。
> 
> Windows 版加上 GFWList（注：就是自动分流，PAC） 功能以来，我反复呼吁给 GFWList 提交规则，但是一个月过去了竟然一个提交都没有。如果没有人做一点什么，它自己是不会更新的啊，没有人会义务地帮你打理这些。我觉得，政府无限的权力，都是大部分人自己放弃的。假货坑爹，让政府审核。孩子管不好，让政府关网吧。房价太高，让政府去限购。我们的文化实在太独特，创造出了家长式威权政府，GFW 正是在这种背景下产生的，一个社会矛盾的终极调和器，最终生活不能自理的你每天做的每一件事情都要给政府审查一遍，以免伤害到其他同样生活不能自理的人。这是一个零和游戏，越和这样的用户打交道，越对未来持悲观态度，觉得 GFW 可能永远也不会消失，而墙内的这个局域网看起来还似乎生机勃勃的自成一体，真是让人绝望。

clowwindy是我在Twitter上面最崇拜的人，他耐心解答问题，为人低调。不过最近避风头没有在出现过我的视线了。哎，可惜。

