# ----------------------------------------------------------------------------
#  Android CMake toolchain file, for use with the ndk r5,r6,r6b,r7
#  See home page: http://code.google.com/p/android-cmake/
#
#  Updated by http://lightningtoads.com for use with 
#  the toadlet engine http://code.google.com/p/toadlet
#
#  Usage Linux:
#   $ export ANDROID_NDK=/<absolute path to NDK>
#   $ cmake -DCMAKE_TOOLCHAIN_FILE=<path to this file>/android.toolchain.cmake ..
#   $ make
#
#  Usage Linux (using standalone toolchain):
#   $ export ANDROID_NDK_TOOLCHAIN_ROOT=/<absolute path to standalone toolchain>
#   $ cmake -DCMAKE_TOOLCHAIN_FILE=<path to this file>/android.toolchain.cmake ..
#   $ make
#
#  Usage Windows:
#     You need native port of make to build your project.
#     For example this one: http://gnuwin32.sourceforge.net/packages/make.htm
#
#   $ SET ANDROID_NDK=C:\<absolute path to NDK>\android-ndk-r6
#   $ cmake.exe -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=<path to this file>\android.toolchain.cmake -DCMAKE_MAKE_PROGRAM=C:\<absolute path to make>\make.exe ..
#   $ C:\<absolute path to make>\make.exe
#
#     OR: You may instead use nmake, which is part of visual studio
#   $ cmake.exe -G"NMake Makefiles" etc.
#
#
#  Toolchain options (can be set as cmake parameters: -D<option_name>=<value>):
#    ANDROID_NDK=/opt/android-ndk-r6 - path to NDK root.
#      Can be set as environment variable.
#
#    ANDROID_NDK_TOOLCHAIN_ROOT=/opt/android-toolchain - path to standalone toolchain.
#      Option is not used if full NDK is found. Can be set as environment variable.
#
#    ANDROID_NDK_API_LEVEL=8 - level of android NDK API to use.
#      Option is ignored when build uses stanalone toolchain.
#
#    ANDROID_SDK=no default - path to SDK root. Used to locate the ANDROID_JAR file.
#      Can be set as environment variable.
#
#    ANDROID_SDK_API_LEVEL=8 - level of android SDK API to use. 
#
#    ANDROID_JAR=no default - direct path to the ANDROID_JAR file. If set then the ANDROID_SDK_* values have no effect.
#
#    ANDROID_ARCH=ARM - target architecture for the android build, may be either ARM or X86. 
#      The X86 architecture is only available with the ANDROID_NDK_API_LEVEL>=9
#
#    ARM_TARGET=armeabi - type of floating point support.
#      Other possible values are: "armeabi", "armeabi-v7a with NEON", "armeabi-v7a with VFPV3"
#
#    ANDROID_STL=(SYSTEM GNU_SHARED GNU_STATIC GABI_SHARED GABI_STATIC STLPORT_SHARED STLPORT_STATIC) - choose one
#      Select from among any of the android STL versions, shared or static. SYSTEM is the default
#
#    FORCE_ARM=false - set true to generate 32-bit ARM instructions instead of Thumb-1.
#
#    NO_UNDEFINED=true - set true to show all undefined symbols will as linker errors even if they are not used.
#
#
#  Toolcahin will search for NDK/toolchain in following order:
#    ANDROID_NDK - cmake parameter
#    ANDROID_NDK - environment variable
#    ANDROID_NDK - default location
#    ANDROID_NDK_TOOLCHAIN_ROOT - cmake parameter
#    ANDROID_NDK_TOOLCHAIN_ROOT - environment variable
#    ANDROID_NDK_TOOLCHAIN_ROOT - default location
#
#
#  What?:
#     Make sure to do the following in your scripts:
#       SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${my_cxx_flags}")
#       SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  ${my_cxx_flags}")
#       The flags will be prepopulated with critical flags, so don't loose them.
#    
#     ANDROID and BUILD_ANDROID will be set to true, you may test these 
#     variables to make necessary changes.
#    
#     Also ARMEABI and ARMEABI_V7A will be set true, mutually exclusive. V7A is
#     for floating point. NEON option will be set true if fpu is set to neon.
#
#     LIBRARY_OUTPUT_PATH_ROOT should be set in cache to determine where android
#     libraries will be installed.
#        default is ${CMAKE_SOURCE_DIR} , and the android libs will always be
#        under ${LIBRARY_OUTPUT_PATH_ROOT}/libs/armeabi* depending on target.
#        this will be convenient for android linking
#
#     Base system is Linux, but you may need to change things 
#     for android compatibility.
#   
#
#   - initial version December 2010 Ethan Rublee ethan.ruble@gmail.com
#   - modified April 2011 Andrey Kamaev andrey.kamaev@itseez.com
#     [+] added possibility to build with NDK (without standalone toolchain)
#     [+] support croos compilation on Windows (native, no cygwin support)
#     [+] added compiler option to force "char" type to be signed
#     [+] added toolchain option to compile to 32-bit ARM instructions
#     [+] added toolchain option to disable SWIG search
#     [+] added platform "armeabi-v7a with VFPV3"
#     [~] ARM_TARGETS renamed to ARM_TARGET
#   - modified April 2011 Andrey Kamaev andrey.kamaev@itseez.com
#     [+] EXECUTABLE_OUTPUT_PATH is set by toolchain (required on Windows)
#     [~] Fixed bug with ANDROID_NDK_API_LEVEL variable
#     [~] turn off SWIG search if it is not found first time
#   - modified May 2011 Andrey Kamaev andrey.kamaev@itseez.com
#     [~] ANDROID_LEVEL is renamed to ANDROID_NDK_API_LEVEL
#     [+] ANDROID_NDK_API_LEVEL is detected by toolchain if not specified
#     [~] added guard to prevent changing of output directories on first cmake pass
#     [~] toolchain exits with error if ARM_TARGET is not recognized
#   - modified June 2011 Andrey Kamaev andrey.kamaev@itseez.com
#     [~] default NDK path is updated for version r5c 
#     [+] variable CMAKE_SYSTEM_PROCESSOR is set based on ARM_TARGET
#     [~] toolchain install directory is added to linker paths
#     [-] removed SWIG-related stuff from toolchain
#     [+] added macro find_host_package, find_host_program to search packages/programs on host system
#     [~] fixed path to STL library
#   - modified July 2011 Andrey Kamaev andrey.kamaev@itseez.com
#     [~] fixed options caching
#     [~] search for all supported NDK versions
#     [~] allowed spaces in NDK path
#   - modified October 2011 Andrew Fischer andrew@lightningtoads.com
#     [~] included the latest ndk version r6b 
#     [~] added support for the android x86 platform (via ANDROID_ARCH)
#     [~] added the CMAKE_PREFIX_PATH variable to the CMAKE_FIND_ROOT_PATH
#     [~] changed ANDROID_API_LEVEL to ANDROID_NDK_API_LEVEL to avoid ambiguity
#     [~] added ANDROID_SDK location for android SDK awareness, used by toadlet
#     [~] added ANDROID_SDK_API_LEVEL for android SKD awareness, used by toadlet
#     [~] added ANDROID_ARCH=ARM or X86. (X86 only available with ANDROID_NDK_API_LEVEL>=9)
#     [~] fixed CMAKE_INSTALL_NAME_TOOL definition on OSX platforms
#   - modified December 2011 Andrew Fischer andrew@lightningtoads.com
#     [~] included the latest ndk version r7
#     [~] added the ANDROID_STL variable which lets the user choose the STL libraries
# ----------------------------------------------------------------------------

# this one is important
set( CMAKE_SYSTEM_NAME Linux )
# this one not so much
set( CMAKE_SYSTEM_VERSION 1 )

set( ANDROID_NDK_DEFAULT_SEARCH_PATH /opt/android-ndk )
set( ANDROID_NDK_SUPPORTED_VERSIONS -r7 -r6b -r6 -r5c -r5b -r5 "")
set( ANDROID_NDK_TOOLCHAIN_DEFAULT_SEARCH_PATH /opt/android-toolchain )
set( TOOL_OS_SUFFIX "" )

macro( __TOOLCHAIN_DETECT_API_LEVEL _path )
 SET( _expected ${ARGV1} )
 if( NOT EXISTS ${_path} )
  message( FATAL_ERROR "Could not verify Android API level. Probably you have specified invalid level value, or your copy of NDK/toolchain is broken." )
 endif()
 SET( API_LEVEL_REGEX "^[\t ]*#define[\t ]+__ANDROID_API__[\t ]+([0-9]+)[\t ]*$" )
 FILE( STRINGS ${_path} API_FILE_CONTENT REGEX "${API_LEVEL_REGEX}")
 if( NOT API_FILE_CONTENT )
  message( FATAL_ERROR "Could not verify Android API level. Probably you have specified invalid level value, or your copy of NDK/toolchain is broken." )
 endif()
 string( REGEX REPLACE "${API_LEVEL_REGEX}" "\\1" ANDROID_LEVEL_FOUND "${API_FILE_CONTENT}" )
 if( DEFINED _expected )
  if( NOT ${ANDROID_LEVEL_FOUND} EQUAL ${_expected} )
   message( FATAL_ERROR "Specified Android API level does not match level found. Probably your copy of NDK/toolchain is broken." )
  endif()
 endif()
 set( ANDROID_NDK_API_LEVEL ${ANDROID_LEVEL_FOUND} CACHE STRING "android API level" FORCE )
endmacro()

if( NOT DEFINED ANDROID_NDK )
 set( ANDROID_NDK $ENV{ANDROID_NDK} )
endif()

if( NOT DEFINED ANDROID_COMPILER )
 set( ANDROID_COMPILER 4.8 )
endif()

if( NOT DEFINED ANDROID_NDK_TOOLCHAIN_ROOT )
 set( ANDROID_NDK_TOOLCHAIN_ROOT $ENV{ANDROID_NDK_TOOLCHAIN_ROOT} )
endif()

#set path for android NDK -- look
if( NOT EXISTS "${ANDROID_NDK}" AND NOT DEFINED ANDROID_NDK_TOOLCHAIN_ROOT )
 foreach(ndk_version ${ANDROID_NDK_SUPPORTED_VERSIONS})
  if( EXISTS ${ANDROID_NDK_DEFAULT_SEARCH_PATH}${ndk_version} )
   set ( ANDROID_NDK ${ANDROID_NDK_DEFAULT_SEARCH_PATH}${ndk_version} )
   message( STATUS "Using default path for android NDK ${ANDROID_NDK}" )
   message( STATUS "If you prefer to use a different location, please define the variable: ANDROID_NDK" )
   break()
  endif()
 endforeach()
endif()

if( EXISTS "${ANDROID_NDK}" )
 set( ANDROID_NDK "${ANDROID_NDK}" CACHE PATH "root of the android ndk" FORCE )
 
 if( APPLE )
  if (DEFINED ENV{HOSTTYPE})
   set( NDKSYSTEM "darwin-$ENV{HOSTTYPE}" )
  else ()
   set( NDKSYSTEM "darwin-x86" )
  endif()
 elseif( WIN32 )
  set( NDKSYSTEM "windows" )
  set( TOOL_OS_SUFFIX ".exe" )
 elseif( UNIX )
  set( NDKSYSTEM "linux-x86_64" )
 else( APPLE )
  message( FATAL_ERROR "Your platform is not supported" )
 endif( APPLE )

 # On some platforms (OSX) this may not be defined, so search for it
 if( NOT DEFINED CMAKE_INSTALL_NAME_TOOL)
  find_program( CMAKE_INSTALL_NAME_TOOL install_name_tool)
 endif( NOT DEFINED CMAKE_INSTALL_NAME_TOOL)

 set( ANDROID_NDK_API_LEVEL $ENV{ANDROID_NDK_API_LEVEL} )
 string( REGEX REPLACE "[\t ]*android-([0-9]+)[\t ]*" "\\1" ANDROID_NDK_API_LEVEL "${ANDROID_NDK_API_LEVEL}" )
 string( REGEX REPLACE "[\t ]*([0-9]+)[\t ]*" "\\1" ANDROID_NDK_API_LEVEL "${ANDROID_NDK_API_LEVEL}" )

 set( PossibleAndroidLevels "3;4;5;8;9" )
 set( ANDROID_NDK_API_LEVEL ${ANDROID_NDK_API_LEVEL} CACHE STRING "android NDK API level" )
 set_property( CACHE ANDROID_NDK_API_LEVEL PROPERTY STRINGS ${PossibleAndroidLevels} )
 
 if( NOT ANDROID_NDK_API_LEVEL GREATER 2 )
  set( ANDROID_NDK_API_LEVEL 8)
  message( STATUS "Using default android NDK API level ${ANDROID_NDK_API_LEVEL}" )
  message( STATUS "If you prefer to use a different NDK API level, please define the variable: ANDROID_NDK_API_LEVEL" )
 endif( NOT ANDROID_NDK_API_LEVEL GREATER 2 )
 
 # Set the android SDK if defined
 if( NOT DEFINED ANDROID_SDK )
  set( ANDROID_SDK $ENV{ANDROID_SDK} )
 endif( NOT DEFINED ANDROID_SDK )
 set( ANDROID_SDK ${ANDROID_SDK} CACHE PATH "root of the android SDK" FORCE )

 # Set the default api level for the SDK
 set( ANDROID_SDK_API_LEVEL $ENV{ANDROID_SDK_API_LEVEL} )
 if( NOT DEFINED ANDROID_SDK_API_LEVEL )
  set( ANDROID_SDK_API_LEVEL 8)
  if ( EXISTS ${ANDROID_SDK} )
   message( STATUS "Using default android SDK API level ${ANDROID_SDK_API_LEVEL}" )
   message( STATUS "If you prefer to use a different SDK API level, please define the variable: ANDROID_SDK_API_LEVEL" )
  endif ( EXISTS ${ANDROID_SDK} )
 endif( NOT DEFINED ANDROID_SDK_API_LEVEL )
 set (ANDROID_SDK_API_LEVEL ${ANDROID_SDK_API_LEVEL} CACHE STRING "android SDK API level" FORCE)

 # Find and set the android.jar file for the selected API level
 if( NOT DEFINED ANDROID_JAR)
  set( ANDROID_JAR ${ANDROID_SDK}/platforms/android-${ANDROID_SDK_API_LEVEL}/android.jar )
  if ( EXISTS ${ANDROID_JAR} )
   message( STATUS "Using android.jar found at ${ANDROID_JAR}")
  endif ( EXISTS ${ANDROID_JAR} )
 endif( NOT DEFINED ANDROID_JAR)
 set( ANDROID_JAR ${ANDROID_JAR} CACHE FILEPATH "android SDK jar file" FORCE )

 # Setup the android architecture
 if( NOT ANDROID_ARCH )
  set( ANDROID_ARCH "ARM" )
  message( STATUS "Using default architecture ANDROID_ARCH=${ANDROID_ARCH}. Valid options are ARM or X86.")
 endif (NOT ANDROID_ARCH )
 set( ANDROID_ARCH ${ANDROID_ARCH} CACHE STRING "android architecture" FORCE )

 # x86 builds are only supported for ndk api level 9 and greater
 if( ANDROID_NDK_API_LEVEL LESS 9 AND ANDROID_ARCH STREQUAL "X86" )
  message( FATAL_ERROR "Android api levels < 9 do not support x86 builds. Please choose a different api level or switch architectures" )
 endif( ANDROID_NDK_API_LEVEL LESS 9 AND ANDROID_ARCH STREQUAL "X86" )

 # Use architecture to setup the toolchains
 if( ANDROID_ARCH STREQUAL "X86" )
  set( ANDROID_NDK_TOOLCHAIN_ARCH "x86" )
  set( ANDROID_NDK_TOOLS_PREFIX "i686-android-linux" )
  set( ANDROID_NDK_SYSROOT_ARCH "arch-x86" )
 elseif( ANDROID_ARCH STREQUAL "ARM" )
  set( ANDROID_NDK_TOOLCHAIN_ARCH "arm-linux-androideabi" )
  set( ANDROID_NDK_TOOLS_PREFIX "arm-linux-androideabi" )
  set( ANDROID_NDK_SYSROOT_ARCH "arch-arm" )
 else( ANDROID_ARCH STREQUAL "X86" )
  message( FATAL_ERROR "ANDROID_ARCH invalid choice: ${ANDROID_ARCH}")
 endif( ANDROID_ARCH STREQUAL "X86" )

 set( ANDROID_NDK_TOOLCHAIN_ROOT "${ANDROID_NDK}/toolchains/${ANDROID_NDK_TOOLCHAIN_ARCH}-${ANDROID_COMPILER}/prebuilt/${NDKSYSTEM}" )
 set( ANDROID_NDK_SYSROOT "${ANDROID_NDK}/platforms/android-${ANDROID_NDK_API_LEVEL}/${ANDROID_NDK_SYSROOT_ARCH}" )

 __TOOLCHAIN_DETECT_API_LEVEL( "${ANDROID_NDK_SYSROOT}/usr/include/android/api-level.h" ${ANDROID_NDK_API_LEVEL} )

 set( BUILD_WITH_ANDROID_NDK True )
else( EXISTS "${ANDROID_NDK}" )
 #try to find toolchain
 if( NOT EXISTS "${ANDROID_NDK_TOOLCHAIN_ROOT}" )
  set( ANDROID_NDK_TOOLCHAIN_ROOT "${ANDROID_NDK_TOOLCHAIN_DEFAULT_SEARCH_PATH}" )
  message( STATUS "Using default path for toolchain ${ANDROID_NDK_TOOLCHAIN_ROOT}" )
  message( STATUS "If you prefer to use a different location, please define the variable: ANDROID_NDK_TOOLCHAIN_ROOT" )
 endif( NOT EXISTS "${ANDROID_NDK_TOOLCHAIN_ROOT}" )

 set( ANDROID_NDK_TOOLCHAIN_ROOT "${ANDROID_NDK_TOOLCHAIN_ROOT}" CACHE PATH "root of the Android NDK standalone toolchain" FORCE )
 set( ANDROID_NDK_SYSROOT "${ANDROID_NDK_TOOLCHAIN_ROOT}/sysroot" )

 if( NOT EXISTS "${ANDROID_NDK_TOOLCHAIN_ROOT}" )
  message( FATAL_ERROR "neither ${ANDROID_NDK} nor ${ANDROID_NDK_TOOLCHAIN_ROOT} does not exist!
    You should either set an environment variable:
      export ANDROID_NDK=~/my-android-ndk
    or
      export ANDROID_NDK_TOOLCHAIN_ROOT=~/my-android-toolchain
    or put the toolchain or NDK in the default path:
      sudo ln -s ~/my-android-ndk ${ANDROID_NDK_DEFAULT_SEARCH_PATH}
      sudo ln -s ~/my-android-toolchain ${ANDROID_NDK_TOOLCHAIN_DEFAULT_SEARCH_PATH}" )
 endif( NOT EXISTS "${ANDROID_NDK_TOOLCHAIN_ROOT}" )
 
 __TOOLCHAIN_DETECT_API_LEVEL( "${ANDROID_NDK_SYSROOT}/usr/include/android/api-level.h" )

 #message( STATUS "Using android NDK standalone toolchain from ${ANDROID_NDK_TOOLCHAIN_ROOT}" )
 set( BUILD_WITH_ANDROID_NDK_TOOLCHAIN True )
endif( EXISTS "${ANDROID_NDK}" )

# specify the cross compiler
set( CMAKE_C_COMPILER   "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ANDROID_NDK_TOOLS_PREFIX}-gcc${TOOL_OS_SUFFIX}"     CACHE PATH "gcc" FORCE )
set( CMAKE_CXX_COMPILER "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ANDROID_NDK_TOOLS_PREFIX}-g++${TOOL_OS_SUFFIX}"     CACHE PATH "g++" FORCE )
#there may be a way to make cmake deduce these TODO deduce the rest of the tools
set( CMAKE_AR           "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ANDROID_NDK_TOOLS_PREFIX}-ar${TOOL_OS_SUFFIX}"      CACHE PATH "archive" FORCE )
set( CMAKE_LINKER       "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ANDROID_NDK_TOOLS_PREFIX}-ld${TOOL_OS_SUFFIX}"      CACHE PATH "linker" FORCE )
set( CMAKE_NM           "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ANDROID_NDK_TOOLS_PREFIX}-nm${TOOL_OS_SUFFIX}"      CACHE PATH "nm" FORCE )
set( CMAKE_OBJCOPY      "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ANDROID_NDK_TOOLS_PREFIX}-objcopy${TOOL_OS_SUFFIX}" CACHE PATH "objcopy" FORCE )
set( CMAKE_OBJDUMP      "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ANDROID_NDK_TOOLS_PREFIX}-objdump${TOOL_OS_SUFFIX}" CACHE PATH "objdump" FORCE )
set( CMAKE_STRIP        "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ANDROID_NDK_TOOLS_PREFIX}-strip${TOOL_OS_SUFFIX}"   CACHE PATH "strip" FORCE )
set( CMAKE_RANLIB       "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin/${ANDROID_NDK_TOOLS_PREFIX}-ranlib${TOOL_OS_SUFFIX}"  CACHE PATH "ranlib" FORCE )

if( ANDROID_ARCH STREQUAL "ARM" )
 #setup build targets, mutually exclusive
 set( PossibleArmTargets "armeabi;armeabi-v7a;armeabi-v7a with NEON;armeabi-v7a with VFPV3" )
 #compatibility junk for previous version of toolchain
 if( DEFINED ARM_TARGETS AND NOT DEFINED ARM_TARGET )
  set( ARM_TARGET "${ARM_TARGETS}" )
 endif( DEFINED ARM_TARGETS AND NOT DEFINED ARM_TARGET )
 if( NOT ARM_TARGET )
  set( ARM_TARGET armeabi )
  message( STATUS "Using default ARM_TARGET=${ARM_TARGET}, valid options are: armeabi, armeabi-v7a, armeabi-v7a with NEON, armeabi-v7a with VFPV3.")
 endif( NOT ARM_TARGET )
 set( ARM_TARGET "${ARM_TARGET}" CACHE INTERNAL "the arm target for android, recommend armeabi-v7a for floating point support and NEON." )
 set_property( CACHE ARM_TARGET PROPERTY STRINGS ${PossibleArmTargets} )

 #set these flags for client use
 if( ARM_TARGET STREQUAL "armeabi" )
  set( ARMEABI true )
  set( ARMEABI_NDK_NAME "armeabi" )
  set( NEON false )
  set( CMAKE_SYSTEM_PROCESSOR "armv5te" )
 else( ARM_TARGET STREQUAL "armeabi" )
  if( ARM_TARGET STREQUAL "armeabi-v7a with NEON" )
   set( NEON true )
   set( VFPV3 true )
  elseif( ARM_TARGET STREQUAL "armeabi-v7a with VFPV3" )
   set( VFPV3 true )
  elseif( NOT ARM_TARGET STREQUAL "armeabi-v7a")
   message( FATAL_ERROR "Unsupported ARM_TARGET=${ARM_TARGET} is specified. Supported values are: \"armeabi\", \"armeabi-v7a\", \"armeabi-v7a with NEON\", \"armeabi-v7a with VFPV3\"" )
  endif( ARM_TARGET STREQUAL "armeabi-v7a with NEON" )
  set( ARMEABI_V7A true )
  set( ARMEABI_NDK_NAME "armeabi-v7a" )
  set( CMAKE_SYSTEM_PROCESSOR "armv7-a" )
 endif( ARM_TARGET STREQUAL "armeabi" )
 set (NDK_NAME_ARCH ${ARMEABI_NDK_NAME})
elseif( ANDROID_ARCH STREQUAL "X86" )
 set( NDK_NAME_ARCH "x86" )
endif( ANDROID_ARCH STREQUAL "ARM" )

# Set find root path to the target environment, plus the user defined search path
set( CMAKE_FIND_ROOT_PATH "${ANDROID_NDK_TOOLCHAIN_ROOT}/bin" "${ANDROID_NDK_TOOLCHAIN_ROOT}/${ANDROID_NDK_TOOLCHAIN_ARCH}" "${ANDROID_NDK_SYSROOT}" "${CMAKE_PREFIX_PATH}")

if( BUILD_WITH_ANDROID_NDK )
 #setup STL targets, mutually exclusive
 set( PossibleSTLTargets "SYSTEM;GNU_SHARED;GNU_STATIC;GABI_SHARED;GABI_STATIC;STLPORT_SHARED;STLPORT_STATIC" )
 set( PossibleGNUSTLVersions "4.6;4.8" )
 if( NOT ANDROID_STL )
  set( ANDROID_STL "GNU_SHARED" )
  message (STATUS "Using default ANDROID_STL=${ANDROID_STL}, valid options are:${PossibleSTLTargets}" )
 endif( NOT ANDROID_STL )
 if( ${ANDROID_STL} STREQUAL "GNU_SHARED" OR ${ANDROID_STL} STREQUAL "GNU_STATIC" )
    if( NOT ANDROID_GNU_STL_VERSION )
      set( ANDROID_GNU_STL_VERSION "4.8" )
      message( STATUS "Using default ANDROID_GNU_STL_VERSION=${ANDROID_GNU_STL_VERSION}, valid options are:${PossibleGNUSTLVersions}" )
    endif( NOT ANDROID_GNU_STL_VERSION)
 endif( ${ANDROID_STL} STREQUAL "GNU_SHARED" OR ${ANDROID_STL} STREQUAL "GNU_STATIC" )
 set( ANDROID_STL "${ANDROID_STL}" CACHE INTERNAL "The choice of STL versions. GNU_SHARED is the default." )
 set_property( CACHE ANDROID_STL PROPERTY STRINGS ${PossibleSTLTargets} )

 if ( ${ANDROID_STL} STREQUAL "SYSTEM" )
  set( STL_CXX_FLAGS "-fno-exceptions -fno-rtti" )
  set( STL_PATH "${ANDROID_NDK}/sources/cxx-stl/system" )
  set( STL_LIBS "-lstdc++" )
 elseif( ${ANDROID_STL} STREQUAL "GNU_SHARED" )
  if( NOT DEFINED ANDROID_GNU_STL_VERSION )
	  message( SEND_ERROR "When using GNU stl you must define ANDROID_GNU_STL_VERSION")
  endif()
  set( STL_CXX_FLAGS "-fexceptions -frtti" )
  set( STL_PATH "${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/${ANDROID_GNU_STL_VERSION}" )
  set( STL_LIBS "-lgnustl_shared -lsupc++" )
 elseif( ${ANDROID_STL} STREQUAL "GNU_STATIC" )
  if( NOT DEFINED ANDROID_GNU_STL_VERSION )
	  message( SEND_ERROR "When using GNU stl you must define ANDROID_GNU_STL_VERSION")
  endif()
  set( STL_CXX_FLAGS "-fexceptions -frtti" )
  set( STL_PATH "${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/${ANDROID_GNU_STL_VERSION}" )
  set( STL_LIBS "-Wl,-Bstatic -Wl,--start-group -lgnustl_static -lsupc++ -Wl,-Bdynamic,--end-group" )
 elseif( ${ANDROID_STL} STREQUAL "GABI_SHARED" )
  set( STL_CXX_FLAGS "-fno-exceptions -frtti" )
  set( STL_PATH "${ANDROID_NDK}/sources/cxx-stl/gabi++" )
  set( STL_LIBS "-lgabi++_shared" )
 elseif( ${ANDROID_STL} STREQUAL "GABI_STATIC" )
  set( STL_CXX_FLAGS "-fno-exceptions -frtti" )
  set( STL_PATH "${ANDROID_NDK}/sources/cxx-stl/gabi++" )
  set( STL_LIBS "-lgabi++_static" )
 elseif( ${ANDROID_STL} STREQUAL "STLPORT_SHARED" )
  set( STL_CXX_FLAGS "-fno-exceptions -frtti" )
  set( STL_PATH "${ANDROID_NDK}/sources/cxx-stl/stlport" )
  set( STL_LIBS "-lstlport_shared" )
 elseif( ${ANDROID_STL} STREQUAL "STLPORT_STATIC" )
  set( STL_CXX_FLAGS "-fno-exceptions -frtti" )
  set( STL_PATH "${ANDROID_NDK}/sources/cxx-stl/stlport" )
  set( STL_LIBS "-lstlport_static" )
 else( ${ANDROID_STL} STREQUAL "SYSTEM" )
  message( STATUS "No C++ STL chosen." )
 endif( ${ANDROID_STL} STREQUAL "SYSTEM" )

 set( STL_LIBRARIES_PATH "${STL_PATH}/libs/${NDK_NAME_ARCH}" )
 include_directories(SYSTEM "${STL_PATH}/include" "${STL_LIBRARIES_PATH}/include" )
# if ( NOT ARMEABI AND NOT FORCE_ARM )
#  set( STL_LIBRARIES_PATH "${ANDROID_NDK_TOOLCHAIN_ROOT}/${ANDROID_NDK_TOOLCHAIN_ARCH}/lib/${CMAKE_SYSTEM_PROCESSOR}/thumb" )
# endif()
endif( BUILD_WITH_ANDROID_NDK )

if( BUILD_WITH_ANDROID_NDK_TOOLCHAIN )
 set( STL_LIBRARIES_PATH "${ANDROID_NDK_TOOLCHAIN_ROOT}/${ANDROID_NDK_TOOLCHAIN_ARCH}/lib" )
 if( NOT ARMEABI )
  set( STL_LIBRARIES_PATH "${STL_LIBRARIES_PATH}/${CMAKE_SYSTEM_PROCESSOR}" )
 endif( NOT ARMEABI )
 if( NOT FORCE_ARM )
  set( STL_LIBRARIES_PATH "${STL_LIBRARIES_PATH}/thumb" )
 endif( NOT FORCE_ARM )
 #for some reason this is needed? TODO figure out why...
 include_directories(SYSTEM "${ANDROID_NDK_TOOLCHAIN_ROOT}/${ANDROID_NDK_TOOLCHAIN_ARCH}/include/c++/${ANDROID_COMPILER}/${ANDROID_NDK_TOOLCHAIN_ARCH}" )
 # If using a toolchain, default to the GNU_SHARED libraries
 set( STL_LIBS "-lstdc++ -lsupc++" )
endif( BUILD_WITH_ANDROID_NDK_TOOLCHAIN )


# only search for libraries and includes in the ndk toolchain
set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )

set( CMAKE_CXX_FLAGS "-fPIC -DANDROID -Wno-psabi -fno-strict-aliasing -fsigned-char" )
set( CMAKE_C_FLAGS "-fPIC -DANDROID -Wno-psabi -fno-strict-aliasing -fsigned-char" )

set( FORCE_ARM OFF CACHE BOOL "Use 32-bit ARM instructions instead of Thumb-1" )
if( ANDROID_ARCH STREQUAL "ARM")
 if( NOT FORCE_ARM )
  #It is recommended to use the -mthumb compiler flag to force the generation
  #of 16-bit Thumb-1 instructions (the default being 32-bit ARM ones).
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mthumb" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mthumb" )
 else( NOT FORCE_ARM )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -marm" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -marm" )
 endif( NOT FORCE_ARM )
endif( ANDROID_ARCH STREQUAL "ARM")

if( BUILD_WITH_ANDROID_NDK )
 set( CMAKE_CXX_FLAGS "--sysroot=\"${ANDROID_NDK_SYSROOT}\" ${CMAKE_CXX_FLAGS} ${STL_CXX_FLAGS}" )
 set( CMAKE_C_FLAGS "--sysroot=\"${ANDROID_NDK_SYSROOT}\" ${CMAKE_C_FLAGS}" )

 # workaround for ugly cmake bug - compiler identification replaces all spaces (and somethimes " !!!) in compiler flags with ; symbol
 # as result identification fails if ANDROID_NDK_SYSROOT contain spaces
 include(CMakeForceCompiler)
 CMAKE_FORCE_C_COMPILER("${CMAKE_C_COMPILER}" GNU)
 CMAKE_FORCE_CXX_COMPILER("${CMAKE_CXX_COMPILER}" GNU)
endif( BUILD_WITH_ANDROID_NDK )

if( ARMEABI_V7A )
 #these are required flags for android armv7-a
 set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mfloat-abi=softfp" )
 set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfloat-abi=softfp" )
 if( NEON )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon" )
 elseif( VFPV3 )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=vfpv3" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=vfpv3" )
 endif( NEON )
endif( ARMEABI_V7A )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "c++ flags" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "c flags" )
      
#-Wl,-L${LIBCPP_LINK_DIR},-lstdc++,-lsupc++
#-L${LIBCPP_LINK_DIR} -lstdc++ -lsupc++
#Also, this is *required* to use the following linker flags that routes around
#a CPU bug in some Cortex-A8 implementations:
if( ANDROID_ARCH STREQUAL "ARM")
 set( LINKER_FLAGS "-Wl,--fix-cortex-a8 -L\"${STL_LIBRARIES_PATH}\" -L\"${CMAKE_INSTALL_PREFIX}/libs/${NDK_NAME_ARCH}\" ${STL_LIBS} " )
elseif( ANDROID_ARCH STREQUAL "X86")
 set( LINKER_FLAGS "-L\"${STL_LIBRARIES_PATH}\" -L\"${CMAKE_INSTALL_PREFIX}/libs/${NDK_NAME_ARCH}\" ${STL_LIBS} " )
endif( ANDROID_ARCH STREQUAL "ARM")

set( NO_UNDEFINED ON CACHE BOOL "Don't all undefined symbols" )
if( NO_UNDEFINED )
 set( LINKER_FLAGS "-Wl,--no-undefined ${LINKER_FLAGS}" )
endif( NO_UNDEFINED )

set( CMAKE_SHARED_LINKER_FLAGS "${LINKER_FLAGS}" CACHE STRING "linker flags" FORCE )
set( CMAKE_MODULE_LINKER_FLAGS "${LINKER_FLAGS}" CACHE STRING "linker flags" FORCE )
set( CMAKE_EXE_LINKER_FLAGS "${LINKER_FLAGS}" CACHE STRING "linker flags" FORCE )

#set these global flags for cmake client scripts to change behavior
set( ANDROID True )
set( BUILD_ANDROID True )

#macro to find packages on the host OS
macro(find_host_package)
 set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
 set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER )
 set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER )
 if( CMAKE_HOST_WIN32 )
  SET( WIN32 1 )
  SET( UNIX )
 elseif( CMAKE_HOST_APPLE )
  SET( APPLE 1 )
  SET( UNIX )
 endif( CMAKE_HOST_WIN32 )
 find_package( ${ARGN} )
 SET( WIN32 )
 SET( APPLE )
 SET( UNIX 1)
 set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY )
 set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
 set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
endmacro(find_host_package)
#macro to find programs on the host OS
macro(find_host_program)
 set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
 set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER )
 set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER )
 if( CMAKE_HOST_WIN32 )
  SET( WIN32 1 )
  SET( UNIX )
 elseif( CMAKE_HOST_APPLE )
  SET( APPLE 1 )
  SET( UNIX )
 endif( CMAKE_HOST_WIN32 )
 find_program( ${ARGN} )
 SET( WIN32 )
 SET( APPLE )
 SET( UNIX 1)
 set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY )
 set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
 set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
endmacro(find_host_program)

MARK_AS_ADVANCED(FORCE_ARM NO_UNDEFINED)

