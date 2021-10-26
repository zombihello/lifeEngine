# 	---------------------------------
#	[in] 	LUAJIT_PATH		- root dir LuaJIT
#	[out] 	LUAJIT_INCLUDE	- dir with includes
#	[out]	LUAJIT_LIB		- lib LuaJIT
#	[out]	LUAJIT_FOUND	- is found LuaJIT
# 	---------------------------------

set( LUAJIT_FOUND OFF )
SET( LUAJIT_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${LUAJIT_PATH} )

#
# Set path suffixed to libs for current platform
#
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( LUAJIT_LIB_PATH_SUFFIXES "lib/Win64" )
	elseif( PLATFORM_32BIT )
		set( LUAJIT_LIB_PATH_SUFFIXES "lib/Win32" )
	endif()
else()
	message( SEND_ERROR "LuaJIT :: Unknown platform" )
	return()
endif()

#
# Find pathes
#
find_path( 		LUAJIT_INCLUDE
				NAMES "lua.h"
				PATH_SUFFIXES include
				PATHS ${LUAJIT_SEARCH_PATHS} )			
find_library( 	LUAJIT_LUA_LIB 
                NAMES lua51
                PATH_SUFFIXES ${LUAJIT_LIB_PATH_SUFFIXES}
                PATHS ${LUAJIT_SEARCH_PATHS} )
find_library( 	LUAJIT_LUAJIT_LIB 
                NAMES luajit
                PATH_SUFFIXES ${LUAJIT_LIB_PATH_SUFFIXES}
                PATHS ${LUAJIT_SEARCH_PATHS} )
				
if ( NOT LUAJIT_INCLUDE OR NOT LUAJIT_LUA_LIB OR NOT LUAJIT_LUAJIT_LIB )
    return()
else()
	set( LUAJIT_FOUND ON )
	set( LUAJIT_LIB ${LUAJIT_LUA_LIB} ${LUAJIT_LUAJIT_LIB} )
endif()
