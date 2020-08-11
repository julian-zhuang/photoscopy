# Photos copy

从存储卡中复制照片到目标目录，读取照片的Exif信息，并按照日期进行归类的小工具软件。



**Exif信息**

读物EXIF信息使用exiv2库(https://www.exiv2.org/)



**界面**

界面使用Qt(>=5.7)进行开发(https://www.qt.io/)



## 如何使用

### Linux用户

关联库在常见linux发行版仓库中都有存在，所以在使用前你需要先安装运行库，之后便可以直接使用Qt-create进行编译出输出执行文件

Arch/Manjaro:

`sudo pacman -S exiv2`

Debian

`sudo apt-get install exiv2`

