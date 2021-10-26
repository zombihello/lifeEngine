# 	---------------------------------
#	[in] 	LUABRIDGE_PATH		- root dir LuaBridge
#	[out] 	LUABRIDGE_INCLUDE	- dir with includes
#	[out]	LUABRIDGE_FOUND		- is found LuaBridge
# 	---------------------------------

set( LUABRIDGE_FOUND OFF )
SET( LUABRIDGE_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${LUABRIDGE_PATH} )

#
# Find pathes
#
find_path( 		LUABRIDGE_INCLUDE
				NAMES "LuaBridge/LuaBridge.h"
				PATH_SUFFIXES include
				PATHS ${LUABRIDGE_SEARCH_PATHS} )
				
if ( NOT LUABRIDGE_INCLUDE )
    return()
else()
	set( LUABRIDGE_FOUND ON )
endif()
