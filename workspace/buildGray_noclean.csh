#!/bin/csh

cd ./DataStructs/Debug;   make all; cd ..; cd ..;
cd ./Graphics/Release;   make all; cd ..; cd ..;
cd ./Physics/Debug;   make all; cd ..; cd ..;
cd ./Random/Release;   make all; cd ..; cd ..;
cd ./VrMath/Release;   make all; cd ..; cd ..;
cd ./DataStructs/Debug;   make all; cd ..; cd ..;
cd ./Gray/Debug;   make all; cd ..; cd ..;
cd ./OpenGLRender/Debug;   make all; cd ..; cd ..;
cd ./Output/Debug;   make all; cd ..; cd ..;
cd ./OutputShared/Debug;   make all; cd ..; cd ..;
cd ./Physics/Debug;   make all; cd ..; cd ..;
cd ./RayTraceMgr/Debug;   make all; cd ..; cd ..;
cd ./Sources/Debug;   make all; cd ..; cd ..;
cd ./VrMath/Debug;   make all; cd ..; cd ..;
cd ./RayTraceKd/Debug; make clean;  make all; cd ..; cd ..;
cd ../
ln -s ./workspace/RayTraceKd/Debug/Gray .
