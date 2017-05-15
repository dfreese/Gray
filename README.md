# Gray
A ray-tracing based Monte-Carlo simulation for PET

## Dependencies
* CMake 2.8.12 or later
* OpenGL, GLU, and GLUT (Optional for viewing)
* Python 2.5 or later to generate the materials files from XCOM

## Building
How to install Gray:

1. If glut libraries are not installed and you want to run the viewer:

    ```
    sudo apt-get install freeglut3 freeglut3-dev
    ```
    (This assumes a debian system.  Gray has been tested on Ubuntu 14.04 and MacOS Sierra primarily)

2. If you don't already have CMake:

    ```
    sudo apt-get install cmake
    ```

3. Compile program:

    ```
    mkdir build
    cd build
    cmake ../workspace/ -DCMAKE_BUILD_TYPE=Release
    make
    ```

    To use a different build system than Make, add the -G option before the
    period for cmake, and then the name of the generator you want to use, such as
    Xcode.

4. Build the materials files
    run the following from the main directory to tell Gray where to find the materials files
    ```
    cd materials
    python build_materials.py
    ```

5. Setup directory
    run the following from the main directory to tell Gray where to find the materials files
    ```
    source set_gray_dir.sh
    ```

5. Execute
    The previous step adds the ```bin``` folder to your ```$PATH``` variable.  You can now run
    Gray by typing into the shell.  If run without arguments, it will display all of the possible
    options to run that program.
    ```
    gray
    gray-view
    gray-daq
    ```
