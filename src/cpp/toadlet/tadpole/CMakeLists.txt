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

set (TADPOLE_HANDLER_FFMPEG_SRC
	handler/FFmpegStreamContext.cpp
	handler/FFmpegTextureModifier.cpp
)

set (TADPOLE_HANDLER_PLATFORM_ANDROID_SRC
	handler/platform/android/AndroidAssetArchive.cpp
	handler/platform/android/AndroidTextureHandler.cpp
	handler/platform/android/JStream.cpp
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
	ResourceController.h
	ResourceCreator.h
	ResourceData.h
	ResourceManager.h
	ResourceModifier.h
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

set (TADPOLE_HANDLER_FFMPEG_HEADERS
	handler/FFmpegStreamContext.h
	handler/FFmpegTextureModifier.h
)

set (TADPOLE_HANDLER_PLATFORM_ANDROID_HEADERS
	handler/platform/android/AndroidAssetArchive.h
	handler/platform/android/AndroidTextureHandler.h
	handler/platform/android/JStream.h
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


# Sources, headers and namespaces
set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_ANIMATION_SRC} ${TADPOLE_CREATOR_SRC} ${TADPOLE_HANDLER_SRC} ${TADPOLE_MATERIAL_SRC} ${TADPOLE_NODE_SRC} ${TADPOLE_SENSOR_SRC} ${TADPOLE_TERRAIN_SRC})
set (TADPOLE_HEADERS ${TADPOLE_HEADERS} ${TADPOLE_ANIMATION_HEADERS} ${TADPOLE_CREATOR_HEADERS} ${TADPOLE_MATERIAL_HEADERS} ${TADPOLE_NODE_HEADERS} ${TADPOLE_SENSOR_HEADERS} ${TADPOLE_TERRAIN_HEADERS})
set (TADPOLE_NAMESPACES ${TADPOLE_NAMESPACES} animation creator material node sensor terrain)


# Fixed point builds use fewer features
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
# TODO: The handlers/ subdirectory is going to become a plugin system, thus they are NOT included in these aggregate header files
set (TADPOLE_HEADERS ${TADPOLE_HEADERS} PARENT_SCOPE)
set (TADPOLE_NAMESPACES ${TADPOLE_NAMESPACES} PARENT_SCOPE)


# Setup dependecy specific files and libraries

# Freetype is not required on OSX
if (NOT TOADLET_PLATFORM_OSX)
	find_package (Freetype)
	if (FREETYPE_FOUND)
		set (TOADLET_HAS_FREETYPE ON PARENT_SCOPE)
		include_directories (${FREETYPE_INCLUDE_DIRS})
		set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_FREETYPE_HEADERS})

		# Dynamic Library
		set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_FREETYPE_SRC})
		set (TADPOLE_HANDLER_LIBS ${TADPOLE_HANDLER_LIBS} ${FREETYPE_LIBRARY})
		
		# Static Library
		LOCATE_STATIC_LIBRARY (${FREETYPE_LIBRARY} FREETYPE_LIBRARY_S)
		if (FREETYPE_LIBRARY_S)
			set (TADPOLE_HANDLER_SRC_S ${TADPOLE_HANDLER_SRC_S} ${TADPOLE_HANDLER_FREETYPE_SRC})
			set (TADPOLE_HANDLER_LIBS_S ${TADPOLE_HANDLER_LIBS_S} ${FREETYPE_LIBRARY_S})
		endif (FREETYPE_LIBRARY_S)
	endif (FREETYPE_FOUND)
endif (NOT TOADLET_PLATFORM_OSX)

find_package (MXML)
if (MXML_FOUND)
	set (TOADLET_HAS_MXML ON PARENT_SCOPE)
	include_directories (${MXML_INCLUDE_DIR})
	set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_XML_HEADERS})

	# Dynamic Library
	set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_XML_SRC})
	set (TADPOLE_HANDLER_LIBS ${TADPOLE_HANDLER_LIBS} ${MXML_LIBRARY})

	# Static Library
	LOCATE_STATIC_LIBRARY (${MXML_LIBRARY} MXML_LIBRARY_S)
	if (MXML_LIBRARY_S)
		set (TADPOLE_HANDLER_SRC_S ${TADPOLE_HANDLER_SRC_S} ${TADPOLE_HANDLER_XML_SRC})
		set (TADPOLE_HANDLER_LIBS_S ${TADPOLE_HANDLER_LIBS_S} ${MXML_LIBRARY_S})
	endif (MXML_LIBRARY_S)
endif (MXML_FOUND)

find_package (GIF)
if (GIF_FOUND)
	set (TOADLET_HAS_GIF ON PARENT_SCOPE)
	include_directories (${GIF_INCLUDE_DIR})
	set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_GIF_HEADERS})

	# Dynamic Library
	set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_GIF_SRC})
	set (TADPOLE_HANDLER_LIBS ${TADPOLE_HANDLER_LIBS} ${GIF_LIBRARY})

	# Static Library
	LOCATE_STATIC_LIBRARY (${GIF_LIBRARY} GIF_LIBRARY_S)
	if (GIF_LIBRARY_S)
		set (TADPOLE_HANDLER_SRC_S ${TADPOLE_HANDLER_SRC_S} ${TADPOLE_HANDLER_GIF_SRC})
		set (TADPOLE_HANDLER_LIBS_S ${TADPOLE_HANDLER_LIBS_S} ${GIF_LIBRARY_S})
	endif (GIF_LIBRARY_S)
endif (GIF_FOUND)

find_package (JPEG)
if (JPEG_FOUND)
	set (TOADLET_HAS_JPEG ON PARENT_SCOPE)
	include_directories (${JPEG_INCLUDE_DIR})
	set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_JPEG_HEADERS})
	
	# Dynamic Library
	set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_JPEG_SRC})
	set (TADPOLE_HANDLER_LIBS ${TADPOLE_HANDLER_LIBS} ${JPEG_LIBRARY})

	# Static Library
	LOCATE_STATIC_LIBRARY (${JPEG_LIBRARY} JPEG_LIBRARY_S)
	if (JPEG_LIBRARY_S)
		set (TADPOLE_HANDLER_SRC_S ${TADPOLE_HANDLER_SRC_S} ${TADPOLE_HANDLER_JPEG_SRC})
		set (TADPOLE_HANDLER_LIBS_S ${TADPOLE_HANDLER_LIBS_S} ${JPEG_LIBRARY_S})
	endif (JPEG_LIBRARY_S)
endif (JPEG_FOUND)

find_package (PNG)
if (PNG_FOUND)
	set (TOADLET_HAS_PNG ON PARENT_SCOPE)
	include_directories (${PNG_INCLUDE_DIR})
	set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_PNG_HEADERS})
	
	# Dynamic Library
	set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_PNG_SRC})
	set (TADPOLE_HANDLER_LIBS ${TADPOLE_HANDLER_LIBS} ${PNG_LIBRARY})

	# Static Library
	LOCATE_STATIC_LIBRARY (${PNG_LIBRARY} PNG_LIBRARY_S)
	if (PNG_LIBRARY_S)
		set (TADPOLE_HANDLER_SRC_S ${TADPOLE_HANDLER_SRC_S} ${TADPOLE_HANDLER_PNG_SRC})
		set (TADPOLE_HANDLER_LIBS_S ${TADPOLE_HANDLER_LIBS_S} ${PNG_LIBRARY_S})
	endif (PNG_LIBRARY_S)
endif (PNG_FOUND)

find_package (OggVorbis)
if (OGGVORBIS_FOUND)
	set (TOADLET_HAS_OGGVORBIS ON PARENT_SCOPE)
	include_directories (${OGGVORBIS_INCLUDES})
	set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_OGGVORBISDECODER_HEADERS})

	# Dynamic Library
	set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_OGGVORBISDECODER_SRC})
	set (TADPOLE_HANDLER_LIBS ${TADPOLE_HANDLER_LIBS} ${OGGVORBIS_LIBRARIES})

	# Static Library
	LOCATE_STATIC_LIBRARY (${OGG_LIBRARY} OGG_LIBRARY_S)
        if (OGG_LIBRARY_S)
                set (TADPOLE_HANDLER_SRC_S ${TADPOLE_HANDLER_SRC_S} ${TADPOLE_HANDLER_OGGVORBISDECODER_SRC})
                set (TADPOLE_HANDLER_LIBS_S ${TADPOLE_HANDLER_LIBS_S} ${OGG_LIBRARY_S})
        endif (OGG_LIBRARY_S)
	LOCATE_STATIC_LIBRARY (${VORBIS_LIBRARY} VORBIS_LIBRARY_S)
        if (VORBIS_LIBRARY_S)
                set (TADPOLE_HANDLER_LIBS_S ${TADPOLE_HANDLER_LIBS_S} ${VORBIS_LIBRARY_S})
        endif (VORBIS_LIBRARY_S)
	LOCATE_STATIC_LIBRARY (${VORBISFILE_LIBRARY} VORBISFILE_LIBRARY_S)
        if (VORBISFILE_LIBRARY_S)
                set (TADPOLE_HANDLER_LIBS_S ${TADPOLE_HANDLER_LIBS_S} ${VORBISFILE_LIBRARY_S})
        endif (VORBISFILE_LIBRARY_S)
	LOCATE_STATIC_LIBRARY (${VORBISENC_LIBRARY} VORBISENC_LIBRARY_S)
        if (VORBISENC_LIBRARY_S)
                set (TADPOLE_HANDLER_LIBS_S ${TADPOLE_HANDLER_LIBS_S} ${VORBISENC_LIBRARY_S})
        endif (VORBISENC_LIBRARY_S)
endif (OGGVORBIS_FOUND)

find_package (Sidplay)
if (SIDPLAY_FOUND)
	set (TOADLET_HAS_SIDPLAY ON PARENT_SCOPE)
	include_directories (${SIDPLAY_INCLUDES})
	add_definitions(-DSIDPLAY_VERSION=${SIDPLAY_VERSION})
	set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_SIDDECODER_HEADERS})

	# Dynamic Library
	set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_SIDDECODER_SRC})
	set (TADPOLE_HANDLER_LIBS ${TADPOLE_HANDLER_LIBS} ${SIDPLAY_LIBRARY})

	# Static Library
	LOCATE_STATIC_LIBRARY (${SIDPLAY_LIBRARY} SIDPLAY_LIBRARY_S)
	if (SIDPLAY_LIBRARY_S)
		set (TADPOLE_HANDLER_SRC_S ${TADPOLE_HANDLER_SRC_S} ${TADPOLE_HANDLER_SIDDECODER_SRC})
		set (TADPOLE_HANDLER_LIBS_S ${TADPOLE_HANDLER_LIBS_S} ${SIDPLAY_LIBRARY_S})
	endif (SIDPLAY_LIBRARY_S)

	# Resid is only used with sidplay 2
	if (${SIDPLAY_VERSION} EQUAL 2)
		set (TADPOLE_HANDLER_LIBS ${TADPOLE_HANDLER_LIBS} ${RESID_LIBRARY})

		LOCATE_STATIC_LIBRARY (${RESID_LIBRARY} RESID_LIBRARY_S)
		if (RESID_LIBRARY_S)
			set (TADPOLE_HANDLER_LIBS_S ${TADPOLE_HANDLER_LIBS_S} ${RESID_LIBRARY_S})
		endif (RESID_LIBRARY_S)
	endif (${SIDPLAY_VERSION} EQUAL 2)
endif (SIDPLAY_FOUND)

find_package (ZLIB)
if (ZLIB_FOUND)
	set (TOADLET_HAS_ZLIB ON PARENT_SCOPE)
	include_directories (${ZLIB_INCLUDE_DIR})
	
	# Dynamic Library
	set (TADPOLE_HANDLER_LIBS ${TADPOLE_HANDLER_LIBS} ${ZLIB_LIBRARY})

	# Static Library
	LOCATE_STATIC_LIBRARY (${ZLIB_LIBRARY} ZLIB_LIBRARY_S)
        if (ZLIB_LIBRARY_S)
                set (TADPOLE_HANDLER_LIBS_S ${TADPOLE_HANDLER_LIBS_S} ${ZLIB_LIBRARY_S})
        endif (ZLIB_LIBRARY_S)
endif (ZLIB_FOUND)

find_package (ZZIP)
if (ZZIP_FOUND)
	set (TOADLET_HAS_ZZIP ON PARENT_SCOPE)
	include_directories (${ZZIP_INCLUDE_DIR})
	set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_ZIP_HEADERS})
	
	# Dynamic Library
	set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_ZIP_SRC})
	set (TADPOLE_HANDLER_LIBS ${TADPOLE_HANDLER_LIBS} ${ZZIP_LIBRARY})

	# Static Library
	LOCATE_STATIC_LIBRARY (${ZZIP_LIBRARY} ZZIP_LIBRARY_S)
        if (ZZIP_LIBRARY_S)
		set (TADPOLE_HANDLER_SRC_S ${TADPOLE_HANDLER_SRC_S} ${TADPOLE_HANDLER_ZIP_SRC})
                set (TADPOLE_HANDLER_LIBS_S ${TADPOLE_HANDLER_LIBS_S} ${ZZIP_LIBRARY_S})
        endif (ZZIP_LIBRARY_S)
endif (ZZIP_FOUND)

#TODO: Fix the ffmpeg hack
#find_package (FFmpeg)
if (FFMPEG_FOUND)
	set (TOADLET_HAS_FFMPEG ON PARENT_SCOPE)
	include_directories (${FFMPEG_INCLUDE_DIR})
	set (TADPOLE_HANDLER_HEADERS ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_HANDLER_FFMPEG_HEADERS})
	
	# Dynamic Library
	set (TADPOLE_HANDLER_SRC ${TADPOLE_HANDLER_SRC} ${TADPOLE_HANDLER_FFMPEG_SRC})
	set (TADPOLE_HANDLER_LIBS ${TADPOLE_HANDLER_LIBS} ${FFMPEG_LIBRARIES})

	# Static Library
	LOCATE_STATIC_LIBRARY (${FFMPEG_LIBRARIES} FFMPEG_LIBRARIES_S)
	if (FFMPEG_LIBRARIES_S)
		set (TADPOLE_HANDLER_SRC_S ${TADPOLE_HANDLER_SRC_S} ${TADPOLE_HANDLER_FFMPEG_SRC})
		set (TADPOLE_HANDLER_LIBS_S ${TADPOLE_HANDLER_LIBS_S} ${FFMPEG_LIBRARIES_S})
	endif (FFMPEG_LIBRARIES_S)
endif (FFMPEG_FOUND)


# Platform specific files 
if (TOADLET_PLATFORM_ANDROID)
	source_group ("Source Files\\\\handler\\\\platform\\\\android" FILES ${TADPOLE_HANDLER_PLATFORM_ANDROID_SRC})
	source_group ("Header Files\\\\handler\\\\platform\\\\android" FILES ${TADPOLE_HANDLER_PLATFORM_ANDROID_HEADERS})
	set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_HANDLER_PLATFORM_ANDROID_SRC})
elseif (TOADLET_PLATFORM_OSX)
	source_group ("Source Files\\\\handler\\\\platform\\\\osx" FILES ${TADPOLE_HANDLER_PLATFORM_OSX_SRC})
	source_group ("Header Files\\\\handler\\\\platform\\\\osx" FILES ${TADPOLE_HANDLER_PLATFORM_OSX_HEADERS})
	set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_HANDLER_PLATFORM_OSX_SRC})
	if (TOADLET_PLATFORM_IOS)
		find_library (UIKIT_LIBS UIKit)
		set (TADPOLE_LIBS ${TADPOLE_LIBS} ${UIKIT_LIBS})
	else (TOADLET_PLATFORM_IOS)
		find_library (APPKIT_LIBS AppKit)
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
endif (TOADLET_PLATFORM_ANDROID)


# Bring in dependency results
source_group ("Source Files\\\\handler" FILES ${TADPOLE_HANDLER_SRC})
source_group ("Header Files\\\\handler" FILES ${TADPOLE_HANDLER_HEADERS})
set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_HANDLER_SRC})
set (TADPOLE_SRC_S ${TADPOLE_SRC} ${TADPOLE_HANDLER_SRC_S})


# Dynamic library
if (NOT TOADLET_PLATFORM_IOS)
	add_library (toadlet_tadpole SHARED ${TADPOLE_SRC} ${TADPOLE_HEADERS})
	set_target_properties (toadlet_tadpole PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_tadpole toadlet_egg toadlet_peeper toadlet_ribbit ${TADPOLE_LIBS} ${TADPOLE_HANDLER_LIBS})
	install (TARGETS toadlet_tadpole DESTINATION ${LIB_INSTALL_DIR})
endif (NOT TOADLET_PLATFORM_IOS)


# Static library
add_library (toadlet_tadpole_s STATIC ${TADPOLE_SRC_S} ${TADPOLE_HEADERS_S})
set_target_properties (toadlet_tadpole_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_tadpole_s toadlet_egg_s toadlet_peeper_s toadlet_ribbit_s ${TADPOLE_LIBS})
MERGE_STATIC_LIBRARIES (toadlet_tadpole_s "${TADPOLE_HANDLER_LIBS_S}")
install (TARGETS toadlet_tadpole_s DESTINATION ${LIB_INSTALL_DIR})


# Add plugins
add_subdirectory (plugins/hop)

