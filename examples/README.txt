*** The Toadlet Engine examples folder ***

The examples folder contains small programs that
demonstrate features of the toadlet engine and that 
can be used to test a proper install of toadlet.

The example types are subdivided by toadlet module.
Each can be built using CMake scripts or platform
specific build files found in their respective 
subfolders.


==Android==

An example cmake line for building an example/android build is:
  cmake -G"NMake Makefiles" -DCMAKE_TOOLCHAIN_FILE=$TOADLET/cmake/Toolchains/Android.cmake -DANDROID_NDK=path/to/androidNdkDir -DANDROID_NDK_API_LEVEL=8 -DANDROID_SDK=path/to/androidSdkDir -DARM_TARGET=armeabi -DCMAKE_PREFIX_PATH=path/to/toadletAndroidInstall .

Then you must generate the build.xml file:
  android update project -p . -n exampleName -t 3
  
Then build the native code:
  nmake install

And build the java code:
  ant install