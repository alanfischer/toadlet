# Standard setup
cmake_minimum_required (VERSION 2.8)
project (toadlet CXX C Java)


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


# Custom cmake scripts
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${toadlet_SOURCE_DIR}/cmake/Modules/")


# TODO: Right now WinCE is being blocked due to the uncertain future of Toadlet and Windows Phone 7. 
if (TOADLET_PLATFORM_WINCE)
	message (FATAL_ERROR "WindowsCE builds are currently disabled given the uncertain future of native code on Windows Phone 7. You may comment out this error to attempt the build anyway. Your mileage may vary.")
endif (TOADLET_PLATFORM_WINCE)


# Platform options
if (WIN32)
	set (TOADLET_PLATFORM_WIN32 ON)
	option (TOADLET_PLATFORM_ANDROID "Build for Android devices" OFF)
	option (TOADLET_PLATFORM_BLACKBERRY "Build for Blackberry devices" OFF)
elseif (UNIX)
	set (TOADLET_PLATFORM_POSIX ON)
	option (TOADLET_PLATFORM_ANDROID "Build for Android devices" OFF)
	if (APPLE) 
		set (TOADLET_PLATFORM_OSX ON)
	endif (APPLE)
endif (WIN32)


# Doxygen options
option (TOADLET_BUILD_DOXYGEN "Build doxygen documentation along with source files" OFF)
option (TOADLET_BUILD_DOXYGEN_ONLY "Build ONLY the doxygen documentation" OFF)


# Android and Blackberry specific options
if (TOADLET_PLATFORM_ANDROID)
	# We require the android.jar
	if (NOT ANDROID_JAR)
		message (FATAL_ERROR "Required variable ANDROID_JAR not set as path/to/android.jar, please set ANDROID_JAR and rerun!")
	else (NOT ANDROID_JAR)
		message ("Using android.jar file: ${ANDROID_JAR}")
	endif (NOT ANDROID_JAR)

	# Android uses fixed point by default 
	option (TOADLET_FIXED_POINT "Use a fixed point build of toadlet" ON)

	# Add the gpp executable for java file preprocessing
	add_subdirectory (gpp)
elseif (TOADLET_PLATFORM_BLACKBERRY)
	# We require the blackberry JDE install dir
	if (NOT BLACKBERRY_JDE_DIR)
		message (FATAL_ERROR "Required variable BLACKBERRY_JDE_DIR not set as path/to/blackberryJDE, please set BLACKBERRY_JDE_DIR and rerun!")
	else (NOT BLACKBERRY_JDE_DIR)
		message ("Using BLACKBERRY_JDE_DIR directory: ${BLACKBERRY_JDE_DIR}")
	endif (NOT BLACKBERRY_JDE_DIR)

	# Android uses fixed point by default 
	option (TOADLET_FIXED_POINT "Use a fixed point build of toadlet" ON)

	# Add the gpp executable for java file preprocessing
	add_subdirectory (gpp)
else (TOADLET_PLATFORM_ANDROID)
	# All Other platforms default to using floating point math
	option (TOADLET_FIXED_POINT "Use a fixed point build of toadlet" OFF)
endif (TOADLET_PLATFORM_ANDROID)


# Add the source code subdirectory
if (NOT TOADLET_BUILD_DOXYGEN_ONLY)
	add_subdirectory (src)
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
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/egg/toadlet_egg.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/egg/toadlet_egg_s.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/flick/toadlet_flick.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/flick/toadlet_flick_s.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/flick/plugins/htcmotiondetector/toadlet_flick_htcmotiondetector.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/flick/plugins/htcmotiondetector/toadlet_flick_htcmotiondetector_s.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/flick/plugins/samsungmotiondetector/toadlet_flick_samsungmotiondetector.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/flick/plugins/samsungmotiondetector/toadlet_flick_samsungmotiondetector_s.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/hop/toadlet_hop.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/hop/toadlet_hop_s.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/knot/toadlet_knot.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/knot/toadlet_knot_s.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/pad/toadlet_pad.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/pad/toadlet_pad_s.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/peeper/toadlet_peeper.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/peeper/toadlet_peeper_s.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/peeper/plugins/d3dmrenderer/toadlet_peeper_d3dmrenderer.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/peeper/plugins/d3dmrenderer/toadlet_peeper_d3dmrenderer_s.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/peeper/plugins/glrenderer/toadlet_peeper_glrenderer.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/peeper/plugins/glrenderer/toadlet_peeper_glrenderer_s.vcproj"	
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/ribbit/toadlet_ribbit.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/ribbit/toadlet_ribbit_s.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/ribbit/plugins/win32player/toadlet_ribbit_win32player.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/ribbit/plugins/win32player/toadlet_ribbit_win32player_s.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/tadpole/toadlet_tadpole.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/tadpole/toadlet_tadpole_s.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/tadpole/plugins/hop/toadlet_tadpole_hop.vcproj"
		"${CMAKE_BINARY_DIR}/src/cpp/toadlet/tadpole/plugins/hop/toadlet_tadpole_hop_s.vcproj"
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

