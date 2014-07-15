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
set (PEEPER_D3DMRENDERDEVICE_HDR
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
if (TOADLET_BUILD_DYNAMIC)
	add_library (toadlet_peeper_d3dmrenderdevice SHARED ${PEEPER_D3DMRENDERDEVICE_SRC} ${PEEPER_D3DMRENDERDEVICE_HDR})
	set_target_properties (toadlet_peeper_d3dmrenderdevice PROPERTIES ${DYNAMIC_LIB_PROP})
	target_compile_definitions (toadlet_peeper_d3dmrenderdevice PUBLIC ${DYNAMIC_LIB_DEFS})
	target_compile_options (toadlet_peeper_d3dmrenderdevice PUBLIC ${DYNAMIC_LIB_FLAG})
	target_link_libraries (toadlet_peeper_d3dmrenderdevice ${DYNAMIC_LIB_LINK} toadlet_peeper ${PEEPER_D3DMRENDERDEVICE_LIBS})
	install (TARGETS toadlet_peeper_d3dmrenderdevice DESTINATION ${LIB_INSTALL_DIR} COMPONENT peeper)
	set (PEEPER_INSTALL ${PEEPER_INSTALL} toadlet_peeper_d3dmrenderdevice PARENT_SCOPE)
endif (TOADLET_BUILD_DYNAMIC)


# Static library
if (TOADLET_BUILD_STATIC)
	add_library (toadlet_peeper_d3dmrenderdevice_s STATIC ${PEEPER_D3DMRENDERDEVICE_SRC} ${PEEPER_D3DMRENDERDEVICE_HDR})
	set_target_properties (toadlet_peeper_d3dmrenderdevice_s PROPERTIES ${STATIC_LIB_PROP})
	target_compile_definitions (toadlet_peeper_d3dmrenderdevice_s PUBLIC ${STATIC_LIB_DEFS})
	target_compile_options (toadlet_peeper_d3dmrenderdevice_s PUBLIC ${STATIC_LIB_FLAG})
	target_link_libraries (toadlet_peeper_d3dmrenderdevice_s ${STATIC_LIB_LINK} toadlet_peeper_s ${PEEPER_D3DMRENDERDEVICE_LIBS})
	install (TARGETS toadlet_peeper_d3dmrenderdevice_s DESTINATION ${LIB_INSTALL_DIR} COMPONENT peeper)
	set (PEEPER_INSTALL ${PEEPER_INSTALL} toadlet_peeper_d3dmrenderdevice_s PARENT_SCOPE)
endif (TOADLET_BUILD_STATIC)

