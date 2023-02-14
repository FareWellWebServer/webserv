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

# plain_text = os.getenv("QUERY_STRING", "sadfsadf?origin=1234asb")
plain_text = "sadfsadf?origin=1234asb"
res= plain_text.split("?")[1].split("=")[1]

print ("Content-type: text/html")
print ()
print ("<!DOCTYPE html>")
print ("<html>")
print ("<head>")
print ("<title>decoded text</title>")
print ("</head>")
print ("<body><h1>")
print(res)
print (caesar(res))
print ("</h1></body>")
print ("</html>")