#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 FILENAME"
    exit 1
fi
file=$1

if [[ ! -f "$file" ]]; then
    echo "File not found: $file"
    exit 1
fi

cname=$file
cname=${cname//-/_}
cname=${cname//./_}

# Gimp 16bit save includes a 70 byte header that we stip off
echo "static unsigned char $cname[]  __attribute__ ((section (\".text\"))) = { "
hexdump -s 70 -v -e '" " 16/1 "  0x%02x, " "\n"' $file | \
   sed -e '$s/0x  ,//g'
echo "};"

