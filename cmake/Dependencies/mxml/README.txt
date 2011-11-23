MXML may easily be built for iOS and Android platforms with these cmake files. 
The instructions below explain how to use them.


1. Extract the version of MXML you have downloaded to a directory (say mxml/)

2. Copy these 2 files (CMakeLists.txt and config.h.cmake) into the mxml/ directory.

3. Create a platform build directory inside of mxml/ (say mxml/build.ios for iOS or mxml/build.android for Android)

4. Call the proper toadlet cmake toolchain file from within the build directory:

   4a. For iOS: cmake -GXcode -DCMAKE_TOOLCHAIN_FILE=/path/to/toadlet/cmake/Toolchains/iOS.cmake -DCMAKE_INSTALL_PREFIX=$HOME/ios ..

   4b. For Android on OSX: cmake -GXcode -DCMAKE_TOOLCHAIN_FILE=/path/to/toadlet/cmake/Toolchains/Android.cmake -ANDROID_NDK=/path/to/android-ndk -DCMAKE_INSTALL_PREFIX=/path/to/install ..

   4c. For Android on Windows: cmake -G"NMake Makefiles" -DCMAKE_TOOLCHAIN_FILE="C:\path\to\toadlet\cmake\Toolchains\Android.cmake" -ANDROID_NDK="C:\path\to\android-ndk" -DCMAKE_INSTALL_PREFIX="C:\path\to\install" ..

   4b. For Android on Posix: cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/toadlet/cmake/Toolchains/Android.cmake -ANDROID_NDK=/path/to/android-ndk -DCMAKE_INSTALL_PREFIX=/path/to/install ..

5. Open the resulting xcode project (OSX) or run the makefile system (Windows & Posix) and build the install target

