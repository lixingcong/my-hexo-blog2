title: goproxy部署方案
date: 2015-10-21 22:38:18
tags: shadowsocks
categories: 网络
---
goproxy是两款同名的代理
## Shell.Xu.版本
项目地址：https://github.com/shell909090/goproxy
<!-- more -->
### 服务端设置

先安装Go 1.5版本

	wget https://storage.googleapis.com/golang/go1.5.linux-amd64.tar.gz
	tar xf go1.5.linux-amd64.tar.gz -C /usr/local

设置环境变量 其中GOROOT为安装go的目录，GOPATH为一般工程目录，操作步骤：*把以下内容，追加到/etc/profile文件末尾：*

	export GOROOT=/usr/local/go
	export GOPATH=/root/go_path
	export PATH=$GOROOT/bin:$GOPATH/bin:$PATH

重启reboot以使生效

编译并安装：

	mkdir go_path
	go get github.com/shell909090/goproxy/goproxy
	go build github.com/shell909090/goproxy/goproxy

生成一个key，服务器和客户端都用到这个

	head -c 16 /dev/random | base64
    
设置服务端config

	cd go_path/src/bin
	vi config.json

输入，表示监听本机8888端口

    {
        "mode": "server",
        "listen": ":8888",
        "logfile": "my.log",
        "loglevel": "WARNING",
        "adminiface": "127.0.0.1:5234",
        "cipher": "aes",
        "key": "[your key]",
        "passwd": {
            "username": "password"
        }
    }
    
保存，执行
	
    ./goproxy -config ./config.json

或者使用screen后台运行

	screen -dmS goproxy ./goproxy -config ./config.json

### 客户端设置

在vps上面进行交叉编译:
参考文章: [编译教程](http://www.dwhd.org/20150829_125826.html)

32bit

    GOOS=linux  GOARCH=386 go build -o /tmp/goproxy_linux_i386 github.com/shell909090/goproxy/goproxy
    GOOS=darwin  GOARCH=386 go build -o /tmp/goproxy_darwin_i386 github.com/shell909090/goproxy/goproxy
    GOOS=freebsd  GOARCH=386 go build -o /tmp/goproxy_freebsd_i386 github.com/shell909090/goproxy/goproxy
    GOOS=windows  GOARCH=386 go build -o /tmp/goproxy_windows_i386.exe github.com/shell909090/goproxy/goproxy

64bit

    GOOS=linux  GOARCH=amd64 go build -o /tmp/goproxy_linux_amd64 github.com/shell909090/goproxy/goproxy
    GOOS=darwin  GOARCH=amd64 go build -o /tmp/goproxy_darwin_amd64 github.com/shell909090/goproxy/goproxy
    GOOS=freebsd  GOARCH=amd64 go build -o /tmp/goproxy_freebsd_amd64 github.com/shell909090/goproxy/goproxy
    GOOS=windows  GOARCH=amd64 go build -o /tmp/goproxy_windows_amd64.exe github.com/shell909090/goproxy/goproxy

ARM

    GOOS=linux  GOARCH=arm go build -o /tmp/goproxy_arm github.com/shell909090/goproxy/goproxy
    GOOS=linux  GOARCH=arm64 go build -o /tmp/goproxy_arm64 github.com/shell909090/goproxy/goproxy

配置文件config.json，表示监听本地8889端口

    {
        "mode": "http",
        "listen": ":8889",
        "server": "SERVER_IP:82",
        "logfile": "",
        "loglevel": "WARNING",
        "adminiface": "127.0.0.1:5234"
        "dnsnet": "internal",
        "cipher": "aes",
        "key": "[your key]",
        // Windows client you should move route.list.gz to the dir of goproxy.exe
        // Add change to "./routes.list.gz"
        "blackfile": "/usr/share/goproxy/routes.list.gz",
        "username": "username",
        "password": "password"
    }

## phuslu版本


项目地址 https://github.com/phuslu/goproxy

友情提示：该项目已被移除。可以寻找fork仓库审阅代码。

### 服务端(openshift)

	https://github.com/phuslu/fetchserver/tree/golang

点击大图标openshift即可部署

其中密码可以修改 index.go ，默认为123456

### 客户端

去 https://github.com/phuslu/goproxy/releases 下载一个版本
解压，修改 php.json：

    "url": "https://xxxx-xxxx.rhcloud.com",
                "password": "123456",
                "sslverify": true
			
默认监听8000端口，可以在main.json修改

提示：客户端r169版本以下：需要新建chrome快捷方式（否则提示证书不安全），附加命令 
--ignore-certificate-errors

运行goagent.exe，在浏览器使用http代理127.0.0.1:8000即可
提示：*openshift不适宜长期使用，容易被封，容易被停机。*

## 使用体验

Shell909090的版本**把所有流量封装成一个tcp流**，以此逃避GFW检测。
带来的效果是看视频很流畅，但是刷推特不流畅，毕竟连续的流量才比较快。
整个代理是建立在tcp协议上的。
总体上部署shell909090，有一定难度，不适合大多数人使用。

phuslu版本是经典的goagent升级版，其实很好用
确缺点是在vps上无法部署，老是被reset重置。
**免费的app空间有诸多限制**
作者phuslu貌似不再维护了，以免跨省追捕。（项目有1000+的关注量）
