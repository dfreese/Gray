#!/bin/sh

if [ -e neg_sphere.dat ]; then
    rm neg_sphere.dat
fi
./Gray -b -s 5489 -f ./detectors/neg_sphere.dff -i neg_sphere.dat

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
