# 	---------------------------------
#	[in] 	VORBIS_PATH		- root dir vorbis
#	[out] 	VORBIS_INCLUDE	- dir with includes
#	[out]	VORBIS_LIB		- lib vorbis
#	[out]	VORBISSENC_LIB	- lib vorbissenc
#	[out]	VORBISFILE_LIB	- lib vorbisfile
#	[out]	VORBIS_FOUND	- is found vorbis
# 	---------------------------------

set( VORBIS_FOUND OFF )
SET( VORBIS_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${VORBIS_PATH} )

#
# Set path suffixed to libs for current platform
#
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( VORBIS_LIB_PATH_SUFFIXES "lib/Win64" )
	elseif( PLATFORM_32BIT )
		set( VORBIS_LIB_PATH_SUFFIXES "lib/Win32" )
	endif()
else()
	message( SEND_ERROR "Vorbis :: Unknown platform" )
	return()
endif()

#
# Find pathes
#
find_path( 		VORBIS_INCLUDE
				NAMES "vorbis/vorbisenc.h"
				PATH_SUFFIXES include
				PATHS ${VORBIS_SEARCH_PATHS} )			
find_library( 	VORBIS_LIB 
                NAMES vorbis
                PATH_SUFFIXES ${VORBIS_LIB_PATH_SUFFIXES}
                PATHS ${VORBIS_SEARCH_PATHS} )
find_library( 	VORBISENC_LIB 
                NAMES vorbisenc
                PATH_SUFFIXES ${VORBIS_LIB_PATH_SUFFIXES}
                PATHS ${VORBIS_SEARCH_PATHS} )
find_library( 	VORBISFILE_LIB 
                NAMES vorbisfile
                PATH_SUFFIXES ${VORBIS_LIB_PATH_SUFFIXES}
                PATHS ${VORBIS_SEARCH_PATHS} )
				
if ( NOT VORBIS_INCLUDE OR NOT VORBIS_LIB OR NOT VORBISENC_LIB OR NOT VORBISFILE_LIB )
    return()
else()
	set( VORBIS_FOUND ON )
endif()
