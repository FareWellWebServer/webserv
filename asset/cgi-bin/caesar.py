import sys

def caesar(s):
    s = list(s)
    for i in range(len(s)):
        if s[i].isupper():
            s[i]=chr((ord(s[i]) - ord('A')+ 42) % 26 + ord('A'))
        elif s[i].islower():
            s[i]=chr((ord(s[i]) - ord('a')+ 42) % 26 + ord('a'))
 
    return "".join(s)

plain_text = sys.argv[1]

print(caesar(plain_text))