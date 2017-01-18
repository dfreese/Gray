#!/bin/sh

if [ -e neg_sphere.dat ]; then
    rm neg_sphere.dat
fi
./Gray -b ./detectors/neg_sphere.dff neg_sphere.dat

diff -sq neg_sphere.dat detectors/ref_neg_sphere.dat
