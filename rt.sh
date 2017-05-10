#!/bin/sh

if [ -e neg_sphere.dat ]; then
    rm neg_sphere.dat
fi
gray -b -s 5489 -f detectors/neg_sphere.dff -i neg_sphere.dat \
    -p detectors/sample_process_file.pdc -m detectors/sample_mapping_file.map \
    -o neg_sphere_singles.dat

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
