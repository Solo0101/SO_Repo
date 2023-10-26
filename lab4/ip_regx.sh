#!/bin/bash

while read line 
do
    #supports both ip with or without port specifier
    echo $line | grep -E "^((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\.){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])+[\:]*[0-9]{1,5}$"
done