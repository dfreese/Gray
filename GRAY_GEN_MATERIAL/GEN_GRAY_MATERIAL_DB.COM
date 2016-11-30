#!/bin/bash
mkdir TMP
grep -v '#' Gray_Materials.txt | awk '{ printf("%s\n%s\n%lf\n\n",$1,$2,$3)  > "./TMP/"$1".deta" }'
grep -v '#' Gray_Materials.txt | awk '{ print $1".deta"  > "./TMP/materials.txt" }'
cd TMP
ln -s ../XCOM/XCOM.EXE .
ln -s ../XCOM/MDAT* .
for i in `cat materials.txt`; do ../XCOM/generate_dat_file_master.sh < $i; done;
cd ..
for i in `cat Gray_Materials.txt | grep -v '#' | awk '{ print $1}'`; do mv ./TMP/$i.dat .; done;
for i in `cat Gray_Materials.txt | grep -v '#' | awk '{ print $1}'`; do cp ./$i.dat ../include ; done;
rm -r TMP
