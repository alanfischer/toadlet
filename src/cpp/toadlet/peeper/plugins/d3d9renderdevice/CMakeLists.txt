# Source files for toadlet_peeper_d3d9renderdevice
set (PEEPER_D3D9RENDERDEVICE_SRC
	D3D9ConstantBuffer.cpp
	D3D9IndexBuffer.cpp
	D3D9PixelBuffer.cpp
	D3D9PixelBufferRenderTarget.cpp
	D3D9Query.cpp
	D3D9RenderDevice.cpp
	D3D9Shader.cpp
	D3D9Texture.cpp
	D3D9TextureMipPixelBuffer.cpp
	D3D9VertexBuffer.cpp
	D3D9VertexFormat.cpp
	D3D9WindowRenderTarget.cpp
)


# Headers
set (PEEPER_D3D9RENDERDEVICE_HEADERS
	D3D9ConstantBuffer.h
	D3D9Includes.h
	D3D9IndexBuffer.h
	D3D9PixelBuffer.h
	D3D9PixelBufferRenderTarget.h
	D3D9Query.h
	D3D9RenderState.h
	D3D9RenderTarget.h
	D3D9RenderDevice.h
	D3D9Texture.h
	D3D9TextureMipPixelBuffer.h
	D3D9VertexBuffer.h
	D3D9VertexFormat.h
	D3D9WindowRenderTarget.h
)


# Definitions
add_definitions(-DTOADLET_SET_D3D9)


# Dependencies - note that D3D9 has already been found
include_directories (${D3D9_INCLUDE_PATH})
set (PEEPER_D3D9RENDERDEVICE_LIBS ${D3D9_LIBRARIES})


# Dynamic library
add_library (toadlet_peeper_d3d9renderdevice SHARED ${PEEPER_D3D9RENDERDEVICE_SRC} ${PEEPER_D3D9RENDERDEVICE_HEADERS})
set_target_properties (toadlet_peeper_d3d9renderdevice PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_peeper_d3d9renderdevice toadlet_peeper ${PEEPER_D3D9RENDERDEVICE_LIBS})
install (TARGETS toadlet_peeper_d3d9renderdevice DESTINATION lib)


# Static library
add_library (toadlet_peeper_d3d9renderdevice_s STATIC ${PEEPER_D3D9RENDERDEVICE_SRC} ${PEEPER_D3D9RENDERDEVICE_HEADERS})
set_target_properties (toadlet_peeper_d3d9renderdevice_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_peeper_d3d9renderdevice_s toadlet_peeper_s ${PEEPER_D3D9RENDERDEVICE_LIBS})
install (TARGETS toadlet_peeper_d3d9renderdevice_s DESTINATION lib)

