import os
import sys

def caesar(s):
    s = list(s)
    for i in range(len(s)):
        if s[i].isupper():
            s[i]=chr((ord(s[i]) - ord('A')+ 42) % 26 + ord('A'))
        elif s[i].islower():
            s[i]=chr((ord(s[i]) - ord('a')+ 42) % 26 + ord('a'))
 
    return "".join(s)

plain_text = os.getenv("QUERY_STRING")
res = plain_text.split("?")[1].split("=")[1]

print ("Content-Type: text/html\r")
print ("\r")
print ("<!DOCTYPE html>")
print ("<html>")
print ("<head>")
print ("<title>decoded text</title>")
print ("</head>")
print ("<body><h1>")
print (caesar(res))
print ("</h1></body>")
print ("</html>")

exit(1)