# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_tadpole
set (TADPOLE_SRC
	AnaglyphCamera.cpp
	ActionComponent.cpp
	AudioComponent.cpp
	ArchiveManager.cpp
	AudioManager.cpp
	BaseComponent.cpp
	BufferManager.cpp
	Camera.cpp
	CameraComponent.cpp
	Engine.cpp
	Font.cpp
	FontManager.cpp
	LabelComponent.cpp
	LightComponent.cpp
	MaterialManager.cpp
	Mesh.cpp
	MeshComponent.cpp
	Node.cpp
	NodeManager.cpp
	Noise.cpp
	ParticleComponent.cpp
	PartitionNode.cpp
	RenderableSet.cpp
	ResourceManager.cpp
	Scene.cpp
	Sequence.cpp
	ShaderManager.cpp
	Skeleton.cpp
	SkeletonComponent.cpp
	SpriteComponent.cpp
	StereoscopicCamera.cpp
	Track.cpp
	TextureManager.cpp
	Version.cpp
)

set (TADPOLE_ACTION_SRC
	action/AnimationAction.cpp
	action/AudioAction.cpp
	action/AudioGainAnimation.cpp
	action/CompositeAction.cpp
	action/CompositeAnimation.cpp
	action/BaseAnimation.cpp
	action/CameraProjectionAnimation.cpp
	action/MaterialStateAnimation.cpp
	action/TextureStateAnimation.cpp
	action/TransformAnimation.cpp
)

set (TADPOLE_BSP_SRC
	bsp/BSP30Map.cpp
	bsp/BSP30MaterialCreator.cpp
	bsp/BSP30ModelComponent.cpp
	bsp/BSP30Node.cpp
	bsp/BSP30Streamer.cpp
	bsp/BSP30StyleComponent.cpp
)

set (TADPOLE_PLUGINS_SRC
	plugins/SimpleRenderManager.cpp
	plugins/DecalShadowRenderManager.cpp
	plugins/ShadowMappedRenderManager.cpp
	plugins/DiffuseMaterialCreator.cpp
	plugins/SkyBoxMaterialCreator.cpp
	plugins/AABoxMeshCreator.cpp
	plugins/GridMeshCreator.cpp
	plugins/GeoSphereMeshCreator.cpp
	plugins/NormalizationTextureCreator.cpp
	plugins/SkyBoxMeshCreator.cpp
	plugins/SkyDomeMeshCreator.cpp
	plugins/SphereMeshCreator.cpp
	plugins/TorusMeshCreator.cpp
	plugins/BMPStreamer.cpp
	plugins/DDSStreamer.cpp
	plugins/TGAStreamer.cpp
	plugins/TMSHStreamer.cpp
	plugins/TPKGArchive.cpp
	plugins/WADArchive.cpp
	plugins/WaterMaterialCreator.cpp
	plugins/WaveDecoder.cpp
)

set (TADPOLE_PLUGINS_XML_SRC
	plugins/toadlet_mxml.cpp
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

set (TADPOLE_PLUGINS_GMEDECODER_SRC
	plugins/GMEDecoder.cpp
)

set (TADPOLE_PLUGINS_MODDECODER_SRC
	plugins/MODDecoder.cpp
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

set (TADPOLE_PLUGINS_HOP_SRC
	plugins/HopComponent.cpp
	plugins/HopManager.cpp
)

set (TADPOLE_MATERIAL_SRC
	material/Material.cpp
	material/RenderPass.cpp
	material/RenderPath.cpp
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
	studio/SpriteModelComponent.cpp
	studio/StudioStreamer.cpp
	studio/StudioModelComponent.cpp
)

set (TADPOLE_TERRAIN_SRC
	terrain/DiffuseTerrainMaterialSource.cpp
	terrain/TerrainNode.cpp
	terrain/TerrainPatchComponent.cpp
	terrain/TextureDataSource.cpp
)

set (TADPOLE_PLATFORM_ANDROID_SRC
	platform/android/AndroidTextureStreamer.cpp
)


set (TADPOLE_PLATFORM_OSX_SRC
	platform/osx/OSXFontStreamer.mm
	platform/osx/OSXTextureStreamer.mm
)

set (TADPOLE_PLATFORM_WIN32_SRC
	platform/win32/Win32TextureResourceArchive.cpp
)

set (TADPOLE_PLATFORM_WIN32_GDIPLUS_SRC
	platform/win32/Win32FontStreamer.cpp
	platform/win32/Win32TextureStreamer.cpp
)


# Headers
set (TADPOLE_HDR
	AnaglyphCamera.h
	ActionComponent.h
	Animatable.h
	Attachable.h
	ArchiveManager.h
	AudioStreamer.h
	AudioManager.h
	AudioComponent.h
	AudioData.h
	BaseComponent.h
	Bound.h
	BufferManager.h
	Camera.h
	CameraComponent.h
	Colors.h
	Component.h
	ContextListener.h
	Engine.h
	Font.h
	FontData.h
	FontManager.h
	LabelComponent.h
	LightComponent.h
	MaterialManager.h
	Mesh.h
	MeshComponent.h
	Node.h
	NodeManager.h
	Noise.h
	ParticleComponent.h
	PartitionNode.h
	ProgressListener.h
	PhysicsCollision.h
	PhysicsCollisionListener.h
	PhysicsComponent.h
	PhysicsManager.h
	Renderable.h
	ResourceCreator.h
	ResourceData.h
	ResourceManager.h
	ResourceStreamer.h
	RenderManager.h
	RenderableSet.h
	Scene.h
	Sequence.h
	ShaderManager.h
	Skeleton.h
	SkeletonComponent.h
	SpriteComponent.h
	StereoscopicCamera.h
	TextureData.h
	TextureManager.h
	PhysicsTraceable.h
	Track.h
	Transform.h
	TransformListener.h
	Types.h
	UpdateListener.h
	VertexFormats.h
	Version.h
	VideoController.h
	VideoHandler.h
	Visible.h
)

set (TADPOLE_ACTION_HDR
	action/AnimationAction.h
	action/AudioAction.h
	action/AudioGainAnimation.h
	action/Action.h
	action/ActionListener.h
	action/CompositeAction.h
	action/CompositeAnimation.h
	action/Animation.h
	action/AnimationListener.h
	action/BaseAnimation.h
	action/CameraProjectionAnimation.h
	action/MaterialStateAnimation.h
	action/TextureStateAnimation.h
	action/TransformAnimation.h
)

set (TADPOLE_BSP_HDR
	bsp/BSP30Map.h
	bsp/BSP30MaterialCreator.h
	bsp/BSP30ModelComponent.h
	bsp/BSP30Node.h
	bsp/BSP30Streamer.h
	bsp/BSP30StyleComponent.h
	bsp/BSP30Types.h
)

set (TADPOLE_PLUGINS_HDR
	plugins/SimpleRenderManager.h
	plugins/DecalShadowRenderManager.h
	plugins/ShadowMappedRenderManager.h
	plugins/DiffuseMaterialCreator.h
	plugins/SkyBoxMaterialCreator.h
	plugins/AABoxMeshCreator.h
	plugins/GridMeshCreator.h
	plugins/GeoSphereMeshCreator.h
	plugins/NormalizationTextureCreator.h
	plugins/SkyBoxMeshCreator.h
	plugins/SkyDomeMeshCreator.h
	plugins/TorusMeshCreator.h
	plugins/SphereMeshCreator.h
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
	plugins/WaterMaterialCreator.h
)

set (TADPOLE_PLUGINS_XML_HDR
	plugins/toadlet_mxml.h
	plugins/XANMStreamer.h
	plugins/XMLMeshUtilities.h
	plugins/XMATStreamer.h
	plugins/XMSHStreamer.h
)

set (TADPOLE_PLUGINS_GIF_HDR
	plugins/GIFStreamer.h
)

set (TADPOLE_PLUGINS_JPEG_HDR
	plugins/JPEGStreamer.h
)

set (TADPOLE_PLUGINS_PNG_HDR
	plugins/PNGStreamer.h
)

set (TADPOLE_PLUGINS_FREETYPE_HDR
	plugins/FreeTypeStreamer.h
)

set (TADPOLE_PLUGINS_FFMPEG_HDR
	plugins/FFmpegStreamContext.h
	plugins/FFmpegVideoHandler.h
)

set (TADPOLE_PLUGINS_GMEDECODER_HDR
	plugins/GMEDecoder.h
	plugins/GMEStreamer.h
)

set (TADPOLE_PLUGINS_MODDECODER_HDR
	plugins/MODDecoder.h
	plugins/MODStreamer.h
)

set (TADPOLE_PLUGINS_OGGVORBISDECODER_HDR
	plugins/OggVorbisDecoder.h
)

set (TADPOLE_PLUGINS_SIDDECODER_HDR
	plugins/SIDDecoder.h
	plugins/SIDStreamer.h
)

set (TADPOLE_PLUGINS_ZIP_HDR
	plugins/ZIPArchive.h
	plugins/ZIPStream.h
)

set (TADPOLE_PLUGINS_HOP_HDR
	plugins/HopComponent.h
	plugins/HopManager.h
)

set (TADPOLE_MATERIAL_HDR
	material/Material.h
	material/RenderPass.h
	material/RenderPath.h
	material/RenderVariable.h
	material/RenderVariables.h
	material/SceneParameters.h
)

set (TADPOLE_SENSOR_HDR
	sensor/BoundingVolumeSensor.h
	sensor/InterfaceSensor.h
	sensor/NameSensor.h
	sensor/PotentiallyVisibleSensor.h
	sensor/Sensor.h
	sensor/SensorResults.h
	sensor/SensorResultsListener.h
	sensor/TypeSensor.h
)

set (TADPOLE_STUDIO_HDR
	studio/SpriteStreamer.h
	studio/SpriteModel.h
	studio/SpriteModelComponent.h
	studio/SpriteTypes.h
	studio/StudioStreamer.h
	studio/StudioModel.h
	studio/StudioModelComponent.h
	studio/StudioTypes.h
)

set (TADPOLE_TERRAIN_HDR
	terrain/DiffuseTerrainMaterialSource.h
	terrain/TerrainNodeDataSource.h
	terrain/TerrainNodeMaterialSource.h
	terrain/TerrainNodeListener.h
	terrain/TerrainNode.h
	terrain/TerrainPatchComponent.h
	terrain/TextureDataSource.h
)

set (TADPOLE_PLATFORM_ANDROID_HDR
	platform/android/AndroidTextureStreamer.h
)

set (TADPOLE_PLATFORM_OSX_HDR
	platform/osx/OSXFontStreamer.h
	platform/osx/OSXTextureStreamer.h
) 

set (TADPOLE_PLATFORM_WIN32_HDR
	platform/win32/Win32TextureResourceArchive.h
)

set (TADPOLE_PLATFORM_WIN32_GDIPLUS_HDR
	platform/win32/StreamIStream.h
	platform/win32/Win32FontStreamer.h
	platform/win32/Win32TextureStreamer.h
)


# Bundle source
source_group ("Source Files\\\\action" FILES ${TADPOLE_ACTION_SRC})
source_group ("Source Files\\\\material" FILES ${TADPOLE_MATERIAL_SRC})
source_group ("Source Files\\\\sensor" FILES ${TADPOLE_SENSOR_SRC})
source_group ("Source Files\\\\terrain" FILES ${TADPOLE_TERRAIN_SRC})


# Bundle headers
source_group ("Header Files\\\\action" FILES ${TADPOLE_ACTION_HDR})
source_group ("Header Files\\\\material" FILES ${TADPOLE_MATERIAL_HDR})
source_group ("Header Files\\\\sensor" FILES ${TADPOLE_SENSOR_HDR})
source_group ("Header Files\\\\terrain" FILES ${TADPOLE_TERRAIN_HDR})


# Sources, headers and namespaces
set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_ACTION_SRC} ${TADPOLE_PLUGINS_SRC} ${TADPOLE_MATERIAL_SRC} ${TADPOLE_SENSOR_SRC} ${TADPOLE_TERRAIN_SRC})
set (TADPOLE_HDR ${TADPOLE_HDR} ${TADPOLE_ACTION_HDR} ${TADPOLE_MATERIAL_HDR} ${TADPOLE_SENSOR_HDR} ${TADPOLE_TERRAIN_HDR})
set (TADPOLE_NAMESPACES ${TADPOLE_NAMESPACES} action material node sensor terrain)


# Fixed point builds use fewer features
if (NOT TOADLET_FIXED_POINT)
	source_group ("Source Files\\\\bsp" FILES ${TADPOLE_BSP_SRC})
	source_group ("Header Files\\\\bsp" FILES ${TADPOLE_BSP_HDR})
	source_group ("Source Files\\\\studio" FILES ${TADPOLE_STUDIO_SRC})
	source_group ("Header Files\\\\studio" FILES ${TADPOLE_STUDIO_HDR})
	set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_BSP_SRC} ${TADPOLE_STUDIO_SRC})
	set (TADPOLE_HDR ${TADPOLE_HDR} ${TADPOLE_BSP_HDR} ${TADPOLE_STUDIO_HDR})
	set (TADPOLE_NAMESPACES ${TADPOLE_NAMESPACES} bsp studio)
endif (NOT TOADLET_FIXED_POINT)


# Give the headers and namespaces parent scope for inclusion into the master tadpole.h file
set (TADPOLE_HDR ${TADPOLE_HDR} PARENT_SCOPE)
set (TADPOLE_NAMESPACES ${TADPOLE_NAMESPACES} PARENT_SCOPE)


# Setup dependecy specific files and libraries

# Freetype is not required on OSX
find_package (Freetype)
if (FREETYPE_FOUND)
	set (TOADLET_HAS_FREETYPE ON PARENT_SCOPE)
	include_directories (${FREETYPE_INCLUDE_DIRS})
	set (TADPOLE_PLUGINS_HDR ${TADPOLE_PLUGINS_HDR} ${TADPOLE_PLUGINS_FREETYPE_HDR})

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
	set (TADPOLE_PLUGINS_HDR ${TADPOLE_PLUGINS_HDR} ${TADPOLE_PLUGINS_XML_HDR})

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
	set (TADPOLE_PLUGINS_HDR ${TADPOLE_PLUGINS_HDR} ${TADPOLE_PLUGINS_GIF_HDR})

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
	set (TADPOLE_PLUGINS_HDR ${TADPOLE_PLUGINS_HDR} ${TADPOLE_PLUGINS_JPEG_HDR})
	
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
	set (TADPOLE_PLUGINS_HDR ${TADPOLE_PLUGINS_HDR} ${TADPOLE_PLUGINS_PNG_HDR})
	
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
	set (TADPOLE_PLUGINS_HDR ${TADPOLE_PLUGINS_HDR} ${TADPOLE_PLUGINS_ZIP_HDR})
	
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
	set (TADPOLE_PLUGINS_HDR ${TADPOLE_PLUGINS_HDR} ${TADPOLE_PLUGINS_SIDDECODER_HDR})

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

find_path (GME_INCLUDES gme/gme.h)
find_library (GME_LIBRARY NAMES gme)
if (GME_INCLUDES AND GME_LIBRARY)
	message(STATUS "GME Found")

	set (TOADLET_HAS_GME ON PARENT_SCOPE)
	include_directories (${GME_INCLUDES})
	
	# Dynamic Library
	set (TADPOLE_PLUGINS_SRC ${TADPOLE_PLUGINS_SRC} ${TADPOLE_PLUGINS_GMEDECODER_SRC})
	set (TADPOLE_PLUGINS_LIBS ${TADPOLE_PLUGINS_LIBS} ${GME_LIBRARY})
	
	# Static Library
	LOCATE_STATIC_LIBRARIES (GME_LIBRARY_S "${GME_LIBRARY}")
	if (GME_LIBRARY_S)
		set (TADPOLE_PLUGINS_SRC_S ${TADPOLE_PLUGINS_SRC_S} ${TADPOLE_PLUGINS_GMEDECODER_SRC})
		set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} ${GME_LIBRARY_S})
	endif (GME_LIBRARY_S)
elseif (GME_INCLUDES AND GME_LIBRARY)
	message(STATUS "Could NOT find GME")
endif (GME_INCLUDES AND GME_LIBRARY)

find_path (MODPLUG_INCLUDES modplug/modplug.h)
find_library (MODPLUG_LIBRARY NAMES modplug)
if (MODPLUG_INCLUDES AND MODPLUG_LIBRARY)
	message(STATUS "ModPlug Found")

	set (TOADLET_HAS_MODPLUG ON PARENT_SCOPE)
	include_directories (${MODPLUG_INCLUDES})
	
	# Dynamic Library
	set (TADPOLE_PLUGINS_SRC ${TADPOLE_PLUGINS_SRC} ${TADPOLE_PLUGINS_MODDECODER_SRC})
	set (TADPOLE_PLUGINS_LIBS ${TADPOLE_PLUGINS_LIBS} ${MODPLUG_LIBRARY})
	
	# Static Library
	LOCATE_STATIC_LIBRARIES (MODPLUG_LIBRARY_S "${MODPLUG_LIBRARY}")
	if (MODPLUG_LIBRARY_S)
		set (TADPOLE_PLUGINS_SRC_S ${TADPOLE_PLUGINS_SRC_S} ${TADPOLE_PLUGINS_MODDECODER_SRC})
		set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} ${MODPLUG_LIBRARY_S})
	endif (MODPLUG_LIBRARY_S)
elseif (MODPLUG_INCLUDES AND MODPLUG_LIBRARY)
	message(STATUS "Could NOT find ModPlug")
endif (MODPLUG_INCLUDES AND MODPLUG_LIBRARY)

find_package (FFmpeg)
if (FFMPEG_FOUND)
	set (TOADLET_HAS_FFMPEG ON PARENT_SCOPE)
	include_directories (${FFMPEG_INCLUDE_DIR})
	set (TADPOLE_PLUGINS_HDR ${TADPOLE_PLUGINS_HDR} ${TADPOLE_PLUGINS_FFMPEG_HDR})

	# Dynamic Library
	set (TADPOLE_PLUGINS_SRC ${TADPOLE_PLUGINS_SRC} ${TADPOLE_PLUGINS_FFMPEG_SRC})
	set (TADPOLE_PLUGINS_LIBS ${TADPOLE_PLUGINS_LIBS} ${FFMPEG_LIBRARIES})

	if (TOADLET_PLATFORM_POSIX)
		# HACK: FFmpeg requires this definition for 32bit posix
		set_source_files_properties (${TADPOLE_SRC} PROPERTIES COMPILE_DEFINITIONS "__STDC_CONSTANT_MACROS")
		set_source_files_properties (${TADPOLE_PLUGINS_SRC} PROPERTIES COMPILE_DEFINITIONS "__STDC_CONSTANT_MACROS")
	endif (TOADLET_PLATFORM_POSIX)

	# Static Library
	LOCATE_STATIC_LIBRARIES (FFMPEG_LIBRARIES_S "${FFMPEG_LIBRARIES}")
	if (FFMPEG_LIBRARIES_S)
		set (TADPOLE_PLUGINS_SRC_S ${TADPOLE_PLUGINS_SRC_S} ${TADPOLE_PLUGINS_FFMPEG_SRC})
		set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} ${FFMPEG_LIBRARIES_S})
		if (TOADLET_PLATFORM_WIN32)
			# HACK: FFmpeg on windows incorrectly exposes symbols from zlib, which then generate warnings.
			# We can hide them by passing ignore flags directly to the linker, as if they were libraries.
			set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} "/ignore:4006" "/ignore:4221")
		elseif (TOADLET_PLATFORM_POSIX)
			set_source_files_properties (${TADPOLE_SRC} PROPERTIES COMPILE_DEFINITIONS "__STDC_CONSTANT_MACROS")
			set_source_files_properties (${TADPOLE_PLUGINS_SRC} PROPERTIES COMPILE_DEFINITIONS "__STDC_CONSTANT_MACROS")
		endif (TOADLET_PLATFORM_WIN32)
	endif (FFMPEG_LIBRARIES_S)
endif (FFMPEG_FOUND)

#TODO: Depreicate ogg vorbis in favor of ffmpeg. Right now it serves as a backup.
if (NOT FFMPEG_FOUND)
	find_package (OggVorbis)
	if (OGGVORBIS_FOUND)
		set (TOADLET_HAS_OGGVORBIS ON PARENT_SCOPE)
		include_directories (${OGGVORBIS_INCLUDES})
		set (TADPOLE_PLUGINS_HDR ${TADPOLE_PLUGINS_HDR} ${TADPOLE_PLUGINS_OGGVORBISDECODER_HDR})

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
	source_group ("Header Files\\\\platform\\\\android" FILES ${TADPOLE_PLATFORM_ANDROID_HDR})
	set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_PLATFORM_ANDROID_SRC})
	set (TADPOLE_LIBS ${TADPOLE_LIBS} toadlet_ribbit_jaudiodevice)
elseif (TOADLET_PLATFORM_OSX)
	source_group ("Source Files\\\\platform\\\\osx" FILES ${TADPOLE_PLATFORM_OSX_SRC})
	source_group ("Header Files\\\\platform\\\\osx" FILES ${TADPOLE_PLATFORM_OSX_HDR})
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
find_library(GDIPLUS_LIBRARY NAMES gdiplus)
message ("GDI PLUS LIBRARY=${GDIPLUS_LIBRARY}")
	if (GDIPLUS_FOUND)
		set (TOADLET_HAS_GDIPLUS ON PARENT_SCOPE)
		include_directories (${GDIPLUS_INCLUDE_PATH})
		source_group ("Source Files\\\\platform\\\\win32" FILES ${TADPOLE_PLATFORM_WIN32_GDIPLUS_SRC})
		source_group ("Header Files\\\\platform\\\\win32" FILES ${TADPOLE_PLATFORM_WIN32_GDIPLUS_HDR})
		set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_PLATFORM_WIN32_GDIPLUS_SRC})
	endif (GDIPLUS_FOUND)
	source_group ("Source Files\\\\platform\\\\win32" FILES ${TADPOLE_PLATFORM_WIN32_SRC})
	source_group ("Header Files\\\\platform\\\\win32" FILES ${TADPOLE_PLATFORM_WIN32_HDR})
	set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_PLATFORM_WIN32_SRC})
endif (TOADLET_PLATFORM_ANDROID)

# Hop
set (TADPOLE_PLUGINS_SRC ${TADPOLE_PLUGINS_SRC} ${TADPOLE_PLUGINS_HOP_SRC})
set (TADPOLE_PLUGINS_HDR ${TADPOLE_PLUGINS_HDR} ${TADPOLE_PLUGINS_HOP_HDR})
set (TADPOLE_PLUGINS_LIBS ${TADPOLE_PLUGINS_LIBS} toadlet_hop)
#set (TADPOLE_PLUGINS_LIBS_S ${TADPOLE_PLUGINS_LIBS_S} toadlet_hop_s)


# Bring in dependency results
source_group ("Source Files\\\\plugins" FILES ${TADPOLE_PLUGINS_SRC})
source_group ("Header Files\\\\plugins" FILES ${TADPOLE_PLUGINS_HDR})
set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_PLUGINS_SRC})
set (TADPOLE_SRC_S ${TADPOLE_SRC} ${TADPOLE_PLUGINS_SRC_S})


# If we are using swig create the wrapper file from the interface files
if (SWIG_FOUND AND TADPOLE_SWIG_INT)
	swig_wrap_files (tadpole_wrap.cpp tadpole ${TADPOLE_SWIG_INT} "${TADPOLE_SWIG_INT_DEPS}" "${TADPOLE_SWIG_INT_OUTPUTS}")
	set (TADPOLE_SRC ${TADPOLE_SRC} tadpole_wrap.cpp)
endif (SWIG_FOUND AND TADPOLE_SWIG_INT)


# For individual component installation 
set (TADPOLE_INSTALL)


# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_tadpole SHARED ${TADPOLE_SRC} ${TADPOLE_HDR})
	set_target_properties (toadlet_tadpole PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_tadpole toadlet_egg toadlet_peeper toadlet_ribbit ${TADPOLE_LIBS} ${TADPOLE_PLUGINS_LIBS})
	install (TARGETS toadlet_tadpole DESTINATION ${LIB_INSTALL_DIR} COMPONENT tadpole)
	set (TADPOLE_INSTALL ${TADPOLE_INSTALL} toadlet_tadpole)
endif (TOADLET_BUILD_DYNAMIC)


# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_tadpole_s STATIC ${TADPOLE_SRC_S} ${TADPOLE_HDR_S})
	set_target_properties (toadlet_tadpole_s PROPERTIES ${STATIC_LIB_PROPS})
	target_link_libraries (toadlet_tadpole_s toadlet_egg_s toadlet_peeper_s toadlet_ribbit_s ${TADPOLE_LIBS})
	MERGE_STATIC_LIBRARIES (toadlet_tadpole_s ALL "${TADPOLE_PLUGINS_LIBS_S}")
	install (TARGETS toadlet_tadpole_s DESTINATION ${LIB_INSTALL_DIR} COMPONENT tadpole)
	set (TADPOLE_INSTALL ${TADPOLE_INSTALL} toadlet_tadpole_s)

	# HACK: install the library on iOS. This will disappear once cmake issue:0012506 is resolved. 
	if (TOADLET_PLATFORM_IOS)
	    install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/\$ENV{CONFIGURATION}\$ENV{EFFECTIVE_PLATFORM_NAME}/ DESTINATION ${LIB_INSTALL_DIR} COMPONENT tadpole FILES_MATCHING PATTERN "*.a")
	endif (TOADLET_PLATFORM_IOS)
endif (TOADLET_BUILD_STATIC)


# Rule to install only tadpole
add_custom_target (tadpole_install
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=required -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=egg -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=peeper -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=ribbit -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIGURATION> -DCOMPONENT=tadpole -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
	DEPENDS ${TADPOLE_INSTALL}
)
