# SocksTunnel
一个类Shadowsocks工具, 虽然有点搓。。



###编译
-------------
```shell
cd build
cmake ..
make
```
编译需要libevent, libev, cmake, ubuntu可以直接
```shell
echo y | sudo apt-get install libevent-dev libev-dev cmake
```
编译时提示cmake版本过低可以将CMakeLists.txt中的版本改低, 应该没影响。。

###使用
-------------------
编译完成后会生成SocksTunnelLocal还有SocksTunnelRemote
额,, 部署方式跟ss差不多, 这里就不说了(一个部署在本地, 一个部署在远端, 配置文件是setting.json, 写死在代码里面了, 找个时间再改改), 加密部分也只是简单的弄了Xor还有Rc4, 支持多次加密, 找个时间再加多点加密算法。。。
