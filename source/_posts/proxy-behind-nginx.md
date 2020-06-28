---
title: nginx反代MTProxy和v2ray-plugin
date: 2019-12-18 19:03:32
tags: nginx
categories: 网络
---
将常用的代理隐藏在nginx背后，使其看起来像是正常的HTTPS流量。
<!-- more -->
## 需求

部署MTProxy和V2Ray-plugin，要求和现有HTTPS网站共存
- 共用https端口
- 使用Web浏览器打开时为正常的网页，而使用对应的代理客户端功能正常

## Nginx

编译：
- nginx 1.13 以上的版本
- openssl 1.1.1 以上的版本

nginx主要开启的关键模块：
- http_v2_module
- stream
- stream_ssl_module
- stream_ssl_preread_module

打印信息

    user@ubuntu:~# nginx -V
    nginx version: nginx/1.17.6
    built by gcc 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1) 
    built with OpenSSL 1.1.1d  10 Sep 2019
    TLS SNI support enabled
    configure arguments: --with-cc-opt='-O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security
    -D_FORTIFY_SOURCE=2' --with-ld-opt='-Wl,-Bsymbolic-functions -Wl,-z,relro' --conf-path=/etc/nginx/nginx.conf --with-debug
    --with-pcre-jit --with-http_ssl_module --with-http_stub_status_module --with-http_realip_module --with-http_auth_request_module
    --with-http_addition_module --with-http_dav_module --with-http_geoip_module --with-http_gunzip_module
    --with-http_gzip_static_module --with-http_image_filter_module --with-http_v2_module --with-http_sub_module
    --with-http_xslt_module --with-stream --with-stream_ssl_module --with-threads --without-mail_pop3_module
    --without-mail_imap_module --without-mail_smtp_module --with-stream_ssl_preread_module --with-openssl=../openssl-1.1.1d

## MTProxy

用python3写第三方的服务端[mtprotoproxy](https://github.com/alexbers/mtprotoproxy)可以满足需求，能作为一个stream服务器隐藏在nginx背后。

官方的[MTPorxy](https://github.com/TelegramMessenger/MTProxy)和第三方[mtg](https://github.com/9seconds/mtg)暂时未能满足需求。因为MTP需要知道客户端的ip和端口，如果没有实现HAProxy的proxy_protocol，则无法获IP和端口。。。proxy_protocol就是传达端口信息的。

安装并测试

```
git clone https://github.com/alexbers/mtprotoproxy
cd mtprotoproxy
```

修改config.py内容如下
```
PORT = 3000
USERS = { "tg":  "xxxxxx" }
MODES = { "classic": False, "secure": False, "tls": True }
TLS_DOMAIN = "www.cloudflare.com"
PROXY_PROTOCOL = False
```

参数解读：
- PORT指定默认监听端口，程序默认绑定0.0.0.0固定值，要监听localhost则需要改源码
- USER内的xxxxx值为secret，可以用命令```head -c 16 /dev/urandom | xxd -ps```生成
- TLS_DOMAIN不能随意填，详见[这篇讨论](https://github.com/alexbers/mtprotoproxy/issues/165#issuecomment-558648069)，要填入支持TLS1.3的网站域名，而且要合适的证书长度，不合适的证书长度有```several TLS records```之类的错误。**同时不建议填入自己申请的域名**
- PROXY_PROTOCOL默认为False，表示不使用nginx作为反向代理。若使用nginx反向代理必须设置为True，详见[这篇讨论](https://github.com/alexbers/mtprotoproxy/pull/119)

运行py，根据屏幕上面弹出```tg://t.me/xxxxxxx```之类的链接，直接设置到TG内作为MTPROTO代理。

    python3 ./mtprotoproxy.py

确认TG可以通过该代理联机后，再进行下一步。此时可以通过wireshark抓包看到与代理连接是TLS1.3方式，且host为cloudflare，是真正的HTTPS/SSL流量。

为了开启下文的nginx反向代理，需要修改config.py中的```PROXY_PROTOCOL = True```，然后重新开启监听3000端口。

修改nginx.conf，在最外层加入

    stream {
        map $ssl_preread_server_name $name {
            www.cloudflare.com MT; # 映射域名到合适的后端
            default LocalBackEnd;
        }
        upstream MT {
            server 127.0.0.1:3000; # 这里是mtproxy监听端口
        }
        upstream LocalBackEnd {
            server localhost:1024; # 临时的服务器
        }
        upstream RealBackEnd {
            server localhost:1025; # 真正的服务器，见下文用1025端口
        }
        server {
            listen 443 reuseport; # 对外的443端口
            proxy_pass $name;
            proxy_protocol on; # 关键的一步，支持HAProxy的proxy_protocol
            ssl_preread on; # 预读SNI主机名
        }
        server {
            listen localhost:1024 reuseport proxy_protocol;
            proxy_pass RealBackEnd;
        }
    }


使用```nginx -t```测试配置文件是否正确。然后直接运行nginx。在TG客户端上将端口改成443。

这样你的代理就真正隐藏在443端口背后了。而且使用curl测试这个域名也能正常伪装成cloudflare。

```
# xx.xx.xx.xx为运行nginx的IP地址，下面将解析cloudflare到特定的地址
curl -v -s https://www.cloudflare.com --resolve 'www.cloudflare.com:443:xx.xx.xx.xx'
```

因为是Python写的，故有优化的余地，可以提高性能，详见[官方WIKI](https://github.com/alexbers/mtprotoproxy/wiki/Optimization-and-Fine-Tuning)

## V2Ray-plugin

关键点：
- v2ray插件需要运行在HTTP WebSocket模式，因为Nginx与客户端通讯已经完成了TLS握手和加密，Nginx与ss通讯是HTTP明文。详见[这篇讨论](https://github.com/shadowsocks/v2ray-plugin/issues/150#issuecomment-552345749)
- nginx的SSL证书需要指定fullchain，不能仅仅指定cert。否则会出现“电脑能用，而安卓机不能用”的情况。详见[这篇讨论](https://github.com/shadowsocks/v2ray-plugin/issues/118#issuecomment-530111876)，至于怎么获取fullchain，可以简单认为fullchain由cert和chain合并而成。
- v2ray的path参数要与nginx的location一致。默认为```/```，可以修改为其它值
- 域名和证书要自己申请，下文以abc.example.com为例

运行带插件的libev服务端，监听是HTTP端口，配置文件可参考[config.json](https://github.com/shadowsocks/shadowsocks-libev/blob/master/tests/aes.json)样例

```
ss-server -c config.json -p 3001 --plugin v2ray-plugin --plugin-opts "server"
```

修改nginx.conf，在http层加入一个服务器，监听1025端口，与上文的RealBackEnd对应。

    server {
        listen                      localhost:1025 ssl http2 reuseport;
        server_name                 abc.example.com;
        ssl_certificate             "/path/to/fullchain.crt";
        ssl_certificate_key         "/path/to/domain.key";
        ssl_ciphers                 HIGH:!aNULL:!MD5;
        ssl_prefer_server_ciphers   on;
        ssl_protocols               TLSv1.1 TLSv1.2 TLSv1.3;

        location / {
            proxy_redirect off;
            proxy_http_version 1.1;
            proxy_set_header Host $http_host;
            proxy_set_header Upgrade $http_upgrade;
            proxy_set_header Connection "upgrade";
            proxy_pass http://127.0.0.1:3001;
        }
    }

## 其它

- nginx的加密套件可以选```流加密```（如chacha20-poly1305等），这样对安卓机子或者移动设备的CPU性能要求低一点，解密的速度也快一点，毕竟这个系统瓶颈还是得优化的。
- nginx经过stream反向代理后，在```/var/log/nginx/access.log```日志文件里面，所有的访问者都是localhost，因此无法追踪来源。所以根据实际选择是否要stream共用端口。大体上就是拿stream的443端口作为对外端口。nginx使用```ssl_preread```模块预先读取SNI域名。如果是cloudflare则反代MTP，否则反代localhost的1025端口，对应V2Ray。
- MTProxy的TLS伪装让DPI检测代价更大，除非将该域名或者IP封锁。
- 开启TLS1.3更安全，因为握手包没有暴露任何明文的证书信息

自从用上了TLS，我们又可以愉快地刷小黄图了！！
