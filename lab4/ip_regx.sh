#!/bin/bash

while read line 
do
    #supports both ip with or without port specifier
    echo $line | grep -E "^[0-9]{1,3}+[\.]+[0-9]{1,3}+[\.]+[0-9]{1,3}+[\.]+[0-9]{1,3}*[\:]*[0-9]{1,5}$"
done