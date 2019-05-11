title: Configuring the ShadowVPN Server/Client
date: 2015-09-06 16:21:04
tags: shadowsocks
categories: 网络
---
## Configuring the Server

If you are running a VPS, ensure you have the TUN / TAP interface enabled in your VPS control panel.
<!-- more -->

### Generate the User token and Password

User tokens are hex representations of 8 byte values, you can generate one with the following command:

	xxd -l 8 -p /dev/random

A long, random password for encrypting traffic can be generated with the following command:

	dd if=/dev/urandom bs=64 count=1 | sha256sum

Keep those two values handy, you will need them for the server and client configuration.

### Modify the default server config file

	sudo vi /etc/shadowvpn/server.conf

Change **server=0.0.0.0** (Line 6) to the IP address of the server if it has a static IP, if not leave it at **0.0.0.0** to listen on all interfaces.

Change **# user_token=7e335d67f1dc2c01,ff593b9e6abeb2a5,e3c7b8db40a96105** (Line 14) to **user_token=[GENERATED_USER_TOKEN]** (Replace **[GENERATED_USER_TOKEN]** with the User token generated in the previous step.)

Change **password=my_password** (Line 18) to **password=[GENERATED_PASSWORD]**

### Check the configuration

You can check the configuration file for errors by starting up a server in the console in verbose mode:

    sudo shadowvpn -c /etc/shadowvpn/server.conf -v
    
If you see something along the lines of **Sat Aug 22 14:03:32 2015 VPN started**, then it's all good, quit the console server with CTRL+C

### Start the server daemon

Start the server as a background process with 

	sudo shadowvpn -c /etc/shadowvpn/server.conf -s start

## Configuring the client

### Modify the default client config file

    sudo vi /etc/shadowvpn/client.conf
    
Change **server=0.0.0.0** (Line 6) to the IP address of the server.

Change** # user_token=7e335d67f1dc2c01,ff593b9e6abeb2a5,e3c7b8db40a96105** (Line 14) to **user_token=[GENERATED_USER_TOKEN]** (Replace **[GENERATED_USER_TOKEN]** with the User token generated in the previous step.)
Change **password=my_password** (Line 18) to **password=[GENERATED_PASSWORD]**
Change **net=10.7.0.2/24** (Line 36) to **net=10.7.0.1/16**

### Check the configuration

You can check the configuration file for errors by starting up a client in the console in verbose mode:
    
    sudo shadowvpn -c /etc/shadowvpn/client.conf -v

If you get something along the lines of **Sat Aug 22 14:03:54 2015 VPN started** then you are now connected to the VPN !
Quit the console client by pressing CTRL+C.

### Start the client daemon

Start the client as a background process with 

	sudo shadowvpn -c /etc/shadowvpn/client.conf -s start
    
Now make sure it works by verifying your IP with the following command: 

	wget -qO- http://ipecho.net/plain ; echo
    
If the returned IP address is that of the server then you're all set up, have fun with your new VPN connection !

[原文链接](http://www.cuonic.com/posts/setting-up-shadowvpn-on-debian)
