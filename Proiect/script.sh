#!/bin/bash

if [ $# -ne 1 ]
then
    echo "Invalid use of script.sh! Use 'bash script.sh <c>' instead!"
    exit 1
fi

character=$1
counter=0
valid_sentence=""

while read line 
do
    # $valid_sentence = echo $line | grep -E "^[A-Z]+[a-zA-Z0-9\,\ ]*[\.!?]$" | grep -E -v "(si[\ ]*\,)|(Si[\ ]*\,)" | grep -E -v "n[pb]"
    # line="$(expr "$line" : '^[A-Z]+[a-zA-Z0-9\,\ ]*[\.!?]$')"
    if [[ "$line" =~ ^[A-Z]+[a-zA-Z0-9\,\ ]*[\.!?]$ ]]; then
        # if [[ "$line" =~ (si[\ ]*\,)|(Si[\ ]*\,) ]]; then
        #         if [[ "$line" =~ n[pb] ]]; then
                $counter = $counter + 1
                echo $counter
        #     fi
        # fi
    fi
done