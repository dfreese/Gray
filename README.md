# Gray
A ray-tracing based Monte-Carlo simulation for PET

## Dependencies
* CMake
* OpenGL (Optional for viewing)
* Python 2.5 or later to generate the materials files from XCOM

## Install
How to install Gray:

1. If glut libraries are not installed:

    ```sudo apt-get install freeglut3 freeglut3-dev```

2. If you don't already have CMake:

    ```sudo apt-get cmake```

3. Compile program:

    ```cd workspace; cmake .; make```

4. Setup directory

    run ```source set_gray_dir.sh``` from the main Gray Directory

    If you wish to disable OpenGL and the scene viewer, run ```cmake
-DUSE_OPENGL=OFF .``` instead.

    To use a different build system than Make, add the -G option before the
period for cmake, and then the name of the generator you want to use, such as
Xcode.



5. Execute

    code is workspace/Gray/Gray
    set_gray_dir.sh creates symbolic link in the Gray home directory
