title: 单网络接口多IP设置
date: 2017-01-07 00:53:46
tags: [shell, iptables]
categories: 网络
---
今天买的VPS另外送了一个IP，我该怎么绑定2个IP到一个网络接口上面呢？答：使用IP alias实现。
<!-- more -->
默认情况下一个网络接口只能绑定一个IPV4地址。这个"Main IP"能在VPS商家的控制面板中设置。如果拥有多个IP，想都用在这机器上，创建IP别名即可达到目的。

IP别名类似于Openwrt单线多拨叠加带宽的原理，都是一个网络接口虚拟出多个IP，就是少了拨号和负载均衡两个步骤而已。

假设我的IP情况如下
- 主IP: 66.66.66.66/27
- 副IP: 111.111.111.111/25

## 别名

查看默认网卡接口名字

	ifconfig
	
名字一般是eth0之类的，我的ubuntu 16.04显示的是ens3

那么别名就是ens3:0，也就是在原网络接口后面加上冒号，数字即可。

可以创建多个别名例如ens3:1, ens3:2。直接ifconfig。

	ifconfig ens3:0 111.111.111.111/25 up
	
本机貌似是ping不通这个111.111.111.111的。用另一机子ping它看看是否正常响应ICMP

	ping 111.111.111.111
	
若有ICMP回复，那么基本上完成了。可以使用主IP和副IP访问VPS了。

不知道为什么出现了这个现象：两个IP的TCP数据包畅行无阻，对于UDP数据包则被本地链路全部丢弃。

我的ss开启了UDP转发，不可能就这么阉割udp部份吧。。。如果遇到UDP被丢弃的情况，可以做NAT

## 对副IP做UDP-NAT

因为副IP相对于主IP可以看作是内网IP，因此做DNAT操作。

举例，我要做一个对从111.111.111.111传入的UDP数据包进行DNAT

	iptables -t nat -A PREROUTING -p udp --dst 111.111.111.111 -j DNAT --to-destination 66.66.66.66

测试无误，可以将IP Alias和iptables命令，写入到系统自启的脚本文件里面。ubuntu是这里

	vi /etc/rc.local

## 参考博文

[使用 MAC VLANs 创建 MAC 地址不同的虚拟网卡](http://answ.me/post/use-macvlan-to-create-multiple-virtual-interfaces-with-different-mac-addresses/)：另一种不同于IP alias的方法
[分配多个公网IP到2台KVM机子上面](http://serverfault.com/questions/372408/how-to-assign-multiple-public-ip-adresses-for-2-kvm-guests)：教你如何做iptables NAT
[清华大学_金枪鱼之夜：坏人的 iptables小讲堂](https://www.youtube.com/watch?v=w_vGD-96O54)：非常清晰明了的iptables入门教程，社团形质的录像
