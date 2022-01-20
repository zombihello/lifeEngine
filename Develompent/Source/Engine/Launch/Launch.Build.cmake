# *.H
file( GLOB INCLUDE_FILES "${PROJECT_LAUNCH}/Include/*.h" )
file( GLOB INCLUDE_MISC_FILES "${PROJECT_LAUNCH}/Include/Misc/*.h" )

# *.CPP
file( GLOB SOURCE_FILES "${PROJECT_LAUNCH}/Source/*.cpp" )

# Source groups
source_group( "Engine/Launch/Include" FILES ${INCLUDE_FILES} )
source_group( "Engine/Launch/Include/Misc" FILES ${INCLUDE_MISC_FILES} )
source_group( "Engine/Launch/Source" FILES ${SOURCE_FILES} )

# Set include dirs
set( INCLUDE_DIRS ${INCLUDE_DIRS} "${PROJECT_LAUNCH}/Include" )

# Set all source files
set( ALL_SOURCE_FILES ${ALL_SOURCE_FILES} 
	${INCLUDE_FILES} ${INCLUDE_MISC_FILES} ${SOURCE_FILES} )