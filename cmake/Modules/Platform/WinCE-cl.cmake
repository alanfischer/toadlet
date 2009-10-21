SET(WIN32 1)
SET(WINCE 1)

IF(MSVC_VERSION LESS 1310)
	MESSAGE (FATAL_ERROR "Visual Studio 2005 or higher is required for WinCE builds")
ENDIF(MSVC_VERSION LESS 1310)
 
# Grab the standard windows settings
INCLUDE(Platform/Windows-cl)

# Reset the compiler flags for WinCE
SET(CMAKE_CXX_FLAGS_INIT "/W3 /Zm1000 /GR- /GS- /GX-")
SET(CMAKE_C_FLAGS_INIT "/W3 /Zm1000 /GR- /GS- /GX-")
SET(CMAKE_CXX_FLAGS_DEBUG_INIT "/D_DEBUG /MTd /Gm /Zi /Ob0 /Od")
SET(CMAKE_C_FLAGS_DEBUG_INIT "/D_DEBUG /MTd /Gm /Zi /Ob0 /Od")

# Reset the link flags for WinCE
SET(CMAKE_EXE_LINKER_FLAGS_INIT "/STACK:${CMAKE_WINCE_STACKSIZE} /subsystem:windowsce /machine:${CMAKE_CL_MACHINE_TYPE}")
SET(CMAKE_SHARED_LINKER_FLAGS_INIT "/STACK:${CMAKE_WINCE_STACKSIZE} /subsystem:windowsce /machine:${CMAKE_CL_MACHINE_TYPE}")

# Setup libraries for WinCE
SET(CMAKE_C_STANDARD_LIBRARIES_INIT "coredll.lib corelibc.lib ole32.lib oleaut32.lib uuid.lib")
SET(CMAKE_CXX_STANDARD_LIBRARIES_INIT "${CMAKE_C_STANDARD_LIBRARIES_INIT}")

# Setup the entry point
SET(CMAKE_CREATE_WIN32_EXE "/subsystem:windowsce,${CMAKE_SYSTEM_VERSION}")
SET(CMAKE_CREATE_CONSOLE_EXE "/subsystem:windowsce,${CMAKE_SYSTEM_VERSION} /ENTRY:mainACRTStartup")
