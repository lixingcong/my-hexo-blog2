title: 搭建git服务器
date: 2017-04-03 13:39:02
tags: git
categories: 网络
---
对于几个人的小团队，可以自行在公司内网搭建git服务器，实现版本控制。配合gitolite实现权限控制。
<!-- more -->
我搭建的服务器框架大致如下图：

![](/images/setup_git_server/git-topo.png)

那么以下的搭建操作就是基于这个图进行配置。每个节点都是ubuntu 16.04发行版，图示有4个节点

|节点|功能|
|--|--|
|Server|中心化的git仓库，本文假设IP为192.168.100.100|
|Alice|网管，负责创建仓库或者各种访问权限|
|Bob|项目组长，负责Code-Review和Merge分支到master,拥有修改仓库权限|
|Carl|项目开发者，只能在自己分支上面修改|

## 搭建服务器

搭建服务器操作在Server和Alice节点进行

### Alice端

生成一个rsa密钥对

	cd /tmp
	ssh-keygen -t rsa -b 4096 -C "alice"

假设生成的公私钥为

	# 私钥
	~/.ssh/alice
	# 公钥
	~/.ssh/alice.pub

将私钥写入当前用户ssh配置文件中

	vi ~/.ssh/config
	# 添加
	Host server
	  User git
	  Hostname 192.168.100.100
	  Port 22
	  ServerAliveInterval 30
	  IdentityFile ~/.ssh/alice


将公钥上传到Server备用

	scp ~/.ssh/alice.pub root@server:/tmp/

### Server端

留意gitolite的README，提到依赖的软件，有最低版本的要求

- git 1.6.6 or later
- perl 5.8.8 or later
- openssh 5.0 or later

创建git账户并切换到新帐户

	adduser git
	su git
	cd ~

创建空的ssh配置目录

	mkdir -p ~/.ssh

克隆gitolite仓库

	git clone https://github.com/sitaramc/gitolite
	cd gitolite

创建一个目录存放gitolite二进制文件,然后安装

	mkdir -p ~/bin
	./install -to ~/bin

设置Alice的公钥，这样Alice就成为了gitolite管理员

	~/bin/gitolite setup -pk /tmp/alice.pub

所有仓库存放在```~/repositories```下，gitolite会自动修改```~/.ssh/authorized_keys```实现不同用户的访问。

因此单独使用一个git用户的目的是，不希望用户手动修改authorized_keys里面的内容，而是通过gitolite来间接修改它。

## 仓库创建与权限

修改访问权限在Alice节点进行

克隆admin仓库，因为服务器只有Alice的公钥，其它用户无权访问。

	cd ~
	git clone git@server:gitolite-admin
	cd gitolite-admin

直接编辑这个conf文件实现权限管理

	conf/gitolite.conf

详细的权限和仓库创建可以参考官方README:

	http://gitolite.com/gitolite/conf
	https://github.com/sitaramc/gitolite#adding-users-and-repos

比如我修改为

	repo foo
		RW+                     =   bob
		-   master              =   carl
		-   refs/tags/v[0-9]    =   carl
		RW+ carl                =   carl
		R                       =   carl

那么达到的效果是：
1. 创建了一个名字为foo的仓库
2. RW+表示可读可写可overwrite，Bob拥有仓库最大权限
3. 减号说明Carl没有master分支和tags的读写权限
4. Carl只能在自己分支(carl分支)上面进行修改,拥有carl分支的最大权限
5. Carl可以读取其它分支，这时候就可以读master分支了

注意等号后面的名字是跟ssh公钥文件名字对应的，如果gitolite-admin/keydir目录下的公钥文件名字是

	carl_ssh_key.pub

那么等号后面的内容就不是carl，而是carl_ssh_key

为了与conf/gitolite.conf中的帐户对应，创建ssh公私鈅要保存为正确的文件名。
	
	# 输出id_rsa的时候,保存为~/my_gitolite_keys/bob
	ssh-keygen -t rsa -b 4096 -C "bob"
	
	# 输出id_rsa的时候,保存为~/my_gitolite_keys/carl
	ssh-keygen -t rsa -b 4096 -C "carl"

把~/my_gitolite_keys/中对应的公私鈅交给Bob和Carl，下面测试步骤，要用到公私鈅

将公钥文件添加到gitolite-admin仓库中

	cp ~/my_gitolite_keys/*.pub ~/gitolite-admin/keydir/

修改conf和生成密钥完毕，就可以commit，然后将新配置push给server端

	cd ~/gitolite-admin
	git add *
	git commit -m "add user Bob, Carl; generate keys"
	git push

那么server端在push结束后自动执行perl脚本，实现权限管理。

## 测试

测试git在Bob和Carl节点进行

将由Alice交给Bob和Carl的公私鈅，分别存放到各自节点的.ssh目录下

|节点|公私鈅存放路径|
|---|---|
|Bob|公钥 ~/.ssh/bob.pub<br>私钥 ~/.ssh/bob|
|Carl|公钥 ~/.ssh/carl.pub<br>私钥 ~/.ssh/carl|

### Bob节点

将私钥写入当前用户ssh配置文件中

	vi ~/.ssh/config
	# 添加
	Host server
	  User git
	  Hostname 192.168.100.100
	  Port 22
	  ServerAliveInterval 30
	  IdentityFile ~/.ssh/bob

本地克隆服务器上的foo仓库，测试修改代码

	cd ~
	git clone git@server:foo.git
	cd foo
	echo "hello world" > README.md
	git add README.md
	git commit -m "print hello world"
	git push

这样即验证了Bob有读写服务器master分支的权限

### Carl节点

将私钥写入当前用户ssh配置文件中

	vi ~/.ssh/config
	# 添加
	Host server
	  User git
	  Hostname 192.168.100.100
	  Port 22
	  ServerAliveInterval 30
	  IdentityFile ~/.ssh/carl

本地克隆服务器上的foo仓库，测试修改代码

	cd ~
	git clone git@server:foo.git
	cd foo
	echo "try to modify branch master" > README.md
	git add README.md
	git commit -m "invalid commit"
	git push

这么做push的话会被拒绝，即验证了Carl没有写服务器master分支的权限。

下面测试在carl分支的工作流程

情况1: 如果上游没有carl分支，可以添加一个carl分支并Push到服务器上

	git checkout -b carl
	echo "create branch carl" > README.md
	git add README.md
	git commit -m "branch: carl created"
	git push --set-upstream origin carl

情况2: 如果上游已经存在carl分支，直接切换到carl分支

	git checkout carl

经常性使用git pull以拉取服务器上最新的代码版本。

## 安全设置

修改默认的shell为gitolite专用的，而不是默认的bash

	sudo vi /etc/passwd
	# 修改git用户的shell程序为/home/git/bin/gitolite-shell

不允许git用户使用密码登陆

	sudo vi /etc/ssh/sshd_config
	# 增加以下
	Match User git
		PasswordAuthentication no
	
	# 重启ssh服务
	sudo service ssh restart

## 参考链接

[ssh key配置](/2016/01/21/ssh-key-usage)
[gitolite-README](https://github.com/sitaramc/gitolite/blob/master/README.markdown)
[gitolite搭建git仓库管理服务](http://perthcharles.github.io/2015/08/24/setup-gitolite-service-git-1/)
