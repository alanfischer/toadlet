# Source files for toadlet_peeper_d3dmrenderdevice
set (PEEPER_D3DMRENDERDEVICE_SRC
	../d3d9renderdevice/D3D9IndexBuffer.cpp
	../d3d9renderdevice/D3D9RenderDevice.cpp
	../d3d9renderdevice/D3D9Texture.cpp
	../d3d9renderdevice/D3D9Surface.cpp
	../d3d9renderdevice/D3D9SurfaceRenderTarget.cpp
	../d3d9renderdevice/D3D9VertexBuffer.cpp
	../d3d9renderdevice/D3D9VertexFormat.cpp
	../d3d9renderdevice/D3D9WindowRenderTarget.cpp
)


# Headers
set (PEEPER_D3DMRENDERDEVICE_HEADERS
	../d3d9renderdevice/D3D9Includes.h
	../d3d9renderdevice/D3D9IndexBuffer.h
	../d3d9renderdevice/D3D9RenderState.h
	../d3d9renderdevice/D3D9RenderTarget.h
	../d3d9renderdevice/D3D9RenderDevice.h
	../d3d9renderdevice/D3D9Surface.h
	../d3d9renderdevice/D3D9SurfaceRenderTarget.h
	../d3d9renderdevice/D3D9Texture.h
	../d3d9renderdevice/D3D9VertexBuffer.h
	../d3d9renderdevice/D3D9VertexFormat.h
	../d3d9renderdevice/D3D9WindowRenderTarget.h
)


# Definitions
add_definitions(-DTOADLET_SET_D3DM)


# Dependencies - note that D3DM has already been found
include_directories (${D3DM_INCLUDE_PATH})
# We don't add the D3DM_LIBRARIES, since those are dynamically loaded


# Dynamic library
add_library (toadlet_peeper_d3dmrenderdevice SHARED ${PEEPER_D3DMRENDERDEVICE_SRC} ${PEEPER_D3DMRENDERDEVICE_HEADERS})
set_target_properties (toadlet_peeper_d3dmrenderdevice PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_peeper_d3dmrenderdevice toadlet_peeper ${PEEPER_D3DMRENDERDEVICE_LIBS})
install (TARGETS toadlet_peeper_d3dmrenderdevice DESTINATION ${LIB_INSTALL_DIR})


# Static library
add_library (toadlet_peeper_d3dmrenderdevice_s STATIC ${PEEPER_D3DMRENDERDEVICE_SRC} ${PEEPER_D3DMRENDERDEVICE_HEADERS})
set_target_properties (toadlet_peeper_d3dmrenderdevice_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_peeper_d3dmrenderdevice_s toadlet_peeper_s ${PEEPER_D3DMRENDERDEVICE_LIBS})
install (TARGETS toadlet_peeper_d3dmrenderdevice_s DESTINATION ${LIB_INSTALL_DIR})

