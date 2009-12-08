# Generic PreProcessor - gpp
project (gpp C)

# Perform configure file checks
include (CheckIncludeFiles)
include (CheckFunctionExists)
check_include_files (fnmatch.h HAVE_FNMATCH_H)
check_function_exists (strdup HAVE_STRDUP)
if (WIN32)
	check_function_exists (stricmp HAVE_STRCASECMP)
	set (WIN_NT TRUE)
	set (GPP_DEFS _CRT_SECURE_NO_WARNINGS)
else (WIN32)
	check_function_exists (strcasecmp HAVE_STRCASECMP)	
endif (WIN32)

# Setup configure file
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Definitions
set (GPP_DEFS ${GPP_DEFS} PACKAGE_STRING="GPP 2.24")

# Includes
include_directories (${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# Source files
set (GPP_SRC
	gpp.c
)

# Executable
add_executable (gpp ${GPP_SRC})
set_property (TARGET gpp APPEND PROPERTY COMPILE_DEFINITIONS ${GPP_DEFS})

