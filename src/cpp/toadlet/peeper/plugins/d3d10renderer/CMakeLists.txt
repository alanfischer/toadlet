# Source files for toadlet_peeper_d3d10renderer
set (PEEPER_D3D10RENDERER_SRC
	D3D10Buffer.cpp
	D3D10PixelBufferRenderTarget.cpp
	D3D10Query.cpp
	D3D10RenderTarget.cpp
	D3D10Renderer.cpp
	D3D10Texture.cpp
	D3D10TextureMipPixelbuffer.cpp
	D3D10VertexFormat.cpp
	D3D10WindowRenderTarget.cpp
)


# Headers
set (PEEPER_D3D10RENDERER_HEADERS
	D3D10Buffer.h
	D3D10Includes.h
	D3D10PixelBufferRenderTarget.h
	D3D10Query.h
	D3D10RenderTarget.h
	D3D10Renderer.h
	D3D10Texture.h
	D3D10TextureMipPixelBuffer.h
	D3D10VertexFormat.h
	D3D10WindowRenderTarget.h
)


# Definitions
add_definitions(-DTOADLET_SET_D3D10)


# Dependencies - note that D3D10 has already been found
include_directories (${D3D10_INCLUDE_PATH})
set (PEEPER_D3D10RENDERER_LIBS ${D3D10_LIBRARIES})


# Dynamic library
add_library (toadlet_peeper_d3d10renderer SHARED ${PEEPER_D3D10RENDERER_SRC} ${PEEPER_D3D10RENDERER_HEADERS})
set_target_properties (toadlet_peeper_d3d10renderer PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_peeper_d3d10renderer toadlet_peeper ${PEEPER_D3D10RENDERER_LIBS})
install (TARGETS toadlet_peeper_d3d10renderer DESTINATION lib)


# Static library
add_library (toadlet_peeper_d3d10renderer_s STATIC ${PEEPER_D3D10RENDERER_SRC} ${PEEPER_D3D10RENDERER_HEADERS})
set_target_properties (toadlet_peeper_d3d10renderer_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_peeper_d3d10renderer_s toadlet_peeper_s ${PEEPER_D3D10RENDERER_LIBS})
install (TARGETS toadlet_peeper_d3d10renderer_s DESTINATION lib)

