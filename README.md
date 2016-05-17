# SocksTunnel
一个类Shadowsocks工具, 虽然有点搓。。

###编译
-------------
```shell
cd build
cmake ..
make
```
编译需要libevent, libev, cmake, libssl, ubuntu可以直接
```shell
echo y | sudo apt-get install libevent-dev libev-dev cmake libssl-dev
```
编译时提示cmake版本过低可以将CMakeLists.txt中的版本改低, **应该**没影响。。

需要修改打DEBUG日志的话, 修改CMakeLists.txt, 把里面的-DNDEBUG去掉然后重新编译就行了

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
  "current_pwd":"hellotest"
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
  }
}
```

###为什么写这个东西？
----
####熟悉下异步=，=,  还有自己的翻墙需要。