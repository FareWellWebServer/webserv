#!/usr/bin/python3

import os

print ("Content-Type: text/html\r")
print ("\r")
print ("<!DOCTYPE html>")
print ("<html>")
print ("<head>")
print ("<title>decoded text</title>")
print ("</head>")
print ("<body><h1>")
print ("caesar(plain_text)")
for key, val in os.environ.items():
    print(key, ": ", val)
print ("</h1></body>")
print ("</html>")