# *.H
file( GLOB INCLUDE_FILES "${PROJECT_RHI}/Include/*.h" )

# *.CPP
file( GLOB SOURCE_FILES "${PROJECT_RHI}/Source/*.cpp" )

# Source groups
source_group( "Engine/RHI/D3D11RHI/Include" FILES ${INCLUDE_FILES} )
source_group( "Engine/RHI/D3D11RHI/Source" FILES ${SOURCE_FILES} )

# Set include dirs
set( INCLUDE_DIRS ${INCLUDE_DIRS} "${PROJECT_RHI}/Include" )

# Set all source files
set( ALL_SOURCE_FILES ${ALL_SOURCE_FILES} ${INCLUDE_FILES} ${SOURCE_FILES} )