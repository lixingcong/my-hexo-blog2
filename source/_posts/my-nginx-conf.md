title: 我的nginx配置
date: 2016-10-05 20:38:55
tags: [nginx, html]
categories: 网络
---
自己网站的nginx配置，减少重复编译nginx查找资料耗费的时间。
<!-- more -->
更新日期：2020年6月28日

## 需求

- 较高的压缩率，加快网页传输
- https安全性提高

最终选用了以下nginx模块

|模块|功能|
|----|---|
|[ngx_http_substitutions_filter_module](https://github.com/yaoweibin/ngx_http_substitutions_filter_module)|正则表达式|
|[ngx_brotli](https://github.com/google/ngx_brotli)|谷歌开源压缩brotli库|

## 编译

在这个目录下工作：

	mkdir ~/nginx_my && cd nginx_my

### 各个模块 

	cd ~/nginx_my
	git clone https://github.com/google/ngx_brotli
	cd ngx_brotli
	git submodule update --init
	
	cd ~/nginx_my
	git clone https://github.com/yaoweibin/ngx_http_substitutions_filter_module

### nginx

编译

	cd ~/nginx_my
	
	# nginx
	NGINX_VER=1.19.0
	wget http://nginx.org/download/nginx-$NGINX_VER.tar.gz
	tar xf nginx-$NGINX_VER.tar.gz
	
打patch，目的是让nginx优先chacha

	# 参考 https://gist.github.com/DimsKyu/c3f3e7f8ef41ded430beefe0690b7b2d 这个代码，大致就是这样改
	cd nginx-$NGINX_VER
	vi src/event/ngx_event_openssl.c
	# 找到ngx_ssl_create函数，在```#ifdef SSL_OP_NO_COMPRESSION```这行上面直接加上
	
	#ifdef SSL_OP_PRIORITIZE_CHACHA
		SSL_CTX_set_options(ssl->ctx, SSL_OP_PRIORITIZE_CHACHA);
	#endif
	
openssl取最新版

	OPENSSL_VER=1.1.1g
	wget https://www.openssl.org/source/openssl-$OPENSSL_VER.tar.gz
	tar xf openssl-$OPENSSL_VER.tar.gz
	
打patch，目的是让nginx可以配置TLS 1.3加密算法套件（截止nginx 1.19.0这个版本，仍不能在nginx.conf中的ssl_ciphers属性配置TLS1.3的加密套件优先顺序，相关讨论如：https://www.v2ex.com/t/547650 ）

	cd openssl-$OPENSSL_VER
	wget https://github.com/hakasenyang/openssl-patch/raw/master/openssl-equal-1.1.1e-dev.patch
	patch -p 1 < *.patch

安装依赖。至于依赖什么，取决于编译nginx开启的模块。即下文的configure参数。

	apt install -y \
	  libgeoip-dev \
	  libgd-dev \
	  libxslt1-dev \
	  zlib1g-dev \
	  libpcre3-dev

configure参数控制要编译哪些模块

	# 切换到Nginx源码目录
	cd ~/nginx_my/nginx-$NGINX_VER
	
	./configure \
	--with-cc-opt='-O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -D_FORTIFY_SOURCE=2' --with-ld-opt='-Wl,-Bsymbolic-functions -Wl,-z,relro' \
	--conf-path=/etc/nginx/nginx.conf \
	--with-debug \
	--with-pcre-jit \
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
	--with-stream_ssl_preread_module \
	--with-threads \
	--without-mail_pop3_module \
	--without-mail_imap_module \
	--without-mail_smtp_module \
	--with-openssl=../openssl-$OPENSSL_VER \
	--with-openssl-opt='enable-tls1_3 enable-ec_nistp_64_gcc_128 enable-weak-ssl-ciphers' \
	--add-module=../ngx_http_substitutions_filter_module \
	--add-module=../ngx_brotli

以上的configure意义是

- 没有指定prefix，采用系统默认的前缀目录，如/usr/local/
- 指定配置文件路径为/etc/nginx/nginx.conf
- 开启basic_auth等模块，关闭邮件模块
- 指定好openssl源码目录，开启TLS1.3加密套件
- 添加几个额外的模块

直接编译即可

	make && make install
	
## 配置

### 反向代理

参考[这篇文章](/2016/07/31/nginx-reverse-proxy-for-google/)，可以非常傻瓜化的部署反代。

### 压缩网页

修改/etc/nginx/nginx.conf达到基本的gzip效果

	gzip  on;
	gzip_vary          on;
	gzip_comp_level    6;
	gzip_buffers       16 8k;
	gzip_min_length    1k;
	gzip_proxied       any;
	gzip_disable       "msie6";
	gzip_http_version  1.0;
	gzip_types         text/plain text/css application/json application/x-javascript text/xml application/xml application/xml+rss text/javascript application/javascript;
	
再加上谷歌的黑科技brotli压缩

	brotli             on;
    brotli_comp_level  6;
    brotli_types       text/plain text/css application/json application/x-javascript text/xml application/xml application/xml+rss text/javascript application/javascript image/svg+xml;
	
### 提升ssl安全等级

前提是网站有HTTPS部署，且对相关概念有基本了解。可以自行参考[imququ大神博客](https://imququ.com)，学习基本的https部署方法！

一、自定义生成密钥交换，取代nginx默认的weak exchange

这个diffie-hellman算法是用来干啥的：作用就是让访问的者浏览器和你的服务器能安全的交换密钥，具体原理就自行谷歌吧，你不能强迫数学渣渣来给你解释密码学的东西是不是？总之nginx默认采用1024位的diffie-hellman，强度太低太危险，容易被别有用心的黑客叔叔破解，自己生成一个4096位的去替换掉就好啦。

	# 如果你的机器cpu性能不够强大，耗时很长，可以用 2048 位
	# 我的笔记本（i5 3317u）跑4096位，跑了二十分钟。。。直接拖到VPS上面/root/
	openssl dhparam -out dhparam.pem 4096
	
nginx.conf中的server标签中添加

	ssl_dhparam /root/dhparam.pem;
	
二、手动指定优先的加密算法

(对TLS 1.2有效。原版nginx 1.19.0对TLS1.3的ciphers暂未支持，因此若没有patch，无论ssl_ciphers设置什么值都是AES-256-GCM-SHA384）

推荐的加密套件设置，详见CloudFlare的仓库 https://github.com/cloudflare/sslconfig/blob/master/conf

	ssl_ciphers                TLS-CHACHA20-POLY1305-SHA256:ECDHE-RSA-CHACHA20-POLY1305:ECDHE+AES128:RSA+AES128:ECDHE+AES256:RSA+AES256:ECDHE+3DES:RSA+3DES;
	ssl_prefer_server_ciphers  on;
	ssl_protocols              TLSv1.2 TLSv1.3;
	
三、跨站攻击

	add_header X-Frame-Options SAMEORIGIN;
	add_header X-Content-Type-Options nosniff;
	add_header X-XSS-Protection 1;
	
四、强制HSTS

新建一个http服务器，强制跳转到https:

	server_name xxx.com
	listen 80;
	location / {
		rewrite ^(.*)$  https://$host$1 permanent;
	}

在https服务器中添加

	add_header Strict-Transport-Security "max-age=31536000;" always;
	
有兴趣可以加入google chrome的Preload List里面，将从浏览器自动强制使用https。需要申请，审批需要几天。但是加进去preload list后很难从其中移除，需要发邮件移除。。
	
