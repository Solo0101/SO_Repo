#!/bin/bash

if [ $# -lt 3 ]
then
    echo "Not enough arguments!"
    exit 1
fi

filename=$1
dirname=$2

shift 2

if [[ ! -f $filename ]]; then
    echo "$filename does not exists!"
    exit 1
fi

count=0
sum=0
man_wc=0;

for arg
do
    if [ "$arg" -ge 10 ]
    then
       count=$(($count+1))
    fi
    sum=$(($sum+"$arg"))
done

man_wc=$((${#sum}))

echo "count: $count" > "$filename"
echo "sum: $sum" >> "$filename"
echo "man wc: $man_wc" >> "$filename"

cat "$dirname"/*.txt >> "$filename"

