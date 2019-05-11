title: GPT分区表上硬盘安装ubuntu
date: 2016-03-12 00:21:14
tags: ubuntu
categories: 编程
---
ubuntu我一直都是使用硬盘安装，没有试过烧录到u盘引导安装，因为不想浪费一个优盘。
<!-- more -->

## 前戏

确保U盘为fat32分区（uefi引导不支持ntfs），大约2GB，能装得下ubuntu的iso镜像，复制iso到该分区根目录下

打开iso中的/boot/grub/grub.cfg，另存为到U盘根目录下。

## 修改grub配置

使用Notepad++或者类似的第三方记事本应用程序修改U盘根目录下grub.cfg
*不使用系统自带记事本，因记事本这个坑爹货无法保存为Unix格式+UTF8*

文件中部找到以 menuentry 开头的四段，把它们都删除了，换成下面的menuentry内容，

    menuentry "Install Ubuntu" {
		set root=(hd1,msdos1)
		loopback loop /ubuntu-14.04-desktop-amd64.iso
		linux (loop)/casper/vmlinuz.efi persistent boot=casper iso-scan/filename="/ubuntu-14.04-desktop-amd64.iso" quiet splash ro locale=zh_CN.UTF-8 noprompt --
		initrd (loop)/casper/initrd.lz
    }

以上内容，根据每个人电脑实际情况，要修改的地方有：
- set root=(hd1,msdos1) 这个值随意写，反正都是错误的，后面步骤会改成正确的
- ubuntu-14.04-desktop-amd64.iso 镜像文件名，需要修改两次
- initrd.lz 和 vmlinuz.efi 根据iso中的casper文件夹下对应名称改动

## 添加LiveCD引导项

在Win10下，使用DiskGenius等工具，挂载系统的ESP分区

WinRAR打开ubuntu的iso镜像文件，提取grubx64.efi解压到系统ESP分区中
- /EFI/Boot/grubx64.efi

最后使用 bootice x64 1.3.3 以上的版本:
![](/images/install_ubuntu_on_gpt/bootice.jpeg)

在UEFI选项卡中添加EFI引导项，引导文件位置：

	\EFI\BOOT\grubx64.efi

该引导项起个名字，例如 my-grub
顺便勾选“下次从该引导项启动”，重启。

记得插着U盘。

注意：Win10如果直接重启无法进入LiveCD。正确做法：开始菜单-> “设置”-> “恢复”-> “使用高级启动”-> 选择“ubuntu_install”。

## 设置LiveCD分区

启动到grub2后，进入GRUB command line，这时候可以查看当前的所有分区

	ls 或者 ls -l

![](/images/install_ubuntu_on_gpt/grub_4.png)

记下U盘所在正确的分区号，比如我的U盘是(hd1,msdos1)，输入加载grub.cfg内容如下

	configfile (hd1,msdos1)/grub.cfg

屏幕会出现"Install Ubuntu"启动项。按e编辑"Install Ubuntu"启动项参数，修改

	set root=(hd1,msdos1)

修改后直接按F10启动系统，进入LiveCD。

## 安装

点击安装系统前，先卸载isodevice卷：少了这一步会分区失败

	sudo umount -l /isodevice

最后到分区那一个步骤时候，选择安装grub2引导器到整个硬盘

## 引导失败处理

现有win8 + ubuntu共存，GPT分区。

正常启动界面应该是这样的

![](/images/install_ubuntu_on_gpt/grub_1.jpg)

出**大事**了：无法引导ubuntu。一般分两种情况

### 找不到grub.cfg

这种情形最容易解决，错误图如下

![](/images/install_ubuntu_on_gpt/grub_2.jpg)

输入看看所有盘

	ls -l

查找/boot分区（不是EFI引导分区），记下来，比如我是(hd1,gpt5)。提示：如果/boot没有单独分区，一般是/分区

按c进入命令行，在grub2命令行执行下列命令，根据实际情况修改root的值为/boot分区

	set root=(hd1,gpt5)
    configfile /boot/grub/grub.cfg

可以顺利引导原来硬盘的ubuntu，说明grub.cfg是正常的。

进入ubuntu后。先挂载EFI分区 我的EFI分区设备文件是/dev/sda1
	
	sudo umount /boot/efi
	sudo mkdir /tmp/efi
	sudo mount /dev/sda1 /tmp/efi

运行以下命令获取UUID

	sudo blkid
	
从输出结果找到/boot分区，比如我是这一行

	/dev/sda3: UUID="78dfa873-8779-48eb-9323-1a0400749a48" TYPE="ext4" PARTUUID="de862c9f-8f69-4a36-8b85-8e2df635c802"
	
那么我的UUID是

	78dfa873-8779-48eb-9323-1a0400749a48

留意/dev/sda3（root的值），对应第一块硬盘的第三个分区。gpt表示法是硬盘从0开始下标，分区号从1开始。转成gpt表示法即

	hd0,gpt3

修改EFI分区的grub.cfg

	sudo vi /tmp/efi/EFI/ubuntu/grub.cfg

将search.fs_uuid修改为正确的/boot所在的分区的UUID，将root改为gpt分区。

最终的grub.cfg内容如下

	search.fs_uuid 78dfa873-8779-48eb-9323-1a0400749a48 root hd0,gpt3
	set prefix=($root)'/boot/grub'
	configfile $prefix/grub.cfg

重启应该可以进入ubuntu了。

### 无法加载grub模块

这种情况略麻烦一点，没有完整地加载grub的模块驱动，此时的grub称为rescue模式（阉割版？）

现象如下图，不过这个图我是从网上随便找到的，实际上看到grub rescue差不多就是这个情况了。

![](/images/install_ubuntu_on_gpt/grub_3.jpg)

原因我猜是GRUB版安装错误？引导器安装到不兼容的CPU架构？

重启，进入LiveCD

安装正确系统版本GRUB-EFI二进制文件

	sudo apt update
	sudo apt install grub-efi -y

此时确保新安装grub二进制文件夹下有normal.mod模块，比如我的的64位系统，架构是x86_x64-efi。

	ls /usr/lib/grub/x86_x64-efi | grep normal.mod

挂载*已经安装好系统的根分区*，比如为/dev/sda3，那么命令如下

	sudo mkdir /media/my-ubuntu
	sudo mount /dev/sda3 /media/my-ubuntu
	
将新的x86_64-efi文件夹复制到/boot/grub目录

	sudo cp -R /usr/lib/grub/x86_x64-efi /media/my-ubuntu/boot/grub/
	
这样就完成了缺失grub模块的修复。重启，继续。一般可以直接进入系统。

如果能进入ubuntu，就继续更新grub，确保将来能引导Windows Boot Manager。

	sudo update-grub

如果无法进入ubutnu，依然是出现错误，需要手工加载grub的normal模块

使用ls命令查看所有分区序号

	ls 或者 ls -l

查找/boot分区（不是EFI引导分区），记下来，比如我是(hd0,gpt3)。下一步将绑定root分区，在grub rescue命令行中输入

	set root=(hd0,gpt3)
	set prefix=/boot/grub
	insmod normal
	normal

即可进入加载好驱动的GRUB2完整版。如果点击"Ubuntu"启动项错误，可以参考第一种情况修改正确的/boot分区，或者指定内核文件。

启动进入Ubuntu，使用boot-repair修复（需要联网）

	sudo apt-add-repository ppa:yannubuntu/boot-repair
	sudo apt-get update
	sudo apt-get install -y boot-repair
	boot-repair

点击高级选项
- GRUB位置" -> 单独的/boot/efi"选择当前引导的ESP分区(/sda1)，一般是第一个分区。
- GRUB选项" -> 取消勾选"Secure Boot"
- GRUB选项" -> 勾选"重装GRUB前先移除它

直接修复，修复时间可能较长，而且屏幕上会出现一定的指示去下载一定的软件包。我花了十分钟boot-repair才弄好。
