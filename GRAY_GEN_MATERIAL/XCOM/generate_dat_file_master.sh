#!/bin/bash

# This is a script for generating GRAY .dat files, users need to specify the material, chemical compounds and the density,
# a dat file is generated which can be read by GRAY.
# cross sections are generated in 10 keV steps
# For the K-edge a special algorithm is implemented. 

echo "Please input your material name:"
read material
#make the file for XCOM:
echo $material > $material"_xcom_gen.txt"
echo 3 >>  $material"_xcom_gen.txt"
echo "Please input chemical formula:"
read formula
echo $formula >>  $material"_xcom_gen.txt"
echo 3 >>  $material"_xcom_gen.txt"
echo 2 >>  $material"_xcom_gen.txt"
echo "./gridfile_XCOM_MeV.txt" >>  $material"_xcom_gen.txt"
echo $material".txt" >>  $material"_xcom_gen.txt"
echo 1 >>  $material"_xcom_gen.txt"
echo "Please specify the density:"
read density
rm gridfile_keV.txt
for i in {0..150}; do  echo "11+$i*10" | bc >>gridfile_keV.txt; done ;
rm gridfile_XCOM_MeV.txt
echo "151" > gridfile_XCOM_MeV.txt
#echo "61" > $material".dat"
for i in {0..150}; do  echo "0.011+$i*0.010" | bc >>gridfile_XCOM_MeV.txt; done ; 
rm gridfile_keV_cslevin.txt
for i in `cat  gridfile_keV.txt` ; do ../CSLEVIN_INTERPOLATE/cslevin_interpolate  $i >> gridfile_keV_cslevin.txt ; done;
./XCOM.EXE < $material"_xcom_gen.txt"  
echo "DENSITY = ${density}"
cat $material".txt"  | awk -v dens=$density ' { if ( $5 ~ /^[0-9]/ ) { if ($1 !~ /1:/ ) printf("%15.9f   %15.9f   %15.9f\n", $1*1000,$4*dens,$3*dens)}}' > $material"_gray.part"
paste $material"_gray.part" gridfile_keV_cslevin.txt > $material"_base.dat"
# now find the K-edge:
echo $material > $material"_xcom_K.txt"
echo 3 >>  $material"_xcom_K.txt"
echo $formula >>  $material"_xcom_K.txt"
echo 1 >>  $material"_xcom_K.txt"
echo $material"_K.txt" >>  $material"_xcom_K.txt"
echo 1 >>  $material"_xcom_K.txt"
./XCOM.EXE < $material"_xcom_K.txt"  
#COLUMN 1 NEEDS TO BE A NUMBER, COLUMN2 NEEDS TO BE ALPHANUMERIC, COLUMN3 NEEDS TO BE LARGER THAN 11 keV
cat $material"_K.txt"  | awk ' { if (( $1 ~ /^[0-9]/ ) && ( $2 ~ /^[^0-9]/ ) && ( $3 > 0.011 )) {printf("%15.9f\n%15.9f\n", ($3-0.0000025)*1000, ($3 + 0.0000025)*1000) }}' > $material"_K_keV.txt"
K_grid=$material"_K_keV.txt"
nredges=`wc -l $K_grid | awk '{print $1}'`
echo $nredges > $material"_K_MeV.txt"
cat $material"_K.txt"  | awk ' { if (( $1 ~ /^[0-9]/ ) && ( $2 ~ /^[^0-9]/ ) && ( $3 > 0.011 )) {printf("%15.9f\n%15.9f\n", ($3-0.0000025), ($3 + 0.0000025)) }}' >> $material"_K_MeV.txt"
echo $material > $material"_xcom_K_gen.txt"
echo 3 >>  $material"_xcom_K_gen.txt"
echo $formula >>  $material"_xcom_K_gen.txt"
echo 3 >>  $material"_xcom_K_gen.txt"
echo 2 >>  $material"_xcom_K_gen.txt"
echo $material"_K_MeV.txt" >>  $material"_xcom_K_gen.txt"
echo $material"_xcom_K_edge.txt" >>  $material"_xcom_K_gen.txt"
echo 1 >>  $material"_xcom_K_gen.txt"
#rm gridfile_keV_K_cslevin.txt
echo $K_grid
for i in `cat  $K_grid` ; do echo "FOUND K_EDGE !!   $i" ; done;
rm gridfile_keV_K_cslevin.txt
for i in `cat  $K_grid` ; do ../CSLEVIN_INTERPOLATE/cslevin_interpolate  $i >> gridfile_keV_K_cslevin.txt ; done;
./XCOM.EXE < $material"_xcom_K_gen.txt"  
cat $material"_xcom_K_edge.txt"  | awk -v dens=$density ' { if ( $5 ~ /^[0-9]/ ) printf("%15.9f   %15.9f   %15.9f\n", $1*1000,$4*dens,$3*dens)}' > $material"_K_gray.part"
paste $material"_K_gray.part" gridfile_keV_K_cslevin.txt > $material"_K.dat"
basefile=$material"_base.dat"
Kfile=$material"_K.dat"
totlines=`expr \` wc -l $basefile | awk '{print $1}'\` + \`wc -l $Kfile | awk '{print $1}'\``
echo $totlines > $material".dat"
sort -n $basefile $Kfile >> $material".dat"
