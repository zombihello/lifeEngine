# *.H
file( GLOB INCLUDE_FILES "${GAME_SOURCE_PATH}/Include/*.h" )
file( GLOB INCLUDE_ACTORS_FILES "${GAME_SOURCE_PATH}/Include/Actors/*.h" )

# *.CPP
file( GLOB SOURCE_FILES "${GAME_SOURCE_PATH}/Source/*.cpp" )
file( GLOB SOURCE_ACTORS_FILES "${GAME_SOURCE_PATH}/Source/Actors/*.cpp" )

set( ALL_CPP_FILES ${SOURCE_FILES} ${SOURCE_ACTORS_FILES} )
set( ALL_H_FILES ${INCLUDE_FILES} ${INCLUDE_ACTORS_FILES} )

# Source groups
source_group( "HeliumGame/Include" FILES ${INCLUDE_FILES} )	
source_group( "HeliumGame/Include/Actors" FILES ${INCLUDE_ACTORS_FILES} )
source_group( "HeliumGame/Source" FILES ${SOURCE_FILES} )
source_group( "HeliumGame/Source/Actors" FILES ${SOURCE_ACTORS_FILES} )

# Set include dirs and game name
set( INCLUDE_DIRS ${INCLUDE_DIRS} "${GAME_SOURCE_PATH}/Include" )
set( GAME_NAME "HeliumGame" )

# Set all source files
set( ALL_SOURCE_FILES ${ALL_SOURCE_FILES} ${ALL_CPP_FILES} ${ALL_H_FILES} )