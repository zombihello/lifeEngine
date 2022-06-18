# 	---------------------------------
#	[in] 	THEORA_PATH			- root dir Theora
#	[out] 	THEORA_INCLUDE		- dir with includes
#	[out]	THEORA_LIB			- lib Theora
#	[out]	THEORA_DEBUG_LIB	- lib debug Theora
#	[out]	THEORA_FOUND		- is found Theora
# 	---------------------------------

set( THEORA_FOUND OFF )
SET( THEORA_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${THEORA_PATH} )

#
# Set path suffixed to libs for current platform
#
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( THEORA_LIB_PATH_SUFFIXES "bin/Win64" )
	elseif( PLATFORM_32BIT )
		set( THEORA_LIB_PATH_SUFFIXES "bin/Win32" )
	endif()
else()
	message( SEND_ERROR "Theora :: Unknown platform" )
	return()
endif()

#
# Find pathes
#
find_path( 		THEORA_INCLUDE
				NAMES "theora/theora.h"
				PATH_SUFFIXES include
				PATHS ${THEORA_SEARCH_PATHS} )			
find_library( 	THEORA_LIB 
                NAMES libtheora_static
                PATH_SUFFIXES ${THEORA_LIB_PATH_SUFFIXES}
                PATHS ${THEORA_SEARCH_PATHS} )
find_library( 	THEORA_DEBUG_LIB 
                NAMES libtheora_static-d
                PATH_SUFFIXES ${THEORA_LIB_PATH_SUFFIXES}
                PATHS ${THEORA_SEARCH_PATHS} )
				
if ( NOT THEORA_INCLUDE OR NOT THEORA_LIB OR NOT THEORA_DEBUG_LIB )
    return()
else()
	set( THEORA_FOUND ON )
endif()
