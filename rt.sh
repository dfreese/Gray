#!/bin/sh

if [ -e neg_sphere.dat ]; then
    rm neg_sphere.dat
fi
gray -f detectors/neg_sphere.dff

diff -sq neg_sphere.dat detectors/ref_neg_sphere.dat

if [ "$?" -eq 0 ]; then
    echo ""
    echo "       _"
    echo "      /(|"
    echo "     (  :"
    echo "    __\  \  _____"
    echo "   (____)  \`|"
    echo "   (____)|   |"
    echo "   (____).__|"
    echo "   (___)__.|_____"
    echo ""
fi
