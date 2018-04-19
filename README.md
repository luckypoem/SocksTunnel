# SocksTunnel
一个类Shadowsocks工具, 虽然有点搓。。

###编译
-------------
####基本编译方式
```shell
mkdir build
cd build
cmake ..
make
```
####编译依赖
编译需要libevent, libev, cmake, libssl, log4cpp
**ubuntu**可以直接
```shell
echo y | sudo apt-get install libevent-dev libev-dev cmake libssl-dev
```
**log4cpp**需要源码编译
```shell
git clone https://github.com/orocos-toolchain/log4cpp.git
cd log4cpp
mkdir build
cd build 
cmake ..
make
sudo make install
```
使用其他包管理软件的自己搜索下相关的包名哈。。
编译时提示cmake版本过低可以将CMakeLists.txt中的版本改低,**应该**没影响。。
```cmake
例如。。。
cmake_minimum_required(VERSION 3.0)
->
cmake_minimum_required(VERSION 2.8)
```
####依赖包的源码安装
#####libev
```shell
wget http://dist.schmorp.de/libev/libev-4.22.tar.gz
tar -zxf libev-4.22.tar.gz
cd libev-4.22
./configure
make
sudo make install
```
#####libevent
```shell
wget https://github.com/libevent/libevent/releases/download/release-2.0.22-stable/libevent-2.0.22-stable.tar.gz
tar -zxf libevent-2.0.22-stable.tar.gz
cd libevent-2.0.22-stable
./configure
make 
sudo make install
```
####libssl
```shell
wget https://www.openssl.org/source/openssl-1.1.0-pre5.tar.gz
tar -zxf openssl-1.1.0-pre5.tar.gz
cd openssl-1.1.0-pre5
./config
make
sudo make intall
```
需要修改打DEBUG日志的话, 修改CMakeLists.txt, 把里面的-DNDEBUG去掉然后重新编译就行了

###Windows下编译
**日志中配置的路径或者setting.json中的路径, 请不要用到中文, 否则会导致程序异常退出!!!!**
####使用msys
**编译的时候需要将系统路径中的cygwin的路径还有cmake的路径移除掉, 如果有安装的话**

需要安装msys
地址:https://msys2.github.io/

之后按照里面的说明对msys进行安装

下载需要的软件包

```shell
pacman -S mingw-w64-i686-toolchain tar make openssl libevent-devel cmake
```
####编译libev
这里我用pacman搜不到libev, 所以手动源码编译了。。
按上面写的libev编译方式去编译就好了
####编译log4cpp
这个我这边编译的时候会提示**FileAppender.cpp**中**O_LARGEFILE**常量未定义什么的, 打开那个文件, 把**O_LARGEFILE**去掉就行了
**上面的依赖包** 都装完后执行下面的命令进行编译
```shell
mkdir build
cd build
cmake ..
make
```
正常来说应该都可以编译完成
如果出现下面的情况的话
```
/usr/lib/gcc/x86_64-pc-msys/5.3.0/../../../../x86_64-pc-msys/bin/ld: cannot find -lev
```
可以locate一下libev.a
将它所在的路径加到CMakeLists.txt里面
```cmake
把里面的那句
link_directories("/usr/local/lib")
改成
link_directories("你的libev.a的地址")
```
之后重新make一下应该就可以编译出来了, 如果还有问题, 可以贴下相关的日志。

编译完成后, 运行时会提示缺少相关的动态链接库, 这里我贴下可能会用到的链接库, 将这些动态链接库复制到与编译出来的程序相同的目录就可以了
```shell
msys-2.0.dll
msys-crypto-1.0.0.dll
msys-event-2-1-4.dll
msys-gcc_s-seh-1.dll
msys-stdc++-6.dll
msys-z.dll
```
**注:这些动态链接库可以在msys的安装目录下搜索到**

####使用cygwin
**妈蛋, 这边用cygwin编译的时候一直出现**
```
错误：‘gettimeofday’在此作用域中尚未声明
```
**弄了个例子测试gettimeofday却可以编译成功, 草莓。。。**
目测是我自己这边环境的原因, 不搞了, 大致应该也就是上面装依赖包, 下cmake, make, g++, 然后编译 这几个过程。。

###怎么使用?
-------------------
编译完成后会生成**SocksTunnelLocal**还有**SocksTunnelRemote**
额, 部署方式跟ss差不多。

###如何部署?
-----
####远端部署
**SocksTunnelRemote**部署到可以翻墙的服务器上, 配置setting.json里面的**remote_addr**使其可以被外网访问, 还有配置**remote_port**作为远端的监听端口。
```shell
./SocksTunnelRemote json文件地址
```
####本地部署
本地部署用到的是**SocksTunnelLocal**, 修改**setting.json**中的**local_addr**, 让其可以被浏览器访问到。 其中的**local_port**是监听端口。
```
./SocksTunnelLocal json文件地址
```
####加密算法
对应的是**setting.json**里面的**method**字段, 可以指定多种加密方式, 按列表中加密方法的顺序进行加密。目前实现的只有3种, 分别是
> * Xor(xor)
> * Rc4(rc4)
> * Chacha20(chacha20)

括号里面的是method列表中可以填写的**对应加密方法的字符串**
这里感觉随便着一种方法就行了, 只要能混淆数据就能翻墙了= =。
与加密方法相关的还有**password**字段, 这个是加密的key, 可以尽可能的填长一点 :)
####多用户支持
需要用到字段user, **这个字段只有远程端需要用到**, 在user map里面进行用户名与密码的配置(键值对应方式), 例如
```json
{
    user:{
        "helloworld":"helloword",
        "hahahaha":"woshidoubi"
    }
}
```
**需要注意的是, 程序中对用户名的长度限制是6-16个字符, 密码字段的长度则是6-32**
与用户配置有关的项还有**current_user**, **current_pwd**, 这2个字段只在本地端使用, 用于远程端对用户, 密码进行校验。

####服务端认证超时
用到的字段:auth_timeout, 客户端连上服务端后在指定时间内没完成认证就会被断开。

####日志配置

用到setting.json中的**log_setting**, 在其对应的值中填写日志配置文件的路径, 日志配置文件的模板可以使用log_template.conf, 复制一份, 然后把里面的
**appender.A3.fileName=input your log file path here**替换成真正的路径, 例如
```shell
appender.A3.fileName=/etc/log.conf
#这里的**/etc/log.conf**为你复制出来的日志配置文件模板的地址
```
其他配置请参考log4cpp的配置说明

####多进程配置
远程端用到**remote_proc_count**
本地端用到**local_proc_count**
进程数范围1-100

###默认配置
####本地端
```json
{
  "remote_server":"127.0.0.1",
  "remote_port":9001,
  "local_server":"127.0.0.1",
  "local_port":9000,
  "password":"this is a password",
  "method":["chacha20"],
  "current_user":"hellotest",
  "current_pwd":"hellotest",
  "local_proc_count":4
}
```
####远程端
```json
{
  "remote_server":"127.0.0.1",
  "remote_port":9001,
  "local_server":"127.0.0.1",
  "local_port":9000,
  "password":"this is a password",
  "method":["chacha20"],
  "user":{
    "hellotest":"hellotest",
    "ohmyfuck":"ohyourfuck?"
  },
  "auth_timeout":30,
  "remote_proc_count":4
}
```

###为什么写这个东西？
----
####熟悉下异步=，=,  还有自己的翻墙需要。

------

https://github.com/luckypoem/sockstunnel（此版本有问题）

这个版本没问题：https://drive.google.com/file/d/1UOtpuqGD55_XOixA56ATV-Wx72sgXuCm/view, 而且这个版本支持在mac上编译。

