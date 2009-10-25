# Source files for toadlet_peeper_d3d9renderer
set (PEEPER_D3D9RENDERER_SRC
	D3D9IndexBufferPeer.cpp
	D3D9Renderer.cpp
	D3D9Texture.cpp
	D3D9Surface.cpp
	D3D9SurfaceRenderTarget.cpp
	D3D9VertexBufferPeer.cpp
	D3D9WindowRenderTarget.cpp
)


# Headers
set (PEEPER_D3D9RENDERER_HEADERS
	D3D9Includes.h
	D3D9IndexBufferPeer.h
	D3D9RenderTarget.h
	D3D9Renderer.h
	D3D9Surface.h
	D3D9SurfaceRenderTarget.h
	D3D9Texture.h
	D3D9VertexBufferPeer.h
	D3D9WindowRenderTarget.h
)


# Dependencies - note that D3D9 has already been found
if (TOADLET_HAS_DIRECT3DMOBILE)
	include_directories (${D3D9_INCLUDE_PATH})
	set (PEEPER_D3D9RENDERER_LIBS ${D3D9_LIBRARIES})
else (TOADLET_HAS_DIRECT3DMOBILE)
	include_directories (${D3DM_INCLUDE_PATH})
	set (PEEPER_D3D9RENDERER_LIBS ${D3DM_LIBRARIES})
endif (TOADLET_HAS_DIRECT3DMOBILE)


# Dynamic library
add_library (toadlet_peeper_d3d9renderer SHARED ${PEEPER_D3D9RENDERER_SRC} ${PEEPER_D3D9RENDERER_HEADERS})
set_target_properties (toadlet_peeper_d3d9renderer PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_peeper_d3d9renderer toadlet_peeper ${PEEPER_D3D9RENDERER_LIBS})
install (TARGETS toadlet_peeper_d3d9renderer DESTINATION lib)


# Static library
add_library (toadlet_peeper_d3d9renderer_s STATIC ${PEEPER_D3D9RENDERER_SRC} ${PEEPER_D3D9RENDERER_HEADERS})
set_target_properties (toadlet_peeper_d3d9renderer_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_peeper_d3d9renderer_s toadlet_peeper_s ${PEEPER_D3D9RENDERER_LIBS})
install (TARGETS toadlet_peeper_d3d9renderer_s DESTINATION lib)

