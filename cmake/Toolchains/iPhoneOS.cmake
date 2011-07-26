message ("Setting up iPhoneOS toolchain")

# Standard settings
set (TOADLET_PLATFORM_IPHONE ON CACHE BOOL "Enable the iPhone platform" FORCE)
set (CMAKE_SYSTEM_NAME iPhoneOS)

# Force the compilers to gcc for iOS
include (CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER (gcc gcc)
CMAKE_FORCE_CXX_COMPILER (g++ g++)

# Skip the platform compiler checks for cross compiling
set (CMAKE_CXX_COMPILER_WORKS TRUE)
set (CMAKE_C_COMPILER_WORKS TRUE)

# Include extra modules for the iOS platform files
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

message ("iPhoneOS toolchain loaded")
