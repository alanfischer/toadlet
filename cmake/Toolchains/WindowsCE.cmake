message ("Make sure you checked the setup of this toolchain file=${CMAKE_TOOLCHAIN_FILE}!")

# Standard settings
set (TOADLET_PLATFORM_WINCE ON CACHE BOOL "Enable the WindowsCE platform" FORCE)
set (CMAKE_SYSTEM_NAME WinCE)

# Force a manual updating of the visual studio project files for the platform SDK
# This should go away once cmake supports WinCE by default
option (UPDATE_WINCE "Update project files for WinCE - Only do this AFTER you have generated them with tUPDATE_WINCE=OFF" OFF)

# This should be set to the version # of WinCE that you are using
set (CMAKE_SYSTEM_VERSION 4.20)

# Set this to whatever SDK you wish to build toadlet against. PPC 2003 is currently the default
set (WINCE_PLATFORM_SDK "Pocket PC 2003 (ARMV4)")

# This value ends in either _PSPC for pocket pc or _WFSP for smartphone, depending on the selection made for WINCE_PLATFORM_SDK
set (WINCE_PLATFORM_TYPE WIN32_PLATFORM_PSPC)

# Set the processor type here
set (CMAKE_CL_MACHINE_TYPE ARM)

# Change if needed
set (CMAKE_WINCE_STACKSIZE 65536)

# Skip the platform compiler checks for cross compiling since they're a major headache
set (CMAKE_CXX_COMPILER_WORKS TRUE)
set (CMAKE_C_COMPILER_WORKS TRUE)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

message ("WinCE toolchain loaded")
