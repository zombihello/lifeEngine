# *.H
file( GLOB INCLUDE_FILES "${PROJECT_CORE}/Include/*.h" )
file( GLOB INCLUDE_CONTAINERS_FILES "${PROJECT_CORE}/Include/Containers/*.h" )
file( GLOB INCLUDE_LOGGER_FILES "${PROJECT_CORE}/Include/Logger/*.h" )
file( GLOB INCLUDE_MATH_FILES "${PROJECT_CORE}/Include/Math/*.h" )
file( GLOB INCLUDE_MISC_FILES "${PROJECT_CORE}/Include/Misc/*.h" )
file( GLOB INCLUDE_SYSTEM_FILES "${PROJECT_CORE}/Include/System/*.h" )

# *.CPP
file( GLOB SOURCE_FILES "${PROJECT_CORE}/Source/*.cpp" )
file( GLOB SOURCE_CONTAINERS_FILES "${PROJECT_CORE}/Source/Containers/*.cpp" )
file( GLOB SOURCE_LOGGER_FILES "${PROJECT_CORE}/Source/Logger/*.cpp" )
file( GLOB SOURCE_MATH_FILES "${PROJECT_CORE}/Source/Math/*.cpp" )
file( GLOB SOURCE_MISC_FILES "${PROJECT_CORE}/Source/Misc/*.cpp" )
file( GLOB SOURCE_SYSTEM_FILES "${PROJECT_CORE}/Source/System/*.cpp" )

# Source groups
source_group( "Engine/Core/Include" FILES ${INCLUDE_FILES} )
source_group( "Engine/Core/Include/Containers" FILES ${INCLUDE_CONTAINERS_FILES} )
source_group( "Engine/Core/Include/Logger" FILES ${INCLUDE_LOGGER_FILES} )
source_group( "Engine/Core/Include/Math" FILES ${INCLUDE_MATH_FILES} )
source_group( "Engine/Core/Include/Misc" FILES ${INCLUDE_MISC_FILES} )
source_group( "Engine/Core/Include/System" FILES ${INCLUDE_SYSTEM_FILES} )
source_group( "Engine/Core/Source" FILES ${SOURCE_FILES} )
source_group( "Engine/Core/Source/Containers" FILES ${SOURCE_CONTAINERS_FILES} )
source_group( "Engine/Core/Source/Logger" FILES ${SOURCE_LOGGER_FILES} )
source_group( "Engine/Core/Source/Math" FILES ${SOURCE_MATH_FILES} )
source_group( "Engine/Core/Source/Misc" FILES ${SOURCE_MISC_FILES} )
source_group( "Engine/Core/Source/System" FILES ${SOURCE_SYSTEM_FILES} )

# Set include dirs
set( INCLUDE_DIRS ${INCLUDE_DIRS} "${PROJECT_CORE}/Include" )

# Set all source files
set( ALL_SOURCE_FILES ${ALL_SOURCE_FILES}
	${INCLUDE_FILES} ${INCLUDE_CONTAINERS_FILES} ${INCLUDE_LOGGER_FILES} ${INCLUDE_MATH_FILES} ${INCLUDE_MISC_FILES} ${INCLUDE_SYSTEM_FILES}
	${SOURCE_FILES} ${SOURCE_CONTAINERS_FILES} ${SOURCE_LOGGER_FILES} ${SOURCE_MATH_FILES} ${SOURCE_MISC_FILES} ${SOURCE_SYSTEM_FILES} )