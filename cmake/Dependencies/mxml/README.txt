MXML is usually built for the iOS platform with these cmake files. The instructions below explain how to use them.

1. Extract the version of MXML you have download to a directory (say mxml/)

2. Copy these 2 files (CMakeLists.txt and config.h.cmake) into the mxml/ directory.

3. Recursively copy the entire toadlet/cmake directory into the mxml/ directory (so you have mxml/cmake)

   * You actually only need 2 files for the cmake build to work: mxml/cmake/Toolchains/iOS.cmake and mxml/cmake/Modules/Platforms/iOS.cmake

   * The above 2 files need to retain their directory structure for cmake to pickup the Platform builds file properly.

   * Copying the whole directory is usually just easier

4. Create an iOS build directory inside of mxml/ (say mxml/build.ios)

5. Call the cmake toolchain file from within build.ios: cmake -GXcode -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchains/iOS.cmake -DCMAKE_INSTALL_PREFIX=$HOME/ios ..

6. Open the resulting xcode project and build the install target
