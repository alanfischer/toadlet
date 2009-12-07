# Generic PreProcessor - gpp
project (gpp C)

# Perform configure file checks
include (CheckIncludeFiles)
include (CheckFunctionExists)
check_include_files (fnmatch.h HAVE_FNMATCH_H)
check_function_exists (strdup HAVE_STRDUP)
check_function_exists (strcasecmp HAVE_STRCASECMP)

# Definitions
set (GPP_DEFS PACKAGE_STRING="GPP 2.24")

# Includes
set (GPP_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${GPP_INCLUDES})

# Source files
set (GPP_SRC
	gpp.c
)

# Executable
add_executable (gpp ${GPP_SRC})
set_property (TARGET gpp APPEND PROPERTY COMPILE_DEFINITIONS ${GPP_DEFS})

