# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
install (FILES  ${CMAKE_CURRENT_BINARY_DIR}/Version.h DESTINATION include/toadlet/tadpole)


# Source files for toadlet_tadpole
set (TADPOLE_SRC
	ArchiveManager.cpp
	AudioBufferManager.cpp
	BufferManager.cpp
	DecalShadowSceneRenderer.cpp
	Engine.cpp
	Font.cpp
	FontManager.cpp
	MaterialManager.cpp
	MathFormatter.cpp
	Mesh.cpp
	MeshManager.cpp
	NodeManager.cpp
	Noise.cpp
	RenderableSet.cpp
	ResourceManager.cpp
	Scene.cpp
	SceneRenderer.cpp
	ShaderManager.cpp
	ShadowMappedSceneRenderer.cpp
	Skeleton.cpp
	TransformKeyFrame.cpp
	TransformSequence.cpp
	TransformTrack.cpp
	TextureManager.cpp
	Version.cpp
)

set (TADPOLE_ANIMATION_SRC
	animation/Controller.cpp
	animation/CameraProjectionAnimation.cpp
	animation/NodePathAnimation.cpp
	animation/SkeletonAnimation.cpp
)

set (TADPOLE_BSP_SRC
	bsp/BSP30Map.cpp
	bsp/BSP30Node.cpp
	bsp/BSP30Handler.cpp
)

set (TADPOLE_CREATOR_SRC
	creator/DiffuseMaterialCreator.cpp
	creator/SkyBoxMaterialCreator.cpp
	creator/AABoxMeshCreator.cpp
	creator/GridMeshCreator.cpp
	creator/GeoSphereMeshCreator.cpp
	creator/SkyBoxMeshCreator.cpp
	creator/SkyDomeMeshCreator.cpp
	creator/SphereMeshCreator.cpp
	creator/TorusMeshCreator.cpp
)

set (TADPOLE_HANDLER_SRC
	handler/BMPHandler.cpp
	handler/DDSHandler.cpp
	handler/TGAHandler.cpp
	handler/TMSHHandler.cpp
	handler/TPKGArchive.cpp
	handler/WADArchive.cpp
	handler/WaveDecoder.cpp
)

set (TADPOLE_HANDLER_XML_SRC
	handler/XANMHandler.cpp
	handler/XMLMeshUtilities.cpp
	handler/XMATHandler.cpp
	handler/XMSHHandler.cpp
)

set (TADPOLE_HANDLER_GIF_SRC
	handler/GIFHandler.cpp
)

set (TADPOLE_HANDLER_JPEG_SRC
	handler/JPEGHandler.cpp
)

set (TADPOLE_HANDLER_PNG_SRC
	handler/PNGHandler.cpp
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

set (TADPOLE_HANDLER_OGGVORBISDECODER_SRC
	handler/OggVorbisDecoder.cpp
)

set (TADPOLE_HANDLER_SIDDECODER_SRC
	handler/SIDDecoder.cpp
)

set (TADPOLE_HANDLER_ZIP_SRC
	handler/ZIPArchive.cpp
	handler/ZIPStream.cpp
)

set (TADPOLE_MATERIAL_SRC
	material/Material.cpp
	material/RenderPass.cpp
	material/RenderPath.cpp
	material/RenderVariableSet.cpp
)

set (TADPOLE_NODE_SRC
	node/AnaglyphCameraNode.cpp
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
	node/SpriteNode.cpp
)

set (TADPOLE_SENSOR_SRC
	sensor/BoundingVolumeSensor.cpp
	sensor/InterfaceSensor.cpp
	sensor/NameSensor.cpp
	sensor/PotentiallyVisibleSensor.cpp
	sensor/Sensor.cpp
	sensor/SensorResults.cpp
	sensor/TypeSensor.cpp
)

set (TADPOLE_STUDIO_SRC
	studio/SpriteHandler.cpp
	studio/SpriteModelNode.cpp
	studio/StudioHandler.cpp
	studio/StudioModelNode.cpp
)

set (TADPOLE_TERRAIN_SRC
	terrain/TerrainNode.cpp
	terrain/TerrainPatchNode.cpp
)


# Headers
set (TADPOLE_HEADERS
	Attachable.h
	ArchiveManager.h
	AudioBufferManager.h
	Bound.h
	BufferManager.h
	Collision.h
	Colors.h
	ContextListener.h
	DecalShadowSceneRenderer.h
	Engine.h
	Font.h
	FontData.h
	FontManager.h
	MaterialManager.h
	MathFormatter.h
	Mesh.h
	MeshManager.h
	NodeManager.h
	Noise.h
	ProgressListener.h
	Renderable.h
	RenderListener.h
	ResourceCreator.h
	ResourceData.h
	ResourceManager.h
	ResourceStreamer.h
	RenderableSet.h
	Scene.h
	SceneRenderer.h
	ShaderManager.h
	ShadowMappedSceneRenderer.h
	Skeleton.h
	TextureManager.h
	Traceable.h
	Transform.h
	Transformable.h
	TransformKeyFrame.h
	TransformSequence.h
	TransformTrack.h
	Types.h
	UpdateListener.h
	VertexFormats.h
	Version.h
	Visible.h
)

set (TADPOLE_ANIMATION_HEADERS
	animation/Animatable.h
	animation/Controller.h
	animation/ControllerFinishedListener.h
	animation/CameraProjectionAnimation.h
	animation/MaterialLightEffectAnimation.h
	animation/NodePathAnimation.h
	animation/SkeletonAnimation.h
)

set (TADPOLE_BSP_HEADERS
	bsp/BSP30Handler.h
	bsp/BSP30Map.h
	bsp/BSP30Node.h
	bsp/BSP30Types.h
)

set (TADPOLE_CREATOR_HEADERS
	creator/DiffuseMaterialCreator.h
	creator/SkyBoxMaterialCreator.h
	creator/AABoxMeshCreator.h
	creator/GridMeshCreator.h
	creator/GeoSphereMeshCreator.h
	creator/SkyBoxMeshCreator.h
	creator/SkyDomeMeshCreator.h
	creator/TorusMeshCreator.h
	creator/SphereMeshCreator.h
)

set (TADPOLE_HANDLER_HEADERS
	handler/AudioBufferHandler.h
	handler/BMPHandler.h
	handler/TMSHHandler.h
	handler/PNGHandler.h
	handler/SPRHandler.h
	handler/TGAHandler.h
	handler/TPKGArchive.h
	handler/TPKGHandler.h
	handler/WADArchive.h
	handler/WADHandler.h
	handler/WaveDecoder.h
)

set (TADPOLE_HANDLER_XML_HEADERS
	handler/XANMHandler.h
	handler/XMLMeshUtilities.h
	handler/XMATHandler.h
	handler/XMSHHandler.h
)

set (TADPOLE_HANDLER_GIF_HEADERS
	handler/GIFHandler.h
)

set (TADPOLE_HANDLER_JPEG_HEADERS
	handler/JPEGHandler.h
)

set (TADPOLE_HANDLER_PNG_HEADERS
	handler/PNGHandler.h
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

set (TADPOLE_HANDLER_OGGVORBISDECODER_HEADERS
	handler/OggVorbisDecoder.h
)

set (TADPOLE_HANDLER_SIDDECODER_HEADERS
	handler/SIDDecoder.h
)

set (TADPOLE_HANDLER_ZIP_HEADERS
	handler/ZIPArchive.h
	handler/ZIPStream.h
)

set (TADPOLE_MATERIAL_HEADERS
	material/Material.h
	material/RenderPass.h
	material/RenderPath.h
	material/RenderVariable.h
	material/RenderVariables.h
	material/RenderVariableSet.h
	material/SceneParameters.h
)

set (TADPOLE_NODE_HEADERS
	node/AnaglyphCameraNode.h
	node/AudioNode.h
	node/CameraNode.h
	node/CameraAlignedNode.h
	node/LabelNode.h
	node/LightNode.h
	node/MeshNode.h
	node/MeshNodeSkeleton.h
	node/Node.h
	node/NodeAttacher.h
	node/NodeListener.h
	node/NodeResource.h
	node/ParentNode.h
	node/ParticleNode.h
	node/PartitionNode.h
	node/SpriteNode.h
)

set (TADPOLE_SENSOR_HEADERS
	sensor/BoundingVolumeSensor.h
	sensor/InterfaceSensor.h
	sensor/NameSensor.h
	sensor/PotentiallyVisibleSensor.h
	sensor/Sensor.h
	sensor/SensorResults.h
	sensor/SensorResultsListener.h
	sensor/TypeSensor.h
)

set (TADPOLE_STUDIO_HEADERS
	studio/SpriteHandler.h
	studio/SpriteModel.h
	studio/SpriteModelNode.h
	studio/SpriteTypes.h
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
source_group ("Source Files\\\\creator" FILES ${TADPOLE_CREATOR_SRC})
source_group ("Source Files\\\\material" FILES ${TADPOLE_MATERIAL_SRC})
source_group ("Source Files\\\\node" FILES ${TADPOLE_NODE_SRC})
source_group ("Source Files\\\\sensor" FILES ${TADPOLE_SENSOR_SRC})
source_group ("Source Files\\\\terrain" FILES ${TADPOLE_TERRAIN_SRC})


# Bundle headers
source_group ("Header Files\\\\animation" FILES ${TADPOLE_ANIMATION_HEADERS})
source_group ("Header Files\\\\creator" FILES ${TADPOLE_CREATOR_HEADERS})
source_group ("Header Files\\\\material" FILES ${TADPOLE_MATERIAL_HEADERS})
source_group ("Header Files\\\\node" FILES ${TADPOLE_NODE_HEADERS})
source_group ("Header Files\\\\sensor" FILES ${TADPOLE_SENSOR_HEADERS})
source_group ("Header Files\\\\terrain" FILES ${TADPOLE_TERRAIN_HEADERS})

set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_ANIMATION_SRC} ${TADPOLE_CREATOR_SRC} ${TADPOLE_HANDLER_SRC} ${TADPOLE_MATERIAL_SRC} ${TADPOLE_NODE_SRC} ${TADPOLE_SENSOR_SRC} ${TADPOLE_TERRAIN_SRC})

# TODO: The handlers/ subdirectory is going to become a plugin system, and so they are NOT included in the aggregate header files
# This is true for all handler/ subdirectory files
set (TADPOLE_HEADERS ${TADPOLE_HEADERS} ${TADPOLE_ANIMATION_HEADERS} ${TADPOLE_CREATOR_HEADERS} ${TADPOLE_MATERIAL_HEADERS} ${TADPOLE_NODE_HEADERS} ${TADPOLE_SENSOR_HEADERS} ${TADPOLE_TERRAIN_HEADERS})
set (TADPOLE_NAMESPACES ${TADPOLE_NAMESPACES} animation creator material node sensor terrain)

# Give the headers and namespaces parent scope for inclusion into the master tadpole.h file
set (TADPOLE_HEADERS ${TADPOLE_HEADERS} PARENT_SCOPE)
set (TADPOLE_NAMESPACES ${TADPOLE_NAMESPACES} PARENT_SCOPE)


if (NOT TOADLET_FIXED_POINT)
	source_group ("Source Files\\\\bsp" FILES ${TADPOLE_BSP_SRC})
	source_group ("Header Files\\\\bsp" FILES ${TADPOLE_BSP_HEADERS})
	source_group ("Source Files\\\\studio" FILES ${TADPOLE_STUDIO_SRC})
	source_group ("Header Files\\\\studio" FILES ${TADPOLE_STUDIO_HEADERS})
	set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_BSP_SRC} ${TADPOLE_STUDIO_SRC})
	set (TADPOLE_HEADERS ${TADPOLE_HEADERS} ${TADPOLE_BSP_HEADERS} ${TADPOLE_STUDIO_HEADERS})
	set (TADPOLE_NAMESPACES ${TADPOLE_NAMESPACES} bsp studio)
endif (NOT TOADLET_FIXED_POINT)

# Give the headers and namespaces parent scope for inclusion into the master tadpole.h file
set (TADPOLE_HEADERS ${TADPOLE_HEADERS} PARENT_SCOPE)
set (TADPOLE_NAMESPACES ${TADPOLE_NAMESPACES} PARENT_SCOPE)


# Setup dependecy specific files and libraries
# Freetype is not required on OSX
if (NOT TOADLET_PLATFORM_OSX)
	find_package (Freetype)
	if (FREETYPE_FOUND)
		set (TOADLET_HAS_FREETYPE ON PARENT_SCOPE)
		include_directories (${FREETYPE_INCLUDE_DIRS})
		set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_FREETYPE_SRC})
		set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_FREETYPE_HEADERS})

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
	set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_XML_SRC})
	set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_XML_HEADERS})

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

	# OSX requires a specific link to mxml
	if (TOADLET_PLATFORM_OSX)
		set (STATIC_LIB_PROPS ${STATIC_LIB_PROPS} STATIC_LIBRARY_FLAGS ${MXML_LIBRARY})	
	endif (TOADLET_PLATFORM_OSX)
endif (MXML_FOUND)

find_package (GIF)
if (GIF_FOUND)
	# Files
	set (TOADLET_HAS_GIF ON PARENT_SCOPE)
	include_directories (${GIF_INCLUDE_DIR})
	set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_GIF_SRC})
	set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_GIF_HEADERS})

	# Libraries
	set (TADPOLE_LIBS ${TADPOLE_LIBS} ${GIF_LIBRARY})
	add_library (libgif_s STATIC IMPORTED)
	if (TOADLET_PLATFORM_POSIX)
		string (REPLACE ".so" ".a" GIF_A ${GIF_LIBRARY})
		if (EXISTS ${GIF_A})
			set (GIF_LIBRARY ${GIF_A})
		endif (EXISTS ${GIF_A})
	endif (TOADLET_PLATFORM_POSIX)
	set_property (TARGET libgif_s PROPERTY IMPORTED_LOCATION ${GIF_LIBRARY})
	set (TADPOLE_DEFS_S ${TADPOLE_DEFS_S} TOADLET_LIBGIF_NAME="${GIF_LIBRARY}")
	set (TADPOLE_LIBS_S ${TADPOLE_LIBS_S} libgif_s)
endif (GIF_FOUND)

find_package (JPEG)
if (JPEG_FOUND)
	# Files 
	set (TOADLET_HAS_JPEG ON PARENT_SCOPE)
	include_directories (${JPEG_INCLUDE_DIR})
	set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_JPEG_SRC})
	set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_JPEG_HEADERS})
	
	# Libraries
	set (TADPOLE_LIBS ${TADPOLE_LIBS} ${JPEG_LIBRARY})
	add_library (libjpeg_s STATIC IMPORTED)
	if (TOADLET_PLATFORM_POSIX)
		string (REPLACE ".so" ".a" JPEG_A ${JPEG_LIBRARY})
		if (EXISTS ${JPEG_A})
			set (JPEG_LIBRARY ${JPEG_A})
		endif (EXISTS ${JPEG_A})
	endif (TOADLET_PLATFORM_POSIX)
	set_property (TARGET libjpeg_s PROPERTY IMPORTED_LOCATION ${JPEG_LIBRARY})
	set (TADPOLE_DEFS_S ${TADPOLE_DEFS_S} TOADLET_LIBJPEG_NAME="${JPEG_LIBRARY}")
	set (TADPOLE_LIBS_S ${TADPOLE_LIBS_S} libjpeg_s)
endif (JPEG_FOUND)

find_package (PNG)
if (PNG_FOUND)
	# Files
	set (TOADLET_HAS_PNG ON PARENT_SCOPE)
	include_directories (${PNG_INCLUDE_DIR})
	set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_PNG_SRC})
	set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_PNG_HEADERS})
	
	# Libraries
	set (TADPOLE_LIBS ${TADPOLE_LIBS} ${PNG_LIBRARY})
	add_library (libpng_s STATIC IMPORTED)
	if (TOADLET_PLATFORM_POSIX)
		string (REPLACE ".so" ".a" PNG_A ${PNG_LIBRARY})
		if (EXISTS ${PNG_A})
			set (PNG_LIBRARY ${PNG_A})
		endif (EXISTS ${PNG_A})
	endif (TOADLET_PLATFORM_POSIX)
	set_property (TARGET libpng_s PROPERTY IMPORTED_LOCATION ${PNG_LIBRARY})
	set (TADPOLE_DEFS_S ${TADPOLE_DEFS_S} TOADLET_LIBPNG_NAME="${PNG_LIBRARY}")
	set (TADPOLE_LIBS_S ${TADPOLE_LIBS_S} libpng_s)
endif (PNG_FOUND)

find_package (OggVorbis)
if (OGGVORBIS_FOUND)
	set (TOADLET_HAS_OGGVORBIS ON PARENT_SCOPE)
	include_directories (${OGGVORBIS_INCLUDES})
	set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_OGGVORBISDECODER_SRC})
	set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_OGGVORBISDECODER_HEADERS})

	# Dynamic libs
	set (TADPOLE_LIBS ${TADPOLE_LIBS} ${OGGVORBIS_LIBRARIES})

	# Static libs are ugly since we add references to external static libraries in toadlet
	add_library (libogg_s STATIC IMPORTED)
	add_library (libvorbis_s STATIC IMPORTED)
	add_library (libvorbisfile_s STATIC IMPORTED)
	add_library (libvorbisenc_s STATIC IMPORTED)
	if (TOADLET_PLATFORM_POSIX)
		string (REPLACE ".so" ".a" OGG_A ${OGG_LIBRARY})
		if (EXISTS ${OGG_A})
			set (OGG_LIBRARY ${OGG_A})
		endif (EXISTS ${OGG_A})
		string (REPLACE ".so" ".a" VORBIS_A ${VORBIS_LIBRARY})
		if (EXISTS ${VORBIS_A})
			set (VORBIS_LIBRARY ${VORBIS_A})
		endif (EXISTS ${VORBIS_A})
		string (REPLACE ".so" ".a" VORBISFILE_A ${VORBISFILE_LIBRARY})
		if (EXISTS ${VORBISFILE_A})
			set (VORBISFILE_LIBRARY ${VORBISFILE_A})
		endif (EXISTS ${VORBISFILE_A})
		string (REPLACE ".so" ".a" VORBISENG_A ${VORBISENC_LIBRARY})
		if (EXISTS ${VORBISENC_A})
			set (VORBISENC_LIBRARY ${VORBISENC_A})
		endif (EXISTS ${VORBISENC_A})
	endif (TOADLET_PLATFORM_POSIX)
	set_property (TARGET libogg_s PROPERTY IMPORTED_LOCATION ${OGG_LIBRARY})
	set_property (TARGET libvorbis_s PROPERTY IMPORTED_LOCATION ${VORBIS_LIBRARY})
	set_property (TARGET libvorbisfile_s PROPERTY IMPORTED_LOCATION ${VORBISFILE_LIBRARY})
	set_property (TARGET libvorbisenc_s PROPERTY IMPORTED_LOCATION ${VORBISENC_LIBRARY})
	set (TADPOLE_DEFS_S ${TADPOLE_DEFS_S} TOADLET_LIBOGG_NAME="${OGG_LIBRARY}" TOADLET_LIBVORBIS_NAME="${VORBIS_LIBRARY}" TOADLET_LIBVORBISFILE_NAME="${VORBISFILE_LIBRARY}" TOADLET_LIBVORBISENC_NAME="${VORBISENC_LIBRARY}")
	set (TADPOLE_LIBS_S ${TADPOLE_LIBS_S} libogg_s libvorbis_s libvorbisfile_s libvorbisenc_s)
endif (OGGVORBIS_FOUND)

find_package (Sidplay)
if (SIDPLAY_FOUND)
	set (TOADLET_HAS_SIDPLAY ON PARENT_SCOPE)
	include_directories (${SIDPLAY_INCLUDES})
	add_definitions(-DSIDPLAY_VERSION=${SIDPLAY_VERSION})
	set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_SIDDECODER_SRC})
	set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_SIDDECODER_HEADERS})

	# Dynamic libs
	set (TADPOLE_LIBS ${TADPOLE_LIBS} ${SIDPLAY_LIBRARY})

	# Static libs are ugly since we add references to external static libraries in toadlet
	set (TADPOLE_DEFS_S ${TADPOLE_DEFS_S} TOADLET_LIBSIDPLAY_NAME="${SIDPLAY_LIBRARY}")
	set (TADPOLE_LIBS_S ${TADPOLE_LIBS_S} libsidplay_s)

	# Resid is only used with sidplay 2
	if (${SIDPLAY_VERSION} EQUAL 2)
		set (TADPOLE_LIBS ${TADPOLE_LIBS} ${RESID_LIBRARY})
		set (TADPOLE_DEFS_S ${TADPOLE_DEFS_S} TOADLET_LIBRESID_NAME="${RESID_LIBRARY}")
	endif (${SIDPLAY_VERSION} EQUAL 2)
endif (SIDPLAY_FOUND)

find_package (ZLIB)
if (ZLIB_FOUND)
	# Files
	set (TOADLET_HAS_ZLIB ON PARENT_SCOPE)
	include_directories (${ZLIB_INCLUDE_DIR})
	
	# Libraries
	set (TADPOLE_LIBS ${TADPOLE_LIBS} ${ZLIB_LIBRARY})
	add_library (zlib_s STATIC IMPORTED)
	if (TOADLET_PLATFORM_POSIX)
		string (REPLACE ".so" ".a" ZLIB_A ${ZLIB_LIBRARY})
		if (EXISTS ${ZLIB_A})
			set (ZLIB_LIBRARY ${ZLIB_A})
		endif (EXISTS ${ZLIB_A})
	endif (TOADLET_PLATFORM_POSIX)
	set_property (TARGET zlib_s PROPERTY IMPORTED_LOCATION ${ZLIB_LIBRARY})
	set (TADPOLE_DEFS_S ${TADPOLE_DEFS_S} TOADLET_ZLIB_NAME="${ZLIB_LIBRARY}")
	set (TADPOLE_LIBS_S ${TADPOLE_LIBS_S} zlib_s)
endif (ZLIB_FOUND)

find_package (ZZIP)
if (ZZIP_FOUND)
	# Files
	set (TOADLET_HAS_ZZIP ON PARENT_SCOPE)
	include_directories (${ZZIP_INCLUDE_DIR})
	set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_ZIP_SRC})
	set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_ZIP_HEADERS})
	
	# Libraries
	set (TADPOLE_LIBS ${TADPOLE_LIBS} ${ZZIP_LIBRARY})
	add_library (zziplib_s STATIC IMPORTED)
	if (TOADLET_PLATFORM_POSIX)
		string (REPLACE ".so" ".a" ZZIP_A ${ZZIP_LIBRARY})
		if (EXISTS ${ZZIP_A})
			set (ZZIP_LIBRARY ${ZZIP_A})
		endif (EXISTS ${ZZIP_A})
	endif (TOADLET_PLATFORM_POSIX)
	set_property (TARGET zziplib_s PROPERTY IMPORTED_LOCATION ${ZZIP_LIBRARY})
	set (TADPOLE_DEFS_S ${TADPOLE_DEFS_S} TOADLET_ZZIPLIB_NAME="${ZZIP_LIBRARY}")
	set (TADPOLE_LIBS_S ${TADPOLE_LIBS_S} zziplib_s)
endif (ZZIP_FOUND)

# Platform specific files
if (TOADLET_PLATFORM_OSX)
	source_group ("Source Files\\\\handler\\\\platform\\\\osx" FILES ${TADPOLE_HANDLER_PLATFORM_OSX_SRC})
	source_group ("Header Files\\\\handler\\\\platform\\\\osx" FILES ${TADPOLE_HANDLER_PLATFORM_OSX_HEADERS})
	set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_HANDLER_PLATFORM_OSX_SRC})
	if (TOADLET_PLATFORM_IOS)
		find_library (UIKIT_LIBS UIKit)
		set (TADPOLE_LIBS_S ${TADPOLE_LIBS_S} ${UIKIT_LIBS})
		set (TADPOLE_LIBS ${TADPOLE_LIBS} ${UIKIT_LIBS})
	else (TOADLET_PLATFORM_IOS)
		find_library (APPKIT_LIBS AppKit)
		set (TADPOLE_LIBS_S ${TADPOLE_LIBS_S} ${APPKIT_LIBS})	
		set (TADPOLE_LIBS ${TADPOLE_LIBS} ${APPKIT_LIBS})	
	endif (TOADLET_PLATFORM_IOS)
elseif (TOADLET_PLATFORM_WIN32)
	find_package (GDIPLUS)
	if (GDIPLUS_FOUND)
		set (TOADLET_HAS_GDIPLUS ON PARENT_SCOPE)
		include_directories (${GDIPLUS_INCLUDE_PATH})
		source_group ("Source Files\\\\handler\\\\platform\\\\win32" FILES ${TADPOLE_HANDLER_PLATFORM_WIN32_GDIPLUS_SRC})
		source_group ("Header Files\\\\handler\\\\platform\\\\win32" FILES ${TADPOLE_HANDLER_PLATFORM_WIN32_GDIPLUS_HEADERS})
		set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_HANDLER_PLATFORM_WIN32_GDIPLUS_SRC})
	endif (GDIPLUS_FOUND)
	source_group ("Source Files\\\\handler\\\\platform\\\\win32" FILES ${TADPOLE_HANDLER_PLATFORM_WIN32_SRC})
	source_group ("Header Files\\\\handler\\\\platform\\\\win32" FILES ${TADPOLE_HANDLER_PLATFORM_WIN32_HEADERS})
	set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_HANDLER_PLATFORM_WIN32_SRC})
endif (TOADLET_PLATFORM_OSX)

# Use dependency results
source_group ("Source Files\\\\handler" FILES ${TADPOLE_HANDLER_SRC})
source_group ("Header Files\\\\handler" FILES ${TADPOLE_HANDLER_HEADERS})

set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_HANDLER_SRC})


# Dynamic library
if (NOT TOADLET_PLATFORM_IOS)
	add_library (toadlet_tadpole SHARED ${TADPOLE_SRC} ${TADPOLE_HEADERS})
	set_target_properties (toadlet_tadpole PROPERTIES ${DYNAMIC_LIB_PROPS})
	if (TOADLET_PLATFORM_ANDROID AND ${NDKSYSTEM} STREQUAL "windows")
		# HACK: The Windows Android NDK uses make under cygwin, which doesn't cooperate very well with cmake. 
		# So one must manually fix library paths to change the windows drive location.
		string (REGEX REPLACE "([A-Z]):" "/cygdrive/\\1" TADPOLE_LIBS ${TADPOLE_LIBS})
	endif (TOADLET_PLATFORM_ANDROID AND ${NDKSYSTEM} STREQUAL "windows")
	target_link_libraries (toadlet_tadpole toadlet_egg toadlet_peeper toadlet_ribbit ${TADPOLE_LIBS})
	install (TARGETS toadlet_tadpole DESTINATION lib)
endif (NOT TOADLET_PLATFORM_IOS)


# Static library
add_library (toadlet_tadpole_s STATIC ${TADPOLE_SRC} ${TADPOLE_HEADERS})
set_target_properties (toadlet_tadpole_s PROPERTIES ${STATIC_LIB_PROPS})
set_property (TARGET toadlet_tadpole_s APPEND PROPERTY COMPILE_DEFINITIONS ${TADPOLE_DEFS_S})
target_link_libraries (toadlet_tadpole_s toadlet_egg_s toadlet_peeper_s toadlet_ribbit_s ${TADPOLE_LIBS_S})
install (TARGETS toadlet_tadpole_s DESTINATION lib)


# Add plugins
add_subdirectory (plugins/hop)

