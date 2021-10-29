function( GameBuild MODULE_DIR ALL_SOURCE_FILES INCLUDE_DIRS )
	# *.H
	file( GLOB INCLUDE_FILES "${MODULE_DIR}/Include/*.h" )
	
	# *.CPP
	file( GLOB SOURCE_FILES "${MODULE_DIR}/Source/*.cpp" )

	set( ALL_CPP_FILES ${SOURCE_FILES} )
	set( ALL_H_FILES ${INCLUDE_FILES} )

	# Source groups
	source_group( "HeliumGame/Include" FILES ${INCLUDE_FILES} )	
	source_group( "HeliumGame/Source" FILES ${SOURCE_FILES} )

	# Set include dirs and game name
	set( INCLUDE_DIRS ${INCLUDE_DIRS} "${MODULE_DIR}/Include" PARENT_SCOPE )
	set( GAME_NAME "HeliumGame" PARENT_SCOPE )

	# Set all source files
	set( ALL_SOURCE_FILES ${ALL_SOURCE_FILES} ${ALL_CPP_FILES} ${ALL_H_FILES} PARENT_SCOPE )
endfunction()