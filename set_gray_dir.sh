#!/bin/sh

if [ -e Gray ]; then
    if [ -h Gray ]; then
        echo "Error: Gray in this folder is not a symbolic link.  Check this."
        exit 1
    fi
else
    ln -s workspace/Gray/Gray
fi

export GRAY_INCLUDE=${PWD}/GRAY_GEN_MATERIAL
