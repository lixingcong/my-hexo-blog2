title: NAT穿透和内网访问
date: 2016-10-03 22:55:18
tags: openwrt
categories: 网络
---
利用minivtun实现点对点非公网NAT穿透，在学校轻松访问家里的路由器。需要一个中心化公网服务器（VPS）作为打洞实现内网访问。
<!-- more -->
一般这种情况用于

- 家里路由器挂载离线下载
- 家里的WEB网络摄像头监控
- 远程修改某些路由设置
- 远程控制路由器相关的“智能家居”
- 远程访问家里的内网或一个网段

现在仅考虑以下拓朴图，本文的目的是想让路由C访问路由A，实现C远程控制A。其中A是非智能路由器，使用非Openwrt系统。A下面挂接一个Openwrt路由器B

![](/images/openwrt_port_fwd/topology.png)

前提是A和C能顺利访问该VPS，而且B工作正常。

## minivtun互访

这个[minivtun](https://github.com/rssnsj/minivtun)是我常用的tun点对点隧道软件，工作原理与shadowvpn类似，可以当梯子使用。现有我移植的的[minivtun-openwrt](https://github.com/lixingcong/minivtun-openwrt)，可以自行编译安装在路由上面。

按照文档编译安装，服务端运行监听555端口

	/usr/sbin/minivtun -l 0.0.0.0:555 -a 172.16.0.1/24 -e password -n mv0 -d

路由器B和C，同样使用minivtun实现与VPS对接，这里指定网络设备为mv001

	# Router B: ip 172.16.0.3
	/usr/sbin/minivtun -r [YOUR_VPS]:555 -a 172.16.0.3/24 -e password -n mv001 -d
	
	# Router C: ip 172.16.0.55
	/usr/sbin/minivtun -r [YOUR_VPS]:555 -a 172.16.0.55/24 -e password -n mv001 -d

使用Ping等工具测试路由B能否顺利访问VPS

	ping 172.16.0.1
	
## Openwrt端口转发

以下三个步骤均在路由B操作

### 新建接口

在network->interface标签下添加一个interface: 命名随意，这里命名为minivtun_intf，协议为DHCP Client，手动输入mv001这个物理接口进行绑定（因上面minivtun启动参数设定了mv001网络设备）

![](/images/openwrt_port_fwd/new_interface.png)

检查这个接口minivtun_intf是否获得正确的172.16.0.3/24地址，并且从数字变化过程中看到能有Tx/Rx流量通过。

### 入站防火墙

切换到Network->Firewall->Gerneral，添加一个新的Zone，随意命名为minivtun,指定入站出站转发三个都accept，勾选masquerading和MSS clamping进行伪装路由器。Covered Network只需要勾选两个区域即可，其中必选的是minivtun_intf表示源，另一个是目的地根据需要，可以选WAN或者LAN，如果访问Openwrt局域网就指定LAN，如果要访问WAN（比如上一级路由）就指定WAN

因为我是利用B去访问上一级的A，因此我勾选了WAN

![](/images/openwrt_port_fwd/new_firewall.png)

### 端口转发

切换到Network->Firewall->Port Forward，新建一个转发规则

外部端口随意，（比如外部端口是444，那么在路由C使用minivtun访问172.16.0.3:444就触发端口转发条件）

|项目|备注|我的值|
|--|--|--|
|名字|随意起名|minivtun_port_fwd|
|外部区域|入站防火墙名字|minivtun|
|外部端口|供外部访问端口|800|
|内部区域|目的端口区域|LAN|
|内部IP|目的地址|192.168.200.1|
|内部端口|目的端口|800|

![](/images/openwrt_port_fwd/new_port_forward.png)

### 测试方法

从路由器C浏览器地址栏输入```http://172.16.0.3:800```即可访问路由A的800端口

## 公网访问家里内网

考虑以下拓朴，按照上文已经实现B-C互访（即ping成功），现在我的需求是，在C节点上实现访问A路由上的192.168.200.0/24网段（做视频监控等应用）

![](/images/openwrt_port_fwd/topology.png)

实现原理是：将A节点的路由表在VPS上面宣告一下，参考minivtun项目的github issue[服务端访问客户端内网](https://github.com/rssnsj/minivtun/issues/12)进行配置。

### 添加节点A的路由表

服务端运行监听555端口，增加-v选项宣告路由表，192.168.200.0/24网段的默认网关是节点B，即172.16.0.3

	/usr/sbin/minivtun -l 0.0.0.0:555 -a 172.16.0.1/24 -e password -n mv0 -d -v "192.168.200.0/24=172.16.0.3"

然后服务器端Linux系统的路由表增加默认路由，192.168.200.0/24走minivtun接口。

	ip route add 192.168.200.0/24 dev mv0

服务器端测试一下能否ping到节点A，若成功就说明公网下使用minivtun可以实现访问192.168.200.0网段。

	ping 192.168.200.1

### 配合shadowsocks进行访问内网

假设服务器上已经有ss-server在运行。在shadowsocks-android上新建一个服务器配置，路由设置为自定义规则，自定义规则可以根据实际修改，满足规则才走ss转发，这样可以实现手机访问节点A内网

![](/images/openwrt_port_fwd/ss-custom-rule.png)

若电脑windows实现内网访问，个人猜测可使用ss的全局模式，或者配合SwitchyOmega等浏览器指定走socks5代理，我没有测试过。