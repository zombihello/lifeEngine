# 	---------------------------------
#	[in] 	QTADS_PATH			- root dir QtADS
#	[out] 	QTADS_INCLUDE		- dir with includes
#	[out]	QTADS_LIB			- lib QtADS
#	[out]	QTADS_DEBUG_LIB		- debug lib QtADS
#	[out]	QTADS_FOUND			- is found QtADS
# 	---------------------------------

set( QTADS_FOUND OFF )
SET( QTADS_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${QTADS_PATH} )

#
# Set path suffixed to libs for current platform
#
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( QTADS_LIB_PATH_SUFFIXES "libs/Win64" )
	elseif( PLATFORM_32BIT )
		set( QTADS_LIB_PATH_SUFFIXES "libs/Win32" )
	endif()
else()
	message( SEND_ERROR "Qt Advanced docking system :: Unknown platform" )
	return()
endif()

#
# Find pathes
#
find_path( 		QTADS_INCLUDE
				NAMES "ads_globals.h"
				PATH_SUFFIXES src
				PATHS ${QTADS_SEARCH_PATHS} )
find_library( 	QTADS_LIB
                NAMES qtadvanceddocking
                PATH_SUFFIXES ${QTADS_LIB_PATH_SUFFIXES}
                PATHS ${QTADS_SEARCH_PATHS} )
find_library( 	QTADS_DEBUG_LIB
                NAMES qtadvanceddocking-d
                PATH_SUFFIXES ${QTADS_LIB_PATH_SUFFIXES}
                PATHS ${QTADS_SEARCH_PATHS} )

if ( NOT QTADS_INCLUDE OR NOT QTADS_LIB OR NOT QTADS_DEBUG_LIB )
    return()
else()
	set( QTADS_FOUND ON )
endif()
