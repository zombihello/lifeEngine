# 	---------------------------------
#	[in] 	BOX2D_PATH			- root dir Box2D
#	[out] 	BOX2D_INCLUDE		- dir with includes
#	[out]	BOX2D_LIB			- lib Box2D
#	[out]	BOX2D_DEBUG_LIB		- debug lib Box2D
#	[out]	BOX2D_FOUND			- is found Box2D
# 	---------------------------------

set( BOX2D_FOUND OFF )
SET( BOX2D_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${BOX2D_PATH} )

#
# Set path suffixed to libs for current platform
#
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( BOX2D_LIB_PATH_SUFFIXES "lib/Win64" )
	elseif( PLATFORM_32BIT )
		set( BOX2D_LIB_PATH_SUFFIXES "lib/Win32" )
	endif()
else()
	message( SEND_ERROR "Box2D :: Unknown platform" )
	return()
endif()

#
# Find pathes
#
find_path( 		BOX2D_INCLUDE
				NAMES "box2d/box2d.h"
				PATH_SUFFIXES include
				PATHS ${BOX2D_SEARCH_PATHS} )			
find_library( 	BOX2D_LIB 
                NAMES box2d
                PATH_SUFFIXES ${BOX2D_LIB_PATH_SUFFIXES}/Release
                PATHS ${BOX2D_SEARCH_PATHS} )
find_library( 	BOX2D_DEBUG_LIB 
                NAMES box2d
                PATH_SUFFIXES ${BOX2D_LIB_PATH_SUFFIXES}/Debug
                PATHS ${BOX2D_SEARCH_PATHS} )
				
if ( NOT BOX2D_INCLUDE OR NOT BOX2D_LIB OR NOT BOX2D_DEBUG_LIB )
    return()
else()
	set( BOX2D_FOUND ON )
endif()
