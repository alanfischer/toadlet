cmake_minimum_required (VERSION 2.8.6)


# Project name and compilers
project (toadlet CXX C)


# Version Settings for cpp libraries
set (TOADLET_BUILD_VERSION_EGG_MAJOR 0)
set (TOADLET_BUILD_VERSION_EGG_MINOR 9)
set (TOADLET_BUILD_VERSION_EGG_MICRO 0)
set (TOADLET_BUILD_VERSION_FLICK_MAJOR 0)
set (TOADLET_BUILD_VERSION_FLICK_MINOR 9)
set (TOADLET_BUILD_VERSION_FLICK_MICRO 0)
set (TOADLET_BUILD_VERSION_HOP_MAJOR 0)
set (TOADLET_BUILD_VERSION_HOP_MINOR 9)
set (TOADLET_BUILD_VERSION_HOP_MICRO 0)
set (TOADLET_BUILD_VERSION_KNOT_MAJOR 0)
set (TOADLET_BUILD_VERSION_KNOT_MINOR 9)
set (TOADLET_BUILD_VERSION_KNOT_MICRO 0)
set (TOADLET_BUILD_VERSION_PEEPER_MAJOR 0)
set (TOADLET_BUILD_VERSION_PEEPER_MINOR 9)
set (TOADLET_BUILD_VERSION_PEEPER_MICRO 0)
set (TOADLET_BUILD_VERSION_RIBBIT_MAJOR 0)
set (TOADLET_BUILD_VERSION_RIBBIT_MINOR 9)
set (TOADLET_BUILD_VERSION_RIBBIT_MICRO 0)
set (TOADLET_BUILD_VERSION_TADPOLE_MAJOR 0)
set (TOADLET_BUILD_VERSION_TADPOLE_MINOR 9)
set (TOADLET_BUILD_VERSION_TADPOLE_MICRO 0)
set (TOADLET_BUILD_VERSION_PAD_MAJOR 0)
set (TOADLET_BUILD_VERSION_PAD_MINOR 9)
set (TOADLET_BUILD_VERSION_PAD_MICRO 0)


# Setup CTest
enable_testing()


# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")


# Get the current hg revision
if (ANDROID OR IOS)
	find_host_package (Mercurial)
else (ANDROID OR IOS)
	find_package (Mercurial)
endif (ANDROID OR IOS)
if (MERCURIAL_FOUND)
	MERCURIAL_HG_INFO(${PROJECT_SOURCE_DIR} TOADLET)
endif (MERCURIAL_FOUND)


# Assign the TOADLET_PLATFORM_X value, used throughout toadlet for consistency
if (ANDROID)
	set (TOADLET_PLATFORM_ANDROID ON CACHE BOOL "Toadlet for Android")

	option (INSTALL_ANDROID_APP "Install just the libs for use by an android application" OFF)
endif (ANDROID)
if (WIN32)
	set (TOADLET_PLATFORM_WIN32 ON CACHE BOOL "Toadlet for windows platforms")
	if (WINCE)
		# TODO: Right now WinCE is being blocked due to the uncertain future of Toadlet on Windows Phone Platforms 
		message (FATAL_ERROR "WindowsCE builds are currently disabled given the uncertain future of native code on Windows Phone 7. You may comment out this error to attempt the build anyway. Your mileage may vary.")
		set (TOADLET_PLATFORM_WINCE ON CACHE BOOL "Toadlet for Windows CE")
	endif(WINCE)
elseif (UNIX)
	set (TOADLET_PLATFORM_POSIX ON CACHE BOOL "Toadlet for unix platforms")
	if (APPLE) 
		set (TOADLET_PLATFORM_OSX ON CACHE BOOL "Toadlet for OSX")
		if (IOS)
			set (TOADLET_PLATFORM_IOS ON CACHE BOOL "Toadlet for iOS")
		endif (IOS)
	endif (APPLE)
elseif (EMSCRIPTEN)
	set (TOADLET_PLATFORM_EMSCRIPTEN ON CACHE BOOL "Toadlet for emscripten")
endif (WIN32)


# Toadlet options
option (TOADLET_RUN_MEMCHECK "Run memory check operations on unit tests" OFF)
option (TOADLET_BUILD_DOXYGEN "Build doxygen documentation along with source files" OFF)
option (TOADLET_BUILD_DOXYGEN_ONLY "Build ONLY the doxygen documentation" OFF)


# Add the source code subdirectory
if (NOT TOADLET_BUILD_DOXYGEN_ONLY)
	add_subdirectory (source)
endif (NOT TOADLET_BUILD_DOXYGEN_ONLY)


# Add the doxygen subdirectory
if (TOADLET_BUILD_DOXYGEN OR TOADLET_BUILD_DOXYGEN_ONLY)
	add_subdirectory (doxygen)
endif (TOADLET_BUILD_DOXYGEN OR TOADLET_BUILD_DOXYGEN_ONLY)


# TODO: Remove this hack once we decide where toadlet and WindowsCE/WindowsPhone is going
# HACK: Directly alter the visual studio project files for WinCE
# This should no longer be necessary once CMake supports WinCE projects
if (UPDATE_WINCE)
	message ("Manually updating the solution projects to use WINCE_PLATFORM_SDK=${WINCE_PLATFORM_SDK}")
	set (PROJS 
		"${CMAKE_BINARY_DIR}/toadlet.sln"
		"${CMAKE_BINARY_DIR}/INSTALL.vcproj"
		"${CMAKE_BINARY_DIR}/ZERO_CHECK.vcproj"
		"${CMAKE_BINARY_DIR}/ALL_BUILD.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/egg/toadlet_egg.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/egg/toadlet_egg_s.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/flick/toadlet_flick.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/flick/toadlet_flick_s.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/flick/plugins/htcmotiondetector/toadlet_flick_htcmotiondetector.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/flick/plugins/htcmotiondetector/toadlet_flick_htcmotiondetector_s.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/flick/plugins/samsungmotiondetector/toadlet_flick_samsungmotiondetector.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/flick/plugins/samsungmotiondetector/toadlet_flick_samsungmotiondetector_s.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/hop/toadlet_hop.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/hop/toadlet_hop_s.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/knot/toadlet_knot.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/knot/toadlet_knot_s.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/pad/toadlet_pad.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/pad/toadlet_pad_s.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/peeper/toadlet_peeper.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/peeper/toadlet_peeper_s.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/peeper/plugins/d3dmrenderer/toadlet_peeper_d3dmrenderer.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/peeper/plugins/d3dmrenderer/toadlet_peeper_d3dmrenderer_s.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/peeper/plugins/glrenderer/toadlet_peeper_glrenderer.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/peeper/plugins/glrenderer/toadlet_peeper_glrenderer_s.vcproj"	
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/ribbit/toadlet_ribbit.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/ribbit/toadlet_ribbit_s.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/ribbit/plugins/win32player/toadlet_ribbit_win32player.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/ribbit/plugins/win32player/toadlet_ribbit_win32player_s.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/tadpole/toadlet_tadpole.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/tadpole/toadlet_tadpole_s.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/tadpole/plugins/hop/toadlet_tadpole_hop.vcproj"
		"${CMAKE_BINARY_DIR}/source/cpp/toadlet/tadpole/plugins/hop/toadlet_tadpole_hop_s.vcproj"
	)
	foreach (VCPROJFILE ${PROJS})
		if (EXISTS "${VCPROJFILE}")
			file (STRINGS ${VCPROJFILE} VCPROJ NEWLINE_CONSUME)
			set (VCPROJFILENEW "${VCPROJFILE}")
			file (WRITE ${VCPROJFILENEW} "")
			foreach (LINE ${VCPROJ})
				string (REPLACE "Name=\"Win32\"" "Name=\"${WINCE_PLATFORM_SDK}\"" OUTLINE "${LINE}")
				string (REPLACE "|Win32" "|${WINCE_PLATFORM_SDK}" OUTLINE "${OUTLINE}")
				file (APPEND ${VCPROJFILENEW} "${OUTLINE}")
			endforeach (LINE)
		endif (EXISTS "${VCPROJFILE}")
	endforeach(VCPROJFILE)
	message (SEND_ERROR "Done! \nThis isn't really an error; just letting you know the WinCE build files are updated. \nYes it's a hack, yes it will go away once CMake supports WinCE by default. But believe me it's more straightforward than patching the source.")
endif (UPDATE_WINCE)

