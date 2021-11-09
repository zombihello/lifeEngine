# 	---------------------------------
#	[in] 	ASSIMP_PATH		- root dir Assimp
#	[out] 	ASSIMP_INCLUDE	- dir with includes
#	[out]	ASSIMP_LIB		- lib Assimp
#	[out]	ASSIMP_FOUND	- is found Assimp
# 	---------------------------------

set( ASSIMP_FOUND OFF )
SET( ASSIMP_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${ASSIMP_PATH} )

#
# Set path suffixed to libs for current platform
#
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( ASSIMP_LIB_PATH_SUFFIXES "lib/Win64" )
	elseif( PLATFORM_32BIT )
		set( ASSIMP_LIB_PATH_SUFFIXES "lib/Win32" )
	endif()
else()
	message( SEND_ERROR "Assimp :: Unknown platform" )
	return()
endif()

#
# Find pathes
#
find_path( 		ASSIMP_INCLUDE
				NAMES assimp/postprocess.h assimp/scene.h assimp/version.h assimp/config.h assimp/cimport.h
				PATH_SUFFIXES include
				PATHS ${ASSIMP_SEARCH_PATHS} )			
find_library( 	ASSIMP_MAIN_LIB 
                NAMES assimp
                PATH_SUFFIXES ${ASSIMP_LIB_PATH_SUFFIXES}
                PATHS ${ASSIMP_SEARCH_PATHS} )

if ( PLATFORM_WINDOWS )
	find_library( 	ASSIMP_ZLIB_LIB 
					NAMES zlibstatic
					PATH_SUFFIXES ${ASSIMP_LIB_PATH_SUFFIXES}
					PATHS ${ASSIMP_SEARCH_PATHS} )
	find_library( 	ASSIMP_IRRXML_LIB 
					NAMES IrrXML
					PATH_SUFFIXES ${ASSIMP_LIB_PATH_SUFFIXES}
					PATHS ${ASSIMP_SEARCH_PATHS} )
else()
	set( ASSIMP_ZLIB_LIB )
	set( ASSIMP_IRRXML_LIB )
endif()
	
if ( NOT ASSIMP_INCLUDE OR NOT ASSIMP_MAIN_LIB OR NOT ASSIMP_ZLIB_LIB OR NOT ASSIMP_IRRXML_LIB )
    return()
else()
	set( ASSIMP_FOUND ON )
	set( ASSIMP_LIB ${ASSIMP_MAIN_LIB} ${ASSIMP_ZLIB_LIB} ${ASSIMP_IRRXML_LIB} )
endif()
