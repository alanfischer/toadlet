GLEW is almost always present on the 3 dekstop platforms (Windows, Posix, OSX). This cmake file is useful if you have to build glew and are lazy..

1. Extract the version of glew you have downloaded to a directory (say glewl/)

2. Copy this file (CMakeLists.txt) into the glew/ directory.

4. Create a build directory inside of mxml/ (say mxml/build)

5. Call cmake from within builds: cmake  -DCMAKE_INSTALL_PREFIX=$HOME/mxml ..

6. Build with the platform build tool of your choice
