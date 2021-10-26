function( LaunchBuild MODULE_DIR ALL_SOURCE_FILES INCLUDE_DIRS )
	# *.H
	file( GLOB INCLUDE_FILES "${MODULE_DIR}/Include/*.h" )
	file( GLOB INCLUDE_MISC_FILES "${MODULE_DIR}/Include/Misc/*.h" )
	
	# *.CPP
	file( GLOB SOURCE_FILES "${MODULE_DIR}/Source/*.cpp" )
	
	# Source groups
	source_group( "Engine/Launch/Include" FILES ${INCLUDE_FILES} )
	source_group( "Engine/Launch/Include/Misc" FILES ${INCLUDE_MISC_FILES} )
	source_group( "Engine/Launch/Source" FILES ${SOURCE_FILES} )

	# Set include dirs
	set( INCLUDE_DIRS ${INCLUDE_DIRS} "${MODULE_DIR}/Include" PARENT_SCOPE )

	# Set all source files
	set( ALL_SOURCE_FILES ${ALL_SOURCE_FILES} 
		${INCLUDE_FILES} ${INCLUDE_MISC_FILES} ${SOURCE_FILES} PARENT_SCOPE )
endfunction()