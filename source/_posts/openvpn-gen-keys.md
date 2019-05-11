title: openvpn生成密钥对
date: 2016-06-09 10:47:57
tags: shadowsocks
categories: 网络
---
静态公私鈅对生成
<!-- more -->
下载 https://github.com/OpenVPN/easy-rsa.git 这个RSA生成工具
## 生成reqs

	# 服务端
	cd easyrsa3
	./easyrsa init-pki
	./easyrsa build-ca nopass
	./easyrsa gen-req my_server nopass

	# 客户端
	cd easyrsa3
	./easyrsa init-pki
	./easyrsa build-ca nopass
	./easyrsa gen-req my_client nopass

## 交换reqs

使用scp工具将：

	服务端的pki/reqs/my_server.req拷贝到客户端的pki/reqs/
	客户端的pki/reqs/my_client.req拷贝到服务端的pki/reqs/

导入reqs

	# 服务端
	./easyrsa import-req pki/reqs/my_client.req my_client
	./easyrsa sign server my_server
	
	# 客户端
	./easyrsa import-req pki/reqs/my_server.req my_server
	./easyrsa sign client my_client
	
## 生成Diffie-Hellman对

分别在服务器和客户端生成，耗时两三分种

	./easyrsa gen-dh

## 提取出密钥

服务端和客户端分别执行

	mkdir ~/openvpn-keys
	cp pki/dh.pem ~/openvpn-keys
	cp pki/private/*.key ~/openvpn-keys
	cp pki/issued/*.crt ~/openvpn-keys
	
然后使用scp工具把两个证书交换

服务器的ca.crt使用客户端生成的ca.crt，放入 ~/openvpn-keys
客户端的ca.crt使用服务器生成的ca.crt，放入 ~/openvpn-keys

## 修改配置

### 服务端

	cp /usr/share/doc/openvpn/examples/sample-config-files/server.conf.gz ~/openvpn-keys
	cd ~/openvpn-keys
	gzip -d server.conf.gz
	vi server.conf

修改为合适的参数，下面列出部份需要修改的参数，不是完整的文件内容

	port 443
	proto tcp
	cert my_server.crt
	key my_server.key
	dh dh.pem
	server 192.168.66.0 255.255.255.0
	push "route 192.168.66.0 255.255.255.0"
	push "dhcp-option DNS 8.8.8.8"
	client-to-client
	cipher AES-128-CBC
	
### 客户端

	cp /usr/share/doc/openvpn/examples/sample-config-files/client.conf ~/openvpn-keys
	cd ~/openvpn-keys
	vi client.conf
	
修改为合适的参数，下面列出部份需要修改的参数，不是完整的文件内容

	proto tcp
	remote [YOUR_VPS_IP] 443
	cert my_client.crt
	key my_client.key
	cipher AES-128-CBC
	
## 测试

分别服务端和客户端执行，哆嗦模式，观察是否有错误，根据错误进行google排错

	sudo openvpn --config server.conf -verb 1
	sudo openvpn --config client.conf -verb 1
	