# Source files for toadlet_peeper_d3d11renderdevice
set (PEEPER_D3D11RENDERDEVICE_SRC
	../d3d10renderdevice/D3D10Buffer.cpp
	../d3d10renderdevice/D3D10PixelBufferRenderTarget.cpp
	../d3d10renderdevice/D3D10Query.cpp
	../d3d10renderdevice/D3D10RenderDevice.cpp
	../d3d10renderdevice/D3D10RenderTarget.cpp
	../d3d10renderdevice/D3D10RenderState.cpp
	../d3d10renderdevice/D3D10Shader.cpp
	../d3d10renderdevice/D3D10ShaderState.cpp
	../d3d10renderdevice/D3D10Texture.cpp
	../d3d10renderdevice/D3D10TextureMipPixelBuffer.cpp
	../d3d10renderdevice/D3D10VertexFormat.cpp
	../d3d10renderdevice/D3D10WindowRenderTarget.cpp
)


# Headers
set (PEEPER_D3D11RENDERDEVICE_HDR
	../d3d10renderdevice/D3D10Buffer.h
	../d3d10renderdevice/D3D10Includes.h
	../d3d10renderdevice/D3D10PixelBufferRenderTarget.h
	../d3d10renderdevice/D3D10Query.h
	../d3d10renderdevice/D3D10RenderDevice.h
	../d3d10renderdevice/D3D10RenderState.h
	../d3d10renderdevice/D3D10RenderTarget.h
	../d3d10renderdevice/D3D10Shader.h
	../d3d10renderdevice/D3D10ShaderState.h
	../d3d10renderdevice/D3D10Texture.h
	../d3d10renderdevice/D3D10TextureMipPixelBuffer.h
	../d3d10renderdevice/D3D10VertexFormat.h
	../d3d10renderdevice/D3D10WindowRenderTarget.h
)


# Definitions
add_definitions(-DTOADLET_SET_D3D11)


# Dependencies - note that D3D11 has already been found
include_directories (${D3D11_INCLUDE_PATH})
# We don't add the D3D11_LIBRARIES, since those are dynamically loaded


# Dynamic library
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_peeper_d3d11renderdevice SHARED ${PEEPER_D3D11RENDERDEVICE_SRC} ${PEEPER_D3D11RENDERDEVICE_HDR})
	set_target_properties (toadlet_peeper_d3d11renderdevice PROPERTIES ${DYNAMIC_LIB_PROPS})
	target_link_libraries (toadlet_peeper_d3d11renderdevice toadlet_peeper ${PEEPER_D3D11RENDERDEVICE_LIBS})
	install (TARGETS toadlet_peeper_d3d11renderdevice DESTINATION ${LIB_INSTALL_DIR} COMPONENT peeper)
	set (PEEPER_INSTALL ${PEEPER_INSTALL} toadlet_peeper_d3d11renderdevice PARENT_SCOPE)
endif (TOADLET_BUILD_DYNAMIC)


# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_peeper_d3d11renderdevice_s STATIC ${PEEPER_D3D11RENDERDEVICE_SRC} ${PEEPER_D3D11RENDERDEVICE_HDR})
	set_target_properties (toadlet_peeper_d3d11renderdevice_s PROPERTIES ${STATIC_LIB_PROPS})
	target_link_libraries (toadlet_peeper_d3d11renderdevice_s toadlet_peeper_s ${PEEPER_D3D11RENDERDEVICE_LIBS})
	install (TARGETS toadlet_peeper_d3d11renderdevice_s DESTINATION ${LIB_INSTALL_DIR} COMPONENT peeper)
	set (PEEPER_INSTALL ${PEEPER_INSTALL} toadlet_peeper_d3d11renderdevice_s PARENT_SCOPE)
endif (TOADLET_BUILD_STATIC)

