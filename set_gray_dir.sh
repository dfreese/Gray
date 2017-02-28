#!/bin/sh

# Get the working directoy of the script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ -x "$DIR/workspace/Gray/Debug/Gray" ]; then
    # Xcode Structure
    gray_exe="$DIR/workspace/Gray/Debug/Gray"
elif [ -x "$DIR/workspace/Gray/Gray" ]; then
    # Make structure
    gray_exe="$DIR/workspace/Gray/Gray"
fi

sym_link_loc="$DIR/Gray"

# Check if the symbolic link exists already
if [ -L "$sym_link_loc" ]; then
    # Then check to see if it points to the right place, and delete it if it
    # doesn't
    if [ ! "$sym_link_loc" -ef "$gray_exe" ]; then
        echo "deleting it"
        rm "$sym_link_loc"
    fi
elif [ -e "$sym_link_loc" ]; then
    # If we didn't find a symbolic link and a file exists, warn the user
    echo "Error: Gray in this folder is not a symbolic link.  Check this."
fi
if [ ! -e "$sym_link_loc" ]; then
    ln -s $gray_exe $sym_link_loc
fi

export GRAY_INCLUDE=${DIR}/materials
