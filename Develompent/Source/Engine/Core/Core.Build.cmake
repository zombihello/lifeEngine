function( CoreBuild MODULE_DIR ALL_SOURCE_FILES INCLUDE_DIRS )
	# *.H
	file( GLOB INCLUDE_FILES "${MODULE_DIR}/Include/*.h" )
	file( GLOB INCLUDE_CONTAINERS_FILES "${MODULE_DIR}/Include/Containers/*.h" )
	file( GLOB INCLUDE_LOGGER_FILES "${MODULE_DIR}/Include/Logger/*.h" )
	file( GLOB INCLUDE_MATH_FILES "${MODULE_DIR}/Include/Math/*.h" )
	file( GLOB INCLUDE_MISC_FILES "${MODULE_DIR}/Include/Misc/*.h" )
	file( GLOB INCLUDE_SYSTEM_FILES "${MODULE_DIR}/Include/System/*.h" )
	
	# *.CPP
	file( GLOB SOURCE_FILES "${MODULE_DIR}/Source/*.cpp" )
	file( GLOB SOURCE_CONTAINERS_FILES "${MODULE_DIR}/Source/Containers/*.cpp" )
	file( GLOB SOURCE_LOGGER_FILES "${MODULE_DIR}/Source/Logger/*.cpp" )
	file( GLOB SOURCE_MATH_FILES "${MODULE_DIR}/Source/Math/*.cpp" )
	file( GLOB SOURCE_MISC_FILES "${MODULE_DIR}/Source/Misc/*.cpp" )
	file( GLOB SOURCE_SYSTEM_FILES "${MODULE_DIR}/Source/System/*.cpp" )
	
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
	set( INCLUDE_DIRS ${INCLUDE_DIRS} "${MODULE_DIR}/Include" PARENT_SCOPE )

	# Set all source files
	set( ALL_SOURCE_FILES ${ALL_SOURCE_FILES}
		${INCLUDE_FILES} ${INCLUDE_CONTAINERS_FILES} ${INCLUDE_LOGGER_FILES} ${INCLUDE_MATH_FILES} ${INCLUDE_MISC_FILES} ${INCLUDE_SYSTEM_FILES}
		${SOURCE_FILES} ${SOURCE_CONTAINERS_FILES} ${SOURCE_LOGGER_FILES} ${SOURCE_MATH_FILES} ${SOURCE_MISC_FILES} ${SOURCE_SYSTEM_FILES} PARENT_SCOPE )
endfunction()