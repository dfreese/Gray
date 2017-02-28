The scripts in this folder will poll the NIST XCOM website to generate
materials files that Gray can use.  To do this run

python build_materials.py

This can actually be run from any directory on the system assuming the
GRAY_INCLUDE environmental variable is set

python $GRAY_INCLUDE/build_materials.py

Otherwise, you can override the materials database and the export location by
specifying them

python build_materials.py (material file) (export location)


The build_materials.py script relies on two python modules.  The xcom module
interfaces with the XCOM website to download the cross-section information. The
dsdom module linearly interpolates a value for the klein-nishina scatter angle
calculation.

These python files require an external internet connection and python 2.5 or
later.
