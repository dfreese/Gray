# Gray
A ray-tracing based Monte-Carlo simulation for PET

## Dependencies
* 64bit Operating System
* CMake 2.8.12 or later
* OpenGL, GLU, and GLUT (Optional for viewing)
* Python 2.7 or later to generate the materials files from EPDL
* A C++11 compatible compiler.  GCC 4.8 is supported, but 5 or greater
 is suggested. Clang 3.3 or later is supported.

## Structure
This repository has or assumes the following structure:
* detector - example gray input formats
* docs - documentation on the scene or process input formats
* materials - files or scripts to build GrayPhysics.json used as input to gray
* python - the gray, epdl, and xcom python modules used for building the
materials file and data processing of gray files
* test - testing of the python modules (nose2 has been used for testing)
* workspace - C++ source structure
* bin - holds the binary files built for the project
* build - out of source cmake build directory

## Building
How to install Gray:

1. If glut libraries are not installed and you want to run the viewer:

    ```
    sudo apt-get install freeglut3 freeglut3-dev
    ```
    (This assumes a debian system.  Gray has been tested on Ubuntu 14.04 and
    macOS Sierra primarily)

2. If you don't already have CMake:

    ```
    sudo apt-get install cmake
    ```

3. Compile:

    ```
    mkdir build
    cd build
    cmake ../workspace/ -DCMAKE_BUILD_TYPE=Release
    make
    ```

    To use a different build system than Make, add the -G option before the
    period for cmake, and then the name of the generator you want to use, such
    as Xcode.

4. Setup directory

    run the following from the main directory to tell Gray where to find the
    materials files as well as the python modules.
    ```
    source set_gray_dir.sh
    ```

    This will set the GRAY_DIR, GRAY_INCLUDE, GRAY_PYTHON, GRAY_BIN variable as
    well as modify the PATH and PYTHONPATH environmental variables.

5. Build Physics

    Run the following from the main directory to build the GrayPhysics.json
    file from the GrayMaterials.db, GrayIsotopes.txt, and GraySensitive.txt.
    ```
    python materials/build_materials.py
    ```
    Note that mac users may run into an issue with the default python
    implementation using an outdated version of the openssl library.  The most
    best approach for this problem is to install a current version of python
    using [Homebrew](https://brew.sh/).

6. Execute

    The setup step adds the ```bin``` folder to your ```$PATH``` variable.
    You can now run Gray by typing into the shell.  If run without arguments,
    it will display all of the possible options to run that program.
    ```
    gray
    gray-daq
    gray-view
    ```
