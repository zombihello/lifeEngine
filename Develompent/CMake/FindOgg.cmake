# 	---------------------------------
#	[in] 	OGG_PATH	- root dir ogg
#	[out] 	OGG_INCLUDE	- dir with includes
#	[out]	OGG_LIB		- lib ogg
#	[out]	OGG_FOUND	- is found ogg
# 	---------------------------------

set( OGG_FOUND OFF )
SET( OGG_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${OGG_PATH} )

#
# Set path suffixed to libs for current platform
#
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( OGG_LIB_PATH_SUFFIXES "lib/Win64" )
	elseif( PLATFORM_32BIT )
		set( OGG_LIB_PATH_SUFFIXES "lib/Win32" )
	endif()
else()
	message( SEND_ERROR "OGG :: Unknown platform" )
	return()
endif()

#
# Find pathes
#
find_path( 		OGG_INCLUDE
				NAMES "ogg/ogg.h"
				PATH_SUFFIXES include
				PATHS ${OGG_SEARCH_PATHS} )			
find_library( 	OGG_LIB 
                NAMES ogg
                PATH_SUFFIXES ${OGG_LIB_PATH_SUFFIXES}
                PATHS ${OGG_SEARCH_PATHS} )
		
if ( NOT OGG_INCLUDE OR NOT OGG_LIB )
    return()
else()
	set( OGG_FOUND ON )
endif()
