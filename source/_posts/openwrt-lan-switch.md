---
title: OpenWrt交换机
date: 2018-07-23 16:16:32
tags: openwrt
categories: 网络
---
给路由器（WR1200JS,4个LAN口）配置交换机，分配两个局域网网段，实现单方向访问。
<!-- more -->

## 操作目的

友华WR1200JS是性价比较高的MT7621设备，[硬件参数Wiki](https://openwrt.org/toh/hwdata/youhua/youhua_wr1200js)显示它具备4个LAN口。

本操作是在Openwrt上操作，将4个LAN口划分为两个VLAN，其中一个VLAN网段为192.168.1.0/24，另一个为192.168.2.0/24。

配合iptables防火墙，让192.168.1.0/24可以访问192.168.2.0/24，却不能反过来让后者访问前者网段。

## 划分VLAN

首先通过使用网线插拔不同的物理LAN口，看看哪个灯亮就是对应哪个。实测这机子网口序号是正确的。

|Luci界面的Port|机子标签上的网口|划分VLAN ID|用途|
|-|-|-|-|
|LAN1|LAN1|1|网段1（Openwrt预置）|
|LAN2|LAN2|1|网段1（Openwrt预置）|
|WAN|WAN|2|WAN拨号（Openwrt预置）|
|LAN3|LAN3|3|网段2|
|LAN4|LAN4|3|网段2|

设置如下图的交换机

![](/images/openwrt-lan-switch/vlan.png)

对于tagged/untagged的解释：
1. Port状态Untagged时，该Port作为本VLAN成员，进行二层交换。
2. Port状态Tagged时，Ports之间没有二层交换，而是以冲突广播（类似于HUB的工作方式）
3. 一个Port最多能设置属于一个VID下的Untagged，该VID为Port的默认VID

按照上图设置会把eth接口瓜分为三个：```eth0.1 eth0.2 eth0.3```

## 添加接口

由于Openwrt预置了eth0.1和eth0.2接口，对应ID为1，2的VLAN。新划出的eth0.3需要手动添加到Luci interface中。

![](/images/openwrt-lan-switch/create-intf.png)

设置静态地址为192.168.2.1，并开启DHCP服务器（可选步骤，开启后能让接在LAN3和LAN4的客户端可以自动获取到192.168.2.0网段的IP）。

## 防火墙

在Firewall中添加新的zone，指定Covered Netword为刚刚创建的接口。

要配置SNAT使得网段1可以主动访问网段2，必须勾选Masquerading和MSS clamping。同时将下方的Allow forward from source勾选lan（即网段1的接口），允许从lan转发到这个新的zone。

配置完成！大功告成。

## 其它案例

与本文类似的案例，操作方法大同小异，可以动手实现一下：
1. 让Openwrt添加一个访客SSID，手动配置接口与防火墙，实现访客只能访问WAN，不能访问内网LAN的NAS设备。
2. 搭建迷你的VPN内网穿透
