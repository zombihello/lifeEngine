# 	---------------------------------
#	[in] 	ZLIB_PATH		- root dir zlib
#	[out] 	ZLIB_INCLUDE	- dir with includes
#	[out]	ZLIB_LIB		- lib zlib
#	[out]	ZLIB_FOUND		- is found zlib
# 	---------------------------------

set( ZLIB_FOUND OFF )
SET( ZLIB_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${ZLIB_PATH} )

#
# Set path suffixed to libs for current platform
#
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( ZLIB_LIB_PATH_SUFFIXES "lib/Win64" )
	elseif( PLATFORM_32BIT )
		set( ZLIB_LIB_PATH_SUFFIXES "lib/Win32" )
	endif()
else()
	message( SEND_ERROR "ZLIB :: Unknown platform" )
	return()
endif()

#
# Find pathes
#
find_path( 		ZLIB_INCLUDE
				NAMES "zlib.h"
				PATH_SUFFIXES include
				PATHS ${ZLIB_SEARCH_PATHS} )			
find_library( 	ZLIB_LIB 
                NAMES zlibstatic
                PATH_SUFFIXES ${ZLIB_LIB_PATH_SUFFIXES}
                PATHS ${ZLIB_SEARCH_PATHS} )
		
if ( NOT ZLIB_INCLUDE OR NOT ZLIB_LIB )
    return()
else()
	set( ZLIB_FOUND ON )
endif()
