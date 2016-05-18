#coding=utf-8

import socket;
import time;

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM);

s.connect(("localhost", 9001));

time.sleep(22);
count = s.send("hehe".encode());
print("send count:" + str(count));
buf = s.recv(1024);
print("recv count:" + str(len(buf)));
s.close();
