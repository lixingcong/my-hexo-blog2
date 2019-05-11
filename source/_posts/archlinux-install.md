---
title: 硬盘安装Archlinux
date: 2018-06-12 10:26:27
tags: archlinux
categories: 网络
---
记录一下Archlinux的安装过程，减少以后折腾耗时。
<!-- more -->

## Prepare

### 分区

不建议在Linux下直接分区。因为我太菜，经常把分区表弄乱，导致数据丢失。直接使用DiskGenius软件分出一个20G大小的新分区供Archlinux使用。

注意，该新分区是Archlinux的安装根分区，而不是下文iso文件存放的位置。

### 引导iso

首先准备能启动到GRUB2的环境，若没有的话可以利用闲置的U盘或者bcd引导，写入一个[SuperGRUB2](https://www.supergrubdisk.org/super-grub2-disk/)镜像（大小约20M），下面将利用GRUB2引导Archlinux的ISO镜像进行硬盘安装。

去[Archlinux官网](https://www.archlinux.org/download/)下载iso镜像，比如我的是```archlinux-2018.05.01-x86_64.iso```。将iso文件扔到一个磁盘根目录下。

在该分区上新建一个grub_iso.cfg文件，用于引导Archlinux ISO文件

    menuentry "Boot ArchLinux ISO" {
        set root=(hd0,xxx)
        set isofile=/archlinux-2018.05.01-x86_64.iso

        # ls -l /dev/disk/by-uuid
        set uuid=xxxx

        loopback loop $isofile
        probe -l $root -s label
        linux (loop)/arch/boot/x86_64/vmlinuz img_dev=/dev/disk/by-uuid/$uuid img_loop=$isofile
        initrd (loop)/arch/boot/x86_64/archiso.img
    }

root分区后面的(hd0,msdos1)需要根据实际做修改，不要照抄我的，有的人是(hd0,msdos5)或者(hd0,gpt4)，要根据你的磁盘的分区表类型。

UUID的获取方法？Linux下用```ls -l /dev/disk/by-uuid```命令，或者在GRUB2终端界面直接按```ls -l```即可看到。

grub_iso.cfg不确定的部分，可以不做修改，稍后在GRUB2终端界面中修改为正确的值。

使用U盘或者其它方式进入GRUB2

按c进入GRUB2的终端界面，使用```configfile (hd0,xxxx)/grub_iso.cfg```命令加载上述用于引导Archlinux ISO文件的配置文件。其中xxxx内容可以按TAB键自动补全。

使用```ls -l```命令，记下正确的root分区号和UUID，如我的是(hd1,msdos7)和0000678400004823

![](/images/archlinux-install/grub-ls-uuid.png)

按e修改```Boot ArchLinux ISO```启动项的命令，修改root和uuid为正确值。按F10直接启动进入Archlinux安装界面。

## Install

根据[官方安装说明](https://wiki.archlinux.org/index.php/installation_guide)整理出基本的安装步骤。

### 分区格式化与挂载

格式化刚刚创建的根分区：

	mkfs.ext4 /dev/sdxY （请将的sdxY替换为刚创建的分区）

挂载

	mount /dev/sdxY /mnt （请将的sdxY替换为刚创建的分区）

### 联网安装

有线使用```dhcpcd```命令，无线使用```wifi-menu```命令。使用```ping qq.com```测试网络连通性。

安装基本包

	pacstrap /mnt base

生成fstab

	genfstab -U /mnt >> /mnt/etc/fstab

改变根分区到新分区上

	arch-chroot /mnt

时区

	ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime
	hwclock --systohc

安装其他软件包

	pacman -S net-tools vim dialog wpa_supplicant ntfs-3g networkmanager
	pacman -S openssh （不需要ssh可以不装）
	pacman -S bash-completion （自动补全插件）
	pacman -S intel-ucode （英特尔CPU才需要安装）

设置语言

	vi /etc/locale.gen
	# 取消注释 zh_CN.UTF-8 UTF-8

创建并修改/etc/locale.conf

	vi /etc/locale.conf
	# 插入一行
	LANG=zh_CN.UTF-8

生成locale

	locale-gen

主机名

	echo "Archlinux" > /etc/hostname

设置root密码
	
	passwd

### 安装GRUB2引导器

如果你本机已经安装了GRUB2引导器，可以跳过这一小节，无需重复重复安装GRUB2

安装软件包

	pacman -S grub

确定自己使用的是EFI还是BIOS引导，如果是EFI引导，下面命令会有结果输出，否则是BIOS引导

	ls /sys/firmware/efi/efivars

若确认自己使用的是EFI引导，需要另外挂载EFI分区到某个目录（如/tmp/efi）才能安装grub2

	mkdir -p /tmp/efi
	mount /dev/sdxY /tmp/efi （请将的sdxY替换为EFI分区）

	pacman -S efibootmgr

	grub-install --efi-directory=/tmp/efi --bootloader-id=grub
	
	umount /tmp/efi

若是自己使用的是BIOS引导

	grub-install /dev/sdx （将sdx换成你安装的硬盘，注意是硬盘，不是sdaxY分区）
	
### 添加到GRUB2引导项

安装系统探测器，便于添加已有的EFI系统引导项。

	pacman -S os-prober

若按照了上一节的说明《安装GRUB2引导器》自行安装了GRUB2，则可以直接将Archlinux添加到grub.cfg中

	grub-mkconfig -o /boot/grub/grub.cfg

否则，若系统本身就有GRUB2（即已存在其它Linux系统，如Ubuntu等），可以挂载ubuntu所在分区，并将Archlinux添加到grub.cfg中。

	mkdir -p /tmp/ubuntu-root
	mount /dev/sdxY /tmp/ubuntu-root/tmp/ubuntu-root （请将的sdxY替换为其它Linux的根分区）
	grub-mkconfig -o /tmp/ubuntu-root/boot/grub/grub.cfg

### 完成安装

重启

	exit
	reboot

重启后遇到EFI Shell或者无法进入GRUB2，可以尝试在EFI Shell手动添加引导项。

参考文章[UEFI Shell](/2018/06/12/uefi-shell/)

## Notes

本文只是安装了基本的系统，不含有X桌面。要使用X桌面，请参考其他文章。

参考博客：[以官方Wiki的方式安装ArchLinux](https://www.viseator.com/2017/05/17/arch_install/)