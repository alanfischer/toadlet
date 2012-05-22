# These platforms require swig to generate bindings before compilation
# TODO: Once swig's gsoc 2012 project finishes, let iOS use swig as well
#if (TOADLET_PLATFORM_ANDROID OR TOADLET_PLATFORM_IOS)
if (TOADLET_PLATFORM_ANDROID)
	# Find swig on our host and include our swig wrapper macro
	find_host_package (SWIG REQUIRED)
	include (SwigWrapFiles)

	# Add the swig files
	add_subdirectory (swig)
#endif (TOADLET_PLATFORM_ANDROID OR TOADLET_PLATFORM_IOS)
endif (TOADLET_PLATFORM_ANDROID)


# Add the c++ source code
add_subdirectory (cpp)


# Python requries the swig bindings be added as a separate module
if (TOADLET_PLATFORM_PYTHON)
	# Find swig and include cmake's swig package
	find_package (SWIG REQUIRED)
	include (UseSwig)
	
	# Add the swig files
	add_subdirectory (swig)
endif (TOADLET_PLATFORM_PYTHON)


# Android requires java libraries
if (TOADLET_PLATFORM_ANDROID)
	if (NOT EXISTS ${ANDROID_JAR})
		message (FATAL_ERROR "ANDROID_JAR not found")
	endif (NOT EXISTS ${ANDROID_JAR})

	add_subdirectory (java)
endif (TOADLET_PLATFORM_ANDROID)

