# Source files for toadlet_ribbit_win32player
set (RIBBIT_WIN32PLAYER_SRC
	Win32AudioBuffer.cpp
	Win32Audio.cpp
	Win32Player.cpp
)


# Headers
set (RIBBIT_WIN32PLAYER_HEADERS
	Win32AudioBuffer.h
	Win32Audio.h
	Win32Player.h
)


source_group ("Source Files\\\\plugins\\\\win32player" FILES ${RIBBIT_WIN32PLAYER_SRC})
source_group ("Header Files\\\\plugins\\\\win32player" FILES ${RIBBIT_WIN32PLAYER_HEADERS})
set (RIBBIT_WIN32PLAYER_SRC ${RIBBIT_WIN32PLAYER_SRC})
set (RIBBIT_WIN32PLAYER_HEADERS ${RIBBIT_WIN32PLAYER_HEADERS})


# Dynamic library
add_library (toadlet_ribbit_win32player SHARED ${RIBBIT_WIN32PLAYER_SRC} ${RIBBIT_WIN32PLAYER_HEADERS})
set_target_properties (toadlet_ribbit_win32player PROPERTIES ${DYNAMIC_LIB_PROPS})
target_link_libraries (toadlet_ribbit_win32player toadlet_ribbit)
install (TARGETS toadlet_ribbit_win32player DESTINATION lib)


# Static library
add_library (toadlet_ribbit_win32player_s STATIC ${RIBBIT_WIN32PLAYER_SRC} ${RIBBIT_WIN32PLAYER_HEADERS})
set_target_properties (toadlet_ribbit_win32player_s PROPERTIES ${STATIC_LIB_PROPS})
target_link_libraries (toadlet_ribbit_win32player_s toadlet_ribbit_s)
install (TARGETS toadlet_ribbit_win32player_s DESTINATION lib)

