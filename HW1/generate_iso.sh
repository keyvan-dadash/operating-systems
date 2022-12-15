#!/bin/bash

input=$1
output=$2

if [ $output == ""]
then
  output="myos.iso"
fi

rm *.iso

nasm $input -f bin -o temp1.bin
dd if=/dev/zero of=floppy.img bs=1024 count=1440
dd if=temp1.bin of=floppy.img seek=0 count=1 conv=notrunc
od -t x1 -A n temp1.bin
rm temp1.bin

mkdir iso
cp floppy.img iso/
genisoimage -quiet -V 'MYOS' -input-charset iso8859-1 -o $output -b floppy.img \
    -hide floppy.img iso/

rm floppy.img
rm -r iso/
