---
title: ss-redir配合ipset转发TCP+UDP
date: 2018-06-11 16:47:36
tags: shadowsocks
categories: 网络
---
想要使用ss-redir代理所有非中国大陆的UDP流量（用于打游戏），需要将ss的项目脚本稍作修改。本文提出一种修改后的脚本方案，可以支持代理不仅仅是53端口的UDP流量，而是所有境外IP的UDP流量。
<!-- more -->
我的电脑是Ubuntu 18.04，现假设ss的服务器IP为```123.123.123.123```，本地ss-redir端口为```12345```（下文同）

广告：推荐使用搬瓦工(BandwagonHost)，一键部署shadowsocks，轻松切换机房，[点我直达购买](https://bandwagonhost.com/aff.php?aff=17660)搬瓦工!

要想代理UDP流量，首先要在服务端开启UDP转发（这不是废话）。ss-server启动参数带上```-u```即可。

## 最简单的例子

在shadowsocks-libev[项目主页](https://github.com/shadowsocks/shadowsocks-libev)上的ss-redir例子是如下

	iptables -t nat -N SHADOWSOCKS
	iptables -t mangle -N SHADOWSOCKS

	iptables -t nat -A SHADOWSOCKS -d 123.123.123.123 -j RETURN

	iptables -t nat -A SHADOWSOCKS -d 0.0.0.0/8 -j RETURN
	iptables -t nat -A SHADOWSOCKS -d 10.0.0.0/8 -j RETURN
	iptables -t nat -A SHADOWSOCKS -d 127.0.0.0/8 -j RETURN
	iptables -t nat -A SHADOWSOCKS -d 169.254.0.0/16 -j RETURN
	iptables -t nat -A SHADOWSOCKS -d 172.16.0.0/12 -j RETURN
	iptables -t nat -A SHADOWSOCKS -d 192.168.0.0/16 -j RETURN
	iptables -t nat -A SHADOWSOCKS -d 224.0.0.0/4 -j RETURN
	iptables -t nat -A SHADOWSOCKS -d 240.0.0.0/4 -j RETURN

	iptables -t nat -A SHADOWSOCKS -p tcp -j REDIRECT --to-ports 12345

	ip route add local default dev lo table 100
	ip rule add fwmark 1 lookup 100
	iptables -t mangle -A SHADOWSOCKS -p udp --dport 53 -j TPROXY --on-port 12345 --tproxy-mark 0x01/0x01

	iptables -t nat -A PREROUTING -p tcp -j SHADOWSOCKS
	iptables -t mangle -A PREROUTING -j SHADOWSOCKS

	ss-redir -u -c /etc/config/shadowsocks.json -f /var/run/shadowsocks.pid
	
项目主页的这个例子，可以实现TCP全局代理，UDP只代理53端口。

## 代理分流(启动)

我们可以借鉴luci-app-shadowsocks项目的[ss-rules脚本](https://github.com/shadowsocks/luci-app-shadowsocks/blob/master/files/root/usr/bin/ss-rules)实现TCP+UDP代理。

更新中国大陆的路由表

	wget -O- 'http://ftp.apnic.net/apnic/stats/apnic/delegated-apnic-latest' | awk -F\| '/CN\|ipv4/ { printf("%s/%d\n", $4, 32-log($5)/log(2)) }' > ~/route.txt

为了减少路由表的体积，可以使用cidrmerge精简合并（可选步骤，实测体积减少20KB左右）

给出完整的启动脚本

	#! /bin/bash

	ss_server_ip=123.123.123.123
	ss_redir_port=12345
	ss_redir_pid=/var/run/shadowsocks.pid
	ss_config_file=/etc/config/shadowsocks.json
	chnroute_file="~/route.txt"

	# 开启redir
	ss-redir -u -c $ss_config_file -f $ss_redir_pid

	# IP内网地址
	BYPASS_RESERVED_IPS=" \
		0.0.0.0/8 \
		10.0.0.0/8 \
		127.0.0.0/8 \
		169.254.0.0/16 \
		172.16.0.0/12 \
		192.168.0.0/16 \
		224.0.0.0/4 \
		240.0.0.0/4 \
	"

	ipset create ss_bypass_set hash:net

	# 添加内网地址到ipset
	for line in $BYPASS_RESERVED_IPS; do
		ipset add ss_bypass_set $line
	done

	# 添加ss地址到ipset
	ipset add ss_bypass_set $ss_server_ip

	# 添加chinaroute到ipset
	if [ -f $chnroute_file ]; then
		IPS=`which ipset`
		$IPS -! restore <<-EOF || return 1
			$(egrep -v "^#|^$" $chnroute_file | sed -e "s/^/add ss_bypass_set /")
		EOF

		echo "China route was loaded"
	else
		echo "China route does not exist"
	fi

	# TCP规则
	iptables -t nat -N SHADOWSOCKS_TCP

	iptables -t nat -A SHADOWSOCKS_TCP -p tcp -m set --match-set ss_bypass_set dst -j RETURN
	iptables -t nat -A SHADOWSOCKS_TCP -p tcp -j REDIRECT --to-ports $ss_redir_port

	# Apply for tcp
	iptables -t nat -A OUTPUT -p tcp -j SHADOWSOCKS_TCP

	# UDP规则
	iptables -t mangle -N SHADOWSOCKS_UDP
	iptables -t mangle -N SHADOWSOCKS_UDP_MARK

	ip route add local default dev lo table 100
	ip rule add fwmark 1 lookup 100

	iptables -t mangle -A SHADOWSOCKS_UDP -p udp -m set --match-set ss_bypass_set dst -j RETURN
	iptables -t mangle -A SHADOWSOCKS_UDP -p udp -j TPROXY --on-port $ss_redir_port --tproxy-mark 0x01/0x01

	iptables -t mangle -A SHADOWSOCKS_UDP_MARK -p udp -m set --match-set ss_bypass_set dst -j RETURN
	iptables -t mangle -A SHADOWSOCKS_UDP_MARK -p udp -j MARK --set-mark 1

	# Apply for udp
	iptables -t mangle -A PREROUTING -p udp -j SHADOWSOCKS_UDP
	iptables -t mangle -A OUTPUT -p udp -j SHADOWSOCKS_UDP_MARK

	echo "ss-redir is loaded"

接着设定Ubuntu的DNS为8.8.8.8，或者使用各种抗DNS污染的方案，清理DNS缓存后就能实现TCP+UDP分流

如何验证UDP转发是有效的呢？可以在ss-redir启动时候使用```-v```参数调试。

## 代理分流(关闭)

开启代理分流后，如何恢复为启动ss-redir前的iptables规则呢？

给出完整的关闭脚本

	#! /bin/bash

	ss_redir_pid=/var/run/shadowsocks.pid

	# TCP规则
	iptables -t nat -D OUTPUT -p tcp -j SHADOWSOCKS_TCP
	
	iptables -t nat -F SHADOWSOCKS_TCP
	iptables -t nat -X SHADOWSOCKS_TCP

	# UDP规则
	iptables -t mangle -D PREROUTING -p udp -j SHADOWSOCKS_UDP
	iptables -t mangle -D OUTPUT -p udp -j SHADOWSOCKS_UDP_MARK

	iptables -t mangle -F SHADOWSOCKS_UDP
	iptables -t mangle -X SHADOWSOCKS_UDP

	iptables -t mangle -F SHADOWSOCKS_UDP_MARK
	iptables -t mangle -X SHADOWSOCKS_UDP_MARK

	ip route del local default dev lo table 100
	ip rule del fwmark 1 lookup 100

	# 删除ipset
	ipset destroy ss_bypass_set >/dev/null 2>&1

	# 杀死进程
	kill -9 `cat $ss_redir_pid`

	echo "ss-redir is stopped"

最后修改为正常的运营商DNS即可。

另一种方法，是启动ss-redir前使用iptables-save备份，使用完毕后iptables-restore还原。

## 参考文章

[luci-app-shadowsocks](https://github.com/shadowsocks/luci-app-shadowsocks)
[iptables基础命令(英文)](https://www.frozentux.net/iptables-tutorial/iptables-tutorial.html)
[ipset官方文档(英文)](http://ipset.netfilter.org/ipset.man.html)
