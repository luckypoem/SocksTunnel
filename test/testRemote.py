#coding=utf-8

import socket;
import struct;
import time;
import select;

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM);

s.connect(("localhost", 9001));

"""
auth:
    total
    rndLen
    rndChar
    type
    userLen
    user
    pwd
    pwdLen
    addrType
    addrLen
    addr
    port
"""

rndChar = "1111111111";
rndLen = len(rndChar);
type = 0x1;
userLen = 7;
user = "sentest";
pwdLen = 7;
pwd = "sentest";
addrType = 0x3;
addr = "www.baidu.com";
addrLen = len(addr);
port = 80;

total = 0;
msg = "".encode();
msg += struct.pack("c", chr(rndLen).encode());
msg += rndChar.encode();
msg += struct.pack("c", chr(type).encode());
msg += struct.pack("c", chr(userLen).encode());
msg += user.encode();
msg += struct.pack("c", chr(pwdLen).encode());
msg += pwd.encode();
msg += struct.pack("c", chr(addrType).encode());
msg += struct.pack("c", chr(addrLen).encode());
msg += addr.encode();
msg += struct.pack("H", socket.htons(port));
total = len(msg);
msg = struct.pack("i", socket.htonl(total)) + msg;

print("auth total:" + str(total));

s.send(msg);

"""
data
    total 
    rndLen
    rndChar
    type
    dataLen
    data
"""
total = 0;
rndChar = "this test";
rndLen = len(rndChar);
type = 0x2;

data = "GET /baidu.html?from=noscript HTTP/1.1\r\nHost: www.baidu.com:80\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.112 Safari/537.36\r\nDNT: 1\r\nAccept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.6\r\n\r\n"
dataLen = len(data);

msg = "".encode();
msg += struct.pack("c", chr(rndLen).encode());
msg += rndChar.encode();
msg += struct.pack("c", chr(0x2).encode());
msg += struct.pack("H", socket.htons(dataLen));
msg += data.encode();
total = len(msg);
msg = struct.pack("I", socket.htonl(total)) + msg;
print("data total:" + str(total));
print("data len:" + str(len(data)));
s.send(msg);

buf = s.recv(8000);
total = len(buf);
print(buf);
while(total > 0):
    packTotal = socket.ntohl(struct.unpack("I", buf[0:4])[0]);
    print("packet total:" + str(packTotal));
    rndLen = ord(struct.unpack("c", buf[4:5])[0]);
    rndData = "";
    for i in buf[5:5 + rndLen]:
        rndData += str(int(i)) + ",";
    print("rndData:" + rndData);
    packData = buf[4 + 1 + rndLen + 1 + 2:packTotal];
    print("packetData:\n" + packData.decode());
    buf = buf[packTotal + 4:];
    total -= packTotal + 4;
