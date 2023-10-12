#!/bin/bash

if [ $# -lt 2 ]
then
    echo "Not enough arguments!"
    exit 1
fi

if [ $# -gt 3 ]
then
    echo "Too many arguments!"
    exit 1
fi

dirname=$1
char=$2
recursive=$3

echo "Before:"
ls -R -l

if [ $recursive = "-r" ]
then
    for file in "$dirname"/*
    do
        if [[ $file == *.txt ]] 
        then
            chmod a+"$char" "$file"
        elif [ -d "$file" ]
        then
            bash $0 "$file" "$char"
        fi
    done
else
    for file in "$dirname"/*
    do
        if [[ $file == *.txt ]] 
        then
            chmod a+"$char" "$file"
        fi
    done
fi    

echo "After:"
ls -R -l
