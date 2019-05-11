title: ssh-key用法
date: 2016-1-21 21:13:37
tags: [shell, git]
categories: 读书笔记
---
ssh-key对于Linux重度用户可是一个牛逼东西，简化登陆流程，可谓神器！
<!-- more -->
## github

生成ssh-key可以免密码进入仓库，参考[github官方教程](https://help.github.com/articles/generating-ssh-keys/)

### 确认重名key
一、首先看看有没有之前生成的旧key

	ls -al ~/.ssh
    
若有旧ssh-key，可以选择删除，也可以后来生成时候指定产生新文件名

### 生成key

	# Generate public/private rsa key pair.
	ssh-keygen -t rsa -b 4096 -C "your_email@example.com"
	# passphrase可以不用设置，设置了的话，每次用ssh都要输入

假设生成的自定义ssh-key为私钥 ~/.ssh/id_rsa_github 和 公钥 id_rsa_github.pub。
    
### 本地添加私钥

首先重新生成hostname

	ssh-keygen -R github.com
    
使得ssh-agent后台运行：

	eval "$(ssh-agent -s)"
    
出现Agent pid XXXX这样的提示之后：

    ssh-add ~/.ssh/id_rsa_github

### 远端添加公钥

打开公钥：【注意是公钥！！】

	cat id_rsa_github.pub
    
将内容拷贝至[https://github.com/settings/ssh](https://github.com/settings/ssh)中，这个key成为完全控制每一个仓库commit权限，要谨慎保存，泄露的话你的github说不定仓库被人清空了。

    
### 联机验证登录

	ssh -T git@github.com
    
添加到本机缓存中，然后github提示You've successfully authenticated。
然后你的deploy key页面的钥匙图标变成绿色，代表可以使用了

### 改用ssh推送

修改你的本地仓库中的config

	vi .git/config
    # 注释掉https
    # url = https://github.com/USERNAME/REPO_NAME.git
    # 更换为ssh,注意有个冒号
    url = git@github.com:USERNAME/REPO_NAME.git
    
+安全提示：+

*强烈建议每一个仓库对应一个key，而不是用一个权限无穷大的key，风险太大。
每个仓库的key可以在下面地址设置：*

	https://github.com/USERNAME/REPO_NAME/settings/keys
    
试一下git push，是不是可以自动推送了。

## 自动登陆VPS

首先确保vps上面的iptables规则允许22端口入站出站

PC机子执行

	ssh-keygen -t rsa -b 4096 -C "your_email@example.com"
	
如私钥保存为~/.ssh/id_rsa_myhost，则修改config

	vi ~/.ssh/config
	# 添加
	Host myhost
	 User root
	 Hostname 111.222.111.222
	 Port 22
	 ServerAliveInterval 30
	 IdentityFile ~/.ssh/id_rsa_myhost
	 
上传公钥到你的vps

	scp ~/.ssh/id_rsa_myhost.pub root@111.222.111.222:/root/.ssh/
	
登陆到vps添加公钥记录

	cd ~/.ssh
	cat id_rsa_myhost.pub >> authorized_keys
	service ssh restart
	
修改权限，防止被他人看见密钥内容

	# PC机子和VPS都要设置
	chmod 700 ~/.ssh/* 

这样在PC就能使用一句话登陆ssh或者scp传文件了

	ssh myhost
	scp /tmp/test.txt myhost:/tmp/
	
安全设置：对于已经开启了公私鈅登陆的主机，可以禁用密码登陆，效果是：使用密码无权进入

	vi /etc/ssh/sshd_config
	
	# 仅允许root用户使用公私鈅登陆（不允许密码登陆）
	PermitRootLogin prohibit-password
	
	# 禁止一切用户使用密码登陆（这个选项根据个人实际设置，默认值是yes）
	PasswordAuthentication no
	
## TC自动部署hexo

可以自动更新博客，这个TC原本的目的不是这样的。
TC原本是代码集成平台，居然用成了博客自动更新机，真是大材小用。

首先关联github账户到travis-CI，然后勾选博客的源代码项目，不是github.io项目。
接下来完成ssh的绑定和.travis.yml修改

在Cloud9上面的虚拟主机操作。因为集成了npm环境。

安装travis：

	gem install travis
    travis login --auto

上传密钥到cloud9，然后放到项目里面，例如我的项目是github.com/aaa/bbb，私钥名称id_rsa_github

	mv id_rsa_github aaa/bbb/
    cd aaa/bbb
    touch .travis.yml
    travis encrypt-file id_rsa_github --add

最后把私钥删除了，留下了这个加密后的私钥。

新建一个ssh配置文件：
	
    vi ssh_config
    # # #
    Host github.com
      User git
      StrictHostKeyChecking no
      IdentityFile ~/.ssh/id_rsa
      IdentitiesOnly yes

修改travis脚本
    
	vi .travis.yml
	# 输入内容
	language: node_js
	node_js:
	- 4.2 
	branches:
	only:
	- master
	- test
	before_install:
	- openssl aes-256-cbc -K $encrypted_f88d79a9e3f2_key -iv $encrypted_f88d79a9e3f2_iv -in .travis/id_rsa.enc -out ~/.ssh/id_rsa -d
	- chmod 600 ~/.ssh/id_rsa
	- eval $(ssh-agent)
	- ssh-add ~/.ssh/id_rsa
	- cp .travis/ssh_config ~/.ssh/config
	- mkdir temp_lxc
	- cd temp_lxc
	install:
	- npm install -g hexo-cli
	- npm install hexo --save
	- hexo init
	- npm install hexo-deployer-git --save
	before_script:
	- cp -R ../source ./
	- cp -R ../themes ./
	- cp ../_config.yml ./
	- git config --global user.name 'lixingcong'
	- git config --global user.email 'lixingcong@live.com'
	script:
	- hexo clean
	- hexo d -g

实际上，这个文件非常灵活，我是折腾很久才摸索出来的。每个人肯定不一样。我的是以自己的仓库改的，我的仓库地址：[my_hexo_blog](https://github.com/lixingcong/my_hexo_blog)

How to work? 向my_hexo_blog推送的同时，自动更新lixingcong.github.io。

如果遇到问题，可以谷歌关键词“Travis CI Hexo”得到很多结果，可供参考

## GPG签名

直接生成，填入正确信息，还有密码（可选），按[Github官方教程](https://help.github.com/articles/generating-a-new-gpg-key/)操作即可。

	gpg --gen-key

生成过程需要大量随机字节。可以动鼠标还有打字。时不时在GPG生成界面按一下Enter提醒它。

查看GPG

	gpg --list-keys
    # 记下 第一行的公钥2048/A8F99211后面的这个A8F99211
    # 后面用到它：<public GPG key>

将公钥导入到github中，将本地提交打上tag

	git config --global user.signingkey <public GPG key>

其中 < public GPG key \>为上面你记下的公钥

备份GPG：(包括公钥，私钥，信赖库)

	gpg -a --export user@mail.com > ~/public-gpg.key
	gpg -a --export-secret-keys user@mail.com > ~/secret-gpg.key
    gpg --export-ownertrust > ~/ownertrust.txt
    
还原GPG(导入私钥时自动导入公钥)

	gpg --import ~/secret-gpg.key
    gpg --import-ownertrust ~/ownertrust.txt
    
git提交commit时候进行签名

	# 配置git
	git config --global user.signingkey <public GPG key>
	
	# 对tag进行GPG签名。参数-s
	git tag -a -s v2.0 -m "SIGN-MY-TAG"
	
