#!/bin/sh

gray -b -s 5489 -f detectors/pet_benchmark_fast.dff -i pet_benchmark_fast.dat \
    -t 0.1

diff -sq pet_benchmark_fast.dat detectors/ref_pet_benchmark_fast.dat

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
