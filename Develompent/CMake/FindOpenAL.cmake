# 	---------------------------------
#	[in] 	OPENAL_PATH		- root dir OpenAL
#	[out] 	OPENAL_INCLUDE	- dir with includes
#	[out]	OPENAL_LIB		- lib OpenAL
#	[out]	OPENAL_FOUND	- is found OpenAL
# 	---------------------------------

set( OPENAL_FOUND OFF )
SET( OPENAL_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${OPENAL_PATH} )

#
# Set path suffixed to libs for current platform
#
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( OPENAL_LIB_PATH_SUFFIXES "libs/Win64" )
	elseif( PLATFORM_32BIT )
		set( OPENAL_LIB_PATH_SUFFIXES "libs/Win32" )
	endif()
else()
	message( SEND_ERROR "OpenAL :: Unknown platform" )
	return()
endif()

#
# Find pathes
#
find_path( 		OPENAL_INCLUDE
				NAMES "AL/al.h"
				PATH_SUFFIXES include
				PATHS ${OPENAL_SEARCH_PATHS} )			
find_library( 	OPENAL_LIB 
                NAMES OpenAL32
                PATH_SUFFIXES ${OPENAL_LIB_PATH_SUFFIXES}
                PATHS ${OPENAL_SEARCH_PATHS} )
		
if ( NOT OPENAL_INCLUDE OR NOT OPENAL_LIB )
    return()
else()
	set( OPENAL_FOUND ON )
endif()
