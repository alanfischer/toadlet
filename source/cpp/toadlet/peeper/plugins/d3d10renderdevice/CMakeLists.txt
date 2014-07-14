# Source files for toadlet_peeper_d3d10renderdevice
set (PEEPER_D3D10RENDERDEVICE_SRC
	D3D10Buffer.cpp
	D3D10PixelBufferRenderTarget.cpp
	D3D10Query.cpp
	D3D10RenderDevice.cpp
	D3D10RenderState.cpp
	D3D10RenderTarget.cpp
	D3D10Shader.cpp
	D3D10ShaderState.cpp
	D3D10Texture.cpp
	D3D10TextureMipPixelbuffer.cpp
	D3D10VertexFormat.cpp
	D3D10WindowRenderTarget.cpp
)


# Headers
set (PEEPER_D3D10RENDERDEVICE_HDR
	D3D10Buffer.h
	D3D10Includes.h
	D3D10PixelBufferRenderTarget.h
	D3D10Query.h
	D3D10RenderDevice.h
	D3D10RenderState.h
	D3D10RenderTarget.h
	D3D10Shader.h
	D3D10ShaderState.h
	D3D10Texture.h
	D3D10TextureMipPixelBuffer.h
	D3D10VertexFormat.h
	D3D10WindowRenderTarget.h
)


# Definitions
add_definitions(-DTOADLET_SET_D3D10)


# Dependencies - note that D3D10 has already been found
include_directories (${D3D10_INCLUDE_PATH})
# We don't add the D3D10_LIBRARIES, since those are dynamically loaded


# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_peeper_d3d10renderdevice SHARED ${PEEPER_D3D10RENDERDEVICE_SRC} ${PEEPER_D3D10RENDERDEVICE_HDR})
	set_target_properties (toadlet_peeper_d3d10renderdevice PROPERTIES ${DYNAMIC_LIB_PROP})
	target_compile_definitions (toadlet_peeper_d3d10renderdevice ${DYNAMIC_LIB_DEFS})
	target_compile_options (toadlet_peeper_d3d10renderdevice ${DYNAMIC_LIB_FLAG})
	target_link_libraries (toadlet_peeper_d3d10renderdevice ${DYNAMIC_LIB_LINK} toadlet_peeper ${PEEPER_D3D10RENDERDEVICE_LIBS})
	install (TARGETS toadlet_peeper_d3d10renderdevice DESTINATION ${LIB_INSTALL_DIR} COMPONENT peeper)
	set (PEEPER_INSTALL ${PEEPER_INSTALL} toadlet_peeper_d3d10renderdevice PARENT_SCOPE)
endif (TOADLET_BUILD_DYNAMIC)


# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_peeper_d3d10renderdevice_s STATIC ${PEEPER_D3D10RENDERDEVICE_SRC} ${PEEPER_D3D10RENDERDEVICE_HDR})
	set_target_properties (toadlet_peeper_d3d10renderdevice_s PROPERTIES ${STATIC_LIB_PROP})
	target_compile_definitions (toadlet_peeper_d3d10renderdevice_s ${STATIC_LIB_DEFS})
	target_compile_options (toadlet_peeper_d3d10renderdevice_s ${STATIC_LIB_FLAG})
	target_link_libraries (toadlet_peeper_d3d10renderdevice_s ${STATIC_LIB_LINK} toadlet_peeper_s ${PEEPER_D3D10RENDERDEVICE_LIBS})
	install (TARGETS toadlet_peeper_d3d10renderdevice_s DESTINATION ${LIB_INSTALL_DIR} COMPONENT peeper)
	set (PEEPER_INSTALL ${PEEPER_INSTALL} toadlet_peeper_d3d10renderdevice_s PARENT_SCOPE)
endif (TOADLET_BUILD_STATIC)

