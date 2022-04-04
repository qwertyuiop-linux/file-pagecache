# file-pagecache

该内核模块可以查看文件在文件系统缓存中大小。

## 编译命令

make KDIR=内核obj目录

## 使用命令

1. insmod file-pagecache.ko //查看默认文件系统内文件缓存情况
2. insmod file-pagecache.ko filesystem_name=ext4 //查看指定文件系统内文件缓存情况
