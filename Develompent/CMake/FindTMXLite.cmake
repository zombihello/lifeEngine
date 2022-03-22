# 	---------------------------------
#	[in] 	TMXLITE_PATH		- root dir TMXLite
#	[out] 	TMXLITE_INCLUDE		- dir with includes
#	[out]	TMXLITE_LIB			- lib TMXLite
#	[out]	TMXLITE_DEBUG_LIB	- debug lib TMXLite
#	[out]	TMXLITE_FOUND		- is found TMXLite
# 	---------------------------------

set( TMXLITE_FOUND OFF )
SET( TMXLITE_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${TMXLITE_PATH} )

#
# Set path suffixed to libs for current platform
#
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( TMXLITE_LIB_PATH_SUFFIXES "lib/Win64" )
	elseif( PLATFORM_32BIT )
		set( TMXLITE_LIB_PATH_SUFFIXES "lib/Win32" )
	endif()
else()
	message( SEND_ERROR "TMXLite :: Unknown platform" )
	return()
endif()

#
# Find pathes
#
find_path( 		TMXLITE_INCLUDE
				NAMES "tmxlite/Map.hpp"
				PATH_SUFFIXES include
				PATHS ${TMXLITE_SEARCH_PATHS} )			
find_library( 	TMXLITE_LIB 
                NAMES tmxlite-s
                PATH_SUFFIXES ${TMXLITE_LIB_PATH_SUFFIXES}
                PATHS ${TMXLITE_SEARCH_PATHS} )
find_library( 	TMXLITE_DEBUG_LIB 
                NAMES tmxlite-s-d
                PATH_SUFFIXES ${TMXLITE_LIB_PATH_SUFFIXES}
                PATHS ${TMXLITE_SEARCH_PATHS} )
				
if ( NOT TMXLITE_INCLUDE OR NOT TMXLITE_LIB OR NOT TMXLITE_DEBUG_LIB )
    return()
else()
	set( TMXLITE_FOUND ON )
endif()
