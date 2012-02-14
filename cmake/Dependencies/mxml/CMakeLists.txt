cmake_minimum_required (VERSION 2.6)
project (mxml C)

# Platform specific dependencies
# This includes all header files from the source tree (SOURCE_DIR) and the cmake build tree (BINARY_DIR)
set (MXML_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set (LIB_INSTALL_DIR lib)

# Actually include the directories for CMake to use
include_directories (${MXML_INCLUDES})

# Keep the source code happy
set (MXML_VERSION "Mini-XML v2.6")

# Platform specific settings - toolchain plaforms do not perform configure checks
if (ANDROID)
	set (LIB_INSTALL_DIR ${LIB_INSTALL_DIR}/${NDK_NAME_ARCH})
	set (HAVE_SNPRINTF TRUE)
	set (HAVE_VSNPRINTF TRUE)
elseif (IOS)
	set (HAVE_SNPRINTF TRUE)
	set (HAVE_VSNPRINTF TRUE)
else (ANDROID)
	include (CheckIncludeFiles)
	include (CheckFunctionExists)
	include (CheckTypeSize)
	include (CheckStructHasMember)

	check_type_size("long long" LONG_LONG)
	set(HAVE_LONG_LONG ${HAVE_LONG_LONG})
	check_function_exists (snprintf HAVE_SNPRINTF)
	check_function_exists (vsnprintf HAVE_VSNPRINTF)
	check_function_exists (strdup HAVE_STRDUP)
	check_include_files (pthread.h HAVE_PTHREAD_H)
endif (ANDROID)

# Setup configure file based on results from the checks performed above
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

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

# Install
install (TARGETS mxml DESTINATION ${LIB_INSTALL_DIR})
install (FILES mxml.h DESTINATION include)

