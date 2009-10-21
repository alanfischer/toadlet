# Source files for toadlet_peeper_d3dmrenderer
set (PEEPER_D3DMRENDERER_SRC
	D3DMIndexBufferPeer.cpp
	D3DMRenderTexturePeer.cpp
	D3DMRenderer.cpp
	D3DMTexturePeer.cpp
	D3DMVertexBufferPeer.cpp
	D3DMWindowRenderContextPeer.cpp
)


# Headers
set (PEEPER_D3DMRENDERER_HEADERS
	D3DMIncludes.h
	D3DMIndexBufferPeer.h
	D3DMRenderContextPeer.h
	D3DMRenderTargetPeer.h
	D3DMRenderTexturePeer.h
	D3DMRenderer.h
	D3DMTexturePeer.h
	D3DMVertexBufferPeer.h
	D3DMWindowRenderContextPeer.h
)


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

