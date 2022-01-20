# *.H
file( GLOB INCLUDE_FILES "${PROJECT_PLATFORM}/Include/*.h" )

# *.CPP
file( GLOB SOURCE_FILES "${PROJECT_PLATFORM}/Source/*.cpp" )

# Source groups
source_group( "Engine/Platform/Windows/Include" FILES ${INCLUDE_FILES} )
source_group( "Engine/Platform/Windows/Source" FILES ${SOURCE_FILES} )

# Set include dirs
set( INCLUDE_DIRS ${INCLUDE_DIRS} "${PROJECT_PLATFORM}/Include" )

# Set all source files
set( ALL_SOURCE_FILES ${ALL_SOURCE_FILES} ${INCLUDE_FILES} ${SOURCE_FILES} )