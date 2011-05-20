# Source files for toadlet_peeper_d3dmrenderer
set (PEEPER_D3DMRENDERER_SRC
	../d3d9renderer/D3D9IndexBuffer.cpp
	../d3d9renderer/D3D9Renderer.cpp
	../d3d9renderer/D3D9Texture.cpp
	../d3d9renderer/D3D9Surface.cpp
	../d3d9renderer/D3D9SurfaceRenderTarget.cpp
	../d3d9renderer/D3D9VertexBuffer.cpp
	../d3d9renderer/D3D9VertexFormat.cpp
	../d3d9renderer/D3D9WindowRenderTarget.cpp
)


# Headers
set (PEEPER_D3DMRENDERER_HEADERS
	../d3d9renderer/D3D9Includes.h
	../d3d9renderer/D3D9IndexBuffer.h
	../d3d9renderer/D3D9RenderState.h
	../d3d9renderer/D3D9RenderTarget.h
	../d3d9renderer/D3D9Renderer.h
	../d3d9renderer/D3D9Surface.h
	../d3d9renderer/D3D9SurfaceRenderTarget.h
	../d3d9renderer/D3D9Texture.h
	../d3d9renderer/D3D9VertexBuffer.h
	../d3d9renderer/D3D9VertexFormat.h
	../d3d9renderer/D3D9WindowRenderTarget.h
)


# Definitions
add_definitions(-DTOADLET_SET_D3DM)


# Dependencies - note that D3DM has already been found
include_directories (${D3DM_INCLUDE_PATH})
set (PEEPER_D3DMRENDERER_LIBS ${D3DM_LIBRARIES})


# Dynamic library
add_library (toadlet_peeper_d3dmrenderer SHARED ${PEEPER_D3DMRENDERER_SRC} ${PEEPER_D3DMRENDERER_HEADERS})
set_target_properties (toadlet_peeper_d3dmrenderer PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_peeper_d3dmrenderer toadlet_peeper ${PEEPER_D3DMRENDERER_LIBS})
install (TARGETS toadlet_peeper_d3dmrenderer DESTINATION lib)


# Static library
add_library (toadlet_peeper_d3dmrenderer_s STATIC ${PEEPER_D3DMRENDERER_SRC} ${PEEPER_D3DMRENDERER_HEADERS})
set_target_properties (toadlet_peeper_d3dmrenderer_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_peeper_d3dmrenderer_s toadlet_peeper_s ${PEEPER_D3DMRENDERER_LIBS})
install (TARGETS toadlet_peeper_d3dmrenderer_s DESTINATION lib)

