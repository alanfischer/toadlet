GLEW is almost always present on the 3 dekstop platforms (Windows, Posix, OSX). 
GLEW is not needed on Android or iOS platforms.

This cmake file is mainly useful if you want an easy to make static build of glew,
since standard installs do not always provide one.


1. Extract the version of glew you have downloaded to a directory (say glewl/)

2. Copy this file (CMakeLists.txt) into the glew/ directory.

3. Create a build directory inside of glew/ (say glew/build)

4. Call cmake from within build: cmake  -DCMAKE_INSTALL_PREFIX=/path/to/install ..

5. Build with the platform build tool of your choice
