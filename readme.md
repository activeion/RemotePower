# Remote Power Server

Based on muduo library, this server software named "RemotePower" is designed for Methanol Mini-PowerStation for eg. M100 which is the recent product of Eastern Hydrogen Tech. Co. Ltd. 

RemotePower can collect data from distributed-installed MiniPowerStation by TCP/IP protocol, and store the data by ASCII files, and respond to the data inquiry from client software which may be installed in mobilephone, PC etc. The commucation among cilent, RemotePower, and MiniPowerStation is designed by ProtoBuff protocol.

The detailed ProtoBuff protocol design information can be refered by topo.easy file.

contact: jzh@eh2tech.com


Compile:
```
# pacman -S libzip protobuf

$ git clone https://github.com/chenshuo/muduo.git
$ cd muduo/
$ git checkout -b cpp11 origin/cpp11
$ BUILD_TYPE=debug ./build
$ BUILD_TYPE=debug ./build install

$ cd remote_power/
$ make

```

