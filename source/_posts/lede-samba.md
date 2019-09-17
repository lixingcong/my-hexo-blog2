---
title: OpenWrt/LEDE的samba服务器配置
date: 2017-11-25 20:11:07
tags: openwrt
categories: 网络
---
在路由器上设置samba服务器，分享插入U盘的内容，兼容Windows读写权限。
<!-- more -->
## 安装USB驱动

首先要成功挂载USB盘符，根据不同的文件系统，可选安装不同的驱动，比如下面只安装ext4和FAT32驱动。若要读写NTFS分区，参考[Openwrt Wiki: Writeable NTFS](https://wiki.openwrt.org/doc/howto/writable_ntfs)，即安装ntfs-3g软件包

	opkg update
	
	opkg install \
	  kmod-usb-storage \
	  kmod-fs-ext4 \
	  kmod-fs-vfat \
	  kmod-usb-ohci \
	  kmod-usb-uhci \
	  mkf2fs \
	  f2fsck

安装完后，插入USB磁盘，应该可以观察到/dev/下面多了sda sda1 sda2等块设备，测试一下是否挂载成功：

	mkdir /tmp/sda1
	mount /dev/sda1 /tmp/sda1
	umount /tmp/sda1

## 自动挂载/卸载U盘

可以直接安装block-mount软件包，这样在luci web的system->mount points界面勾选“Anonymous Mount, Mount filesystems not specifically configured”即可自动挂载。

	opkg install block-mount

若不安装block-mount软件包，也可以通过hotplud脚本实现自动挂载（但是umount部份显得太暴力）。从网上抄的一段代码：

	# 编辑
	vi /etc/hotplug.d/block/10-mount
	
	# 插入以下代码
	case "$ACTION" in
		add)
			for i in $(ls /dev/ | grep 'sd[a-z][1-9]')
			do
				mkdir -p /mnt/$i
				mount -o iocharset=utf8,rw /dev/$i /mnt/$i
				if [ $? -ne 0 ]
				then
					mount -o rw /dev/$i /mnt/$i
				fi
			done
			;;
		remove)
			MOUNT=`mount | grep -o '/mnt/sd[a-z][1-9]'`
			for i in $MOUNT
			do
				umount $i
				if [ $? -eq 0 ]
				then
					rm -r $i
				fi
			done
			;;
	esac
	
实现U盘热插拔自动挂载卸载。

## 配置samba

安装luci程序，它会自动安装samba36。

	opkg install luci-app-samba

在Luci web的Service->Network share中General settings取消这个选项，不让其显示多余的HOME目录

	Share home-directories

由于我是使用root用户进行u盘读写，设置允许root登陆samba才能实现写权限（若追求权限管理安全，请不要开启root登陆，本文贪图一时方便。）

	vi /etc/samba/smb.conf.template
	
	# 注释这句 invalid users = root
	
	# 添加NTLMv2认证，否则samba在NT6内核以上(win7,8,10)登陆samba认证失败
	client ntlmv2 auth = yes

	# 若要监听所有接口(0.0.0.0)，将bind only设置为no
	bind interfaces only = no
	
新建一个samba用户root，并设置合适的密码

	smbpasswd -a root

日后修改密码

	smbpassed root

然后luci界面中Services->Network Share设置分享的目录。下面的文件掩码设置为最大（不安全）

|Name|Path|Allowed users|Read-Only|Allow Guest|CreateMask|DirMask|
|--|--|--|--|--|--|--|
|mnt|/mnt|root|||0777|0666|
|tmp|/tmp/|||✅|0022|0022|

上述设置代表分享两个目录
- mnt: 只允许root登陆，对应/mnt（上面smbpasswd命令中设置的密码）
- tmp: 允许匿名登陆，对应/tmp（实际上匿名只有只读权限，因为/tmp的所有者是root）

点击luci底部的save & apply或者/etc/init.d/samba restart重启samba服务。

## 添加用户

若不想使用root用户作为samba的使用者，可以自行增加用户。但是对相应的目录有对应的读写权限。

因为openwrt默认设计成单用户(root)，故[手工编辑文件增加用户](https://stantsui.blogspot.com/2016/10/openwrt-add-samba-user.html)

这里以新用户newuser和新的组newuser为例，假设新的UID=734和GID=734均未被系统使用。 对以下3个文件，分别添加一行

|文件|添加一行|
|--|--|
|/etc/passwd|newuser:x:734:734:newuser:/tmp:/bin/false|
|/etc/group|newuser:x:734:|
|/etc/shadow|newuser:xyzDummyString:16666:0:99999:7:::|
	
给Linux系统用户重新修改密码

	pass newuser

然后按照正常的samba命令添加用户

	smbpasswd -a newuser
	
将用户填入Luci界面中的Allowed Users即可。