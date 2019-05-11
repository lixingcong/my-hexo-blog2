---
title: 只允许rsync同步的中心机器
date: 2018-11-12 20:21:03
tags: shell
categories: 网络
---
在VPS上新建一个用户，专门用于给其它电脑rysnc同步文件。为了不让用户随意进入终端，必须设置公私钥认证+仅允许rsync同步+文件夹权限为只读。
<!-- more -->
本实验在Ubuntu 18.04测试通过

## 客户端公私钥

生成公私钥，用于给客户端认证。

	ssh-keygen -t rsa -b 4096 -C "RsyncForUser"

客户端公私钥的id_rsa.pub内容，记作RSA_PUBLIC_KEY，下文要粘贴到服务器的authorized_keys中

	cat id_rsa.pub

## 服务器创建用户

提示：由于我全程使用root用户操作修改Tom的文件，需要及时修改目录拥有者为Tom

	chown -R Tom /home/Tom

所以下文会出现很多次这个chown命令

在VPS上创建用户，专门用于rsync，比如用户名叫Tom

	adduser Tom

修改/etc/passwd取消bash作为默认的tty

	vi /etc/passwd
	# 删掉末尾/bin/bash，修改为
	Tom:x:1000:1000:,,,:/home/Tom:

生成的追加粘贴到VPS上的authorized_keys

	mkdir -p /home/Tom/.ssh
	touch /home/Tom/.ssh/authorized_keys
	chown -R Tom /home/Tom

## 服务器设置rrsync

创建bin目录

	mkdir -p /home/Tom/bin

rsync自带的rrsync脚本可以控制只允许客户使用rsync而不能进入bash终端。

	gunzip --to-stdout /usr/share/doc/rsync/scripts/rrsync.gz > /home/Tom/bin/rrsync
	chmod a+x /home/Tom/bin/rrsync
	chown -R Tom /home/Tom

新建一个目录，专门作为使用ssh密钥登陆后的rsync的根目录。

	mkdir /home/Tom/rsync-root
	chown -R Tom /home/Tom

把下面的内容追加到/home/Tom/.ssh/authorized_keys里面

	command="$HOME/bin/rrsync -ro /home/Tom/rsync-root",no-agent-forwarding,no-port-forwarding,no-pty,no-user-rc,no-X11-forwarding RSA_PUBLIC_KEY

这样完成了设置认证及公钥。RSA_PUBLIC_KEY即为上文的公钥内容。目的是让VPS的shell认识客户端的公钥，然后选择rrsync作为默认TTY，根目录设置为刚新建的rsync-root。

提示：去掉"-ro"可以去掉只读属性，即允许客户rsync修改文件。

我们可以在/home/Tom/rsync-root目录下，新建一个testDir目录作为测试rsync

	cd /home/Tom/rsync-root
	mkdir testDir
	echo "helloWorld" > testDir/README.md
	chown -R Tom /home/Tom

## 客户端测试同步

客户端在ssh config中添加即可，注意指定正确的id_rsa私钥路径。

	mkdir -p ~/.ssh
	vi ~/.ssh/config
	
	# 增加以下内容
	Host sync-host
	User Tom
	Hostname 123.123.123.123
	Port 22
	ServerAliveInterval 30
	IdentityFile ~/id_rsa
	
修改默认权限

	chmod -R 700 ~/.ssh

测试

	cd /tmp
	rsync -a --progress sync-host:/testDir /tmp/
	
测试安全性

	ssh sync-host
	
尝试登陆会出现错误，并拒绝登陆，防止客户端意外登陆到VPS修改配置。

	PTY allocation request failed on channel 0
	/home/Tom/bin/rrsync: Not invoked via sshd
	Use 'command="/home/Tom/bin/rrsync [-ro] SUBDIR"'
			in front of lines in /home/Tom/.ssh/authorized_keys
	Connection to 123.123.123.123 closed.
	
## 参考链接

[Restricting rsync access with SSH](https://www.whatsdoom.com/posts/2017/11/07/restricting-rsync-access-with-ssh/)
