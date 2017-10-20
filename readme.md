# Remote Power Server

Based on muduo library, this server software named "RemotePower" is designed for Methanol Mini-PowerStation for eg. M100 which is the recent product of Eastern Hydrogen Tech. Co. Ltd. 

RemotePower can collect data from distributed-installed MiniPowerStation by TCP/IP protocol, and store the data by ASCII files, and respond to the data inquiry from client software which may be installed in mobilephone, PC etc. The commucation among cilent, RemotePower, and MiniPowerStation is designed by ProtoBuff protocol.

The detailed ProtoBuff protocol design information can be refered by topo.easy file.

contact: jzh@eh2tech.com

Communication Scheme
```
// [jizh@iMac remote_power]$ graph-easy topo.easy
//
//
// +-------------+  SubmitedData/QueryAnswer   +-----------------------------------------------+
// |             | --------------------------> |                                               |
// |             |                             |                                               |
// |             |  SettingAnswer/Answer       |                                               |
// |             | --------------------------> |                    Server                     |
// |             |                             |                                               |
// |             |  Setting                    |                                               |
// |             | <-------------------------- |                                               | <+
// |             |                             +-----------------------------------------------+  |
// |             |                               |                      |               ^         |
// | PowerClient |                               | LoginAnswer/Answer   | QueryAnswer   | Login   |
// |             |                               v                      |               |         |
// |             |                             +---------------------+  |               |         |
// |             |                             |                     | <+               |         | Query
// |             |                             |                     |                  |         |
// |             |                             |                     |                  |         |
// |             |                             |    iPhoneClient     | -----------------+         |
// |             |                             |                     |                            |
// |             |                             |                     |                            |
// |             |                             |                     | ---------------------------+
// +-------------+                             +---------------------+
```


Compile:
```
# pacman -S libzip protobuf

$ git clone https://github.com/chenshuo/muduo.git
$ cd muduo/
$ git checkout -b cpp11 origin/cpp11
$ BUILD_TYPE=debug ./build
$ BUILD_TYPE=debug ./build install

$ git clone git://github.com/activeion/RemotePower.git
$ cd RemotePower/
$ git checkout -b dev origin/dev
$ make

```
pache, php, mariadb;


apache配置：
/etc/httpd/httpd.conf


在LoadModule 末尾增加
```
LoadModule php7_module modules/libphp7.so
```


修改 DocumentRoot 为网站文件所在目录
修改权限：
```
<Directory "/win/web">
.....
....
Require all granted  
# 新版本 Apache 中  Require  为必选项
</Directory>
```


设置默认文件:
```
DirectoryIndex index.php index.html
```
若需设置虚拟目录，参考如下:

```
 Alias "/phpMyAdmin" "/win/web2/phpMyAdmin/" 
<Directory /win/web2/phpMyAdmin> 
AllowOverride None 
Order allow,deny 
Allow from all 
Require all granted 
</Directory>
```



