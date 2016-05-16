#coding = utf-8

import socket;
import struct;
import time;

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
s.connect(("localhost", 9000));

msg = bytes([0x5, 0x1, 0x0]);
s.send(msg);
buf = s.recv(1024);
print(buf);
addr = "localhost";
msg = bytes([0x5, 0x1, 0x0, 0x3, len(addr)]);
msg += addr.encode();
msg += struct.pack("H", socket.htons(8000));

s.send(msg);
buf = s.recv(1024);
print(buf);

#==============Begin test=====================
count=256
data = "".encode();
for i in range(count):
    data += "b".encode(); 
s.send(data);
print(s.recv(1024));
s.close();
