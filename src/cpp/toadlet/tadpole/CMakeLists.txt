# Setup Versions
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/Version.h)


# Source files for toadlet_tadpole
set (TADPOLE_SRC
	BufferManager.cpp
	Engine.cpp
	Font.cpp
	Material.cpp
	Mesh.cpp
	MeshSkeleton.cpp
	MeshSkeletonSequence.cpp
	TPKGInputStreamFactory.cpp
	Track.cpp
	ResourceManager.cpp
	MaterialManager.cpp
	MeshManager.cpp
	TextureManager.cpp
	Version.cpp
)

set (TADPOLE_ANIMATION_SRC
	animation/AnimationController.cpp
	animation/CameraProjectionAnimation.cpp
	animation/EntityPathAnimation.cpp
	animation/SkeletonAnimation.cpp
)

set (TADPOLE_ENTITY_SRC
	entity/AnimationControllerEntity.cpp
	entity/AudioEntity.cpp
	entity/CameraEntity.cpp
	entity/Entity.cpp
	entity/LightEntity.cpp
	entity/MeshEntity.cpp
	entity/MeshEntitySkeleton.cpp
	entity/ParentEntity.cpp
	entity/ParticleEntity.cpp
	entity/RenderableEntity.cpp
	entity/Scene.cpp
	entity/SkeletonParentEntity.cpp
	entity/SpriteEntity.cpp
)

set (TADPOLE_HANDLER_SRC
	handler/AudioBufferHandler.cpp
	handler/MMSHHandler.cpp
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
	handler/platform/osx/OSXFontHandler.mm
	handler/platform/osx/OSXTextureHandler.mm
)

set (TADPOLE_HANDLER_PLATFORM_WIN32_SRC
	handler/platform/win32/Win32FontHandler.cpp
)

set (TADPOLE_MESH_SRC
	mesh/Mesh.cpp
	mesh/Sequence.cpp
	mesh/Skeleton.cpp
)

set (TADPOLE_WIDGET_SRC
	widget/AnimationControllerWidget.cpp
	widget/BorderWidget.cpp
	widget/ButtonWidget.cpp
	widget/LabelWidget.cpp
	widget/ParentWidget.cpp
	widget/RenderableWidget.cpp
	widget/SpriteWidget.cpp
	widget/Widget.cpp
	widget/Window.cpp
)


# Headers
set (TADPOLE_HEADERS
	AudioBufferData.h
	BufferManager.h
	ContextResourceManager.h
	Engine.h
	Font.h
	FontData.h
	KeyFrame.h
	Material.h
	Mesh.h
	MeshSkeleton.h
	MeshSkeletonSequence.h
	Renderable.h
	ResourceHandler.h
	ResourceHandlerData.h
	ResourceManager.h
	MaterialManager.h
	MeshManager.h
	TextureManager.h
	TPKGInputStreamFactory.h
	Track.h
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
	animation/EntityPathAnimation.h
	animation/SkeletonAnimation.h
)

set (TADPOLE_ENTITY_HEADERS
	entity/AnimationControllerEntity.h
	entity/AudioEntity.h
	entity/CameraEntity.h
	entity/Entity.h
	entity/EntityDestroyedListener.h
	entity/LightEntity.h
	entity/MeshEntity.h
	entity/MeshEntitySkeleton.h
	entity/ParentEntity.h
	entity/ParticleEntity.h
	entity/RenderableEntity.h
	entity/Scene.h
	entity/SkeletonParentEntity.h
	entity/SpriteEntity.h
)

set (TADPOLE_HANDLER_HEADERS
	handler/AudioBufferHandler.h
	handler/BMPHandler.h
	handler/GIFHandler.h
	handler/JPEGHandler.h
	handler/MMSHHandler.h
	handler/PNGHandler.h
	handler/RGBHandler.h
	handler/SPRHandler.h
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
	handler/platform/osx/OSXFontHandler.h
	handler/platform/osx/OSXTextureHandler.h
) 

set (TADPOLE_HANDLER_PLATFORM_WIN32_HEADERS
	handler/platform/win32/Win32FontHandler.h
) 

set (TADPOLE_MESH_HEADERS
	mesh/Mesh.h
	mesh/Sequence.h
	mesh/Skeleton.h
)

set (TADPOLE_WIDGET_HEADERS
	widget/ActionListener.h
	widget/AnimationControllerWidget.h
	widget/BorderWidget.h
	widget/ButtonWidget.h
	widget/LabelWidget.h
	widget/ParentWidget.h
	widget/RenderableWidget.h
	widget/SpriteWidget.h
	widget/Widget.h
	widget/WidgetDestroyedListener.h
	widget/Window.h
) 


# Bundle source
source_group ("Source Files\\\\animation" FILES ${TADPOLE_ANIMATION_SRC})
source_group ("Source Files\\\\entity" FILES ${TADPOLE_ENTITY_SRC})
source_group ("Source Files\\\\handler" FILES ${TADPOLE_HANDLER_SRC})
source_group ("Source Files\\\\mesh" FILES ${TADPOLE_MESH_SRC})
source_group ("Source Files\\\\widget" FILES ${TADPOLE_WIDGET_SRC})


# Bundle headers
source_group ("Header Files\\\\animation" FILES ${TADPOLE_ANIMATION_HEADERS})
source_group ("Header Files\\\\entity" FILES ${TADPOLE_ENTITY_HEADERS})
source_group ("Header Files\\\\handler" FILES ${TADPOLE_HANDLER_HEADERS})
source_group ("Header Files\\\\mesh" FILES ${TADPOLE_MESH_HEADERS})
source_group ("Header Files\\\\widget" FILES ${TADPOLE_WIDGET_HEADERS})


# Source and headers
set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_ANIMATION_SRC} ${TADPOLE_ENTITY_SRC} ${TADPOLE_HANDLER_SRC} ${TADPOLE_MESH_SRC} ${TADPOLE_WIDGET_SRC})
set (TADPOLE_HEADERS ${TADPOLE_HEADERS} ${TADPOLE_ANIMATION_HEADERS} ${TADPOLE_ENTITY_HEADERS} ${TADPOLE_HANDLER_HEADERS} ${TADPOLE_MESH_HEADERS} ${TADPOLE_WIDGET_HEADERS})


# Dependencies for toadlet_tadpole
find_package (Freetype)
find_package (MXML)


# Setup dependecy specific files and libraries
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
	if (TOADLET_DEPENDENCY_EXTENSION_NAMES)
		string (REPLACE ".lib" "_s.lib" FREETYPE_LIBRARY ${FREETYPE_LIBRARY})
	endif (TOADLET_DEPENDENCY_EXTENSION_NAMES)
	set_property (TARGET libfreetype_s PROPERTY IMPORTED_LOCATION ${FREETYPE_LIBRARY})
	set (TADPOLE_DEFS_S TOADLET_LIBFREETYPE_NAME="${FREETYPE_LIBRARY}")
	set (TADPOLE_LIBS_S libfreetype_s)
endif (FREETYPE_FOUND)
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
        if (TOADLET_DEPENDENCY_EXTENSION_NAMES)
                string (REPLACE ".lib" "_s.lib" MXML_LIBRARY ${MXML_LIBRARY})
        endif (TOADLET_DEPENDENCY_EXTENSION_NAMES)
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

