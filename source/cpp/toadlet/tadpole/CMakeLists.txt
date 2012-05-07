# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_tadpole
set (TADPOLE_SRC
	AnimationActionComponent.cpp
	AudioComponent.cpp
	ArchiveManager.cpp
	AudioManager.cpp
	BaseComponent.cpp
	BufferManager.cpp
	ColorKeyFrame.cpp
	ColorSequence.cpp
	ColorTrack.cpp
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
	SkeletonComponent.cpp
	TransformKeyFrame.cpp
	TransformSequence.cpp
	TransformTrack.cpp
	TextureManager.cpp
	Version.cpp
)

set (TADPOLE_ANIMATION_SRC
	animation/BaseAnimation.cpp
	animation/CameraProjectionAnimation.cpp
	animation/NodePathAnimation.cpp
	animation/MaterialAnimation.cpp
	animation/MeshAnimation.cpp
)

set (TADPOLE_BSP_SRC
	bsp/BSP30Map.cpp
	bsp/BSP30Node.cpp
	bsp/BSP30Streamer.cpp
)

set (TADPOLE_CREATOR_SRC
	creator/DiffuseMaterialCreator.cpp
	creator/SkyBoxMaterialCreator.cpp
	creator/AABoxMeshCreator.cpp
	creator/GridMeshCreator.cpp
	creator/GeoSphereMeshCreator.cpp
	creator/NormalizationTextureCreator.cpp
	creator/SkyBoxMeshCreator.cpp
	creator/SkyDomeMeshCreator.cpp
	creator/SphereMeshCreator.cpp
	creator/TorusMeshCreator.cpp
)

set (TADPOLE_PLUGINS_SRC
	plugins/BMPStreamer.cpp
	plugins/DDSStreamer.cpp
	plugins/TGAStreamer.cpp
	plugins/TMSHStreamer.cpp
	plugins/TPKGArchive.cpp
	plugins/WADArchive.cpp
	plugins/WaveDecoder.cpp
)

set (TADPOLE_PLUGINS_XML_SRC
	plugins/XANMStreamer.cpp
	plugins/XMLMeshUtilities.cpp
	plugins/XMATStreamer.cpp
	plugins/XMSHStreamer.cpp
)

set (TADPOLE_PLUGINS_GIF_SRC
	plugins/GIFStreamer.cpp
)

set (TADPOLE_PLUGINS_JPEG_SRC
	plugins/JPEGStreamer.cpp
)

set (TADPOLE_PLUGINS_PNG_SRC
	plugins/PNGStreamer.cpp
)

set (TADPOLE_PLUGINS_FREETYPE_SRC
	plugins/FreeTypeStreamer.cpp
)

set (TADPOLE_PLUGINS_FFMPEG_SRC
	plugins/FFmpegStreamContext.cpp
	plugins/FFmpegVideoHandler.cpp
)

set (TADPOLE_PLUGINS_OGGVORBISDECODER_SRC
	plugins/OggVorbisDecoder.cpp
)

set (TADPOLE_PLUGINS_SIDDECODER_SRC
	plugins/SIDDecoder.cpp
)

set (TADPOLE_PLUGINS_ZIP_SRC
	plugins/ZIPArchive.cpp
	plugins/ZIPStream.cpp
)

set (TADPOLE_MATERIAL_SRC
	material/Material.cpp
	material/RenderPass.cpp
	material/RenderPath.cpp
	material/RenderVariableSet.cpp
)

set (TADPOLE_NODE_SRC
	node/AnaglyphCameraNode.cpp
	node/CameraNode.cpp
	node/CameraAlignedNode.cpp
	node/LabelNode.cpp
	node/LightNode.cpp
	node/MeshNode.cpp
	node/Node.cpp
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
	studio/SpriteStreamer.cpp
	studio/SpriteModelNode.cpp
	studio/StudioStreamer.cpp
	studio/StudioModelNode.cpp
)

set (TADPOLE_TERRAIN_SRC
	terrain/DiffuseTerrainMaterialSource.cpp
	terrain/TerrainNode.cpp
	terrain/TerrainPatchNode.cpp
	terrain/TextureDataSource.cpp
)

set (TADPOLE_PLATFORM_ANDROID_SRC
	platform/android/AndroidAssetArchive.cpp
	platform/android/AndroidTextureStreamer.cpp
	platform/android/JStream.cpp
)

set (TADPOLE_PLATFORM_ANDROID_8_SRC
	platform/android/us_toadlet_tadpole_CameraNode.cpp
	platform/android/us_toadlet_tadpole_Engine.cpp
	platform/android/us_toadlet_tadpole_LightNode.cpp
	platform/android/us_toadlet_tadpole_MeshNode.cpp
	platform/android/us_toadlet_tadpole_Node.cpp
	platform/android/us_toadlet_tadpole_Scene.cpp
)

set (TADPOLE_PLATFORM_OSX_SRC
	platform/osx/OSXBundleArchive.mm
	platform/osx/OSXFontStreamer.mm
	platform/osx/OSXTextureStreamer.mm
)

set (TADPOLE_PLATFORM_WIN32_SRC
	platform/win32/Win32ResourceArchive.cpp
)

set (TADPOLE_PLATFORM_WIN32_GDIPLUS_SRC
	platform/win32/Win32FontStreamer.cpp
	platform/win32/Win32TextureStreamer.cpp
)


# Headers
set (TADPOLE_HEADERS
	AnimationActionComponent.h
	ActionComponent.h
	Attachable.h
	ArchiveManager.h
	AudioStreamer.h
	AudioManager.h
	AudioComponent.h
	BaseActionComponent.h
	BaseComponent.h
	Bound.h
	BufferManager.h
	Collision.h
	Colors.h
	ColorKeyFrame.h
	ColorSequence.h
	ColorTrack.h
	Component.h
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
	SkeletonComponent.h
	TextureData.h
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
	VideoController.h
	VideoHandler.h
	Visible.h
)

set (TADPOLE_ANIMATION_HEADERS
	animation/Animation.h
	animation/AnimationListener.h
	animation/BaseAnimation.h
	animation/CameraProjectionAnimation.h
	animation/NodePathAnimation.h
	animation/MaterialAnimation.h
	animation/MeshAnimation.h
)

set (TADPOLE_BSP_HEADERS
	bsp/BSP30Streamer.h
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
	creator/NormalizationTextureCreator.h
	creator/SkyBoxMeshCreator.h
	creator/SkyDomeMeshCreator.h
	creator/TorusMeshCreator.h
	creator/SphereMeshCreator.h
)

set (TADPOLE_PLUGINS_HEADERS
	plugins/BMPStreamer.h
	plugins/TMSHStreamer.h
	plugins/PNGStreamer.h
	plugins/SPRStreamer.h
	plugins/TGAStreamer.h
	plugins/TPKGArchive.h
	plugins/TPKGStreamer.h
	plugins/WADArchive.h
	plugins/WADStreamer.h
	plugins/WaveDecoder.h
)

set (TADPOLE_PLUGINS_XML_HEADERS
	plugins/XANMStreamer.h
	plugins/XMLMeshUtilities.h
	plugins/XMATStreamer.h
	plugins/XMSHStreamer.h
)

set (TADPOLE_PLUGINS_GIF_HEADERS
	plugins/GIFStreamer.h
)

set (TADPOLE_PLUGINS_JPEG_HEADERS
	plugins/JPEGStreamer.h
)

set (TADPOLE_PLUGINS_PNG_HEADERS
	plugins/PNGStreamer.h
)

set (TADPOLE_PLUGINS_FREETYPE_HEADERS
	plugins/FreeTypeStreamer.h
)

set (TADPOLE_PLUGINS_FFMPEG_HEADERS
	plugins/FFmpegStreamContext.h
	plugins/FFmpegVideoHandler.h
)

set (TADPOLE_PLUGINS_OGGVORBISDECODER_HEADERS
	plugins/OggVorbisDecoder.h
)

set (TADPOLE_PLUGINS_SIDDECODER_HEADERS
	plugins/SIDDecoder.h
)

set (TADPOLE_PLUGINS_ZIP_HEADERS
	plugins/ZIPArchive.h
	plugins/ZIPStream.h
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
	node/CameraNode.h
	node/CameraAlignedNode.h
	node/LabelNode.h
	node/LightNode.h
	node/MeshNode.h
	node/Node.h
	node/NodeResource.h
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
	studio/SpriteStreamer.h
	studio/SpriteModel.h
	studio/SpriteModelNode.h
	studio/SpriteTypes.h
	studio/StudioStreamer.h
	studio/StudioModel.h
	studio/StudioModelNode.h
	studio/StudioTypes.h
)

set (TADPOLE_TERRAIN_HEADERS
	terrain/DiffuseTerrainMaterialSource.h
	terrain/TerrainNodeDataSource.h
	terrain/TerrainNodeMaterialSource.h
	terrain/TerrainNodeListener.h
	terrain/TerrainNode.h
	terrain/TerrainPatchNode.h
	terrain/TextureDataSource.h
)

set (TADPOLE_PLATFORM_ANDROID_HEADERS
	platform/android/AndroidAssetArchive.h
	platform/android/AndroidTextureStreamer.h
	platform/android/JStream.h
)

set (TADPOLE_PLATFORM_OSX_HEADERS
	platform/osx/OSXBundleArchive.h
	platform/osx/OSXFontStreamer.h
	platform/osx/OSXTextureStreamer.h
) 

set (TADPOLE_PLATFORM_WIN32_HEADERS
	platform/win32/Win32ResourceArchive.h
)

set (TADPOLE_PLATFORM_WIN32_GDIPLUS_HEADERS
	platform/win32/StreamIStream.h
	platform/win32/Win32FontStreamer.h
	platform/win32/Win32TextureStreamer.h
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
set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_ANIMATION_SRC} ${TADPOLE_CREATOR_SRC} ${TADPOLE_PLUGINS_SRC} ${TADPOLE_MATERIAL_SRC} ${TADPOLE_NODE_SRC} ${TADPOLE_SENSOR_SRC} ${TADPOLE_TERRAIN_SRC})
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
set (TADPOLE_HEADERS ${TADPOLE_HEADERS} PARENT_SCOPE)
set (TADPOLE_NAMESPACES ${TADPOLE_NAMESPACES} PARENT_SCOPE)


# Setup dependecy specific files and libraries

# Freetype is not required on OSX
find_package (Freetype)
if (FREETYPE_FOUND)
	set (TOADLET_HAS_FREETYPE ON PARENT_SCOPE)
	include_directories (${FREETYPE_INCLUDE_DIRS})
	set (TADPOLE_PLUGINS_HEADERS ${TADPOLE_PLUGINS_HEADERS} ${TADPOLE_PLUGINS_FREETYPE_HEADERS})

	# Dynamic Library
	set (TADPOLE_PLUGINS_SRC ${TADPOLE_PLUGINS_SRC} ${TADPOLE_PLUGINS_FREETYPE_SRC})
	set (TADPOLE_PLUGINS_LIBS ${TADPOLE_PLUGINS_LIBS} ${FREETYPE_LIBRARY})
	
	# Static Library
	LOCATE_STATIC_LIBRARIES (FREETYPE_LIBRARY_S "${FREETYPE_LIBRARY}")
	if (FREETYPE_LIBRARY_S)
		set (TADPOLE_PLUGINS_SRC_S ${TADPOLE_PLUGINS_SRC_S} ${TADPOLE_PLUGINS_FREETYPE_SRC})
		set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} ${FREETYPE_LIBRARY_S})
	endif (FREETYPE_LIBRARY_S)
endif (FREETYPE_FOUND)

find_package (MXML)
if (MXML_FOUND)
	set (TOADLET_HAS_MXML ON PARENT_SCOPE)
	include_directories (${MXML_INCLUDE_DIR})
	set (TADPOLE_PLUGINS_HEADERS ${TADPOLE_PLUGINS_HEADERS} ${TADPOLE_PLUGINS_XML_HEADERS})

	# Dynamic Library
	set (TADPOLE_PLUGINS_SRC ${TADPOLE_PLUGINS_SRC} ${TADPOLE_PLUGINS_XML_SRC})
	set (TADPOLE_PLUGINS_LIBS ${TADPOLE_PLUGINS_LIBS} ${MXML_LIBRARY})

	# Static Library
	LOCATE_STATIC_LIBRARIES (MXML_LIBRARY_S "${MXML_LIBRARY}")
	if (MXML_LIBRARY_S)
		set (TADPOLE_PLUGINS_SRC_S ${TADPOLE_PLUGINS_SRC_S} ${TADPOLE_PLUGINS_XML_SRC})
		set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} ${MXML_LIBRARY_S})
	endif (MXML_LIBRARY_S)
endif (MXML_FOUND)

find_package (GIF)
if (GIF_FOUND)
	set (TOADLET_HAS_GIF ON PARENT_SCOPE)
	include_directories (${GIF_INCLUDE_DIR})
	set (TADPOLE_PLUGINS_HEADERS ${TADPOLE_PLUGINS_HEADERS} ${TADPOLE_PLUGINS_GIF_HEADERS})

	# Dynamic Library
	set (TADPOLE_PLUGINS_SRC ${TADPOLE_PLUGINS_SRC} ${TADPOLE_PLUGINS_GIF_SRC})
	set (TADPOLE_PLUGINS_LIBS ${TADPOLE_PLUGINS_LIBS} ${GIF_LIBRARY})

	# Static Library
	LOCATE_STATIC_LIBRARIES (GIF_LIBRARY_S "${GIF_LIBRARY}")
	if (GIF_LIBRARY_S)
		set (TADPOLE_PLUGINS_SRC_S ${TADPOLE_PLUGINS_SRC_S} ${TADPOLE_PLUGINS_GIF_SRC})
		set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} ${GIF_LIBRARY_S})
	endif (GIF_LIBRARY_S)
endif (GIF_FOUND)

find_package (JPEG)
if (JPEG_FOUND)
	set (TOADLET_HAS_JPEG ON PARENT_SCOPE)
	include_directories (${JPEG_INCLUDE_DIR})
	set (TADPOLE_PLUGINS_HEADERS ${TADPOLE_PLUGINS_HEADERS} ${TADPOLE_PLUGINS_JPEG_HEADERS})
	
	# Dynamic Library
	set (TADPOLE_PLUGINS_SRC ${TADPOLE_PLUGINS_SRC} ${TADPOLE_PLUGINS_JPEG_SRC})
	set (TADPOLE_PLUGINS_LIBS ${TADPOLE_PLUGINS_LIBS} ${JPEG_LIBRARY})

	# Static Library
	LOCATE_STATIC_LIBRARIES (JPEG_LIBRARY_S "${JPEG_LIBRARY}")
	if (JPEG_LIBRARY_S)
		set (TADPOLE_PLUGINS_SRC_S ${TADPOLE_PLUGINS_SRC_S} ${TADPOLE_PLUGINS_JPEG_SRC})
		set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} ${JPEG_LIBRARY_S})
	endif (JPEG_LIBRARY_S)
endif (JPEG_FOUND)

find_package (PNG)
if (PNG_FOUND)
	set (TOADLET_HAS_PNG ON PARENT_SCOPE)
	include_directories (${PNG_INCLUDE_DIR})
	set (TADPOLE_PLUGINS_HEADERS ${TADPOLE_PLUGINS_HEADERS} ${TADPOLE_PLUGINS_PNG_HEADERS})
	
	# Dynamic Library
	set (TADPOLE_PLUGINS_SRC ${TADPOLE_PLUGINS_SRC} ${TADPOLE_PLUGINS_PNG_SRC})
	set (TADPOLE_PLUGINS_LIBS ${TADPOLE_PLUGINS_LIBS} ${PNG_LIBRARY})

	# Static Library
	LOCATE_STATIC_LIBRARIES (PNG_LIBRARY_S "${PNG_LIBRARY}")
	if (PNG_LIBRARY_S)
		set (TADPOLE_PLUGINS_SRC_S ${TADPOLE_PLUGINS_SRC_S} ${TADPOLE_PLUGINS_PNG_SRC})
		set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} ${PNG_LIBRARY_S})
	endif (PNG_LIBRARY_S)
endif (PNG_FOUND)

find_package (ZLIB)
if (ZLIB_FOUND)
	set (TOADLET_HAS_ZLIB ON PARENT_SCOPE)
	include_directories (${ZLIB_INCLUDE_DIR})
	
	# Dynamic Library
	set (TADPOLE_PLUGINS_LIBS ${TADPOLE_PLUGINS_LIBS} ${ZLIB_LIBRARY})

	# Static Library
	LOCATE_STATIC_LIBRARIES (ZLIB_LIBRARY_S "${ZLIB_LIBRARY}")
	if (ZLIB_LIBRARY_S)
		set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} ${ZLIB_LIBRARY_S})
	endif (ZLIB_LIBRARY_S)
endif (ZLIB_FOUND)

find_package (ZZIP)
if (ZZIP_FOUND)
	set (TOADLET_HAS_ZZIP ON PARENT_SCOPE)
	include_directories (${ZZIP_INCLUDE_DIR})
	set (TADPOLE_PLUGINS_HEADERS ${TADPOLE_PLUGINS_HEADERS} ${TADPOLE_PLUGINS_ZIP_HEADERS})
	
	# Dynamic Library
	set (TADPOLE_PLUGINS_SRC ${TADPOLE_PLUGINS_SRC} ${TADPOLE_PLUGINS_ZIP_SRC})
	set (TADPOLE_PLUGINS_LIBS ${TADPOLE_PLUGINS_LIBS} ${ZZIP_LIBRARY})

	# Static Library
	LOCATE_STATIC_LIBRARIES (ZZIP_LIBRARY_S "${ZZIP_LIBRARY}")
	if (ZZIP_LIBRARY_S)
	set (TADPOLE_PLUGINS_SRC_S ${TADPOLE_PLUGINS_SRC_S} ${TADPOLE_PLUGINS_ZIP_SRC})
		set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} ${ZZIP_LIBRARY_S})
	endif (ZZIP_LIBRARY_S)
endif (ZZIP_FOUND)

find_package (Sidplay)
if (SIDPLAY_FOUND)
	set (TOADLET_HAS_SIDPLAY ON PARENT_SCOPE)
	include_directories (${SIDPLAY_INCLUDES})
	add_definitions(-DSIDPLAY_VERSION=${SIDPLAY_VERSION})
	set (TADPOLE_PLUGINS_HEADERS ${TADPOLE_PLUGINS_HEADERS} ${TADPOLE_PLUGINS_SIDDECODER_HEADERS})

	# Dynamic Library
	set (TADPOLE_PLUGINS_SRC ${TADPOLE_PLUGINS_SRC} ${TADPOLE_PLUGINS_SIDDECODER_SRC})
	set (TADPOLE_PLUGINS_LIBS ${TADPOLE_PLUGINS_LIBS} ${SIDPLAY_LIBRARY})

	# Static Library
	LOCATE_STATIC_LIBRARIES (SIDPLAY_LIBRARY_S "${SIDPLAY_LIBRARY}")
	if (SIDPLAY_LIBRARY_S)
		set (TADPOLE_PLUGINS_SRC_S ${TADPOLE_PLUGINS_SRC_S} ${TADPOLE_PLUGINS_SIDDECODER_SRC})
		set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} ${SIDPLAY_LIBRARY_S})
	endif (SIDPLAY_LIBRARY_S)

	# Resid is only used with sidplay 2
	if (${SIDPLAY_VERSION} EQUAL 2)
		set (TADPOLE_PLUGINS_LIBS ${TADPOLE_PLUGINS_LIBS} ${RESID_LIBRARY})

		LOCATE_STATIC_LIBRARIES (RESID_LIBRARY_S "${RESID_LIBRARY}")
		if (RESID_LIBRARY_S)
			set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} ${RESID_LIBRARY_S})
		endif (RESID_LIBRARY_S)
	endif (${SIDPLAY_VERSION} EQUAL 2)
endif (SIDPLAY_FOUND)

find_package (FFmpeg)
if (FFMPEG_FOUND)
	set (TOADLET_HAS_FFMPEG ON PARENT_SCOPE)
	include_directories (${FFMPEG_INCLUDE_DIR})
	set (TADPOLE_PLUGINS_HEADERS ${TADPOLE_PLUGINS_HEADERS} ${TADPOLE_PLUGINS_FFMPEG_HEADERS})
	
	# Dynamic Library
	set (TADPOLE_PLUGINS_SRC ${TADPOLE_PLUGINS_SRC} ${TADPOLE_PLUGINS_FFMPEG_SRC})
	set (TADPOLE_PLUGINS_LIBS ${TADPOLE_PLUGINS_LIBS} ${FFMPEG_LIBRARIES})

	# Static Library
	LOCATE_STATIC_LIBRARIES (FFMPEG_LIBRARIES_S "${FFMPEG_LIBRARIES}")
	if (FFMPEG_LIBRARIES_S)
		set (TADPOLE_PLUGINS_SRC_S ${TADPOLE_PLUGINS_SRC_S} ${TADPOLE_PLUGINS_FFMPEG_SRC})
		set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} ${FFMPEG_LIBRARIES_S})
		if (TOADLET_PLATFORM_WIN32)
			# HACK: FFmpeg on windows incorrectly exposes symbols from zlib, which then generate warnings.
			# We can hide them by passing ignore flags directly to the linker, as if they were libraries.
			set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} "/ignore:4006" "/ignore:4221")
		endif (TOADLET_PLATFORM_WIN32)
	endif (FFMPEG_LIBRARIES_S)
endif (FFMPEG_FOUND)

#TODO: Depreicate ogg vorbis in favor of ffmpeg. Right now it serves as a backup.
if (NOT FFMPEG_FOUND)
	find_package (OggVorbis)
	if (OGGVORBIS_FOUND)
		set (TOADLET_HAS_OGGVORBIS ON PARENT_SCOPE)
		include_directories (${OGGVORBIS_INCLUDES})
		set (TADPOLE_PLUGINS_HEADERS ${TADPOLE_PLUGINS_HEADERS} ${TADPOLE_PLUGINS_OGGVORBISDECODER_HEADERS})

		# Dynamic Library
		set (TADPOLE_PLUGINS_SRC ${TADPOLE_PLUGINS_SRC} ${TADPOLE_PLUGINS_OGGVORBISDECODER_SRC})
		set (TADPOLE_PLUGINS_LIBS ${TADPOLE_PLUGINS_LIBS} ${OGGVORBIS_LIBRARIES})

		# Static Library
		LOCATE_STATIC_LIBRARIES (OGGVORBIS_LIBRARIES_S "${OGGVORBIS_LIBRARIES}")
		if (OGGVORBIS_LIBRARIES_S)
			set (TADPOLE_PLUGINS_SRC_S ${TADPOLE_PLUGINS_SRC_S} ${TADPOLE_PLUGINS_OGGVORBISDECODER_SRC})
			set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} ${OGGVORBIS_LIBRARIES_S})
		endif (OGGVORBIS_LIBRARIES_S)
	endif (OGGVORBIS_FOUND)
endif (NOT FFMPEG_FOUND)


# Platform specific files 
if (TOADLET_PLATFORM_ANDROID)
	source_group ("Source Files\\\\platform\\\\android" FILES ${TADPOLE_PLATFORM_ANDROID_SRC})
	source_group ("Header Files\\\\platform\\\\android" FILES ${TADPOLE_PLATFORM_ANDROID_HEADERS})
	set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_PLATFORM_ANDROID_SRC})
	if (ANDROID_NDK_API_LEVEL LESS 9)
		source_group ("Source Files\\\\platform\\\\android" FILES ${TADPOLE_PLATFORM_ANDROID_8_SRC})
		set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_PLATFORM_ANDROID_8_SRC})
		set (TADPOLE_LIBS ${TADPOLE_LIBS} toadlet_ribbit_jaudiodevice)
	endif (ANDROID_NDK_API_LEVEL LESS 9)
elseif (TOADLET_PLATFORM_OSX)
	source_group ("Source Files\\\\platform\\\\osx" FILES ${TADPOLE_PLATFORM_OSX_SRC})
	source_group ("Header Files\\\\platform\\\\osx" FILES ${TADPOLE_PLATFORM_OSX_HEADERS})
	set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_PLATFORM_OSX_SRC})
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
		source_group ("Source Files\\\\platform\\\\win32" FILES ${TADPOLE_PLATFORM_WIN32_GDIPLUS_SRC})
		source_group ("Header Files\\\\platform\\\\win32" FILES ${TADPOLE_PLATFORM_WIN32_GDIPLUS_HEADERS})
		set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_PLATFORM_WIN32_GDIPLUS_SRC})
	endif (GDIPLUS_FOUND)
	source_group ("Source Files\\\\platform\\\\win32" FILES ${TADPOLE_PLATFORM_WIN32_SRC})
	source_group ("Header Files\\\\platform\\\\win32" FILES ${TADPOLE_PLATFORM_WIN32_HEADERS})
	set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_PLATFORM_WIN32_SRC})
endif (TOADLET_PLATFORM_ANDROID)


# Bring in dependency results
source_group ("Source Files\\\\plugins" FILES ${TADPOLE_PLUGINS_SRC})
source_group ("Header Files\\\\plugins" FILES ${TADPOLE_PLUGINS_HEADERS})
set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_PLUGINS_SRC})
set (TADPOLE_SRC_S ${TADPOLE_SRC} ${TADPOLE_PLUGINS_SRC_S})


# Dynamic library
if (NOT TOADLET_PLATFORM_IOS)
	add_library (toadlet_tadpole SHARED ${TADPOLE_SRC} ${TADPOLE_HEADERS})
	set_target_properties (toadlet_tadpole PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_tadpole toadlet_egg toadlet_peeper toadlet_ribbit ${TADPOLE_LIBS} ${TADPOLE_PLUGINS_LIBS})
	install (TARGETS toadlet_tadpole DESTINATION ${LIB_INSTALL_DIR})
endif (NOT TOADLET_PLATFORM_IOS)


# Static library
add_library (toadlet_tadpole_s STATIC ${TADPOLE_SRC_S} ${TADPOLE_HEADERS_S})
set_target_properties (toadlet_tadpole_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_tadpole_s toadlet_egg_s toadlet_peeper_s toadlet_ribbit_s ${TADPOLE_LIBS})
MERGE_STATIC_LIBRARIES (toadlet_tadpole_s "${TADPOLE_PLUGINS_LIBS_S}")
install (TARGETS toadlet_tadpole_s DESTINATION ${LIB_INSTALL_DIR})

# HACK: install the library on iOS. This will disappear once cmake issue:0012506 is resolved. 
if (TOADLET_PLATFORM_IOS)
    install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/ DESTINATION ${LIB_INSTALL_DIR} FILES_MATCHING PATTERN "*.a")
endif (TOADLET_PLATFORM_IOS)


# Add plugins
add_subdirectory (plugins/hop)

