---
title: macvlan单线多拨+mwan3负载平衡
date: 2018-04-13 15:46:24
tags: openwrt
categories: 网络
---
LEDE/OpenWrt下的单线多拨和负载平衡，记下以后有可能跳进去的坑，减少走弯路概率。
<!-- more -->
## 前戏

多拨可以是单线多拨，也可以是多个物理WAN同时拨号。它们的区别是：
- 单线多拨一般使用macvlan虚拟出网卡
- 多个物理WAN同时拨号用不同的interface直接获取WAN ip
- 若LAN口有空闲，也可以利用OpenWrt的VLAN交换机划分多余的LAN口为单独的VLAN，配置成WAN实现多个物理WAN

手上的破路由MT7620只有一个WAN，默认情况下只能获取到一个WAN ip

所以这里只讨论单线多拨。后面两种情况（物理WAN）比较容易配置，可以参考其它资料。

## 虚拟网卡

安装macvlan内核模块，实现虚拟网卡

	opkg install kmod-macvlan

先确定物理WAN是哪个物理接口，下图为我的路由器WAN的物理接口为eth0.2，所以命令的$WAN_INTF为eth0.2

![](/images/mwan3-macvlan/interface-wan.png)


### 创建虚拟接口

创建一个临时的虚拟接口veth0（关机后会失效veth0）

	# 替换为你的WAN物理接口
	export WAN_INTF=eth0.2
	
	ip link add link $WAN_INTF name veth0 type macvlan
	ifconfig veth0 up

若要想创建永久的虚拟网卡接口，可以写入到系统配置中（关机重启仍然保留veth0）

	vi /etc/config/network
	
	# 添加内容
	config device 'veth0'
	    option name 'veth0'
	    option ifname 'eth0.2'
	    option type 'macvlan'

接着可以基于这个veth0接口创建WAN2接口了

### 创建拨号接口

在Luci界面的Network->Interfaces里面，点击Add new interface，配置物理接口为veth0，新接口这里名为wan2

![](/images/mwan3-macvlan/interface-wan2.png)

对其配置DHCP/PPPoE等，测试获取到正常IP，方可进行下一步。

	ifconfig
	
	# 可以看到获取到IP
	veth0     Link encap:Ethernet  HWaddr xx:xx:F3:0A:79:5C  
          inet addr:192.168.2.122  Bcast:192.168.2.255  Mask:255.255.255.0
          inet6 addr: fe80::34ae:f3ff:fe0a:795c/64 Scope:Link
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:101169 errors:0 dropped:174 overruns:0 frame:0
          TX packets:5424 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1 
          RX bytes:10191279 (9.7 MiB)  TX bytes:611389 (597.0 KiB)

### 测试防火墙规则

我们把WAN（原来的接口）关掉：在Luci界面的Network->Interfaces里面，WAN右侧的Stop，排除原来WAN的影响。

防火墙中Covered Networks勾选新的接口，使得LAN发往WAN2的数据包具备Masquerad的SNAT能力。

![](/images/mwan3-macvlan/firewall-cover.png)

在局域网任意主机测试一下，能否打开任意网页。此时是单纯用WAN2上网，排除WAN的接口。测试能上网，方可进行下一步。

我们把WAN（原来的接口）重新恢复，在Luci界面的Network->Interfaces里面，WAN右侧的Connect

### 设置跃点

对WAN2和WAN两个接口设置不同的跃点，例如10,20。

![](/images/mwan3-macvlan/interface-wan-metric.png)

## 负载平衡

如果你事先得知ISP多拨后按端口限速，那多拨后没法叠加网速，负载平衡就没必要了。

安装mwan3

	opkg install mwan3 luci-app-mwan3

前往在Luci界面的Network->Load Balancing。在Configuration下面四个标签内，默认的配置全部删掉。
- interface
- Members
- Policies
- Rules

删掉配置的目的是，很多情况下，路由器实际接口都跟MWAN3默认配置不一致。当然若你是高手，可以不删除，手动配置。

下文将按照从上到下的顺序配置好MWAN3
1. interface
2. Members
3. Policies
4. Rules

### 添加接口

定位到MWAN3的interface标签，将wan和wan2分别填入，点击Add，配置ping的服务器就可以了。其它保持默认配置。

![](/images/mwan3-macvlan/mwan3-interface.png)

### 添加成员

基于接口，配置某一接口的跃点与权重组合值。

定位到MWAN3的Members标签，这里以添加WAN接口的权重跃点1，权重2为例，我把这个规则命名为wan_m1_w2

将wan_m1_w2填入，点击Add，直接指定接口wan，跃点，权重即可。

路由优先发往跃点值较小的接口。跃点值相同的接口，按权重走路由。

我建立了几个成员，仅供参考

|名字|接口|跃点|权重|
|--|--|--|--|
|wan_m1_w2|wan|1|2|
|wan2_m1_w2|wan2|1|2|
|wan2_m1_w1|wan2|1|1|

### 添加策略

基于成员，配置某一策略的路由走法

定位到MWAN3的Polices标签

我们创建一个走负载平衡的策略，即wan与wan2跃点一样，且权重一样。我把这个策略命名为balance

将balance填入，点击Add

将MemberUsed设为两个：wan_m1_w2和wan2_m1_w2，把LastResorted设为Unreachablbe。这样完成了一个最简单的策略。

同理，我们也可以创建一个只走wan2的策略，我把这个策略命名为wan2_only

将wan2_only填入，点击Add

将MemberUsed设为：wan2_m1_w2，把LastResorted设为Unreachablbe。

### 添加路由规则

基于策略，可以配置某个协议/IP的路由。

定位到MWAN3的Rules标签

添加一个默认规则，走wan2_only的策略，我把这个策略命名为default_route

将default_route填入，点击Add，把Policy assigned指定为wan2_only即可。

同理，我们可以指定自己Shadowsocks服务器（比如3.4.5.6）走负载均衡的策略，我把这个策略命名为ss_route

将ss_route填入，点击Add，把Destination address设为3.4.5.6/32，把Policy assigned指定为balance即可。


### 故障排除

如果在MWAN3中看到红色的警告/错误，请善于使用搜索，确保无警告无错误。

测试下载大文件，进入Network->interfaces，对于负载平衡的策略，理论上可以看到wan和wan2接口的收发流量均有变化。

至于网速是否有叠加效果，还是取决于ISP。


## 参考链接

[LEDE使用macvlan和mwan3实现单线多拨](https://acris.me/2017/06/25/Load-balancing-multiple-PPPoE-on-LEDE/)

