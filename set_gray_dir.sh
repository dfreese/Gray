#!/bin/sh

# Get the working directoy of the script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# If the variable GRAY_PYTHON exists, remove it from PYTHONPATH.

if [ -n "${GRAY_PYTHON}" ]; then
    if [ -d ${GRAY_PYTHON} ]; then
        # remove the previously defined GRAY_PYTHON and it's leading ':'
        PYTHONPATH=`echo $PYTHONPATH | sed -e 's#:'"${GRAY_PYTHON}"'##g'`
        # remove the previously defined GRAY_PYTHON without a leading ':'
        # couldn't get a \? escape on the : to work for some reason.
        PYTHONPATH=`echo $PYTHONPATH | sed -e 's#'"${GRAY_PYTHON}"'##g'`
    fi
fi

if [ -n "${GRAY_BIN}" ]; then
    if [ -d ${GRAY_BIN} ]; then
        # remove the previously defined GRAYPIPLINE_BIN and it's leading ':'
        PATH=`echo $PATH | sed -e 's#:'"${GRAY_BIN}"'##g'`
        # remove the previously defined GRAYPIPLINE_BIN without a leading ':'
        # couldn't get a \? escape on the : to work for some reason.
        PATH=`echo $PATH | sed -e 's#'"${GRAY_BIN}"'##g'`
    fi
fi

# Now update those variables with the new ones, and add them to PYTHONPATH and
# PATH.
GRAY_DIR=${DIR}
GRAY_INCLUDE=${DIR}/materials
GRAY_PYTHON=$DIR/python
GRAY_BIN=$DIR/bin
# If the python path already has directories in it, append it to the back
if [ -n "$PYTHONPATH" ]; then
    PYTHONPATH=$PYTHONPATH:$GRAY_PYTHON
else
    PYTHONPATH=$GRAY_PYTHON
fi

if [ -n "$PATH" ]; then
    PATH=$PATH:$GRAY_BIN
else
    PATH=$GRAY_BIN
fi

export GRAY_DIR
export GRAY_INCLUDE
export GRAY_PYTHON
export GRAY_BIN
export PYTHONPATH
export PATH
