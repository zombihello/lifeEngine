# 	---------------------------------
#	[in] 	SDL2_PATH		- root dir sdl2
#	[out] 	SDL2_INCLUDE	- dir with includes
#	[out]	SDL2_LIB		- lib sdl2
#	[out]	SDL2MAIN_LIB	- lib sdl2main
#	[out]	SDL2_FOUND		- is found sdl2
# 	---------------------------------

set( SDL2_FOUND OFF )
SET( SDL2_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${SDL2_PATH} )

#
# Set path suffixed to libs for current platform
#
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( SDL2_LIB_PATH_SUFFIXES "lib/Win64" )
	elseif( PLATFORM_32BIT )
		set( SDL2_LIB_PATH_SUFFIXES "lib/Win32" )
	endif()
else()
	message( SEND_ERROR "SDL2 :: Unknown platform" )
	return()
endif()

#
# Find pathes
#
find_path( 		SDL2_INCLUDE
				NAMES "SDL.h"
				PATH_SUFFIXES include
				PATHS ${SDL2_SEARCH_PATHS} )			
find_library( 	SDL2_LIB 
                NAMES SDL2
                PATH_SUFFIXES ${SDL2_LIB_PATH_SUFFIXES}
                PATHS ${SDL2_SEARCH_PATHS} )
find_library( 	SDL2MAIN_LIB
                NAMES SDL2main
                PATH_SUFFIXES ${SDL2_LIB_PATH_SUFFIXES}
                PATHS ${SDL2_SEARCH_PATHS} )
		
if ( NOT SDL2_INCLUDE OR NOT SDL2_LIB OR NOT SDL2MAIN_LIB )
    return()
else()
	set( SDL2_FOUND ON )
endif()
