function( WorldEdBuild MODULE_DIR ALL_SOURCE_FILES INCLUDE_DIRS )
	# *.H
	file( GLOB INCLUDE_FILES "${MODULE_DIR}/Include/*.h" )
	file( GLOB INCLUDE_COMMANDLETS_FILES "${MODULE_DIR}/Include/Commandlets/*.h" )
	file( GLOB INCLUDE_MISC_FILES "${MODULE_DIR}/Include/Misc/*.h" )
	file( GLOB INCLUDE_SYSTEM_FILES "${MODULE_DIR}/Include/System/*.h" )
	file( GLOB INCLUDE_WINDOWS_FILES "${MODULE_DIR}/Include/Windows/*.h" )
	
	# *.CPP
	file( GLOB SOURCE_FILES "${MODULE_DIR}/Source/*.cpp" )
	file( GLOB SOURCE_COMMANDLETS_FILES "${MODULE_DIR}/Source/Commandlets/*.cpp" )
	file( GLOB SOURCE_MISC_FILES "${MODULE_DIR}/Source/Misc/*.cpp" )
	file( GLOB SOURCE_SYSTEM_FILES "${MODULE_DIR}/Source/System/*.cpp" )
	file( GLOB SOURCE_WINDOWS_FILES "${MODULE_DIR}/Source/Windows/*.cpp" )
	
	# Source groups
	source_group( "Engine/WorldEd/Include" FILES ${INCLUDE_FILES} )
	source_group( "Engine/WorldEd/Include/Commandlets" FILES ${INCLUDE_COMMANDLETS_FILES} )
	source_group( "Engine/WorldEd/Include/Misc" FILES ${INCLUDE_MISC_FILES} )
	source_group( "Engine/WorldEd/Include/System" FILES ${INCLUDE_SYSTEM_FILES} )
	source_group( "Engine/WorldEd/Include/Windows" FILES ${INCLUDE_WINDOWS_FILES} )
	source_group( "Engine/WorldEd/Source" FILES ${SOURCE_FILES} )
	source_group( "Engine/WorldEd/Source/Commandlets" FILES ${SOURCE_COMMANDLETS_FILES} )
	source_group( "Engine/WorldEd/Source/Misc" FILES ${SOURCE_MISC_FILES} )
	source_group( "Engine/WorldEd/Source/System" FILES ${SOURCE_SYSTEM_FILES} )
	source_group( "Engine/WorldEd/Source/Windows" FILES ${SOURCE_WINDOWS_FILES} )

	# Set include dirs
	set( INCLUDE_DIRS ${INCLUDE_DIRS} "${MODULE_DIR}/Include" PARENT_SCOPE )

	# Set all source files
	set( ALL_SOURCE_FILES ${ALL_SOURCE_FILES} 
		${INCLUDE_FILES} ${INCLUDE_COMMANDLETS_FILES} ${INCLUDE_MISC_FILES} ${INCLUDE_SYSTEM_FILES} ${INCLUDE_WINDOWS_FILES}
		${SOURCE_FILES} ${SOURCE_COMMANDLETS_FILES} ${SOURCE_MISC_FILES} ${SOURCE_SYSTEM_FILES} ${SOURCE_WINDOWS_FILES} PARENT_SCOPE )
endfunction()