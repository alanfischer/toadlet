# Source files for toadlet_peeper_d3d11renderer
set (PEEPER_D3D11RENDERER_SRC
	../d3d10renderer/D3D10Buffer.cpp
	../d3d10renderer/D3D10PixelBuffer.cpp
	../d3d10renderer/D3D10PixelBufferRenderTarget.cpp
	../d3d10renderer/D3D10Renderer.cpp
	../d3d10renderer/D3D10Texture.cpp
	../d3d10renderer/D3D10TextureMipPixelBuffer.cpp
	../d3d10renderer/D3D10VertexFormat.cpp
	../d3d10renderer/D3D10WindowRenderTarget.cpp
)


# Headers
set (PEEPER_D3D11RENDERER_HEADERS
	../d3d10renderer/D3D10Buffer.h
	../d3d10renderer/D3D10Includes.h
	../d3d10renderer/D3D10PixelBufferRenderTarget.h
	../d3d10renderer/D3D10RenderTarget.h
	../d3d10renderer/D3D10Renderer.h
	../d3d10renderer/D3D10Texture.h
	../d3d10renderer/D3D10TextureMipPixelBuffer.h
	../d3d10renderer/D3D10VertexFormat.h
	../d3d10renderer/D3D10WindowRenderTarget.h
)


# Definitions
add_definitions(-DTOADLET_SET_D3D11)


# Dependencies - note that D3D11 has already been found
include_directories (${D3D11_INCLUDE_PATH})
set (PEEPER_D3D11RENDERER_LIBS ${D3D11_LIBRARIES})


# Dynamic library
add_library (toadlet_peeper_d3d11renderer SHARED ${PEEPER_D3D11RENDERER_SRC} ${PEEPER_D3D11RENDERER_HEADERS})
set_target_properties (toadlet_peeper_d3d11renderer PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_peeper_d3d11renderer toadlet_peeper ${PEEPER_D3D11RENDERER_LIBS})
install (TARGETS toadlet_peeper_d3d11renderer DESTINATION lib)


# Static library
add_library (toadlet_peeper_d3d11renderer_s STATIC ${PEEPER_D3D11RENDERER_SRC} ${PEEPER_D3D11RENDERER_HEADERS})
set_target_properties (toadlet_peeper_d3d11renderer_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_peeper_d3d11renderer_s toadlet_peeper_s ${PEEPER_D3D11RENDERER_LIBS})
install (TARGETS toadlet_peeper_d3d11renderer_s DESTINATION lib)

