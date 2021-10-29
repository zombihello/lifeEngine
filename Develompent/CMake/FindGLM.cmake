# 	---------------------------------
#	[in] 	GLM_PATH		- root dir GLM
#	[out] 	GLM_INCLUDE		- dir with includes
#	[out]	GLM_FOUND		- is found GLM
# 	---------------------------------

set( GLM_FOUND OFF )
SET( GLM_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${GLM_PATH}
)

find_path( 		GLM_INCLUDE
				NAMES "glm.hpp"
				PATH_SUFFIXES glm
				PATHS ${GLM_SEARCH_PATHS} )		
		
if ( NOT GLM_INCLUDE )
    return()
else()
	set( GLM_FOUND ON )
endif()