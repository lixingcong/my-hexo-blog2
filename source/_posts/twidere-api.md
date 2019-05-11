title: 自建Twidere API
date: 2017-02-07 21:22:30
tags: twitter
categories: 网络
---
利用VPS自行搭建Twidere API和Thumbor图片代理，实现免开启shadowsocks上推特，可以无痛看图。
<!-- more -->

## Twidere简介

开源，免费的第三方推特客户端，非常强大，比官方客户端好用一百倍！

作者是跟我同龄的宅撩，在推特中文圈有不少人气~

![](/images/twidere-api/twidere.png)

## 申请API

去[推特API管理界面](https://apps.twitter.com/)新建一个APP，命名随意，比如

	Twidere-2017

然后其它空白随意填写，callback URL可以不填。

将获得的下面两个值记下来，后面登陆Twidere需要使用。

	Consumer Key (API Key)
	Consumer Secret (API Secret)

按图将上面两个值，填入客户端（API地址使用默认即可）

![](/images/twidere-api/api.png)

退出帐号测试是否能重新登陆。如果成功证明自己的API创建成功，以后使用Twidere发推会显示小尾巴

	来自Twidere-2017

## 反代API

测试环境：ubuntu 16.04.1 RAM 128MB

我这次居然拿年付5.99刀的搬瓦工绝版货做测试...醉了

用nginx 新建一个HTTPS网站（强烈建议不适用HTTP传输，会被墙）。关于如何获得SSL证书可以参考[这篇文章](/2016/07/31/nginx-reverse-proxy-for-google/#Let’s-Encrypt证书)

	server {
		server_name proxy.yyy.com;
		listen 443 ssl;
		ssl_certificate /tmp/fullchain.pem;
		ssl_certificate_key /tmp/domain.key;
		resolver 8.8.8.8;
		location ~ ^/domain\.([\w\d]+)/(.*) {
			proxy_pass  https://$1.twitter.com/$2$is_args$args;
			proxy_cookie_domain twitter.com $server_name;
		}
	}
	
那么将下面填入即可完成反代api转发器。

	https://proxy.yyy.com/domain.[DOMAIN]

![](/images/twidere-api/api-proxy.png)

退出帐号测试是否能重新登陆。如果成功证明自己的API代理创建成功，以后登陆推特将不用使用ss，免翻x墙上推特。

## 反代图片

使用api反代时候，若不使用ss，有时候会不显示图片，原因是Twidere默认API无法加载图片。可以搭建Thumbor实现图片反代。

### 编译

安装依赖

	apt-get install \
	         python-pip python-opencv \
	         libjpeg-dev libpng-dev libwebp-dev libtiff-dev libjasper-dev \
	         libgtk2.0-dev python-numpy python-pycurl python-tornado webp

从源码编译，如果编译过程中有依赖未满足请自行解决。

	git clone https://github.com/thumbor/thumbor
	cd thumbor
	python setup.py install

测试一下是否安装成功

	thumbor --help

拷贝一份默认配置作为config

	cp thumbor/thumbor.conf ~/

修改配置。参考这个[Twidere-Thumbor配置文件模板](https://github.com/TwidereProject/Twidere-Thumbor-Heroku/blob/master/thumbor.conf)。根据实际修改，比如我是这样

	vi ~/thumbor.conf
	
	# 增加
	HTTP_LOADER_FORWARD_USER_AGENT=True
	
	# 修改
	STORAGE = 'thumbor.storages.no_storage'

保存配置，测试一下：

	# 监听7777端口
	thumbor -c ~/thumbor.conf -p 7777

注意在vps上面打开7777端口的入站ACCEPT

	iptables -A INPUT -p tcp --dport 7777 -j ACCEPT
	
本地测试一下。假设vps地址为55.55.55.55

	wget -O - "http://55.55.55.55:7777/unsafe/300x200/http://www.waterfalls.hamilton.ca/images/Waterfall_Collage_home_sm1.jpg" > /dev/null

如果没有错误就证明搭建差不多了。

### 防止滥用

为了被有心人滥用，我加上了密钥。防止被恶意调用。

	vi ~/thumbor.conf
	# 修改
	SECURITY_KEY = "password"
	ALLOW_UNSAFE_URL = False

这样就不能使用unsafe方式访问图片了，必须使用加密后的地址访问图片，密钥这里设置为'password'。对应生成合法的地址

	# 使用内置工具生成一个合法地址
	thumbor-url -k "password" http://www.waterfalls.hamilton.ca/images/Waterfall_Collage_home_sm1.jpg
	
	# 输出结果
	/Ohtvvloypfk_auC-7malm5Tw6Gw=/http%3A//www.waterfalls.hamilton.ca/images/Waterfall_Collage_home_sm1.jpg
	
运行thumbor后，继续本地测试一下

	wget -O - "http://55.55.55.55:7777/Ohtvvloypfk_auC-7malm5Tw6Gw=/http%3A//www.waterfalls.hamilton.ca/images/Waterfall_Collage_home_sm1.jpg" > /dev/null
	
测试完毕后，每次都得指定端口访问，看起来较麻烦，建议使用nginx反代（下文）

### nginx反代thumbor

在https服务器的server标签内，添加一个location根目录(/)，作为thumbor代理。

	location / {
		proxy_set_header Host localhost;
		proxy_set_header X-Real-IP $remote_addr;
		proxy_pass http://127.0.0.1:7777$request_uri;
		proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
	}

使用screen 或者 supervisor等方式在后台运行thumbor，监听7777端口。重启nginx

	screen -dmS "thumbor_screen" thumbor -c ~/thumbor.conf -p 7777 -i 127.0.0.1
	nginx -s reload

本地测试一下

	wget -O - "https://proxy.yyy.com/Ohtvvloypfk_auC-7malm5Tw6Gw=/http%3A//www.waterfalls.hamilton.ca/images/Waterfall_Collage_home_sm1.jpg" > /dev/null
	
这样就可以把Thumbor服务器地址和密钥填入Twidere实现免翻x墙刷出图片了。

	# 地址
	https://proxy.yyy.com/
	# 密钥
	password

![](/images/twidere-api/thumbor.png)

实际测试结果

|网络|是否使用Thumbor代理|加载图片速度|
|---|---|----|
|关闭ss|否|非常慢|
|关闭ss|是|一般|
|开启ss|否|快|
|开启ss|是|一般|

由此可以看出，最佳表现为：在开启ss情况下关闭thumbor，或者在没有ss情况下开启thumbor。

注意thumbor无法代理视频文件。遇到视频打不开也是正常。。。

### 压缩图片

压缩图片可以加快加载速度。节省流量。但是对VPS资源消耗较大，在我的垃圾搬瓦工上面运行有可能被suspend。请根据需求开启。推荐内存256M以上机子使用

去[image-optim-buildpack项目](https://github.com/bobbus/image-optim-buildpack/tree/master/vendor/image-optim)下载jpegtran等二进制文件，假设放置在

	/usr/local/bin/jpegtran
	
记得设置可执行权限

	chmod a+x /usr/local/bin/jpegtran

那么修改thumbor.conf文件

	OPTIMIZERS = [
		'thumbor.optimizers.jpegtran'
	]
	JPEGTRAN_PATH = '/usr/local/bin/jpegtran'
	
重载thumbor，看看是不是加载图片快了一点？

可以进一步开启WEBP格式压缩。也可以指定WEBP压缩比率。

	AUTO_WEBP = True
	
	QUALITY = 40
	WEBP_QUALITY = 40
	
设置最大尺寸，分辨率过大将裁剪图片

	MAX_WIDTH = 800
	MAX_HEIGHT = 600
	
## 后记

Twidere是一个无比强大的推特客户端，最近上线了一系列付费功能，我第一时间购买了，同步功能还是不错的。

总体来说，这个客户端让我有一个放弃使用iOS的理由，因为目前来说还是安卓专版！！！其实宅撩正在紧张开发iOS版本了，也是一个好消息！

大家可以去官方主页关注项目进展！

[https://github.com/TwidereProject](https://github.com/TwidereProject)