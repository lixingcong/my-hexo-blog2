---
title: UEFI Shell常用命令
date: 2018-06-12 15:00:49
tags: EFI
categories: 读书笔记
---
UEFI Shell是管理EFI启动项的类Shell环境，可以通过一个U盘进入UEFI Shell
<!-- more -->
## 如何进入

最简单的，参考[这篇文章][create_usb]创建可引导的UEFI Shell
1. 格式化U盘为FAT32格式
2. 在U盘创建这个文件夹/efi/boot
3. [下载][download_uefi_shell]UEFI Shell，文件重命名为Bootx64.efi
4. 将Bootx64.efi复制到U盘的/efi/boot下

启动效果图

![](/images/uefi-shell/main.jpg)

另外也有很多封装了UEFI Shell的引导器：如[rEFInd](https://sourceforge.net/projects/refind)，还有大名鼎鼎的黑苹果引导器[Clover](https://sourceforge.net/projects/cloverefiboot)

## 常用命令

列出所有的EFI引导条目，每项都有一个编号。

	bcfg boot dump

将编号为1的EFI引导条目，移动到编号为0的位置

	bcfg boot mv 1 0

在编号为X的位置插入一个EFI引导项，X是一个数字编号

	bcfg boot add X fs0:\EFI\ubuntu\shimx64.efi "Kubuntu"

删掉编号为 1 的启动项

	bcfg boot rm 1

使用内置文本编辑器（仅支持UTF-8编码）

	edit FS0:\EFI\refind\refind.conf

建立微软的引导项，复制相关文件

	mkdir FS1:\EFI\Microsoft
	mkdir FS1:\EFI\Microsoft\Boot
	cp FS0:\EFI\BOOT\bootx64.efi FS1:\EFI\Microsoft\Boot\bootmgfw.efi

## 图形化

Windows下的Bootice(1.3.3以上)提供了图形化的编辑界面，实现了UEFI Shell的添加删除引导项的功能。

![](/images/uefi-shell/bootice.png)

## 参考文章

[Intel EFI Shells and Scripting](https://software.intel.com/en-us/articles/efi-shells-and-scripting/)
[How to permanently add Linux entry in UEFI menu][uefi_cmds]

[uefi_cmds]: https://gnu-linux.org/how-to-permanently-add-linux-entry-in-uefi-menu.html
[create_usb]: https://github.com/chipsec/chipsec/wiki/Creating-a-Bootable-USB-drive-with-UEFI-Shell
[download_uefi_shell]: https://github.com/tianocore/edk2/raw/master/ShellBinPkg/UefiShell/X64/Shell.efi