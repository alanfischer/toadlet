# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_tadpole
set (TADPOLE_SRC
	ArchiveManager.cpp
	AudioBufferManager.cpp
	BufferManager.cpp
	BoundingVolumeSensor.cpp
	Engine.cpp
	Font.cpp
	FontManager.cpp
	KeyFrame.cpp
	Material.cpp
	MaterialManager.cpp
	MathFormatter.cpp
	MeshManager.cpp
	NameSensor.cpp
	Noise.cpp
	PotentiallyVisibleSensor.cpp
	RenderQueue.cpp
	ResourceManager.cpp
	Scene.cpp
	Track.cpp
	TextureManager.cpp
	Version.cpp
)

set (TADPOLE_ANIMATION_SRC
	animation/AnimationController.cpp
	animation/CameraProjectionAnimation.cpp
	animation/NodePathAnimation.cpp
	animation/SkeletonAnimation.cpp
)

set (TADPOLE_BSP_SRC
	bsp/BSP30Node.cpp
	bsp/BSP30Handler.cpp
)

set (TADPOLE_HANDLER_SRC
	handler/MMSHHandler.cpp
	handler/WADArchive.cpp
)

set (TADPOLE_HANDLER_XML_SRC
	handler/XANMHandler.cpp
	handler/XMLMeshUtilities.cpp
	handler/XMATHandler.cpp
	handler/XMSHHandler.cpp
)

set (TADPOLE_HANDLER_FREETYPE_SRC
	handler/FreeTypeHandler.cpp
)

set (TADPOLE_HANDLER_PLATFORM_OSX_SRC
	handler/platform/osx/OSXBundleArchive.mm
	handler/platform/osx/OSXFontHandler.mm
	handler/platform/osx/OSXTextureHandler.mm
)

set (TADPOLE_HANDLER_PLATFORM_WIN32_SRC
	handler/platform/win32/Win32ResourceArchive.cpp
)

set (TADPOLE_HANDLER_PLATFORM_WIN32_GDIPLUS_SRC
	handler/platform/win32/Win32FontHandler.cpp
	handler/platform/win32/Win32TextureHandler.cpp
)

set (TADPOLE_MESH_SRC
	mesh/Mesh.cpp
	mesh/Sequence.cpp
	mesh/Skeleton.cpp
)

set (TADPOLE_NODE_SRC
	node/AnaglyphCameraNode.cpp
	node/AnimationControllerNode.cpp
	node/AudioNode.cpp
	node/CameraNode.cpp
	node/CameraAlignedNode.cpp
	node/LabelNode.cpp
	node/LightNode.cpp
	node/MeshNode.cpp
	node/MeshNodeSkeleton.cpp
	node/Node.cpp
	node/ParentNode.cpp
	node/ParticleNode.cpp
	node/PartitionNode.cpp
	node/SkeletonParentNode.cpp
	node/SpriteNode.cpp
)

set (TADPOLE_STUDIO_SRC
	studio/StudioHandler.cpp
	studio/StudioModelNode.cpp
)

set (TADPOLE_TERRAIN_SRC
	terrain/TerrainNode.cpp
	terrain/TerrainPatchNode.cpp
)


# Headers
set (TADPOLE_HEADERS
	ArchiveManager.h
	AudioBufferManager.h
	Bound.h
	BufferManager.h
	BoundingVolumeSensor.h
	Collision.h
	ContextListener.h
	Engine.h
	Font.h
	FontData.h
	FontManager.h
	KeyFrame.h
	Material.h
	MaterialManager.h
	MathFormatter.h
	MeshManager.h
	NameSensor.h
	Noise.h
	PotentiallyVisibleSensor.h
	Renderable.h
	ResourceHandler.h
	ResourceHandlerData.h
	ResourceManager.h
	Scene.h
	SensorResultsListener.h
	TextureManager.h
	Traceable.h
	Track.h
	Transform.h
	Types.h
	UpdateListener.h
	VertexFormats.h
	Version.h
)

set (TADPOLE_ANIMATION_HEADERS
	animation/Animation.h
	animation/AnimationController.h
	animation/AnimationControllerFinishedListener.h
	animation/CameraProjectionAnimation.h
	animation/NodePathAnimation.h
	animation/SkeletonAnimation.h
)

set (TADPOLE_BSP_HEADERS
	bsp/BSP30Handler.h
	bsp/BSP30Map.h
	bsp/BSP30Node.h
	bsp/BSP30Types.h
)

set (TADPOLE_HANDLER_HEADERS
	handler/AudioBufferHandler.h
	handler/BMPHandler.h
	handler/GIFHandler.h
	handler/JPEGHandler.h
	handler/MMSHHandler.h
	handler/PNGHandler.h
	handler/SPRHandler.h
	handler/TGAHandler.h
	handler/TPKGHandler.h
	handler/WADArchive.h
	handler/WADHandler.h
	handler/ZIPHandler.h
)

set (TADPOLE_HANDLER_XML_HEADERS
	handler/XANMHandler.h
	handler/XMLMeshUtilities.h
	handler/XMATHandler.h
	handler/XMSHHandler.h
)

set (TADPOLE_HANDLER_FREETYPE_HEADERS
	handler/FreeTypeHandler.h
)

set (TADPOLE_HANDLER_PLATFORM_OSX_HEADERS
	handler/platform/osx/OSXBundleArchive.h
	handler/platform/osx/OSXFontHandler.h
	handler/platform/osx/OSXTextureHandler.h
) 

set (TADPOLE_HANDLER_PLATFORM_WIN32_HEADERS
	handler/platform/win32/Win32ResourceArchive.h
)

set (TADPOLE_HANDLER_PLATFORM_WIN32_GDIPLUS_HEADERS
	handler/platform/win32/StreamIStream.h
	handler/platform/win32/Win32FontHandler.h
	handler/platform/win32/Win32TextureHandler.h
)

set (TADPOLE_MESH_HEADERS
	mesh/Mesh.h
	mesh/Sequence.h
	mesh/Skeleton.h
)

set (TADPOLE_NODE_HEADERS
	node/AnaglyphCameraNode.h
	node/AnimationControllerNode.h
	node/AudioNode.h
	node/CameraNode.h
	node/CameraAlignedNode.h
	node/LabelNode.h
	node/LightNode.h
	node/MeshNode.h
	node/MeshNodeSkeleton.h
	node/Node.h
	node/NodeListener.h
	node/ParentNode.h
	node/ParticleNode.h
	node/PartitionNode.h
	node/SkeletonParentNode.h
	node/SpriteNode.h
)

set (TADPOLE_STUDIO_HEADERS
	studio/StudioHandler.h
	studio/StudioModel.h
	studio/StudioModelNode.h
	studio/StudioTypes.h
)

set (TADPOLE_TERRAIN_HEADERS
	terrain/TerrainNodeDataSource.h
	terrain/TerrainNodeListener.h
	terrain/TerrainNode.h
	terrain/TerrainPatchNode.h
)


# Bundle source
source_group ("Source Files\\\\animation" FILES ${TADPOLE_ANIMATION_SRC})
source_group ("Source Files\\\\handler" FILES ${TADPOLE_HANDLER_SRC})
source_group ("Source Files\\\\mesh" FILES ${TADPOLE_MESH_SRC})
source_group ("Source Files\\\\node" FILES ${TADPOLE_NODE_SRC})
source_group ("Source Files\\\\terrain" FILES ${TADPOLE_TERRAIN_SRC})


# Bundle headers
source_group ("Header Files\\\\animation" FILES ${TADPOLE_ANIMATION_HEADERS})
source_group ("Header Files\\\\handler" FILES ${TADPOLE_HANDLER_HEADERS})
source_group ("Header Files\\\\mesh" FILES ${TADPOLE_MESH_HEADERS})
source_group ("Header Files\\\\node" FILES ${TADPOLE_NODE_HEADERS})
source_group ("Header Files\\\\terrain" FILES ${TADPOLE_TERRAIN_HEADERS})


# Source and headers
set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_ANIMATION_SRC} ${TADPOLE_HANDLER_SRC} ${TADPOLE_MESH_SRC} ${TADPOLE_NODE_SRC} ${TADPOLE_TERRAIN_SRC})
set (TADPOLE_HEADERS ${TADPOLE_HEADERS} ${TADPOLE_ANIMATION_HEADERS} ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_MESH_HEADERS} ${TADPOLE_NODE_HEADERS} ${TADPOLE_TERRAIN_HEADERS})


if (NOT TOADLET_FIXED_POINT)
	source_group ("Source Files\\\\bsp" FILES ${TADPOLE_BSP_SRC})
	source_group ("Header Files\\\\bsp" FILES ${TADPOLE_BSP_HEADERS})
	source_group ("Source Files\\\\studio" FILES ${TADPOLE_STUDIO_SRC})
	source_group ("Header Files\\\\studio" FILES ${TADPOLE_STUDIO_HEADERS})
	set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_BSP_SRC} ${TADPOLE_STUDIO_SRC})
	set (TADPOLE_HEADERS ${TADPOLE_HEADERS} ${TADPOLE_BSP_HEADERS} ${TADPOLE_STUDIO_HEADERS})
endif (NOT TOADLET_FIXED_POINT)

# Setup dependecy specific files and libraries
# Freetype is not required on OSX
if (NOT TOADLET_PLATFORM_OSX)
	find_package (Freetype)
	if (FREETYPE_FOUND)
		set (TOADLET_HAS_FREETYPE ON PARENT_SCOPE)
		include_directories (${FREETYPE_INCLUDE_DIRS})
		source_group ("Source Files\\\\handler" FILES ${TADPOLE_HANDLER_FREETYPE_SRC})
		source_group ("Header Files\\\\handler" FILES ${TADPOLE_HANDLER_FREETYPE_HEADERS})
		set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_HANDLER_FREETYPE_SRC})
		set (TADPOLE_HEADERS ${TADPOLE_HEADERS} ${TADPOLE_HANDLER_FREETYPE_HEADERS})

		# Libraries
		set (TADPOLE_LIBS ${FREETYPE_LIBRARIES})
		add_library (libfreetype_s STATIC IMPORTED)
		if (TOADLET_PLATFORM_POSIX)
			string (REPLACE ".so" ".a" FREETYPE_A ${FREETYPE_LIBRARY})
			if (EXISTS ${FREETYPE_A})
				set (FREETYPE_LIBRARY ${FREETYPE_A})
			endif (EXISTS ${FREETYPE_A})
		endif (TOADLET_PLATFORM_POSIX)
		set_property (TARGET libfreetype_s PROPERTY IMPORTED_LOCATION ${FREETYPE_LIBRARY})
		set (TADPOLE_DEFS_S TOADLET_LIBFREETYPE_NAME="${FREETYPE_LIBRARY}")
		set (TADPOLE_LIBS_S libfreetype_s)
	endif (FREETYPE_FOUND)
endif (NOT TOADLET_PLATFORM_OSX)

find_package (MXML)
if (MXML_FOUND)
	# Files
	set (TOADLET_HAS_MXML ON PARENT_SCOPE)
	include_directories (${MXML_INCLUDE_DIR})
	source_group ("Source Files\\\\handler" FILES ${TADPOLE_HANDLER_XML_SRC})
	source_group ("Header Files\\\\handler" FILES ${TADPOLE_HANDLER_XML_HEADERS})
	set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_HANDLER_XML_SRC})
	set (TADPOLE_HEADERS ${TADPOLE_HEADERS} ${TADPOLE_HANDLER_XML_HEADERS})

	# Libraries
	set (TADPOLE_LIBS ${TADPOLE_LIBS} ${MXML_LIBRARY})
	add_library (libmxml_s STATIC IMPORTED)
	if (TOADLET_PLATFORM_POSIX)
		string (REPLACE ".so" ".a" MXML_A ${MXML_LIBRARY})
		if (EXISTS ${MXML_A})
			set (MXML_LIBRARY ${MXML_A})
		endif (EXISTS ${MXML_A})
	endif (TOADLET_PLATFORM_POSIX)
	set_property (TARGET libmxml_s PROPERTY IMPORTED_LOCATION ${MXML_LIBRARY})
	set (TADPOLE_DEFS_S ${TADPOLE_DEFS_S} TOADLET_LIBMXML_NAME="${MXML_LIBRARY}")
	set (TADPOLE_LIBS_S ${TADPOLE_LIBS_S} libmxml_s)
endif (MXML_FOUND)

# Platform specific files
if (TOADLET_PLATFORM_OSX)
	source_group ("Source Files\\\\handler\\\\platform\\\\osx" FILES ${TADPOLE_HANDLER_PLATFORM_OSX_SRC})
	source_group ("Header Files\\\\handler\\\\platform\\\\osx" FILES ${TADPOLE_HANDLER_PLATFORM_OSX_HEADERS})
	set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_HANDLER_PLATFORM_OSX_SRC})
	set (TADPOLE_HEADERS ${TADPOLE_HEADERS} ${TADPOLE_HANDLER_PLATFORM_OSX_HEADERS})
	if (TOADLET_PLATFORM_IPHONE)
		find_library (UIKIT_LIBS UIKit)
		set (TADPOLE_LIBS_S ${TADPOLE_LIBS_S} ${UIKIT_LIBS})
		set (TADPOLE_LIBS ${TADPOLE_LIBS} ${UIKIT_LIBS})
	else (TOADLET_PLATFORM_IPHONE)
		find_library (APPKIT_LIBS AppKit)
		set (TADPOLE_LIBS_S ${TADPOLE_LIBS_S} ${APPKIT_LIBS})	
		set (TADPOLE_LIBS ${TADPOLE_LIBS} ${APPKIT_LIBS})	
	endif (TOADLET_PLATFORM_IPHONE)
elseif (TOADLET_PLATFORM_WIN32)
	find_package (GDIPLUS)
	if (GDIPLUS_FOUND)
		set (TOADLET_HAS_GDIPLUS ON PARENT_SCOPE)
		include_directories (${GDIPLUS_INCLUDE_PATH})
		source_group ("Source Files\\\\handler\\\\platform\\\\win32" FILES ${TADPOLE_HANDLER_PLATFORM_WIN32_GDIPLUS_SRC})
		source_group ("Header Files\\\\handler\\\\platform\\\\win32" FILES ${TADPOLE_HANDLER_PLATFORM_WIN32_GDIPLUS_HEADERS})
		set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_HANDLER_PLATFORM_WIN32_GDIPLUS_SRC})
		set (TADPOLE_HEADERS ${TADPOLE_HEADERS} ${TADPOLE_HANDLER_PLATFORM_WIN32_GDIPLUS_HEADERS})
	endif (GDIPLUS_FOUND)
	source_group ("Source Files\\\\handler\\\\platform\\\\win32" FILES ${TADPOLE_HANDLER_PLATFORM_WIN32_SRC})
	source_group ("Header Files\\\\handler\\\\platform\\\\win32" FILES ${TADPOLE_HANDLER_PLATFORM_WIN32_HEADERS})
	set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_HANDLER_PLATFORM_WIN32_SRC})
	set (TADPOLE_HEADERS ${TADPOLE_HEADERS} ${TADPOLE_HANDLER_PLATFORM_WIN32_HEADERS})
endif (TOADLET_PLATFORM_OSX)


# Dynamic library
if (NOT TOADLET_PLATFORM_IPHONE)
	add_library (toadlet_tadpole SHARED ${TADPOLE_SRC} ${TADPOLE_HEADERS})
	set_target_properties (toadlet_tadpole PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_tadpole toadlet_egg toadlet_peeper toadlet_ribbit ${TADPOLE_LIBS})
	install (TARGETS toadlet_tadpole DESTINATION lib)
endif (NOT TOADLET_PLATFORM_IPHONE)


# Static library
add_library (toadlet_tadpole_s STATIC ${TADPOLE_SRC} ${TADPOLE_HEADERS})
set_target_properties (toadlet_tadpole_s PROPERTIES ${STATIC_LIB_PROPS})
set_property (TARGET toadlet_tadpole_s APPEND PROPERTY COMPILE_DEFINITIONS ${TADPOLE_DEFS_S})
target_link_libraries (toadlet_tadpole_s toadlet_egg_s toadlet_peeper_s toadlet_ribbit_s ${TADPOLE_LIBS_S})
install (TARGETS toadlet_tadpole_s DESTINATION lib)


# Add plugins
add_subdirectory (plugins/hop)

