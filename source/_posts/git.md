title: Git读书笔记
date: 2015-10-31 20:20:28
tags: git
categories: 读书笔记
---
git入门
简单写一下读后感。
我看完的参考书籍，建议先看译本，再看英文原版加深理解：
<!-- more -->

|书名|Pro Git|Version Control with Git|
|---|---|---|
|作者|Scott Chacon & Ben Straub|Jon Loeliger & Matthew McCullogh|
|下载地址|[在线查看](https://git-scm.com/book/)|[PDF](http://www.foo.be/cours/dess-20122013/b/OReilly%20Version%20Control%20with%20GIT.pdf)|

## 本地仓库操作

先熟练掌握本地仓库的操作才比较容易理解远程仓库

### 初次运行git前配置

    git config --global user.name "YOUR_NAME"
    git config --global user.email "YOUR_EMAIL"
    
若需要提交GPG签名

	git config --global user.signingkey <public GPG key>
    # <public GPG key>是你的公钥id，可以从gpg --list-keys查看
    
若要检查你的git配置，打开～/.gitconfig文件即可。这是我的配置文件：

	[user]
		name = lixingcong
		email = lixingcong@xxx.xxx
		signingkey = xxx  # GPG公钥id(可选)
	[push]
		default = simple  #使用新版的推送模式
	[alias]
		ls = log -1 HEAD  # 别名，快捷命令
		ad = add *
		cm = commit -am
		lg = log --pretty=format:"%Cred%h%Creset-%cD-%Cgreen%s" --graph
		lg1 = log --pretty=oneline
		st = status -s
		ps = stash
	[credential]
		helper = cache  #凭证缓存
	[help]
		autocorrect = 15  #自动更正错误

也可以运行查看当前的configuration
	
    git config list
    
开始工作！
初始化一个文件夹为git仓库：

	mk /tmp/test && cd /tmp/test
    git init


### 工作区与暂存区

你在资源管理器里面看到的文件是“工作区”，你通过git add跟踪的文件存放的缓存位置是“暂存区”。
当你的“工作区”与“暂存区”中的文件是一致的，可以称为“干净”的。反之，称为“脏的”。
查看当前的工作区与暂存区命令

	git status
    
这个命令要尽量多使用，可以了解当前目录的情况。
执行status后，绿色的表示已经跟踪并且存放到暂存区（可以随时commit）。红色的表示尚未跟踪，使用

	git add FILE-NAME
  
可以将该文件加入暂存区，生成一个缓存。（准确来说是一个object）
每次修改玩完一个文件，都需要加入暂存区。
看看当前目录下与暂存区有什么区别？

	git diff

关于git diff，主要有三种使用方法：

	git diff #工作区中文件更新了哪些内容
    git diff HEAD #工作区与HEAD中内容对比
    git diff --cached #暂存区将要添加到commit的内容
    
HEAD指针可以理解为最近一次的提交。

### 提交

通过add跟踪后并使用status确认工作区是干净的，可以进行提交。‘
这里的提交，不是指提交到github上面，而是提交到本地的git仓库。
因为每一个文件夹下都有一个隐藏的.git文件夹，存放提交的历史记录。

	git commit -m "YOUR-COMMENT"
    
若文件file1已经被跟踪，后来又修改file1,可以使用-add选项，不需另外敲add跟踪

	git commit -am "YOUR-COMMENT"
    
若提交后，发现文件A中某句话写错了，修改文件A后，可以追加提交：覆盖上一次提交。建议书写your-comment跟上一次提交一致。

	git commit -am "YOUR-COMMENT" --amend

### 打标签

提交确认无误可以进行打标签，方便后期管理

	git tag -a v1.0 -m "YOUR-COMMENT-ON-THIS-TAG"
    
如果已经有GPG key，可以对tag进行GPG签名。参数-s
	
    git tag -a -s v2.0 -m "SIGN-MY-TAG"
    
验证tag签名

	git -v v2.0

默认情况下，git push 并不会把标签传送到远端服务器上，只有通过显式命令才能分享标签到远端仓库。其命令格式如同推送分支：

	git push origin [tagname]

### 创建忽略提交名单

功能就是防止提交上去。忽略.o这种c语言中间文件例：

	echo *.o > ./.gitignore
    
这个ignore文件支持通配符，参考pro-git.pdf
    
### 查看提交历史
基本命令：

	git log
    
使用图的方式，比较直观看到当前更新了的分支。

	git log --pretty=format:"%s--%h--%ci" --graph
    
查看上一次的提交

	git log -1 HEAD
    
查看某个文件的提交历史

	git log FILE-NAME
    
当然，可以按时间进行筛选结果，参考pro-git.pdf

使用二分搜索查找bug出处，详细看pro-git.pdf：

	git bisect start
    git bisect bad
    git reset v1.4
    git bisect good

查看某个文件在某次Commit的完整内容（比如前2个提交）

	git show master~2:src/main.cpp


### 相对提交名
有时候使用相对提交名，以确定【相对本次提交】的【前某次提交】。

    尖角符号^n表示本次提交的来自第n个祖先（纵向，相当于爸爸/叔叔/伯伯）
    波浪号～n表示当前提交的前第n代（横向，相当于爸爸/爷爷/曾祖父）
    
### 恢复到某次提交
使用reset 命令，能把暂存区恢复到任意一次commit。
使用soft模式，将head指针指向新的提交，暂存区和工作区的内容不变。对当前工作具有最小的影响。
	
    git reset --soft YOUR-COMMIT-SHA1

使用mixed模式，不改变工作区，只恢复暂存区。
因为默认reset就是mixed模式，所以这个命令省略mixed

	git reset YOUR-COMMIT-SHA1
    
若使用\-\-hard具有更大的威力，其破坏性更大，恢复也更彻底。

	git reset --hard YOUR-COMMIT-SHA1

对比一下威力:

|选项|soft|mixed|hard|
|--|--|--|--|
|恢复HEAD指针|yes|yes|yes|
|恢复暂存区|-|yes|yes|
|恢复工作区|-|-|yes|

### 目录下文件移动与重命名
不建议使用system-commands，使用git专用的命令：

	git rm FILE-NAME
    git mv FILE1 FILE2
    
### 分支操作

使用分支是git的一大特色，加速开发。

查看分支：

	git branch

新建分支：

	git branch YOUR-BRANCH-NAME
    
切换分支：

	git checkout YOUR-BRANCH-NAME
    
合并分支：

	git merge ANOTHER-BRANCH-NAME
    
合并时候，通常会遇到冲突，平和冲突完毕，commit一次即可。
    
删除本地分支：

	git branch -D [BRANCH_NAME]
    
保存进度：
有时候需要切换紧急的分支，而又不想提交当前的分支。可以将其暂存下来。
如果不暂存，会提示切换分支失败。
保存的地方类似一个栈的数据结构。“入栈”->“出栈”

	git stash save
    
保存完毕，工作区是干净的，可以进行切换分支

恢复进度：
从紧急分支切换回来，需要恢复进度：

	git stash pop
    
变基:参考pro-git.pdf，说白了就是改祖宗。

Youtube上一段几分钟的视频，讲得非常清楚。请自备梯子观看！

<iframe width="560" height="315" src="https://www.youtube.com/embed/cSf8cO0WB4o" frameborder="0" allowfullscreen></iframe>

干货来了：

花了一小时，我详细地学习了台湾同胞写的一篇非常精彩的博客[《Git-rebase 小筆記》](https://blog.yorkxin.org/2011/07/29/git-rebase)，终于知道怎么用rebase了，功能非常强大！！可以实现分支之间的变基、单分支的历史commit的改写（改commit message，commit代码内容，变换commit顺序等），诸多功能。。说不清楚啊！！

基本命令：分支test_rebse的base-commit变为master

	git checkout test_rebse
	git rebase master
	
交互式变基：可以单分支的改写历史commit

	git rebase -i COMMIT_SHA_ID_TO_CHANGE_START_FROM
    
删除远程分支：

	git push origin :[BRANCH_NAME]
    
    

## 远程仓库操作
主要是在本机实现一下远程的模拟。

### 制作一个远程仓库

	mkdir /tmp/remote_sample && cd /tmp/remote_sample
    git init
    git remote add origin /tmp/remote_sample
    git config receive.denyCurrentBranch warn #允许从别的地方推送
    echo hello world > test
    git add test && git commit -m "INIT_COMMENT"
    
就可以简单制作一个服务器了，有趣的是只运行在自己电脑上！

添加一个合作者，如Tony，新建一个Tony分支，以防止推送到master主线：
	
    git checkout -b Tony

当然，可以添加更多的参与者。这个remote仓库只需适时进行多分支合并到master。
多人开发某个分支会导致某些合并问题并造成覆盖。不建议多人公用一个master分支。
master分支作为最终产品发布的主线。

### 克隆，修改，提交一条龙
模拟一下github的克隆：

	mkdir /tmp/local_test && cd /tmp/local_test
    git clone /tmp/remote_sample
    cd remote_sample
    
假如我是tony，我的原则：只在自己的分支进行工作。
为了不影响其他人分支，先切换自己的分支

	git checkout Tony
    
想要进行提交，必须先跟上游服务器的HEAD一致。先从远处拉！

	git pull
    
提示Already up-to-date 就把自己的Tony专属分支拿回来了。
若多人开发这个分支，在pull时候，会遇到冲突，也许你克隆仓库后没有及时跟上分支的步伐。
强烈建议一人一个分支。
修改过程模拟：

	echo hello world from Tony! > test

按照正常步骤add ， commit提交。最后推送给远处：
	
    git push
   
### remote端收到push后合并
当remote仓库管理者得知你推送后，考虑是否把tony跟master分支合并。
模拟一下合并：

	cd /tmp/remote_sample
	git checkout master
    git merge Tony

修正冲突，合并。master分支变为新master，称为Fast-Forward，表示前进了一步

好奇，是否remote的Tony分支跟Tony的文件一致呢？
切换一下Tony分支看看：

	git checkout Tony
    git status

什么！有未提交的文件！这是什么鬼！
原因很简单，暂存区是tony提交的，在【Tony分支暂存区】test文件内容是Hello world form tony。
而在【remote工作区】的test文件内容是hello world。
因此造成一种“别人都改了，怎么我变化没有呢？”的错觉。实际上缓存区已经是Tony最新的副本了。
HEAD指针还停留在原来地方，落后于Tony分支。
证明一下：查看从工作区准备commit的内容

	git diff --cached
    
可以看到，当前工作区居然想把Tony写的那句from tony删掉了。
因此，要想保持工作区与分支是一致的，使用reset更新当前工作区。
(不可逆的工作区清倒)

	git reset --hard SHA1-OF-TONY-LAST-PUSH
   

## Github常用操作

### Fork
在以前,“fork”是一个贬义词,指的是某个人使开源项目向不同的方向发展,或者创建一个竞争项目,使得原项目的贡献者分裂。
在 GitHub,“fork”指的是你自己的空间中创建的项目副本,这个副本允许你以一种更开放的方式对其进行修改。

### Pull Request
GitHub 设计了一个以合并请求为中心的特殊合作流程。不管你是在一个紧密的团队中使用单独的版本库,或者使用许多的“Fork”来为一个由陌生人组成的国际企业或网络做出贡献,这种合作流程都能应付。
流程通常如下:
- Fork
- 从 master 分支中创建一个新分支
- 提交一些修改来改进项目
- 将这个分支推送push到 GitHub 上
- 创建一个合并请求Pull Request
- 讨论(通过网页或者邮件),根据实际情况继续修改
- 项目的拥有者合并或关闭你的合并请求

:eyes:


### Wiki
wiki页面的制作。

## 有待了解的知识点

列个表，自己还不懂的:
- 以补丁形式的Merge
- Git服务器搭建


未完成：配图片。实际的pul request操作