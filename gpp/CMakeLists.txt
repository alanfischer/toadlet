# Generic PreProcessor - gpp

# Perform configure file checks
include (CheckIncludeFiles)
include (CheckFunctionExists)
check_include_files (fnmatch.h HAVE_FNMATCH_H)
check_function_exists (strdup HAVE_STRDUP)
check_function_exists (strcasecmp HAVE_STRCASECMP)

# Includes
set (GPP_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${GPP_INCLUDES})

# Libraries
set (GPP_LIBS)

# Source files
set (GPP_SRC
	gpp.c
)

# Executable
add_executable (gpp ${GPP_SRC} ${GPP_H})
target_link_libraries (gpp ${GPP_LIBS})
install (TARGETS gpp DESTINATION bin)

