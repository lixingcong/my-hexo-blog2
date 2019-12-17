---
title: git的rebase
date: 2019-12-04 10:19:36
tags: git
categories: 网络
---
对rebase(变基)文档翻译、并转载部份博客，同时加入自己的理解。
<!-- more -->
## 变基与合并区别

merge合并，将两个分支合并成一个，从提交图上可以看见分叉的提交历史**有分叉、又重新汇合**成一个分支。（如下动图）

![](/images/git-rebase/merge.webp)

merge就以结果来看，就像两条公路最后会聚成一条公路。

rebase，又叫变基、衍合。实际上就是重新定义参考基准。从提交图上看见提交历史**不会有分叉与汇合**（如下动图）

![](/images/git-rebase/rebase.webp)

rebase就以结果来看，像是「把feature分支剪下来，然后贴在master分支上」，与园艺插花时「嫁接」的操作一致。

其实选用merge还是rebase取决于你到底是以什么意图来避免合并冲突，当然本文侧重rebase。

## 变基原则

一般我们使用rebase的目的，是想要得到一个能在远程分支上干净应用的补丁（Fast-forward patch）。

比如某些项目你不是维护者，但想帮点忙的话，最好用rebase：先在自己的一个分支里进行开发，当准备向主项目提交补丁的时候，根据最新的 origin/master 进行一次rebase然后再提交，这样维护者就不需要做任何整合（merge）工作，合并后就是fast forward

实际上是把解决分支补丁同最新主干代码之间冲突的责任，化转为由提交补丁的人来解决。要rebase就必须要遵守一条准则：

一旦分支中的改动发布到公共主线，就千万不要对该分支进行rebase操作。

如果你遵循这条准则，就不会出错。否则，广大人民群众会仇恨你，你的朋友和家人也会嘲笑你，唾弃你。因为rebase抛弃了一些现存的提交对象而创造了一些**类似但不同的新的提交对象**，它们的SHA-1值和原来的提交不一样。。如果你把原来分支中的提交对象发布出去，并且其他人更新下载后在其基础上开展工作，而稍后你又用rebase抛弃这些提交对象，把新的重演后的提交对象发布出去的话，你的同事就不得不重新合并他们的工作，这样当你再次从他们那里获取内容时，**提交历史就会变得一团糟，交叉乱象险生。**

## 最简单的rebase

当前提交历史图如下

![](/images/git-rebase/basic-rebase-1.png)

操作意图：切换到experiment分支，然后变基到master分支，得到一个基于master分支的fast-forward分支。

```
$ git checkout experiment
$ git rebase master
```

![](/images/git-rebase/basic-rebase-3.png)

变基后的experiment分支可以直接发起pull request给master分支的维护者进行fast-forward合并。

## 交互式rebase

假如当前的提交历史是这样

![](/images/git-rebase/interactive-1.png)

操作意图：原本想切换到style分支，然后变基到list分支。但list分支并不希望style分支的这么做，因为list分支提交太乱了，提交历史有如下的问题：
- "wrap the form with div" 排得太靠后了，想移动到前面
- "fix typo of age field name" 跟 "add student id and age..." 可以合并成一次提交
- "add student id and age ..." 改动内容太多，应拆分成两次提交
- "form to add more studetns" 提交信息有错别字 “studetns”
- "add gender select box" 里面的代码有typo（如int a=100应该为int a=1000）

利用rebase的Interactive Mode。连commit的內容都可以改，只需加入```-i```参数运行即可。以上面例子，list分支是based on 0580eab8 (fill in gender column）：

```
git checkout list
git rebase -i 0580eab8
```

此时git会打开文件```.git/rebase-merge/git-rebase-todo```让你指示rebase应该怎么做。

此时的git-rebase-todo文件内容
```
pick 2c97b26 form to add more studetns
pick fd19f8e add student id and age field into the form
pick 02849bf fix typo of age field name
pick bd73d4d wrap the form with div
pick 74d8a3d add gender select box

# Rebase 0580eab..74d8a3d onto 0580eab
# ...[chunked]
```

每一行开头动作有如下几种操作：

|命令|备注|
|--|--|
|pick|保留这次commit，无改动|
|reword|保留这次commit ，要改commit message|
|edit|保留这次commit，要改commit的內容（源代码）|
|squash|保留这次commit，要跟前一次提交合并，并保commit message|
|fixup|squash + 只使用前一次提交的message，丢弃这条message|
|exec|执行一个命令（没有实测过）|

编辑这个文件，保存后就会自动触发rebase。遇到reword会弹出vi编辑提交信息。

### 调整提交顺序、修改提交信息

操作意图：
- 把 "wrap the form with div" 移到 "form to add more studetns" 后面
- 提交 "form to add more studetns" 要改 commit message ，修复students

对应的git-rebase-todo文件内容改成

```
reword 2c97b26 form to add more studetns
pick bd73d4d wrap the form with div
pick fd19f8e add student id and age field into the form
pick 02849bf fix typo of age field name
pick 74d8a3d add gender select box
```

保存并关闭vi编辑器，自动执行rebase，遇到reword则弹出让你修改。完成后提示

```
Successfully rebased and updated refs/heads/list.
```

此时的提交历史图如下，list分支依旧是based on 0580eab8，而0580eab8后rebase过的commit全部更换了commit SHA ID

![](/images/git-rebase/interactive-2.png)

### 多个提交合并成一个

操作意图：
- "fix typo of age field name" 跟 "add student id and age..." 可以合并成一次提交

同样使用```git rebase -i 0580eab8```命令，对应的git-rebase-todo文件内容改成

```
pick c3cff8a form to add more students
pick 7e128b4 wrap the form with div
pick 0d450ea add student id and age field into the form
fixup 8f5899e fix typo of age field name
pick e323dbc add gender select box
```

变基后，提交历史如下，那两次提交，已经成功合并成一次提交```53616deb6f```

![](/images/git-rebase/interactive-3.png)

### 修改某次提交

操作意图：
- "add gender select box" 里面的代码有typo（如int a=100应该为int a=1000）

同样使用```git rebase -i 0580eab8```命令，对应的git-rebase-todo文件内容改成

```
pick c3cff8a form to add more students
pick 7e128b4 wrap the form with div
pick 53616de add student id and age field into the form
edit c5b9ad8 add gender select box
```

保存关闭vi后，此刻工作区状态是停在刚commit 完 "add gender select box" 那一刻，所以现在可以偷改你想修改的源代码，用 git add 将改动存入暂存区staging area然后继续rebase

```
git rebase --continue
```

git会将暂存区与"add gender select box"通过命令```commit --amend```一起重新 commit

### 一个提交拆散成多个

操作意图：
- "add student id and age ..." 改动内容太多，应拆分成两次提交

同样使用```git rebase -i 0580eab8```命令，对应的git-rebase-todo文件内容改成

```
pick c3cff8a form to add more students
pick 7e128b4 wrap the form with div
edit 53616de add student id and age field into the form
pick 4dbcf49 add gender select box
```

保存关闭vi后，此刻工作区状态是停在刚commit 完 "add gender select box" 那一刻，此刻可以移动HEAD指针到前一次提交（soft-reset）

```
git reset HEAD^
```

此时工作区为dirty状态，可以通过```git status```查看。

现在就可以逐一逐一改动地commit了。原本是 add student id and age field ，想拆分成两次提交：
- add student id field
- add age field

两次commit完成后，继续变基

```
git rebase --continue
```

![](/images/git-rebase/interactive-4.png)

此时list分支的程序员已经折腾完毕，可以放心地告诉style分支：你看我瞎倒腾一天，终于可以让你rebase了！！！

```
git checkout style
git rebase list
```

![](/images/git-rebase/interactive-5.png)

## 基于分支再分支的rebase

TODO：尚未整理。。。。

## 怎么取消变基

新手可能在没搞懂rebase概念时误操作，想还原变基前的操作，可以使用reflog命令，找到变基前的提交SHA值，直接reset hard到那一次即可。

```
$ git reflog           
718d8a7 (HEAD -> b34) HEAD@{0}: rebase -i (finish): returning to refs/heads/b34
718d8a7 (HEAD -> b34) HEAD@{1}: rebase -i (continue): yy2
3962665 (b12) HEAD@{2}: rebase -i (start): checkout b12
2d4340a HEAD@{3}: commit: yy2
d5451a4 (master) HEAD@{4}: checkout: moving from master to b34
d5451a4 (master) HEAD@{5}: checkout: moving from b12 to master
3962665 (b12) HEAD@{6}: commit: xx2
d5451a4 (master) HEAD@{7}: checkout: moving from master to b12
d5451a4 (master) HEAD@{8}: commit: v2
ae51330 HEAD@{9}: commit (initial): ini
```

如上面，rebase操作前的一次提交是2d4340a，直接```git reset --hard 2d4340a```即可。

## 参考文档

[git rebase ref](https://git-scm.com/book/en/v2/Git-Branching-Rebasing)

[Git-rebase 小筆記](https://blog.yorkxin.org/2011/07/29/git-rebase)

[知乎:为什么你应该停止使用rebase](https://zhuanlan.zhihu.com/p/29682134)
