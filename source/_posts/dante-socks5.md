---
title: dante搭建socks5代理
date: 2018-05-25 10:11:07
tags: shadowsocks
categories: 网络
---
服务器上搭建带认证Socks5代理，供Telegram等程序使用。
<!-- more -->
俄罗斯不是封堵电报嘛，Telegram老板们呼吁大家搭建socks代理突破封锁。。。

## Build

当前用的版本是1.4.2，建议使用新版本。官网[下载地址](https://www.inet.no/dante/download.html)。

	wget https://www.inet.no/dante/files/dante-1.4.2.tar.gz
	tar xf dante-1.4.2.tar.gz
	cd dante-1.4.2
	autoreconf --install --force
	./configure
	make install
	
编译完产生的二进制文件叫sockd，以前的老版本是叫danted

## Configuration

创建并编辑/etc/sockd.conf，修改为如下，该配置参考自[这篇博客](https://www.binarytides.com/setup-dante-socks5-server-on-ubuntu)

	# /etc/sockd.conf

	logoutput: syslog
	user.privileged: root
	user.unprivileged: nobody

	# The listening network interface or address.
	internal: 0.0.0.0 port=1080

	# The proxying network interface or address.
	external: eth0

	# socks-rules determine what is proxied through the external interface.
	# The default of "none" permits anonymous access.
	socksmethod: username

	# client-rules determine who can connect to the internal interface.
	# The default of "none" permits anonymous access.
	clientmethod: none

	client pass {
		from: 0.0.0.0/0 to: 0.0.0.0/0
		#log: connect disconnect error
	}

	socks pass {
		from: 0.0.0.0/0 to: 0.0.0.0/0
		#log: connect disconnect error
	}

将上述配置文件中的external修改为VPS实际的流量出口interface，比如我的openVZ机子是venet0:0

创建一个用户并修改密码，用于，赋予最低的权限。

	useradd -r -s /bin/false USERNAME
	passwd USERNAME

## Test

使用内置的-V命令测试sockd.conf文件是否正确，若正确则无任何输出

	sockd -V

直接以daemon模式运行即可

	sockd -D

使用Telegram设置该地址为socks代理，填入刚才新建的用户名即可。

## Access Control

需求如下：socks5服务器仅代理Telegram的IP段（类似于Shadowsocks-libev的ACL功能），对其它代理请求一律拒绝

只需把sockd.conf最后的

	socks pass {
		from: 0.0.0.0/0 to: 0.0.0.0/0
		#log: connect disconnect error
	}

替换为

	socks pass {
		from: 0.0.0.0/0 to: 91.108.4.0/22
	}

	socks pass {
		from: 0.0.0.0/0 to: 91.108.8.0/21
	}

	socks pass {
		from: 0.0.0.0/0 to: 91.108.16.0/21
	}

	socks pass {
		from: 0.0.0.0/0 to: 91.108.36.0/23
	}

	socks pass {
		from: 0.0.0.0/0 to: 91.108.38.0/23
	}

	socks pass {
		from: 0.0.0.0/0 to: 91.108.56.0/22
	}

	socks pass {
		from: 0.0.0.0/0 to: 149.154.160.0/2
	}

	socks block {
		from: 0.0.0.0/0 to: 0.0.0.0/0
	}

参考dante官方的文档，client规则和socks规则的区别：

|项目|区别|备注|
|--|--|--|
|Client规则|对于限制客户端接入，服务端有选择地拒绝建立TCP|工作在TCP层，优先于Socks规则|
|Socks规则|对于已经accept connection的连接，服务端有选择的拒绝转发Socket|工作在Socks层|

所以Client规则是在TCP的accept阶段进行控制，Socks规则是满足Client规则后且建立TCP连接后的Sock层控制。有顺序之分。

