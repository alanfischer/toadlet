message ("Setting up iPhoneSimulator toolchain")

# Standard settings
set (TOADLET_PLATFORM_IPHONE ON CACHE BOOL "Enable the iPhone platform")
set (CMAKE_SYSTEM_NAME iPhoneSimulator)

# Skip the platform compiler checks for cross compiling since they're a headache
set (CMAKE_CXX_COMPILER_WORKS TRUE)
set (CMAKE_C_COMPILER_WORKS TRUE)

# Include extra modules
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

message ("iPhoneSimulator toolchain loaded")
