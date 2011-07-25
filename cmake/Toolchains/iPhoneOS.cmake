message ("Setting up iPhoneOS toolchain")

# Standard settings
set (TOADLET_PLATFORM_IPHONE ON CACHE BOOL "Enable the iPhone platform" FORCE)
set (CMAKE_SYSTEM_NAME iPhoneOS)

# Skip the platform compiler checks for cross compiling since they're a headache
set (CMAKE_CXX_COMPILER_WORKS TRUE)
set (CMAKE_C_COMPILER_WORKS TRUE)

# Include extra modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

message ("iPhoneOS toolchain loaded")
