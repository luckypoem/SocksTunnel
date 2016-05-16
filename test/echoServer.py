#coding=utf-8
import socket;
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1);
s.bind(("0.0.0.0", 8000));
s.listen(100);
while(True):
    client,addr = s.accept();
    print("Got connect");
    while(True):
        buf = client.recv(1024);
        if(len(buf) == 0):
            break;
        client.send(buf);
    client.close();
