# 	---------------------------------
#	[in] 	STB_PATH		- root dir STB
#	[out] 	STB_INCLUDE		- dir with includes
#	[out]	STB_FOUND		- is found STB
# 	---------------------------------

set( STB_FOUND OFF )
SET( STB_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${STB_PATH} )

#
# Find pathes
#
find_path( 		STB_INCLUDE
				NAMES "stb_image.h"
				PATH_SUFFIXES include
				PATHS ${STB_SEARCH_PATHS} )
				
if ( NOT STB_INCLUDE )
    return()
else()
	set( STB_FOUND ON )
endif()
