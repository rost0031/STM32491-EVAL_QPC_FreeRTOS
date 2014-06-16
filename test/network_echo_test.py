#!/usr/bin/env python

import socket


TCP_IP = '192.168.1.75'
TCP_PORT = 7778
BUFFER_SIZE = 1024
MESSAGE = "Hello, World! Sending some data to test echo of my TCP"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
while True:
    s.send(MESSAGE)
    data = s.recv(BUFFER_SIZE)
s.close()

print "received data:", data
