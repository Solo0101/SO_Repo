#!/bin/bash

#grep -E "^[a-zA-Z0-9\.\_]+@[a-zA-Z]+\.[a-z]{2, 3}$"\

#cat exemplu.txt | grep -E "^[A-Z]+[a-zA-Z0-9\,\ ]*[\.]$" | grep -E -v "(si[\ ]*\,)|(Si[\ ]*\,)" | grep -E -v "n[pb]"

while read line 
do
    echo $line | grep -E "^[A-Z]+[a-zA-Z0-9\,\ ]*[\.!?]$" | grep -E -v "(si[\ ]*\,)|(Si[\ ]*\,)" | grep -E -v "n[pb]"
done