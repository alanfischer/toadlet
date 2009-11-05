# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_egg
set (EGG_SRC
	Error.cpp
	Exception.cpp
	Logger.cpp
	Profile.cpp
	Random.cpp
	String.cpp
	Version.cpp
)

# Specific Image handlers are separated out here due to different ones being built for different platforms
# Yes it's ugly, no I don't have a better idea right now
set (EGG_IMAGE_SRC
	image/Image.cpp
	image/ImageUtilities.cpp
)
set (EGG_IMAGE_BMP_SRC
	image/BMPHandler.cpp
)
set (EGG_IMAGE_GIF_SRC
	image/GIFHandler.cpp
)
set (EGG_IMAGE_JPG_SRC
	image/JPEGHandler.cpp
)
set (EGG_IMAGE_PNG_SRC
	image/PNGHandler.cpp
)
set (EGG_IMAGE_RGB_SRC
	image/RGBHandler.cpp	
)
set (EGG_IMAGE_SPR_SRC
	image/SPRHandler.cpp
)

set (EGG_IO_SRC
	io/DataInputStream.cpp
	io/DataOutputStream.cpp
	io/FileInputStream.cpp
	io/FileOutputStream.cpp
	io/MemoryInputStream.cpp
	io/MemoryOutputStream.cpp
	io/PushbackInputStream.cpp
	io/StringInputStream.cpp
	io/StringOutputStream.cpp
)

set (EGG_MATH_SRC
	math/AABox.cpp
	math/Capsule.cpp
	math/Math.cpp
	math/Matrix3x3.cpp
	math/Matrix4x4.cpp
	math/Plane.cpp
)

set (EGG_MATHFIXED_SRC
	mathfixed/AABox_fixed.cpp
	mathfixed/Capsule_fixed.cpp
	mathfixed/Math_fixed.cpp
	mathfixed/Matrix3x3_fixed.cpp
	mathfixed/Matrix4x4_fixed.cpp
	mathfixed/Plane_fixed.cpp
)

set (EGG_NET_SRC
	net/Socket.cpp
)

set (EGG_NET_PLATFORM_WIN32_SRC
	net/platform/win32/Win32BluetoothClient.cpp
	net/platform/win32/Win32BluetoothServer.cpp
	net/platform/win32/Win32BluetoothServerQuery.cpp
)

set (EGG_NET_PLATFORM_WINCE_SRC
	net/platform/wince/WinCEConnectionManager.cpp
)

set (EGG_PLATFORM_POSIX_SRC
	platform/posix/PosixDynamicLibrary.cpp
	platform/posix/PosixMutex.cpp
	platform/posix/PosixSystem.cpp
	platform/posix/PosixThread.cpp
	platform/posix/PosixWaitCondition.cpp
)

set (EGG_PLATFORM_WIN32_SRC
	platform/win32/Win32DynamicLibrary.cpp
	platform/win32/Win32Mutex.cpp
	platform/win32/Win32System.cpp
	platform/win32/Win32Thread.cpp
	platform/win32/Win32WaitCondition.cpp
)


# Headers
set (EGG_HEADERS
	Collection.h
	Categories.h
	DynamicLibrary.h
	EndianConversion.h
	Error.h
	Event.h
	EventFactory.h
	Exception.h
	Extents.h
	IntrusivePointer.h
	Logger.h
	LoggerListener.h
	MathConversion.h
	Map.h
	Mutex.h
	PointerCounter.h
	PointerQueue.h
	Profile.h
	Random.h
	Resource.h
	Runnable.h
	SharedPointer.h
	String.h
	System.h
	Thread.h
	WaitCondition.h
	WeakPointer.h
	Version.h
)

# Specific Image handlers are separated out here due to different ones being built for different platforms
# Yes it's ugly, no I don't have a better idea right now
set (EGG_IMAGE_HEADERS
	image/Image.h
	image/ImageDefinitions.h
	image/ImageFormatConversion.h
	image/ImageUtilities.h
)
set (EGG_IMAGE_BMP_HEADERS
	image/BMPHandler.h
)
set (EGG_IMAGE_GIF_HEADERS
	image/GIFHandler.h
)
set (EGG_IMAGE_JPG_HEADERS
	image/JPEGHandler.h
)
set (EGG_IMAGE_PNG_HEADERS
	image/PNGHandler.h
)
set (EGG_IMAGE_RGB_HEADERS
	image/RGBHandler.h	
)
set (EGG_IMAGE_SPR_HEADERS
	image/SPRHandler.h	
)

set (EGG_IO_HEADERS
	io/DataInputStream.h
	io/DataOutputStream.h
	io/FileInputStream.h
	io/FileOutputStream.h
	io/InputStream.h
	io/InputStreamFactory.h
	io/MemoryInputStream.h
	io/MemoryOutputStream.h
	io/OutputStream.h
	io/PushbackInputStream.h
	io/StringInputStream.h
	io/StringOutputStream.h
)

set (EGG_MATH_HEADERS
	math/AABox.h
	math/BaseMath.h
	math/Capsule.h
	math/ConvexSolid.h
	math/EulerAngle.h
	math/Math.h
	math/Matrix3x3.h
	math/Matrix4x4.h
	math/Plane.h
	math/Quaternion.h
	math/Segment.h
	math/Sphere.h
	math/Vector2.h
	math/Vector3.h
	math/Vector4.h
)

set (EGG_MATHFIXED_HEADERS
	mathfixed/AABox.h
	mathfixed/BaseMath.h
	mathfixed/Capsule.h
	mathfixed/ConvexSolid.h
	mathfixed/EulerAngle.h
	mathfixed/Math.h
	mathfixed/Matrix3x3.h
	mathfixed/Matrix4x4.h
	mathfixed/Plane.h
	mathfixed/Quaternion.h
	mathfixed/Segment.h
	mathfixed/Sphere.h
	mathfixed/Vector2.h
	mathfixed/Vector3.h
	mathfixed/Vector4.h
)

set (EGG_NET_HEADERS
	net/BluetoothAddress.h
	net/BluetoothClient.h
	net/BluetoothServer.h
	net/BluetoothServerQuery.h
	net/ConnectionManager.h
	net/Socket.h
)

set (EGG_NET_PLATFORM_WIN32_HEADERS
	net/platform/win32/Win32BluetoothClient.h
	net/platform/win32/Win32BluetoothIncludes.h
	net/platform/win32/Win32BluetoothServer.h
	net/platform/win32/Win32BluetoothServerQuery.h
)

set (EGG_NET_PLATFORM_WINCE_HEADERS
	net/platform/wince/WinCEConnectionManager.h
)

set (EGG_PLATFORM_POSIX_HEADERS
	platform/posix/PosixDynamicLibrary.h
	platform/posix/PosixMutex.h
	platform/posix/PosixSystem.h
	platform/posix/PosixThread.h
	platform/posix/PosixWaitCondition.h
)

set (EGG_PLATFORM_WIN32_HEADERS
	platform/win32/Win32DynamicLibrary.h
	platform/win32/Win32Mutex.h
	platform/win32/Win32System.h
	platform/win32/Win32Thread.h
	platform/win32/Win32WaitCondition.h
)


# Bundle source
source_group ("Source Files\\\\image" FILES ${EGG_IMAGE_SRC} ${EGG_IMAGE_BMP_SRC} ${EGG_IMAGE_RGB_SRC} ${EGG_IMAGE_SPR_SRC})
source_group ("Source Files\\\\io" FILES ${EGG_IO_SRC})
source_group ("Source Files\\\\math" FILES ${EGG_MATH_SRC})
source_group ("Source Files\\\\mathfixed" FILES ${EGG_MATHFIXED_SRC})
source_group ("Source Files\\\\net" FILES ${EGG_NET_SRC})


# Bundle headers
source_group ("Header Files\\\\image" FILES ${EGG_IMAGE_HEADERS} ${EGG_IMAGE_BMP_HEADERS} ${EGG_IMAGE_RGB_HEADERS} ${EGG_IMAGE_SPR_HEADERS})
source_group ("Header Files\\\\io" FILES ${EGG_IO_HEADERS})
source_group ("Header Files\\\\math" FILES ${EGG_MATH_HEADERS})
source_group ("Header Files\\\\mathfixed" FILES ${EGG_MATHFIXED_HEADERS})
source_group ("Header Files\\\\net" FILES ${EGG_NET_HEADERS})


# Sources and headers
set (EGG_SRC ${EGG_SRC} ${EGG_IMAGE_SRC} ${EGG_IO_SRC} ${EGG_MATH_SRC} ${EGG_MATHFIXED_SRC} ${EGG_NET_SRC} ${EGG_IMAGE_BMP_SRC} ${EGG_IMAGE_RGB_SRC} ${EGG_IMAGE_SPR_SRC})
set (EGG_HEADERS ${EGG_HEADERS} ${EGG_IMAGE_HEADERS} ${EGG_IO_HEADERS} ${EGG_MATH_HEADERS} ${EGG_MATHFIXED_HEADERS} ${EGG_NET_HEADERS} ${EGG_IMAGE_BMP_HEADERS} ${EGG_IMAGE_RGB_HEADERS} ${EGG_IMAGE_SPR_HEADERS})


# Platform specific files
if (TOADLET_PLATFORM_WIN32)	
	source_group ("Source Files\\\\platform\\\\win32" FILES ${EGG_PLATFORM_WIN32_SRC})
	source_group ("Header Files\\\\platform\\\\win32" FILES ${EGG_PLATFORM_WIN32_HEADERS})
	set (EGG_SRC ${EGG_SRC} ${EGG_PLATFORM_WIN32_SRC})
	set (EGG_HEADERS ${EGG_HEADERS} ${EGG_PLATFORM_WIN32_HEADERS})
	if (TOADLET_USE_BLUETOOTH)
		find_package (WS2BTH)
	endif (TOADLET_USE_BLUETOOTH)
	if (WS2BTH_FOUND)
		source_group ("Source Files\\\\net\\\\platform\\\\win32" FILES ${EGG_NET_PLATFORM_WIN32_SRC})
		source_group ("Header Files\\\\net\\\\platform\\\\win32" FILES ${EGG_NET_PLATFORM_WIN32_HEADERS})
		set (EGG_SRC ${EGG_SRC} ${EGG_NET_PLATFORM_WIN32_SRC})
		set (EGG_HEADERS ${EGG_HEADERS} ${EGG_NET_PLATFORM_WIN32_HEADERS})
	endif (WS2BTH_FOUND)
	if (TOADLET_PLATFORM_WINCE)
		source_group ("Source Files\\\\net\\\\platform\\\\wince" FILES ${EGG_NET_PLATFORM_WINCE_SRC})
		source_group ("Header Files\\\\net\\\\platform\\\\wince" FILES ${EGG_NET_PLATFORM_WINCE_HEADERS})
		set (EGG_SRC ${EGG_SRC} ${EGG_NET_PLATFORM_WINCE_SRC})
		set (EGG_HEADERS ${EGG_HEADERS} ${EGG_NET_PLATFORM_WINCE_HEADERS})
	endif (TOADLET_PLATFORM_WINCE)
elseif (TOADLET_PLATFORM_POSIX) 
	source_group ("Source Files\\\\platform\\\\posix" FILES ${EGG_PLATFORM_POSIX_SRC})
	source_group ("Header Files\\\\platform\\\\posix" FILES ${EGG_PLATFORM_POSIX_HEADERS})
	set (EGG_SRC ${EGG_SRC} ${EGG_PLATFORM_POSIX_SRC})
	set (EGG_HEADERS ${EGG_HEADERS} ${EGG_PLATFORM_POSIX_HEADERS})
	set (EGG_LIBS ${EGG_LIBS} pthread)
endif (TOADLET_PLATFORM_WIN32)


# Dependencies for toadlet_egg
if (NOT TOADLET_PLATFORM_OSX)
	find_package (GIF)
	find_package (JPEG)
	find_package (ZLIB)
	find_package (PNG)
endif (NOT TOADLET_PLATFORM_OSX)


# Setup dependecy specific  files and libraries
if (GIF_FOUND)
	# Files
	set (TOADLET_HAS_GIF ON PARENT_SCOPE)
	include_directories (${GIF_INCLUDE_DIR})
	source_group ("Source Files\\\\image" FILES ${EGG_IMAGE_GIF_SRC})
	source_group ("Header Files\\\\image" FILES ${EGG_IMAGE_GIF_HEADERS})
	set (EGG_SRC ${EGG_SRC} ${EGG_IMAGE_GIF_SRC})
	set (EGG_HEADERS ${EGG_HEADERS} ${EGG_IMAGE_GIF_HEADERS})
	
	# Libraries
	set (EGG_LIBS ${EGG_LIBS} ${GIF_LIBRARY})
	add_library (libgif_s STATIC IMPORTED)
	if (TOADLET_DEPENDENCY_EXTENSION_NAMES)
		string (REPLACE ".lib" "_s.lib" GIF_LIBRARY ${GIF_LIBRARY})
	endif (TOADLET_DEPENDENCY_EXTENSION_NAMES)
	set_property (TARGET libgif_s PROPERTY IMPORTED_LOCATION ${GIF_LIBRARY})
	set (EGG_DEFS_S ${EGG_DEFS_S} TOADLET_LIBGIF_NAME="${GIF_LIBRARY}")
	set (EGG_LIBS_S ${EGG_LIBS_S} libgif_s)
endif (GIF_FOUND)

if (JPEG_FOUND)
	# Files 
	set (TOADLET_HAS_JPEG ON PARENT_SCOPE)
	include_directories (${JPEG_INCLUDE_DIR})
	source_group ("Source Files\\\\image" FILES ${EGG_IMAGE_JPG_SRC})
	source_group ("Header Files\\\\image" FILES ${EGG_IMAGE_JPG_HEADERS})
	set (EGG_SRC ${EGG_SRC} ${EGG_IMAGE_JPG_SRC})
	set (EGG_HEADERS ${EGG_HEADERS} ${EGG_IMAGE_JPG_HEADERS})
	
	# Libraries
	set (EGG_LIBS ${EGG_LIBS} ${JPEG_LIBRARY})
	add_library (libjpeg_s STATIC IMPORTED)
	if (TOADLET_DEPENDENCY_EXTENSION_NAMES)
		string (REPLACE ".lib" "_s.lib" JPEG_LIBRARY ${JPEG_LIBRARY})
	endif (TOADLET_DEPENDENCY_EXTENSION_NAMES)
	set_property (TARGET libjpeg_s PROPERTY IMPORTED_LOCATION ${JPEG_LIBRARY})
	set (EGG_DEFS_S ${EGG_DEFS_S} TOADLET_LIBJPEG_NAME="${JPEG_LIBRARY}")
	set (EGG_LIBS_S ${EGG_LIBS_S} libjpeg_s)
endif (JPEG_FOUND)

if (ZLIB_FOUND)
	# Files
	set (TOADLET_HAS_ZLIB ON PARENT_SCOPE)
	include_directories (${ZLIB_INCLUDE_DIR})
	
	# Libraries
	set (EGG_LIBS ${EGG_LIBS} ${ZLIB_LIBRARY})
	add_library (zlib_s STATIC IMPORTED)
	if (TOADLET_DEPENDENCY_EXTENSION_NAMES)
		string (REPLACE ".lib" "_s.lib" ZLIB_LIBRARY ${ZLIB_LIBRARY})
	endif (TOADLET_DEPENDENCY_EXTENSION_NAMES)
	set_property (TARGET zlib_s PROPERTY IMPORTED_LOCATION ${ZLIB_LIBRARY})
	set (EGG_DEFS_S ${EGG_DEFS_S} TOADLET_ZLIB_NAME="${ZLIB_LIBRARY}")
	set (EGG_LIBS_S ${EGG_LIBS_S} zlib_s)
endif (ZLIB_FOUND)

if (PNG_FOUND)
	# Files
	set (TOADLET_HAS_PNG ON PARENT_SCOPE)
	include_directories (${PNG_INCLUDE_DIR})
	source_group ("Source Files\\\\image" FILES ${EGG_IMAGE_PNG_SRC})
	source_group ("Header Files\\\\image" FILES ${EGG_IMAGE_PNG_HEADERS})
	set (EGG_SRC ${EGG_SRC} ${EGG_IMAGE_PNG_SRC})
	set (EGG_HEADERS ${EGG_HEADERS} ${EGG_IMAGE_PNG_HEADERS})
	
	# Libraries
	set (EGG_LIBS ${EGG_LIBS} ${PNG_LIBRARY})
	add_library (libpng_s STATIC IMPORTED)
	if (TOADLET_DEPENDENCY_EXTENSION_NAMES)
		string (REPLACE ".lib" "_s.lib" PNG_LIBRARY ${PNG_LIBRARY})
	endif (TOADLET_DEPENDENCY_EXTENSION_NAMES)
	set_property (TARGET libpng_s PROPERTY IMPORTED_LOCATION ${PNG_LIBRARY})
	set (EGG_DEFS_S ${EGG_DEFS_S} TOADLET_LIBPNG_NAME="${PNG_LIBRARY}")
	set (EGG_LIBS_S ${EGG_LIBS_S} libpng_s)
endif (PNG_FOUND)


#  Dynamic library
if (NOT TOADLET_PLATFORM_IPHONE)
	add_library (toadlet_egg SHARED ${EGG_SRC} ${EGG_HEADERS})
	set_target_properties (toadlet_egg PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_egg ${EGG_LIBS})
	install (TARGETS toadlet_egg DESTINATION lib)
endif (NOT TOADLET_PLATFORM_IPHONE)


# Static library
add_library (toadlet_egg_s STATIC ${EGG_SRC} ${EGG_HEADERS})
set_target_properties (toadlet_egg_s PROPERTIES ${STATIC_LIB_PROPS})
set_property (TARGET toadlet_egg_s APPEND PROPERTY COMPILE_DEFINITIONS ${EGG_DEFS_S})
target_link_libraries (toadlet_egg_s ${EGG_LIBS_S})
install (TARGETS toadlet_egg_s DESTINATION lib)

