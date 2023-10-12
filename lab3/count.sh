#!/bin/bash

if [ $# -lt 2 ]
then
    echo "Not enough arguments!"
    exit 1
fi

if [ $# -gt 2 ]
then
    echo "Too many arguments!"
    exit 1
fi


dirname=$1
out_filename=$2

total_chars=0
chars_file=0

echo "" >$out_filename

for file in "$dirname"/*.txt
do

    chars_file=$(wc -m < "$file")
    total_chars=$(($total_chars+$chars_file))
    echo "$file $chars_file" >> $out_filename

done

echo "TOTAL $total_chars" >> $out_filename