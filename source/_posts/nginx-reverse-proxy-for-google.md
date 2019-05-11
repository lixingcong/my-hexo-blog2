title: nginx反代google
date: 2016-07-31 14:25:13
tags: [html, nginx]
categories: 网络
---
一键部署xx之类的脚本使用有风险，试想一下脚本弄出异常的'sudo rm -rf /'就让VPS挂掉的情形。。因为已经有前车之鉴，不敢再偷懒，自己实践下反代过程也不错哦！
<!-- more -->
## nginx

目的是编译带有以下模块的nginx，实现正则表达式匹配谷歌的地址

|模块|功能|
|--|--|
|[ngx\_http\_google\_filter\_module](https://github.com/cuber/ngx_http_google_filter_module)|google反代|
|[ngx\_http\_substitutions\_filter\_module](https://github.com/yaoweibin/ngx_http_substitutions_filter_module)|正则表达式|

### 获取module

	cd /root && mkdir ng && cd ng
	git clone https://github.com/cuber/ngx_http_google_filter_module
	git clone https://github.com/yaoweibin/ngx_http_substitutions_filter_module
	
安装module依赖

	apt install libpcre3-dev libssl-dev zlib1g-dev libxslt1-dev libgd-dev libgeoip-dev

### 编译nginx

去[nginx download page](http://nginx.org/en/download.html)下载需要的版本，以1.10.1为例

	wget http://nginx.org/download/nginx-1.10.1.tar.gz
	tar xf nginx-1.10.1.tar.gz
	cd nginx-1.10.1
	
configure参数填入，再加上两个Module，生成Makefile

	./configure \
	--with-cc-opt='-g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -D_FORTIFY_SOURCE=2' \
	--with-ld-opt='-Wl,-Bsymbolic-functions -Wl,-z,relro' \
	--prefix=/usr/share/nginx \
	--conf-path=/etc/nginx/nginx.conf \
	--with-debug \
	--with-pcre-jit \
	--with-ipv6 \
	--with-http_ssl_module \
	--with-http_stub_status_module \
	--with-http_realip_module \
	--with-http_auth_request_module \
	--with-http_addition_module \
	--with-http_dav_module \
	--with-http_geoip_module \
	--with-http_gunzip_module \
	--with-http_gzip_static_module \
	--with-http_image_filter_module \
	--with-http_v2_module \
	--with-http_sub_module \
	--with-http_xslt_module \
	--with-stream \
	--with-stream_ssl_module \
	--with-threads \
	--add-module=../ngx_http_google_filter_module \
	--add-module=../ngx_http_substitutions_filter_module

若外置openssl，使用这个参数

	--with-openssl=../openssl-1.1.0c
	
*PS:*这个configure参数是从apt install nginx后执行nginx -V得到的，根据个人需要编译模块
	
没有问题就编译安装吧

	make -j2
	make install

看看nginx模块是否正确，方法是看configure末尾是否有ngx\_http\_google\_filter\_module

	nginx -V
	# 若command not found可以创建符号链接
	# ln -s /usr/share/nginx/sbin/nginx /usr/sbin/nginx

然后配置一下网站，看看是否能从外网打开本机网站

	vi /etc/nginx/nginx.conf
	# 修改server_name为自己的域名
	nginx -s reload
	
最后，记得添加开机启动，写入.bashrc脚本或者rc.local脚本

	/usr/sbin/nginx

## Let's Encrypt证书

### 生成私鈅

挑选一个合适的letsencrypt客户端（网上大约有十多种），我以这个acme-tiny为例

	cd /root/ng
	git clone https://github.com/diafygi/acme-tiny
	cd acme-tiny	

生成自己用于续证书有效期的私钥，用于let's Encrypt识别你的个人身份，需要妥善保管，不能与下面的domain.key混用。

	openssl genrsa 4096 > account.key
	
生成 CSR（Certificate Signing Request，证书签名请求）

1.先生成RSA私钥，用于生成CSR（实际中可以选用ECC私钥）

	openssl genrsa 4096 > domain.key

2.接下来生成CSR文件

	# 单域名
	openssl req -new -sha256 -key domain.key -subj "/CN=MY_DOMAIN.COM" > domain.csr

	# 多域名(use this one if you want both MY_DOMAIN.COM and WWW.MY_DOMAIN.COM)
	openssl req -new -sha256 -key domain.key -subj "/" -reqexts SAN -config <(cat /etc/ssl/openssl.cnf <(printf "[SAN]\nsubjectAltName=DNS:MY_DOMAIN.COM,DNS:WWW.MY_DOMAIN.COM")) > domain.csr

### 签发证书

Let's Encrypt 在你的服务器上生成一个随机验证文件，再通过创建 CSR 时指定的域名访问，如果可以访问则表明你对这个域名有控制权。验证通过才允许下一步签证书。

创建用于存放验证文件的目录，不能用root权限的目录，建议使用nginx的www目录下（貌似权限是www-data？）

	mkdir /var/www/challenges/

往nginx配置一个HTTP服务器，用于验证let's Encrypt域名所有权, 添加前请注释掉之前已经存在的监听80端口的服务器

	vi /etc/nginx/nginx.conf
	# http标签内
	server {
		server_name MY_DOMAIN.COM;
		listen 80;
		# ipv6
		# listen [::]:80;

		location ^~ /.well-known/acme-challenge/ {
			alias /var/www/challenges/;
			try_files $uri =404;
		}
	}
	
重载服务

	nginx -s reload
	
这个验证服务以后（比如三个月后）更新证书还要用到，建议一直保留。

有了验证服务器，就可以验证域名并签发证书了。

	cd /root/ng/acme-tiny
	# 注意验证目录是/var/www/challenges，与上面mkdir一致
	python acme_tiny.py --account-key ./account.key --csr ./domain.csr --acme-dir /var/www/challenges/ > ./signed.crt
	
提示*Certificate signed!*就可以生成一个singed.crt网站证书。

接下来还要下载 Let's Encrypt 的中间证书，配置 HTTPS 证书时既不要漏掉中间证书，也不要包含根证书。在 Nginx 配置中，需要把中间证书和网站证书合在一起

	wget -O - https://letsencrypt.org/certs/lets-encrypt-x3-cross-signed.pem > intermediate.pem
	cat signed.crt intermediate.pem > chained.pem
	
### 续证书脚本

创建 renew_cert.sh 并通过 chmod a+x renew_cert.sh 赋予执行权限

	#! /bin/bash
	
	export ACME_TINY_DIR=/root/ng/acme-tiny
	
	cd $ACME_TINY_DIR && python acme_tiny.py --account-key account.key --csr domain.csr --acme-dir /var/www/challenges/ > signed.crt || exit
	cd $ACME_TINY_DIR && wget -O - https://letsencrypt.org/certs/lets-encrypt-x3-cross-signed.pem > intermediate.pem
	cd $ACME_TINY_DIR && cat signed.crt intermediate.pem > chained.pem
	cd $ACME_TINY_DIR && wget -O - https://letsencrypt.org/certs/isrgrootx1.pem > root.pem
	cd $ACME_TINY_DIR && cat intermediate.pem root.pem > full_chained.pem
	
	nginx -s reload || echo "renew fail"
	
写入crontab，定期执行续证书脚本（比如每个月20号续一次）

## 反代google

	vi /etc/nginx/nginx.conf
	# http标签内加入一个HTTPS服务器
	server {
		server_name MY_DOMAIN.COM;
		listen 443;
		# ipv6
		# listen [::]:443;

		ssl on;
		# specify your cert location
		ssl_certificate /root/ng/acme-tiny/chained.pem;
		ssl_certificate_key /root/ng/acme-tiny/domain.key;

		resolver 8.8.8.8;
		location / {
			google on;
			google_scholar on;
			google_language "en";
		}
		
		# forbid search engine spider
		if ($http_user_agent ~* "qihoobot|Baiduspider|Googlebot|Googlebot-Mobile|Googlebot-Image|Mediapartners-Google|Adsbot-Google|Feedfetcher-Google|Yahoo! Slurp|Yahoo! Slurp China|YoudaoBot|Sosospider|Sogou spider|Sogou web spider|MSNBot|ia_archiver|Tomato Bot"){
			return 403;
		}
		
		# not allow robots
		location /robots.txt {
			add_header Content-Type text/plain;
			return 200 "User-agent: *\nDisallow: /\n";
		}
		
		# forbid illegal domain request
		if ( $host != $server_name ) {
			return 403;
		}
	}
	
这样可以实现反代了，重载nginx看看效果

	nginx -s reload

### 其它设置

#### nginx权限

默认是nobody权限执行nginx，建议指定nginx的执行者为www-data

	vi /etc/nginx/nginx.conf
	user=www-data

因为用到的proxy模块，需要更改proxy module的拥有者为www-data

	chown -R www-data:www-data /var/lib/nginx/proxy

#### ssl安全设置

这个设置不是必要的，但是我看到[SSL Lab](https://www.ssllabs.com/ssltest/)对我的网站https评分仅有B等级，安全性不足，需要设定更好的密钥交换机制。

当然，尽量使用最新的Nginx，保证安全性，编译nginx也尽量使用指定模块最新源码的方式进行编译，堵住0day。

以下ssl设置均在http-server(HTTPS)标签内进行修改

在其listen 443改为

	listen 443 ssl http2 fastopen=2 reuseport;
	
开启OCSP

先把根证书和中间证书合在一起

	cd /root/ng/acme-tiny
	wget -O - https://letsencrypt.org/certs/isrgrootx1.pem > root.pem
	cat intermediate.pem root.pem > full_chained.pem
	
在nginx配置中指定ssl_stapling

	ssl_stapling               on;
	ssl_stapling_verify        on;
	ssl_trusted_certificate    /root/ng/acme-tiny/full_chained.pem;

指定加密算法

	ssl_ciphers                EECDH+CHACHA20:EECDH+CHACHA20-draft:EECDH+AES128:RSA+AES128:EECDH+AES256:RSA+AES256:EECDH+3DES:RSA+3DES:!MD5;
	ssl_prefer_server_ciphers  on;
	ssl_protocols              TLSv1 TLSv1.1 TLSv1.2;

超时等

	ssl_session_cache          shared:SSL:50m;
	ssl_session_timeout        1d;
	ssl_session_tickets        on;
	
	# openssl rand 48 > session_ticket.key
	# 单机（standalone）部署可以不指定 ssl_session_ticket_key
	# ssl_session_ticket_key     /root/ng/acme-tiny/session_ticket.key;
    
经过以上ssl设置，我的网站安全等级上升到A级，有些许提升。

#### 设置上游ip

防止谷歌认为你是机器人，要求输入验证码

在vps上面多次执行以下命令获得不同的ip（至少能获取3个吧，多一些比较好）

	dig google.com +short @8.8.8.8
	dig google.com +short @208.67.222.222

对每一个dns结果进行ping测试，选择延迟低的dns结果，按权重放入upstream标签内

	vi /etc/nginx/nginx.conf
	# http标签内加入upstream上游
	upstream www.google.com{
		server 216.58.217.206:443 weight=34;
		server 172.217.4.142:443 weight=33;
		server 216.58.193.206:443 weight=33;
	}
	
#### 限制连接数

设置同一个ip访问本站频率，防止滥用，具体数值根据服务负荷设置

这里设置某个ip频率每秒10次请求，并发burst最多允许50：效果可以从打开“谷歌图片”搜索一个关键词，看加载图片速度中体会得到。被限制的请求将返回503错误

	# http标签内加入setlimit
	limit_req_zone $binary_remote_addr zone=setfreq:10m rate=10r/s;
	limit_req zone=setfreq burst=50 nodelay;

#### http重定向

设置http访问后重定向为baidu.com（纯属恶搞，专门对付那些不开https的人）

	# http-server(HTTP)标签内加入rewrite
	location / {
		# change to your target website
		rewrite ^/(.*)$ http://www.baidu.com permanent;
	}
	
如果不恶搞，同理可以rewrite为https，达到http跳转https目的

	rewrite ^/(.*)$ https://$host/$1 permanent;
	
#### TCP优化设置

	vi /etc/nginx/nginx.conf
	# http标签内修改
	sendfile           on;
	tcp_nopush         on;
	tcp_nodelay        on;
	keepalive_timeout  60;
	
	# gzip压缩（可选）
	gzip               on;
	gzip_vary          on;
	gzip_comp_level    6;
	gzip_buffers       16 8k;
	gzip_min_length    1000;
	gzip_proxied       any;
	gzip_disable       "msie6";
	gzip_http_version  1.0;
	gzip_types         text/plain text/css application/json application/x-javascript text/xml application/xml application/xml+rss text/javascript application/javascript;
	
#### nginx缓存

参考[ngx_http_proxy_module官方手册](http://nginx.org/en/docs/http/ngx_http_proxy_module.html#proxy_cache)有关缓存的设置。

首先mkdir几个文件夹/etc/nginx/cache/one、two、three。

	vi /etc/nginx/nginx.conf
	# http标签内，这里定义了三个zone，后面google实际上只用到一个zone（two）
	proxy_cache_path  /etc/nginx/cache/one    levels=1      keys_zone=one:10m;
	proxy_cache_path  /etc/nginx/cache/two    levels=2:2    keys_zone=two:10m;
	proxy_cache_path  /etc/nginx/cache/three  levels=1:1:2  keys_zone=three:10m;
	
	# 缓存有效时间
	proxy_cache_valid  200 302 10m;
	proxy_cache_valid  301 30m;
	proxy_cache_valid  any 1m;

	# http->server标签内，加入允许缓存，zone名字为two
	location / {
		proxy_cache two;
		google on;
	}
	
这样每次请求谷歌的结果都缓存在nginx服务器内，减少VPS与谷歌的通信次数。

需要定期清理缓存，nginx没有自动清理缓存的功能，可以向crontab写入定时任务清理缓存。

	find /etc/nginx/cache/ -type f -delete

## 后记

设置到这里就可以使用反代了，建议搭建后域名不要公开使用，亲友几个人使用还是没问题的，人多了你的ip或者域名容易被GFW认证，最后只能更换VPS或者域名。

谷歌反代教程来源于项目[ngx\_http\_google\_filter\_module](https://github.com/cuber/ngx_http_google_filter_module)的wiki，证书的获取方法参考了[Let's Encrypt，免费好用的 HTTPS 证书](https://imququ.com/post/letsencrypt-certificate.html)，至于nginx的HTTPS安全部份则参考了[Nginx 配置之完整篇](https://imququ.com/post/my-nginx-conf.html)。经过本人实践记录下来而成。附上自己的配置文件[nginx.conf](https://gist.github.com/lixingcong/276ae24f8a0bedd147ac7489f3c58fc2)，可以参考一下。

另外有个不依赖http_google_filter_module的纯nginx配置文件[nginx.conf](https://github.com/arnofeng/ngx_google_deployment/blob/master/nginx.conf)，实际上这个conf更具有一般性，读懂它配置，理论上可以代理任意网站，例如草榴。

文中的MY_DOMAIN.COM即自己的域名，注意替换
