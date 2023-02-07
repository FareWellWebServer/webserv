#!/usr/bin/env python3

import cgi

def caesar(s):
    s = list(s)
    for i in range(len(s)):
        if s[i].isupper():
            s[i]=chr((ord(s[i]) - ord('A')+ 42) % 26 + ord('A'))
        elif s[i].islower():
            s[i]=chr((ord(s[i]) - ord('a')+ 42) % 26 + ord('a'))
 
    return "".join(s)

form = cgi.FieldStorage()
plain_text = form.getvalue("origin")

print(caesar(plain_text))