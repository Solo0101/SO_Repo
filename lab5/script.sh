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

director=$1
outfile=$2
aux=""
nr_leg=0
nr_leg_aux=0

if [ ! -d "$director" ]; then
    echo "Invalid folder"
    exit 1
fi

if [ -e "$outfile" ]; then
    if [ ! -f "$outfile" ]; then
        echo "Corrupted existing output file $outfile"
        exit 1
    fi
else
    touch $outfile
fi

for file in "$director"/*; do
    if [ -f "$file" ] && [[ "$file" == *.txt ]]; then
        aux=$(cat $file | grep -E "^((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\.){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])+[\:]*[0-9]{1,5}$")
        if [ ! -z "$aux" ]; then
            echo "File name: $file" >> "$outfile"
        aux=""
        fi
    elif [ -L "$file" ]; then
        nr_leg=$((nr_leg + 1))
    elif [ -d "$file" ]; then
        nr_leg_aux=`bash $0 $file $outfile`
        nr_leg=$((nr_leg + $nr_leg_aux))
    fi
done

echo $nr_leg