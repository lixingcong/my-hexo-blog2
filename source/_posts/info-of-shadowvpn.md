title: ShadowVPN项目简介
date: 2015-09-06 16:26:07
tags: shadowsocks
categories: 网络
---
## A fast, safe VPN based on libsodium

https://shadowvpn.org/
License: GPL-3.0
Language: C
Created: September 13, 2014 15:40
Last updated: August 20, 2015 03:07
<!-- more -->
Last pushed: August 19, 2015 10:06
Size: 6.43 MB
Stars: 985
Forks: 122
Watchers: 136
Open issues: 26

## 简介

ShadowVPN is a fast, safe VPN based on libsodium. Designed for low end devices, i.e. OpenWRT routers.

ShadowVPN is currently beta and have much work to do. If you want a stable version, please come by here a few months later.

目前shadowvpn已经被项目所有者移除

### Install

#### Debian & Ubuntu

For Debian 7 and Ubuntu 12+, add the following line to /etc/apt/sources.list

	deb http://shadowvpn.org/debian wheezy main
Then

	apt-get update && apt-get install shadowvpn
	service shadowvpn restart

#### Unix

Currently Linux, FreeBSD and OS X are supported. Download a release and build. Do not clone the repo, since it's not stable. Make sure to set --sysconfdir=/etc. You'll find conf files under /etc.

For Debian-based Linux

    sudo apt-get update
    sudo apt-get install build-essential automake libtool git -y
    ./configure --enable-static --sysconfdir=/etc
    make && sudo make install
    
#### OpenWRT

Download bundled [ShadowVPN with LuCI](https://github.com/aa65535/openwrt-shadowvpn), or just download ShadowVPN itself,

Or build ShadowVPN yourself: cd into [SDK](http://wiki.openwrt.org/doc/howto/obtain.firmware.sdk) root, then

    pushd package
    git clone https://github.com/clowwindy/ShadowVPN.git
    popd
    make menuconfig # select Network/ShadowVPN
    make V=s
    scp bin/xxx/ShadowVPN-xxx-xxx.ipk root@192.168.1.1
    # then log in your box and use opkg to install that ipk file
    
#### iOS
See iOS

#### Android
See Android

#### Windows
See Build for Windows.

### Configuration

1. You can find all the conf files under /etc/shadowvpn.
2. For the client, edit client.conf.
3. For the server, edit server.conf.
3. Update server and password in those files.
4. The script file specified by up will be executed after VPN is up.
5. The script file specified by down will be executed after VPN is down.
6. If you need to specify routing rules, modify those scripts. You'll see a placeholder at the end of those scripts.
7. If you are using Windows, the IP address of TUN/TAP device tunip is required to be specified in the conf file.
8. You can configure multiple users(详见下一篇博客)

        多服务端配置很简单，更改端口，tun序号，内网ip即可。
        新建server.conf  server_down.sh  server_up.sh

### Usage

#### Server:

    sudo shadowvpn -c /etc/shadowvpn/server.conf -s start
    sudo shadowvpn -c /etc/shadowvpn/server.conf -s stop
    
If you installed using apt-get, you can use sudo service shadowvpn start instead.

#### Client:

    sudo shadowvpn -c /etc/shadowvpn/client.conf -s start
    sudo shadowvpn -c /etc/shadowvpn/client.conf -s stop

#### Client(OpenWRT):

    /etc/init.d/shadowvpn start
    /etc/init.d/shadowvpn stop
    
You can also read LuCI Configuration.

Wiki
You can find all the documentation in the wiki:

	https://github.com/clowwindy/ShadowVPN/wiki

License
Copyright (C) 2015 clowwindy

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/.
