---
title: Linux下分区加密LUKS
date: 2019-07-09 16:38:44
tags: [ubuntu, 加密]
categories: 网络
---
Linux内核自带的磁盘分区加密，与Android的dm-crypt同样方便。

编程随想曾经介绍过LUKS和VeraCrypt，博主把两者都测试了一下，写下LUKS的使用方法。
<!-- more -->
## 创建

假设要在/dev/sda2上进行创建，创建过程会格式化该分区。验证密码的方式常用有两种：密码和key-file

### 使用密码创建

    sudo cryptsetup luksFormat /dev/sda2

创建过程输入大写YES确认格式化分区，然后输入密码。

### 使用key-file创建

生成一个随机数二进制文件（熵较大的随机数适合做key-file，普通文本熵较小,因此比纯密码要安全）

以生成一个64K的文件（/tmp/MyKey.bin）为例，为安全起见不要小于1KB。妥善保管，丢后无法打开加密分区。

    dd if=/dev/urandom of=/tmp/MyKey.bin bs=1k count=64

创建LUKS分区，此时指定Key-file

    sudo cryptsetup --key-file /tmp/MyKey.bin luksFormat /dev/sda2

## 挂载、卸载分区

创建后，挂载加密分区

    # 密码
    sudo cryptsetup luksOpen /dev/sda2 xxx

    # key-file
    sudo cryptsetup luksOpen /dev/sda2 --key-file /tmp/MyKey.bin xxx

xxx为/dev/mapper下将要创建的文件名，可以随意设置。当解密成功后可以直接操作/dev/mapper/xxx这个块设备，而不是操作/dev/sda2。

实际上系统会自动创建软链接到/dev/dm-yyy（yyy是数字），指向/dev/mapper/xxx这个块设备（。

第一次挂载时，需要格式化这个分区，设置卷标（可选操作）

    sudo mkfs.ext4 /dev/mapper/xxx
    sudo e2label /dev/mapper/xxx "my-private"

挂载该分区

    mkdir /tmp/my-priavte
    sudo mount /dev/mapper/xxx /tmp/my-priavte

使用完毕后，卸载分区

    sudo umount /tmp/my-priavte
    sudo cryptsetup luksClose /dev/mapper/xxx

实测Kubuntu 18.04支持直接从资源管理器输入密码挂载加密分区。

## 增加、删除slot

LUKS具备8个slot，每个slot可以设置密码或者key-file验证。这8个slot目的就是加密master-key。

格式化后的分区只有一个slot。

每个“Key Slot”好比是一个独立的钥匙：都可以用来打开这个加密盘。

查看当前的slot状态 显示Disabled就是尚未使用的slot

    sudo cryptsetup luksDump /dev/sda2

增加slot：密码或者key-file，增加时候需要验证已有的slot

    # 密码方式
    sudo cryptsetup luksAddKey

    # key-file方式
    dd if=/dev/urandom of=/tmp/MyAnotherKey.bin bs=1k count=64
    sudo cryptsetup luksAddKey --key-file /tmp/MyAnotherKey.bin

删除slot的方法一：通过slot号码删除

    # 先记下slot的号码(0~7)，比如1
    sudo cryptsetup luksKillSlot /dev/sda2 1

删除slot的方法二：通过输入一个有效的slot验证删除

    # 输入有效密码
    sudo cryptsetup luksRemoveKey /dev/sda2

    # 删除一个key-file对应的slot
    sudo cryptsetup luksRemoveKey /dev/sda2 --key-file /tmp/MyAnotherKey.bin

若要删除所有slots（不需要验证密码，不可逆的毁灭性操作，只能通过恢复已备份的header来拯救）

    sudo cryptsetup luksErase

## 备份、恢复、擦除Header

man手册中有这么一段话

> If the header of a LUKS volume gets damaged, all data is permanently lost unless you have a header-backup. If a key-slot is damaged, it can only be restored from a header-backup or if another active key-slot with known passphrase is undamaged. 

上面的意思：如果LUKS的Header被破坏了，数据将永远丢失。如果Slot被破坏了则只能从header备份恢复。要么使用一个没有被破坏的slot。

### 备份header

    sudo cryptsetup luksHeaderBackup /dev/sda2 --header-backup-file /tmp/header.bin

header属于机密信息，确保不让别人发现你的header。如果你擦除了header，而别人使用你的header恢复了header，那么你的擦除将是徒劳。

### 恢复header

    sudo cryptsetup luksHeaderRestore /dev/sda2 --header-backup-file /tmp/header.bin

恢复后，所有slot将被重置为/tmp/header.bin的内容

### 擦除header

> Damaging the LUKS header is something people manage to do with surprising frequency. This risk is the result of a trade-off between security and safety, as LUKS is designed for fast and secure wiping by just overwriting header and key-slot area.

上面的意思：有些人会将header毁灭，用于将数据无法恢复，LUKS则出于性能和简单的设计，毁灭的操作是直接覆盖header数据。

擦除前，从header可以判断这是一个Luks分区

    sudo cryptsetup -v isLuks /dev/sdb2 # Command successful.

擦除头部1052672个字节（默认的header大小，仅包含1个slot，实际情况是slot越多，header尺寸越大）

    head -c 1052672 /dev/zero > /dev/sda2
    sync

那么除了头部1052672字节，后面都是原来加密的数据，只是这个header被我们手工破坏了。再次验证这个分区是否为有效的LUKS分区

    sudo cryptsetup -v isLuks /dev/sdb2 # Command successful. # is not a valid LUKS device


## 备份MasterKey

设计8个slot的目的只有一个：解密master-key，因此master-key才是最重要的。一旦master-key泄漏，什么slot验证都是虚设，能直接被解密数据。

> Beware that the master key cannot be changed without reencryption and can be used to decrypt the data stored in the LUKS container without a passphrase and even without the LUKS header. This means that if the master key is compromised, the whole device has to be erased to prevent further access. Use this option carefully.

上面这段话凸显了MasterKey比Header更重要，因为直接绕过了slot验证。一旦masterKey落入他人手中，你只能擦除整个分区来避免他读取机密数据。

下面这个命令可以导出MasterKey，输入大写YES确定。

    # 密码验证
    sudo cryptsetup luksDump --dump-master-key /dev/sda2

    # key-file验证
    sudo cryptsetup luksDump --dump-master-key /dev/sda2 --key-file /tmp/MyKey.bin

dump的时候，无论是通过哪个slot验证，输出的master key都是同一个。

将MK dump内容字节串：复制好

    89 53 d5 3e a0 4a 80 ea a3 ec 69 fc f0 6d ff 22 
    1f 3e df 2b 1a c2 05 6e bf 2d f1 61 39 dd 77 5e

创建为一个dumpInput.txt纯文本，粘贴字节串，然后xxd命令转成master-key二进制文件

    xxd -r -p dumpInput.txt /tmp/master-key.bin

使用master-key直接打开加密盘，无需验证slot：

    sudo cryptsetup luksOpen --master-key-file /tmp/master-key.bin

知道这玩意的可怕了吗？Master Key更需要妥善保管！！

## 参考链接

[编程随想：扫盲 dm-crypt——多功能 Linux 磁盘加密工具](https://program-think.blogspot.com/2015/10/dm-crypt-cryptsetup.html)
[10 Linux cryptsetup Examples for LUKS Key Management](https://www.thegeekstuff.com/2016/03/cryptsetup-lukskey/)
[LUKS: Add a Backup Key, Backup, Restore and Delete LUKS Volume Header](https://www.lisenet.com/2013/luks-add-keys-backup-and-restore-volume-header/)