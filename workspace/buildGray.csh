#!/bin/csh

cd ./DataStructs/Debug; make clean; make all; cd ..; cd ..;
cd ./Graphics/Release; make clean; make all; cd ..; cd ..;
cd ./Physics/Debug; make clean; make all; cd ..; cd ..;
cd ./Random/Release; make clean; make all; cd ..; cd ..;
cd ./VrMath/Release; make clean; make all; cd ..; cd ..;
cd ./DataStructs/Debug; make clean; make all; cd ..; cd ..;
cd ./Gray/Debug; make clean; make all; cd ..; cd ..;
cd ./OpenGLRender/Debug; make clean; make all; cd ..; cd ..;
cd ./Output/Debug; make clean; make all; cd ..; cd ..;
cd ./OutputShared/Debug; make clean; make all; cd ..; cd ..;
cd ./Physics/Debug; make clean; make all; cd ..; cd ..;
cd ./RayTraceMgr/Debug; make clean; make all; cd ..; cd ..;
cd ./Sources/Debug; make clean; make all; cd ..; cd ..;
cd ./VrMath/Debug; make clean; make all; cd ..; cd ..;
cd ./RayTraceKd/Debug; make clean; make all; cd ..; cd ..;
# Symbolically Link Gray_Materials.txt to include directory
cd ..;cd include;ln -s ../GRAY_GEN_MATERIAL/Gray_Materials.txt .;cd ..;cd workspace;
cd ..;ln -s ./workspace/RayTraceKd/Debug/Gray .; cd workspace;
