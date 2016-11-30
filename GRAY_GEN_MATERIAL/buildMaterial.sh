#!/bin/sh
echo "Cleaning all material files"; rm -f *.dat; echo "Done."
echo "Compiling interpolation function"
cd CSLEVIN_INTERPOLATE; rm -f cslevin_interpolate; gcc cslevin_interpolate.c -o cslevin_interpolate; cd ..
echo "done"
echo "Compiling XCOM"
cd XCOM; rm XCOM.EXE; gfortran XCOM.f -o XCOM.EXE; cd ..
echo "done"
echo "Running Build Script"
sh GEN_GRAY_MATERIAL_DB.COM
echo "done"
