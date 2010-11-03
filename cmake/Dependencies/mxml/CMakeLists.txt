cmake_minimum_required (VERSION 2.6)

# Setup project includes
include_directories (${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set (MXML_VERSION "\"Mini-XML v2.6\"")

# Special CMake functions for include checks
include (CheckFunctionExists)
include (CheckIncludeFiles)
include (CheckTypeSize)

# Perform configure file checks
check_function_exists (snprintf HAVE_SNPRINTF)
check_function_exists (vsnprintf HAVE_VSNPRINTF)
check_function_exists (strdup HAVE_STRDUP)
check_include_files (pthread.h HAVE_PTHREAD_H)
check_type_size ("long long" LONG_LONG)
set(HAVE_LONG_LONG ${HAVE_LONG_LONG})

# Setup configure file based on results from the checks performed above
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.in.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Source files
set (MXML_SRC
	mxml-attr.c
	mxml-entity.c
	mxml-file.c
	mxml-index.c
	mxml-node.c
	mxml-private.c
	mxml-search.c
	mxml-set.c
	mxml-string.c
)

# Headers
set (MXML_H
	mxml.h
	mxml-private.h
)

# Static library
add_library (mxml STATIC ${MXML_SRC} ${MXML_H})
install (TARGETS mxml DESTINATION lib)
