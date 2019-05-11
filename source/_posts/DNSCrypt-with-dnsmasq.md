title: dnsmasq配合dnscrypt解决DNS污染openwrt
date: 2016-05-02 01:04:13
tags: shadowsocks
categories: 网络
---
很久以前使用clowwindy的ChinaDNS清洗DNS方法会遇到经常失效，具体表现为dns解析没有国外返回结果，一直是timeout，甚是懊恼。因此我更换了另一种抗DNS污染的方案，达到与ChinaDNS相同的效果，并且更强大、可靠。推荐使用这个dnscrypt
<!-- more -->

```2018-06-11```友情提示：鉴于国内ISP对UDP数据包疯狂丢包，不建议使用dnscrypt，更好的方式是使用udp2raw伪装TCP流量，建立到8.8.8.8的隧道。

## dnscrypt-wrapper

PS:搭建私人转发器非必要步骤。可以不搭建，直接[跳到dnscrypt-proxy部份](#dnscrypt-proxy)。

这个wrapper是运行在VPS的，用于搭建个人的dnscrypt-proxy转发器。

[dnscrypt-wrapper项目地址](https://github.com/Cofyc/dnscrypt-wrapper)
按照README编译安装libevent 2.1.1+的版本，libsodium可以从apt下载安装。然后编译安装dnscrypt-wrapper

生成证书步骤参考[这篇博客](https://www.logcg.com/archives/981.html)

	cd ~
	mkdir dnskey && cd dnskey
	# 新建一个目录来存放证书
	dnscrypt-wrapper --gen-provider-keypair
	# 生成提供商密钥对

这里系统会反馈一个指纹信息，这个信息就是客户端配置时候需要的“provider_public_key”！所以一定要保存好。

	# 公钥类似格式
	FF2F:34F2:3EF0:2ED2:A2C7:79A2:5F1B:3DB8:6258:B74A:A806:28C4:9F6D:3AF5:E4D8:61DA
	
然后，我们使用命令生成有时限的加密密钥对以及生成预签名证书

	dnscrypt-wrapper --gen-crypt-keypair --crypt-secretkey-file=1.key
	dnscrypt-wrapper --gen-cert-file --crypt-secretkey-file=1.key --provider-cert-file=1.cert

这样，dnscrypt-wrapper 就已经准备好了。

下面可以自定义一个provider-name，必须以2.dnscrypt-cert.开头，后面的随便写。

	2.dnscrypt-cert.hhhh.com

启动wrapper服务测试（以监听555端口为例）
哆嗦模式，用于调试

	dnscrypt-wrapper --resolver-address=8.8.8.8:53 --listen-address=0.0.0.0:555 --provider-name=2.dnscrypt-cert.hhhh.com --crypt-secretkey-file=1.key --provider-cert-file=1.cert -VV

守护模式，用于常驻监听

	dnscrypt-wrapper --resolver-address=8.8.8.8:53 --listen-address=0.0.0.0:555 --provider-name=2.dnscrypt-cert.hhhh.com --crypt-secretkey-file=1.key --provider-cert-file=1.cert -d

## dnscrypt-proxy

以下在路由器openwrt操作，其实可以先拿linux PC操作一下验证是否正确。

### 安装

	opkg install dnscrypt-proxy

如果不喜欢从openwrt源安装，可以自己编译并安装，我试了试，编译成功，可以参考[我的项目openwrt-dnscypt](https://github.com/lixingcong/dnscrypt-proxy-openwrt)

其实安装ipk后就能使用了，默认情况下内置了大量dnscrypt服务器，如果你搭建了私人wrapper，可以替换官方opendns的服务器。没有搭建的没关系，可以不用做任何修改，直接改listen端口为5355即可。

	vi /etc/config
	# 修改端口为5355，总之不冲突就ok
	/etc/init.d/dnscrypt-proxy restart
	
没有搭建的可以[直接跳到dnsmasq-full设置](#dnsmasq-full)。

### 已搭建wrapper配置

添加私人服务器，添入服务器生成的信息

	vi /usr/share/dnscrypt-proxy/dnscrypt-resolvers.csv
	# 从中增加一行，注意不要加入到文件最后一行，末尾缺逗号
	my_server,,,,,,1,no,no,no,123.45.67.89:555,2.dnscrypt-cert.hhhh.com,FF2F:34F2:3EF0:2ED2:A2C7:79A2:5F1B:3DB8:6258:B74A:A806:28C4:9F6D:3AF5:E4D8:61DA,

自建wrapper配置

	# 取消注释如下内容，将opendns改为你刚的第一个字段，比如上面的my_server
	option resolver 'my_server'
	option resolvers_list '...'

重启服务

	/etc/init.d/dnscrypt-proxy restart
	
这时候应改可以查询了

	nslookup google.com 127.0.0.1:5355

如果不能查询，提示timeout，那么就比较折腾，同样步骤如果在Linux PC上面测试dnscrypt-proxy时区正确应该没问题。但是openwrt的时区很蛋疼，导致加密数据包握手时证书过期，无法返回dns查询结果。具体表现为

	# openwrt日志显示（web->状态->系统日志）
	[ERROR] No useable certificates found
	# 服务端开哆嗦模式只显示下面内容
	client to proxy

使用NTP校时

	# 安装ntpdate测试一下NTP服务器，如果提示no server suitable就换NTP
	ntpdate asia.pool.ntp.org
	# 服务器ok的话，写进去crontab，每半小时同步一次
	crontab -e
	*/30 * * * * /usr/sbin/ntpd -q -p asia.pool.ntp.org
	
除了上面的亚洲授时外，还有别的的NTP服务器
	
	ntp.ubuntu.com
	0.fedora.pool.ntp.org
	0.debian.pool.ntp.org
	time.windows.com

使用test验证证书是否有效

	/etc/init.d/dnscrypt-proxy stop
	dnscrypt-proxy -L /usr/share/dnscrypt-proxy/dnscrypt-resolvers.csv -R <NAME> -t 0

其中NAME为csv文件中的自建服务器名字，即为每行第一列字段。比如刚提到的my_server。

如果test提示没有错，那就可以投入使用了，这个是毫无污染、高强度加密、非常纯净的DNS！但是我的目标是仅仅用于解析境外网站，需要配合下面的dnsmasq区分境外境内分别使用不同dns解析。

重新开启dnscrypt

	/etc/init.d/dnscrypt-proxy start

## dnsmasq-full

### 编译

参考我修改的[openwrt-dnsmasq项目](https://github.com/lixingcong/openwrt-dnsmasq)的README步骤进行编译dnsmasq，这个是2.75版的。

为什么要手动编译？因为dnsmasq从2.73开始支持具有ip过滤黑名单(ignore-address)功能，达到ChinaDNS类似的效果，这个功能是[@aa66535](https://github.com/aa65535)提交给开发者的。openwrt 15.05内置的dnsmasq比较老，不支持ignore-address和min-cache-ttl。

### 安装

首先卸载系统中原有的dnsmasq，还有残留的配置文件

	# 删除前先update
	opkg update
	opkg remove dnsmasq
	rm /etc/dnsmasq.conf && rm /etc/config/dhcp
	# 忽略SHA256不符合的警告安装
	opkg install /tmp/dnsmasq_2.7.5_ramips_24k.ipk --force-checksum
	
### 修改conf

	vi /etc/dnsmasq.conf
	
	# 添加如下
	all-servers
	server=114.114.114.114
	server=127.0.0.1#5355
	cache-size=2500
	min-cache-ttl=300
	
	# 根据你需要的域名走指定的DNS查询，配合ipset
	# 以Facebook为例，自己手动添加几个常用域名即可，可以参考他人的列表
	server=/.fb.me/127.0.0.1#5355
	server=/.thefacebook.com/127.0.0.1#5355
	server=/.fbsbx.com/127.0.0.1#5355
	server=/.akamaihd.net/127.0.0.1#5355
	server=/.fbcdn.net/127.0.0.1#5355
	server=/.facebook.com/127.0.0.1#5355
	
	# 过滤的黑名单ip，当dns返回如下结果就忽略。
	# 仅列出4条，可以参考Chinadns-github项目中的iplist.txt
	ignore-address=1.1.127.45
	ignore-address=1.1.67.51
	ignore-address=1.2.3.4
	ignore-address=1.209.208.200
	
PS:有关链接：
[自定义DNS域名解析dnsmasq.conf](https://github.com/aa65535/openwrt-dnsmasq/blob/master/etc/dnsmasq.d/server-custom.conf)
[ChinaDNS-iplist.txt](https://github.com/shadowsocks/ChinaDNS/blob/master/iplist.txt)
[ChinaDNS-iplist生成的python脚本](https://github.com/clowwindy/ChinaDNS-C/blob/master/tests/iplist.py)

重启dnsmasq就能解析了

	/etc/init.d/dnsmasq restart
	nslookup facebook.com
	# 正确结果是 31.13.76.68
	
### ubuntu下的dnsmasq

与openwrt平台设置类似
1. 首先更新至dnsmasq 2.73+
2. 指定好dnsmasq的监听（默认是127.0.0.1）和dnscrypt监听（默认是127.0.2.1）
3. 修改/etc/dnsmasq.conf中的server=dnscrypt-porxy端口
4. sudo service dnscrypt-proxy restart && sudo service dnsmasq restart

ubuntu默认安装了resolvconf这个自动管理dns的程序，但有时候很烦他会自动更改dns地址，我采用暴力方法去除这个影响，强制指定使用dnsmasq为系统上游dns

这个/etc/resolv.conf是一个符号链接，链接到/run/resolvconf/resolv.conf，可以删掉这个链接。

	sudo rm /etc/resolv.conf
	sudo vi /etc/resolv.conf
	# 添加dnsmasq的地址，以127.0.0.1为例
	nameserver 127.0.0.1
	
如果不想用dnsmasq了，恢复为默认resolvconf（其实我压根就不想用resolvconf,因dns更强大），创建链接即可。或者直接修改nameserver为127.0.1.1

	sudo rm /etc/resolv.conf
	sudo ln -s /run/resolvconf/resolv.conf /etc/resolv.conf
	sudo resolvconf -u

## 与shadowsocks关系

shadowsocks-libev从2.4.6开始才正式修复了udp转发的bug，可以使用ss-tunnel作为dnsmasq的上游DNS。而不必使用dnscrypt-proxy。

经过无数次测试，发现ss-tunnel比dnscrypt-proxy稳定！所以到这里才告诉大家可以放心使用ss-tunnel，把dnscrypt作为备用的上游dns吧！

在openwrt下，注意ss-tunnel依赖kmod-ipt-tproxy和iptables-mod-tproxy。默认的opkg不安装该内核模块。

	# opewrt 编辑 /etc/rc.local
	# 如果使用一次性验证OTA，需要加上-A参数
	/usr/bin/ss-tunnel -s [VPS_ADDR] -p [Shadowsocks-port] -l 5355 -m aes-128-cfb -k password -L 8.8.8.8:53 -u -f /tmp/ss-tunnel.pid
