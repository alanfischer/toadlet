# With java, you have the option of building separate jars for each library
option (TOADLET_JAVA_SPLIT_JARS "Build separate jars for each library instead of a single one." OFF)

# Java is really fussy about pathing, so we do all setup here instead of
# in subdirectories. For simplicity, define our paths here
set (JAVA_SRC_DIR com/lightningtoads/toadlet)
set (JAVA_GPP_DIR ${CMAKE_CURRENT_BINARY_DIR}/gpp)

# We include the JAVA_GPP_DIR files since those have been preprocessed
include_directories (${JAVA_GPP_DIR})
if (TOADLET_PLATFORM_ANDROID)
	include_directories(${ANDROID_JAR})
elseif (TOADLET_PLATFORM_BLACKBERRY)
	include_directories(${BLACKBERRY_JDE_DIR}/lib/net_rim_api.jar)
endif (TOADLET_PLATFORM_ANDROID)

# Configure and copy the Config and Types file
configure_file (${JAVA_SRC_DIR}/Config.h.in ${JAVA_GPP_DIR}/${JAVA_SRC_DIR}/Config.h)
configure_file (${JAVA_SRC_DIR}/Types.h ${JAVA_GPP_DIR}/${JAVA_SRC_DIR}/Types.h COPYONLY)

# Special setup for java cmake compiling - MAKE THESE AN OVERRIDE FILE?
set (CMAKE_Java_COMPILE_OBJECT
	"<CMAKE_Java_COMPILER> -Xlint:unchecked <FLAGS> <SOURCE>"
)
set (CMAKE_Java_CREATE_STATIC_LIBRARY
	"<CMAKE_Java_ARCHIVE> -cf <TARGET> -C <OBJECT_DIR>/gpp ."
)


# Building toadlet_egg

# Configure
configure_file (${JAVA_SRC_DIR}/egg/Version.h.in ${JAVA_GPP_DIR}/${JAVA_SRC_DIR}/egg/Version.h)

# Source files
set (EGG_SRC
	${JAVA_SRC_DIR}/egg/Categories.java
	${JAVA_SRC_DIR}/egg/Error.java
	${JAVA_SRC_DIR}/egg/Event.java
	${JAVA_SRC_DIR}/egg/EventFactory.java
	${JAVA_SRC_DIR}/egg/Logger.java
	${JAVA_SRC_DIR}/egg/LoggerListener.java
	${JAVA_SRC_DIR}/egg/MathConversion.java
	${JAVA_SRC_DIR}/egg/ObjectFactory.java
	${JAVA_SRC_DIR}/egg/ObjectManager.java
	${JAVA_SRC_DIR}/egg/Profile.java
	${JAVA_SRC_DIR}/egg/Resource.java
	${JAVA_SRC_DIR}/egg/Version.java
)
set (EGG_IMAGE_SRC 
	${JAVA_SRC_DIR}/egg/image/ImageDefinitions.java
	${JAVA_SRC_DIR}/egg/image/ImageFormatConversion.java
)
set (EGG_IO_SRC 
	${JAVA_SRC_DIR}/egg/io/InputStreamFactory.java
)
set (EGG_MATH_SRC 
	${JAVA_SRC_DIR}/egg/math/Plane.java
	${JAVA_SRC_DIR}/egg/math/Math.java
	${JAVA_SRC_DIR}/egg/math/Vector2.java
	${JAVA_SRC_DIR}/egg/math/EulerAngle.java
	${JAVA_SRC_DIR}/egg/math/Vector4.java
	${JAVA_SRC_DIR}/egg/math/Matrix3x3.java
	${JAVA_SRC_DIR}/egg/math/Matrix4x4.java
	${JAVA_SRC_DIR}/egg/math/Sphere.java
	${JAVA_SRC_DIR}/egg/math/AABox.java
	${JAVA_SRC_DIR}/egg/math/Quaternion.java
	${JAVA_SRC_DIR}/egg/math/Capsule.java
	${JAVA_SRC_DIR}/egg/math/Segment.java
	${JAVA_SRC_DIR}/egg/math/Vector3.java
	${JAVA_SRC_DIR}/egg/math/ConvexSolid.java
)
set (EGG_MATHFIXED_SRC
	${JAVA_SRC_DIR}/egg/mathfixed/Plane.java
	${JAVA_SRC_DIR}/egg/mathfixed/Math.java
	${JAVA_SRC_DIR}/egg/mathfixed/Vector2.java
	${JAVA_SRC_DIR}/egg/mathfixed/EulerAngle.java
	${JAVA_SRC_DIR}/egg/mathfixed/Vector4.java
	${JAVA_SRC_DIR}/egg/mathfixed/Matrix3x3.java
	${JAVA_SRC_DIR}/egg/mathfixed/Matrix4x4.java
	${JAVA_SRC_DIR}/egg/mathfixed/Sphere.java
	${JAVA_SRC_DIR}/egg/mathfixed/AABox.java
	${JAVA_SRC_DIR}/egg/mathfixed/Quaternion.java
	${JAVA_SRC_DIR}/egg/mathfixed/Capsule.java
	${JAVA_SRC_DIR}/egg/mathfixed/Segment.java
	${JAVA_SRC_DIR}/egg/mathfixed/Vector3.java
	${JAVA_SRC_DIR}/egg/mathfixed/ConvexSolid.java
)

# Bundle source files
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JAVA_GPP_DIR}/com/lightningtoads/toadlet/egg)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JAVA_GPP_DIR}/com/lightningtoads/toadlet/egg/image)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JAVA_GPP_DIR}/com/lightningtoads/toadlet/egg/io)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JAVA_GPP_DIR}/com/lightningtoads/toadlet/egg/math)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JAVA_GPP_DIR}/com/lightningtoads/toadlet/egg/mathfixed)
source_group ("Source Files\\\\image" FILES ${EGG_IMAGE_SRC})
source_group ("Source Files\\\\io" FILES ${EGG_IMAGE_SRC})
source_group ("Source Files\\\\math" FILES ${EGG_MATH_SRC})
source_group ("Source Files\\\\mathfixed" FILES ${EGG_MATHFIXED_SRC})
set (EGG_SRC ${EGG_SRC} ${EGG_IMAGE_SRC} ${EGG_IO_SRC} ${EGG_MATH_SRC} ${EGG_MATHFIXED_SRC})

# Copy over source files and run gpp
foreach (SRCFILE ${EGG_SRC})
	add_custom_command (
		OUTPUT ${JAVA_GPP_DIR}/${SRCFILE}
		COMMAND gpp -J -o ${JAVA_GPP_DIR}/${SRCFILE} -I${JAVA_GPP_DIR} -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${SRCFILE}
		DEPENDS ${SRCFILE}
	)
	set (EGG_SRC_GPP ${EGG_SRC_GPP} ${JAVA_GPP_DIR}/${SRCFILE})
endforeach (SRCFILE)

#  Jar library
if (TOADLET_JAVA_SPLIT_JARS)
	add_library (toadlet_egg ${EGG_SRC_GPP})
	set_target_properties (toadlet_egg PROPERTIES COMPILE_FLAGS "-d \"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/toadlet_egg.dir/gpp\"")
	set_target_properties (toadlet_egg PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG)
	install (TARGETS toadlet_egg DESTINATION lib)
endif (TOADLET_JAVA_SPLIT_JARS)


# Building toadlet_hop

# Configure
configure_file (${JAVA_SRC_DIR}/hop/Version.h.in ${JAVA_GPP_DIR}/${JAVA_SRC_DIR}/hop/Version.h)
configure_file (${JAVA_SRC_DIR}/hop/Types.h ${JAVA_GPP_DIR}/${JAVA_SRC_DIR}/hop/Types.h COPYONLY)

# Source files
set (HOP_SRC
	${JAVA_SRC_DIR}/hop/Collision.java
	${JAVA_SRC_DIR}/hop/CollisionListener.java
	${JAVA_SRC_DIR}/hop/Constraint.java
	${JAVA_SRC_DIR}/hop/Manager.java
	${JAVA_SRC_DIR}/hop/Simulator.java
	${JAVA_SRC_DIR}/hop/Shape.java
	${JAVA_SRC_DIR}/hop/Solid.java
	${JAVA_SRC_DIR}/hop/Version.java
)

# Bundle source files
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JAVA_GPP_DIR}/com/lightningtoads/toadlet/hop)

# Copy over source files and run gpp
foreach (SRCFILE ${HOP_SRC})
	add_custom_command (
		OUTPUT ${JAVA_GPP_DIR}/${SRCFILE}
		COMMAND gpp -J -o ${JAVA_GPP_DIR}/${SRCFILE} -I${JAVA_GPP_DIR} -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${SRCFILE}
		DEPENDS ${SRCFILE}
	)
	set (HOP_SRC_GPP ${HOP_SRC_GPP} ${JAVA_GPP_DIR}/${SRCFILE})
endforeach (SRCFILE)

#  Jar library
if (TOADLET_JAVA_SPLIT_JARS)
	add_library (toadlet_hop STATIC ${HOP_SRC_GPP})
	set_target_properties (toadlet_hop PROPERTIES COMPILE_FLAGS "-d \"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/toadlet_hop.dir/gpp\"")
	set_target_properties (toadlet_hop PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG)
	add_dependencies (toadlet_hop toadlet_egg)
	install (TARGETS toadlet_hop DESTINATION lib)
endif (TOADLET_JAVA_SPLIT_JARS)


# Building toadlet_knot

# Configure
configure_file (${JAVA_SRC_DIR}/knot/Version.h.in ${JAVA_GPP_DIR}/${JAVA_SRC_DIR}/knot/Version.h)

# Source files
set (KNOT_SRC
	${JAVA_SRC_DIR}/knot/Connection.java
	${JAVA_SRC_DIR}/knot/PeerEventSynchronizer.java
	${JAVA_SRC_DIR}/knot/TCPConnection.java
	${JAVA_SRC_DIR}/knot/Version.java
)

# Bundle source files
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JAVA_GPP_DIR}/com/lightningtoads/toadlet/knot)

# Copy over source files and run gpp
foreach (SRCFILE ${KNOT_SRC})
	add_custom_command (
		OUTPUT ${JAVA_GPP_DIR}/${SRCFILE}
		COMMAND gpp -J -o ${JAVA_GPP_DIR}/${SRCFILE} -I${JAVA_GPP_DIR} -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${SRCFILE}
		DEPENDS ${SRCFILE}
	)
	set (KNOT_SRC_GPP ${KNOT_SRC_GPP} ${JAVA_GPP_DIR}/${SRCFILE})
endforeach (SRCFILE)

#  Jar library
if (TOADLET_JAVA_SPLIT_JARS)
	add_library (toadlet_knot STATIC ${KNOT_SRC_GPP})
	set_target_properties (toadlet_knot PROPERTIES COMPILE_FLAGS "-d \"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/toadlet_knot.dir/gpp\"")
	set_target_properties (toadlet_knot PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG)
	add_dependencies (toadlet_knot toadlet_egg)
	install (TARGETS toadlet_knot DESTINATION lib)
endif (TOADLET_JAVA_SPLIT_JARS)


# Building toadlet_peeper

# Configure
configure_file (${JAVA_SRC_DIR}/peeper/Version.h.in ${JAVA_GPP_DIR}/${JAVA_SRC_DIR}/peeper/Version.h)
configure_file (${JAVA_SRC_DIR}/peeper/Types.h ${JAVA_GPP_DIR}/${JAVA_SRC_DIR}/peeper/Types.h COPYONLY)

# Source files
set (PEEPER_SRC
	${JAVA_SRC_DIR}/peeper/AnimatedTexture.java
	${JAVA_SRC_DIR}/peeper/Blend.java
	${JAVA_SRC_DIR}/peeper/Buffer.java
	${JAVA_SRC_DIR}/peeper/CapabilitySet.java
	${JAVA_SRC_DIR}/peeper/Color.java
	${JAVA_SRC_DIR}/peeper/Colors.java
	${JAVA_SRC_DIR}/peeper/IndexBuffer.java
	${JAVA_SRC_DIR}/peeper/IndexBufferAccessor.java
	${JAVA_SRC_DIR}/peeper/IndexData.java
	${JAVA_SRC_DIR}/peeper/Light.java
	${JAVA_SRC_DIR}/peeper/LightEffect.java
	${JAVA_SRC_DIR}/peeper/RenderContext.java
	${JAVA_SRC_DIR}/peeper/RenderTarget.java
	${JAVA_SRC_DIR}/peeper/RenderTexture.java
	${JAVA_SRC_DIR}/peeper/Renderer.java
	${JAVA_SRC_DIR}/peeper/StatisticsSet.java
	${JAVA_SRC_DIR}/peeper/Texture.java
	${JAVA_SRC_DIR}/peeper/TextureBlend.java
	${JAVA_SRC_DIR}/peeper/TextureSection.java
	${JAVA_SRC_DIR}/peeper/TextureSequence.java
	${JAVA_SRC_DIR}/peeper/TextureStage.java
	${JAVA_SRC_DIR}/peeper/VertexBuffer.java
	${JAVA_SRC_DIR}/peeper/VertexBufferAccessor.java
	${JAVA_SRC_DIR}/peeper/VertexData.java
	${JAVA_SRC_DIR}/peeper/VertexElement.java
	${JAVA_SRC_DIR}/peeper/VertexFormat.java
	${JAVA_SRC_DIR}/peeper/Viewport.java
	${JAVA_SRC_DIR}/peeper/Visual.java
	${JAVA_SRC_DIR}/peeper/Version.java
)

# Bundle source files
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JAVA_GPP_DIR}/com/lightningtoads/toadlet/peeper)

# Copy over source files and run gpp
foreach (SRCFILE ${PEEPER_SRC})
	add_custom_command (
		OUTPUT ${JAVA_GPP_DIR}/${SRCFILE}
		COMMAND gpp -J -o ${JAVA_GPP_DIR}/${SRCFILE} -I${JAVA_GPP_DIR} -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${SRCFILE}
		DEPENDS ${SRCFILE}
	)
	set (PEEPER_SRC_GPP ${PEEPER_SRC_GPP} ${JAVA_GPP_DIR}/${SRCFILE})
endforeach (SRCFILE)

# Jar library is combined with peeper plugins


# Building toadlet_peeper_glrenderer

# Source files
set (PEEPER_GLRENDERER_SRC
	${JAVA_SRC_DIR}/peeper/plugins/glrenderer/EGLRenderContextPeer.java
	${JAVA_SRC_DIR}/peeper/plugins/glrenderer/EGLRenderTargetPeer.java
	${JAVA_SRC_DIR}/peeper/plugins/glrenderer/GLBufferPeer.java
	${JAVA_SRC_DIR}/peeper/plugins/glrenderer/GLRenderTargetPeer.java
	${JAVA_SRC_DIR}/peeper/plugins/glrenderer/GLRenderer.java
	${JAVA_SRC_DIR}/peeper/plugins/glrenderer/GLTexturePeer.java
)

# Bundle source files
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JAVA_GPP_DIR}/com/lightningtoads/toadlet/peeper/plugins/glrenderer)
source_group ("Source Files\\\\plugins\\\\glrenderer" FILES ${PEEPER_GLRENDERER_SRC})

# Copy over source files and run gpp
foreach (SRCFILE ${PEEPER_GLRENDERER_SRC})
	add_custom_command (
		OUTPUT ${JAVA_GPP_DIR}/${SRCFILE}
		COMMAND gpp -J -o ${JAVA_GPP_DIR}/${SRCFILE} -I${JAVA_GPP_DIR} -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${SRCFILE}
		DEPENDS ${SRCFILE}
	)
	set (PEEPER_GLRENDERER_SRC_GPP ${PEEPER_GLRENDERER_SRC_GPP} ${JAVA_GPP_DIR}/${SRCFILE})
endforeach (SRCFILE)

#  Jar library for peeper and it's plugins
if (TOADLET_JAVA_SPLIT_JARS)
	add_library (toadlet_peeper STATIC ${PEEPER_SRC_GPP} ${PEEPER_GLRENDERER_SRC_GPP})
	set_target_properties (toadlet_peeper PROPERTIES COMPILE_FLAGS "-d \"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/toadlet_peeper.dir/gpp\"")
	set_target_properties (toadlet_peeper PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG)
	add_dependencies (toadlet_peeper toadlet_egg)
	install (TARGETS toadlet_peeper DESTINATION lib)
endif (TOADLET_JAVA_SPLIT_JARS)


# Building toadlet_tadpole

# Configure and copy the types file
configure_file (${JAVA_SRC_DIR}/tadpole/Version.h.in ${JAVA_GPP_DIR}/${JAVA_SRC_DIR}/tadpole/Version.h)
configure_file (${JAVA_SRC_DIR}/tadpole/Types.h ${JAVA_GPP_DIR}/${JAVA_SRC_DIR}/tadpole/Types.h COPYONLY)

# Source files
set (TADPOLE_SRC
	${JAVA_SRC_DIR}/tadpole/AudioBufferData.java
	${JAVA_SRC_DIR}/tadpole/ContextResourceManager.java
	${JAVA_SRC_DIR}/tadpole/Engine.java
	${JAVA_SRC_DIR}/tadpole/Font.java
	${JAVA_SRC_DIR}/tadpole/FontData.java
	${JAVA_SRC_DIR}/tadpole/KeyFrame.java
	${JAVA_SRC_DIR}/tadpole/Material.java
	${JAVA_SRC_DIR}/tadpole/Mesh.java
	${JAVA_SRC_DIR}/tadpole/MeshSkeleton.java
	${JAVA_SRC_DIR}/tadpole/MeshSkeletonSequence.java
	${JAVA_SRC_DIR}/tadpole/Renderable.java
	${JAVA_SRC_DIR}/tadpole/ResourceHandler.java
	${JAVA_SRC_DIR}/tadpole/ResourceHandlerData.java
	${JAVA_SRC_DIR}/tadpole/ResourceManager.java
	${JAVA_SRC_DIR}/tadpole/TPKGInputStreamFactory.java
	${JAVA_SRC_DIR}/tadpole/Track.java
	${JAVA_SRC_DIR}/tadpole/UpdateListener.java
	${JAVA_SRC_DIR}/tadpole/VertexFormats.java
	${JAVA_SRC_DIR}/tadpole/Version.java
)
set (TADPOLE_ANIMATION_SRC
	${JAVA_SRC_DIR}/tadpole/animation/Animation.java
	${JAVA_SRC_DIR}/tadpole/animation/AnimationController.java
	${JAVA_SRC_DIR}/tadpole/animation/AnimationControllerFinishedListener.java
	${JAVA_SRC_DIR}/tadpole/animation/CameraProjectionAnimation.java
	${JAVA_SRC_DIR}/tadpole/animation/EntityPathAnimation.java
	${JAVA_SRC_DIR}/tadpole/animation/EntityScaleAnimation.java
	${JAVA_SRC_DIR}/tadpole/animation/MaterialColorAnimation.java
	${JAVA_SRC_DIR}/tadpole/animation/SkeletonAnimation.java
	${JAVA_SRC_DIR}/tadpole/animation/TextureMatrixAnimation.java
)
set (TADPOLE_ENTITY_SRC
	${JAVA_SRC_DIR}/tadpole/entity/AnimationControllerEntity.java
	${JAVA_SRC_DIR}/tadpole/entity/CameraEntity.java
	${JAVA_SRC_DIR}/tadpole/entity/Entity.java
	${JAVA_SRC_DIR}/tadpole/entity/EntityDestroyedListener.java
	${JAVA_SRC_DIR}/tadpole/entity/LightEntity.java
	${JAVA_SRC_DIR}/tadpole/entity/MeshEntity.java
	${JAVA_SRC_DIR}/tadpole/entity/MeshEntitySkeleton.java
	${JAVA_SRC_DIR}/tadpole/entity/ParentEntity.java
	${JAVA_SRC_DIR}/tadpole/entity/ParticleEntity.java
	${JAVA_SRC_DIR}/tadpole/entity/RenderableEntity.java
	${JAVA_SRC_DIR}/tadpole/entity/Scene.java
	${JAVA_SRC_DIR}/tadpole/entity/SpriteEntity.java
)
set (TADPOLE_HANDLER_SRC
	${JAVA_SRC_DIR}/tadpole/handler/TextureHandler.java
)
set (TADPOLE_WIDGET_SRC
	${JAVA_SRC_DIR}/tadpole/widget/ActionListener.java
	${JAVA_SRC_DIR}/tadpole/widget/AnimationControllerWidget.java
	${JAVA_SRC_DIR}/tadpole/widget/BorderWidget.java
	${JAVA_SRC_DIR}/tadpole/widget/ButtonWidget.java
	${JAVA_SRC_DIR}/tadpole/widget/LabelWidget.java
	${JAVA_SRC_DIR}/tadpole/widget/ParentWidget.java
	${JAVA_SRC_DIR}/tadpole/widget/RenderableWidget.java
	${JAVA_SRC_DIR}/tadpole/widget/SpriteWidget.java
	${JAVA_SRC_DIR}/tadpole/widget/Widget.java
	${JAVA_SRC_DIR}/tadpole/widget/WidgetDestroyedListener.java
	${JAVA_SRC_DIR}/tadpole/widget/Window.java
)

# Bundle source files
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JAVA_GPP_DIR}/com/lightningtoads/toadlet/tadpole)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JAVA_GPP_DIR}/com/lightningtoads/toadlet/tadpole/animation)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JAVA_GPP_DIR}/com/lightningtoads/toadlet/tadpole/entity)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JAVA_GPP_DIR}/com/lightningtoads/toadlet/tadpole/handler)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JAVA_GPP_DIR}/com/lightningtoads/toadlet/tadpole/widget)
source_group ("Source Files\\\\animation" FILES ${TADPOLE_ANIMATION_SRC})
source_group ("Source Files\\\\entity" FILES ${TADPOLE_ENTITY_SRC})
source_group ("Source Files\\\\handler" FILES ${TADPOLE_HANDLER_SRC})
source_group ("Source Files\\\\widget" FILES ${TADPOLE_WIDGET_SRC})
set (TADPOLE_SRC ${TADPOLE_SRC} ${TADPOLE_ANIMATION_SRC} ${TADPOLE_ENTITY_SRC} ${TADPOLE_HANDLER_SRC} ${TADPOLE_WIDGET_SRC})

# Copy over source files and run gpp
foreach (SRCFILE ${TADPOLE_SRC})
	add_custom_command (
		OUTPUT ${JAVA_GPP_DIR}/${SRCFILE}
		COMMAND gpp -J -o ${JAVA_GPP_DIR}/${SRCFILE} -I${JAVA_GPP_DIR} -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${SRCFILE}
		DEPENDS ${SRCFILE}
	)
	set (TADPOLE_SRC_GPP ${TADPOLE_SRC_GPP} ${JAVA_GPP_DIR}/${SRCFILE})
endforeach (SRCFILE)

#  Jar library
if (TOADLET_JAVA_SPLIT_JARS)
	add_library (toadlet_tadpole STATIC ${TADPOLE_SRC_GPP})
	set_target_properties (toadlet_tadpole PROPERTIES COMPILE_FLAGS "-d \"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/toadlet_tadpole.dir/gpp\"")
	set_target_properties (toadlet_tadpole PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG)
	add_dependencies (toadlet_tadpole toadlet_egg toadlet_peeper)
	install (TARGETS toadlet_tadpole DESTINATION lib)
endif (TOADLET_JAVA_SPLIT_JARS)


# Building toadlet_tadpole_hop

# Source files
set (TADPOLE_HOP_SRC
	${JAVA_SRC_DIR}/tadpole/plugins/hop/HopCollision.java
	${JAVA_SRC_DIR}/tadpole/plugins/hop/HopCollisionListener.java
	${JAVA_SRC_DIR}/tadpole/plugins/hop/HopEntity.java
	${JAVA_SRC_DIR}/tadpole/plugins/hop/HopEntityFactory.java
	${JAVA_SRC_DIR}/tadpole/plugins/hop/HopParticleSimulator.java
	${JAVA_SRC_DIR}/tadpole/plugins/hop/HopScene.java
)

# Bundle source files
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JAVA_GPP_DIR}/com/lightningtoads/toadlet/tadpole/plugins/hop)

# Copy over source files and run gpp
foreach (SRCFILE ${TADPOLE_HOP_SRC})
	add_custom_command (
		OUTPUT ${JAVA_GPP_DIR}/${SRCFILE}
		COMMAND gpp -J -o ${JAVA_GPP_DIR}/${SRCFILE} -I${CMAKE_CURRENT_SOURCE_DIR} -I${JAVA_GPP_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${SRCFILE}
		DEPENDS ${SRCFILE}
	)
	set (TADPOLE_HOP_SRC_GPP ${TADPOLE_HOP_SRC_GPP} ${JAVA_GPP_DIR}/${SRCFILE})
endforeach (SRCFILE)

#  Jar library
if (TOADLET_JAVA_SPLIT_JARS)
	add_library (toadlet_tadpole_hop STATIC ${TADPOLE_HOP_SRC_GPP})
	set_target_properties (toadlet_tadpole_hop PROPERTIES COMPILE_FLAGS "-d \"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/toadlet_tadpole_hop.dir/gpp\"")
	set_target_properties (toadlet_tadpole_hop PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG)
	add_dependencies (toadlet_tadpole_hop toadlet_egg toadlet_hop toadlet_tadpole)
	install (TARGETS toadlet_tadpole_hop DESTINATION lib)
endif (TOADLET_JAVA_SPLIT_JARS)


# Building toadlet_pad

# Configure
configure_file (${JAVA_SRC_DIR}/pad/Version.h.in ${JAVA_GPP_DIR}/${JAVA_SRC_DIR}/pad/Version.h)

# Source files
set (PAD_SRC
	${JAVA_SRC_DIR}/pad/Application.java
	${JAVA_SRC_DIR}/pad/ApplicationListener.java
	${JAVA_SRC_DIR}/pad/Version.java
)

# Bundle source files
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${JAVA_GPP_DIR}/com/lightningtoads/toadlet/pad)

# Copy over source files and run gpp
foreach (SRCFILE ${PAD_SRC})
	add_custom_command (
		OUTPUT ${JAVA_GPP_DIR}/${SRCFILE}
		COMMAND gpp -J -o ${JAVA_GPP_DIR}/${SRCFILE} -I${JAVA_GPP_DIR} -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${SRCFILE}
		DEPENDS ${SRCFILE}
	)
	set (PAD_SRC_GPP ${PAD_SRC_GPP} ${JAVA_GPP_DIR}/${SRCFILE})
endforeach (SRCFILE)

#  Jar library
if (TOADLET_JAVA_SPLIT_JARS)
	add_library (toadlet_pad STATIC ${PAD_SRC_GPP})
	set_target_properties (toadlet_pad PROPERTIES COMPILE_FLAGS "-d \"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/toadlet_pad.dir/gpp\"")
	set_target_properties (toadlet_pad PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG)
	add_dependencies (toadlet_pad toadlet_egg toadlet_peeper toadlet_tadpole)
	install (TARGETS toadlet_pad DESTINATION lib)
endif (TOADLET_JAVA_SPLIT_JARS)


# Generate the all encompassing toadlet jar
if (NOT TOADLET_JAVA_SPLIT_JARS)
	add_library (toadlet ${EGG_SRC_GPP} ${HOP_SRC_GPP} ${KNOT_SRC_GPP} ${PEEPER_SRC_GPP} ${PEEPER_GLRENDERER_SRC_GPP} ${TADPOLE_SRC_GPP} ${TADPOLE_HOP_SRC_GPP} ${PAD_SRC_GPP})
	set_target_properties (toadlet PROPERTIES COMPILE_FLAGS "-d \"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/toadlet.dir/gpp\"")
	set_target_properties (toadlet PROPERTIES COMPILE_DEFINITIONS_DEBUG TOADLET_DEBUG)
	if(TOADLET_PLATFORM_BLACKBERRY)
		# The blackberry jar requires preverification, so we have to do that manually with the RIM tools
		install (CODE "execute_process(COMMAND ${BLACKBERRY_JDE_DIR}/bin/preverify.exe -classpath ${BLACKBERRY_JDE_DIR}/lib/net_rim_api.jar -d ${CMAKE_CURRENT_BINARY_DIR}/src/java/output ${CMAKE_CURRENT_BINARY_DIR}/src/java/toadlet.jar)")
		install (FILES ${CMAKE_CURRENT_BINARY_DIR}/src/java/output/toadlet.jar DESTINATION lib)
	elseif(TOADLET_PLATFORM_ANDROID)
		install (TARGETS toadlet DESTINATION lib)
	endif(TOADLET_PLATFORM_BLACKBERRY)
endif (NOT TOADLET_JAVA_SPLIT_JARS)

