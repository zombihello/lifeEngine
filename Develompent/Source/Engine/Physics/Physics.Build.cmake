# *.H
file( GLOB INCLUDE_FILES "${PROJECT_PHYSICS}/Include/*.h" )
file( GLOB INCLUDE_SYSTEM_FILES "${PROJECT_PHYSICS}/Include/System/*.h" )
file( GLOB INCLUDE_MISC_FILES "${PROJECT_PHYSICS}/Include/Misc/*.h" )

# *.CPP
file( GLOB SOURCE_FILES "${PROJECT_PHYSICS}/Source/*.cpp" )
file( GLOB SOURCE_SYSTEM_FILES "${PROJECT_PHYSICS}/Source/System/*.cpp" )
file( GLOB SOURCE_MISC_FILES "${PROJECT_PHYSICS}/Source/Misc/*.cpp" )

# Source groups
source_group( "Engine/Physics/Include" FILES ${INCLUDE_FILES} )
source_group( "Engine/Physics/Include/System" FILES ${INCLUDE_SYSTEM_FILES} )
source_group( "Engine/Physics/Include/Misc" FILES ${INCLUDE_MISC_FILES} )

source_group( "Engine/Physics/Source" FILES ${SOURCE_FILES} )
source_group( "Engine/Physics/Source/System" FILES ${SOURCE_SYSTEM_FILES} )
source_group( "Engine/Physics/Source/Misc" FILES ${SOURCE_MISC_FILES} )

# Set include dirs
set( INCLUDE_DIRS ${INCLUDE_DIRS} "${PROJECT_PHYSICS}/Include" )

# Set all source files
set( ALL_SOURCE_FILES ${ALL_SOURCE_FILES}  
						${INCLUDE_FILES} ${INCLUDE_SYSTEM_FILES} ${INCLUDE_MISC_FILES}
						${SOURCE_FILES} ${SOURCE_SYSTEM_FILES} ${SOURCE_MISC_FILES} )