title: aria2+yaaw离线下载
date: 2016-09-13 00:49:21
tags: [ubuntu, openwrt]
categories: 网络
---
充分利用VPS大硬盘、大流量的特点，让服务器进行BT/磁力/HTTP下载，并实现web资源管理器：文件上传下载，一举两得。
<!-- more -->

特别提醒：使用美国/加拿大的VPS进行电影下载是违法的，主机商可以随时查出你的下载记录，等着被商家封号吧。请买其它版权法管得松的国家VPS进行离线下载。

> System: ubuntu 16.04 x64
> RAM: 256M
> Reverse proxy: nginx 1.10.1

## aria2

### 前戏

最简单的apt

	apt install aria2
	
新建一个拥有者为www-data的目录，用于存放aria2的设置，初始化某些文件，这里选用*/var/www/downloads*作为下载默认目录
	
	mkdir /home/www-data
	mkdir /home/www-data/aria2
	mkdir /var/www/downloads
	
	cd /home/www-data/aria2
	touch aria2.session
	
使用这个配置文件[aria2.conf](/attachments/aria2/aria2.conf)启动，下载后放在*/home/www-data/aria2*下面

为了安全起见，不用root权限运行aria2。修改完后更改拥有者www-data

	chown -R www-data:www-data /home/www-data/aria2
	chown -R www-data:www-data /var/www/downloads
	
测试一下使用该文件能否启动

	aria2c --conf-path=/home/www-data/aria2/aria2.conf
	
![](/images/aria2/aria2.png)

### 开机自启

下载init.d脚本：[点我下载](/attachments/aria2/aria2)，保存位置为/etc/init.d/aria2，并添加可执行权限

	chmod a+x /etc/init.d/aria2
	
测试一下能否启动

	killall aria2c
	/etc/init.d/aria2 start
	
可以加入到系统自动启动

	vi /etc/rc.local
	# 添加 /etc/init.d/aria2 start
	
## yaaw

这个是配合aria2实现web前端控制，纯静态html页面，在客户端javscript实现所有功能。

	cd /var/www/
	git clone https://github.com/binux/yaaw
	
在nginx.conf中的server标签中添加合适的location，指向index.html。实现访问xxx.com/yaaw即可打开页面

	location ^~ /yaaw {
		alias /var/www/yaaw/;
	}

重载nginx，测试一下网站能否打开

	nginx -s reload
	
![](/images/aria2/yaaw.png)
	
第一次会出现Internal server error，正常，因为没有设置RPC地址。在网站右侧“设置”修改jsonrpc为正确的地址，正确值应该是

	http://xxx.com:6800/jsonrpc

修改正确就可以登陆服务器实现下载文件到vps了，更多设置参考[yaaw设置帮助](http://aria2c.com/usage.html)

## eXtplorer

### web文件管理

光有下载还不行，还需要文件管理。实现web界面的删除文件/下载/上传等基本功能。

	apt install php php-cgi php-json php-xml php-mbstring

下载[eXtplorer](http://extplorer.net/projects/extplorer/files)，解压到/var/www/eXtplorer目录

同样为了安全起见，使用较低权限的www-data

	chown -R www-data:www-data /var/www/eXtplorer

在nginx添加一个virtual-server

	vi nginx.conf
	# http标签内增加:
	
	fastcgi_buffers 8 16k;
	fastcgi_buffer_size 32k;
	fastcgi_connect_timeout 300;
	fastcgi_send_timeout 300;
	fastcgi_read_timeout 300;
	

	vi sites-enabled/defualt
	# server标签增加:
	
	root /var/www/eXtplorer/;
	index index.php index.html index.htm;

	location / {
		try_files $uri $uri/ /index.html;
	}

	location ~ \.php$ {
		fastcgi_pass unix:/run/php/php7.0-fpm.sock;
		fastcgi_index index.php;
		fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
		include fastcgi_params;
	}
	
遇到的各种502 Bad Gateway错误、404错误请自己解决。一般都是权限问题，只要保证 *php-fpm的执行者*与*nginx的执行者* 都是*www-data*就不会出错。

重启nginx服务，打开浏览器看看能不能进入界面。这样就可以实网页端管理文件。

貌似第一次进入的密码、帐号都是admin，及时更改密码，如无法更改密码请核实eXtplorer拥有者权限。

![](/images/aria2/eXtplorer.png)

把aria2下载的东西全部在这里eXtplorer下载到自己电脑，还是很不错的

### 上传文件功能

eXtplorer已经集成了。最好设置一下默认最大允许上传文件大小，否则上传超大文件提示413错误:*Request Entity Too Large*

nginx部份

	vi /etc/nginx/nginx.conf
	# http标签内修改
	# Content-Length的最大值限制，默认1MB，这里改为1GB
	client_max_body_size 1024m;

php部份

	vi /etc/php/7.0/fpm/php.ini
	# 是否允许通过HTTP上传文件的开关。默认为ON即是开
	file_uploads = on
	# 允许上传文件大小的最大值。默认为2M
	upload_max_filesize = 100m 望文生意，即
	# 表单POST给PHP的所能接收的最大值，包括表单里的所有值。默认为8M
	post_max_size = 8m
	# 传至服务器上存储临时文件的地方，如果没指定就会用系统默认的临时文件夹
	upload_tmp_dir = /home/tmp_upload

一般来说，设置好上述四个参数后，在网络正常的情况下，上传<=8M的文件是不成问题的

但如果要上传>8M的大文件的话，只设置上述四项还不一定能行的通。除非你的网络真有100Mbps的上传高速，否则你还得继续设置下面的参数。

	# 每个PHP页面运行的最大时间值(秒)，默认30秒
	max_execution_time 600
	# 每个PHP页面接收数据所需的最大时间，默认60秒
	max_input_time 600
	# 每个PHP页面所吃掉的最大内存，默认8M
	memory_limit 8m

这样就可以使用自己的私有云了，存点小电影，老司机偶尔开开车还是不错的！远离国产云存储服务，向8秒教育宣传片say bye bye，从这里开始！！

## openwrt

VPS端下载完毕，接下来就使用openwrt上面的aria2将文件以HTTP下载方式拖回来。

### aria2

从openwrt的官方feed中可以编译出aria2，我选用较新的aria2 1.27.1版本，老版本不稳定。容易bus error。

	make menuconfig
	# 选择 Network->File Trans->Aria2，勾选编译BT和Metalink模块。
	make package/aria2/compile -j4 V=99
	
安装aria2后测试一下能否命令行下载文件

	cd /tmp
	ariac http://ipv4.download.thinkbroadband.com/5MB.zip
	
没问题就使用上文的aria2.conf略作修改（比如修改pre-located参数，下载目录等，session文件位置），直接在路由使用相同的命令启动

	aria2c --conf-path=/root/aria2.conf -D

### USB

openwrt安装以下packages

	opkg install \
	  kmod-usb-storage \
	  kmod-fs-ext4 \
	  kmod-fs-vfat \
	  block-mount \
	  kmod-nls-utf8 \
	  kmod-nls-iso8859-1 \
	  kmod-nls-cp437

LEDE安装：

	opkg install \
	  kmod-usb-storage \
	  kmod-fs-ext4 \
	  kmod-fs-vfat \
	  block-mount \
	  kmod-usb-ohci \
	  kmod-usb-uhci \
	  mkf2fs \
	  f2fsck

	
插入U盘挂载：

	mkdir /mnt/sda1
	mount /dev/sda1 /mnt/sda1
	
### yaaw

可以将上文的yaaw文件夹放入/root/。然后直接创建一个虚拟server:

	vi /etc/config/uhttpd
	# 添加
	config uhttpd 'yaaw'
	list listen_http '0.0.0.0:333'
	list listen_http '[::]:333'
	option home '/root/yaaw'
	option max_requests '3'
	option http_keepalive '20'
	option tcp_keepalive '1'
	
重启uhttp即可访问yaaw前端，地址```http://192.168.1.1:333```。无需复杂的页面设置。

	/etc/init.d/uhttpd restart

若要访问路由器上面的aria2，可以在yaaw右侧的配置，修改jsonrpc为合适的地址。

	http://192.168.1.1:6800/jsonrpc

下一步是，直接将VPS上面已经离线下载好的文件通过路由器```aria2+yaaw```下载到本地。

服务端使用Nginx新建一个dl虚拟server作为HTTP服务器:

	server {
		server_name files.xxxx.com;
		listen 80;
		location / {
			root /var/www/downloads;
			autoindex on;
			autoindex_localtime on;
			autoindex_exact_size off;
		}
	}

浏览器直接打开 http://files.xxxx.com 可以直接获得离线下载地址。
