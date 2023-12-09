#!/bin/bash

if [ $# -ne 1 ]
then
    echo "Invalid use of script.sh! Use 'bash script.sh <c>' instead!"
    exit -1
fi

character=$1
counter=0

while read line || [[ -n "$line" ]];
do
    echo "$line" | grep -qxE "^[A-Z]+[a-zA-Z0-9\,\ ]*[\.!?]$" && 
    echo "$line" | grep -qxE -v "(si[\ ]*\,)|(Si[\ ]*\,)" && 
    echo "$line" | grep -qxE -v "n[pb]" && 
    echo "$line" | grep -qE $character && 
    ((counter++))
done
echo $counter
# exit $counter