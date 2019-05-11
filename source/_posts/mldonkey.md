---
title: 电驴mldonkey
date: 2018-07-24 11:39:06
tags: e2dk
categories: 网络
---
配置简单的Linux电驴客户端
<!-- more -->
## 安装

Ubuntu/Debian:

    apt install mldonkey-server # 选择开机自启，否则systemctl启动失败

ArchLinux:
    
    pacman -S mldonkey
    
## 配置

停止服务(daemon进程为mlnet)

    systemctl stop mldonkey

创建下载文件夹

    mkdir -p /some/path/to/mldonkey/temp
    mkdir -p /some/path/to/mldonkey/shared
    mkdir -p /some/path/to/mldonkey/incoming/directories
    mkdir -p /some/path/to/mldonkey/incoming/files

修改拥有者为donkey，否则无权写入

    chown -R mldonkey /some/path/to/mldonkey/

编辑配置

    # Debian
    vi /var/lib/mldonkey/downloads.ini
    
    # Archlinux
    vi /var/lib/mldonkey/.mldonkey/downloads.ini

修改几处
    
    # Allow IP放行
    allowed_ips = [ "192.168.0.100"; ]
    
    # 临时文件夹
    temp_directory = "/some/path/to/mldonkey/temp"
    
    # 下载文件夹
    shared_directories = [
        {     
            dirname = "/some/path/to/mldonkey/shared" 
            strategy = all_files
            priority = 0  
        }; 
        {
            dirname = "/some/path/to/mldonkey/incoming/files"
            strategy = incoming_files
            priority = 0  
        };
        {
            dirname = "/some/path/to/mldonkey/incoming/directories"
            strategy = incoming_directories
            priority = 0 
        };
    ]

启动服务

    systemctl start mldonkey
    
检查是否运行

    pgrep mlnet
