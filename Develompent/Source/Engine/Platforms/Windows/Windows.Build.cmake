function( WindowsBuild MODULE_DIR ALL_SOURCE_FILES INCLUDE_DIRS )
	# *.H
	file( GLOB INCLUDE_FILES "${MODULE_DIR}/Include/*.h" )
	
	# *.CPP
	file( GLOB SOURCE_FILES "${MODULE_DIR}/Source/*.cpp" )
	
	# Source groups
	source_group( "Engine/Platform/Windows/Include" FILES ${INCLUDE_FILES} )
	source_group( "Engine/Platform/Windows/Source" FILES ${SOURCE_FILES} )

	# Set include dirs
	set( INCLUDE_DIRS ${INCLUDE_DIRS} "${MODULE_DIR}/Include" PARENT_SCOPE )
	
	# Set all source files
	set( ALL_SOURCE_FILES ${ALL_SOURCE_FILES} ${INCLUDE_FILES} ${SOURCE_FILES} PARENT_SCOPE )
endfunction()