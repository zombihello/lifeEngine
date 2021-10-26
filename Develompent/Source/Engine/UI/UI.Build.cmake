function( UIBuild MODULE_DIR ALL_SOURCE_FILES INCLUDE_DIRS )
	# *.H
	file( GLOB INCLUDE_FILES "${MODULE_DIR}/Include/*.h" )
	file( GLOB INCLUDE_IMGUI_FILES "${MODULE_DIR}/Include/ImGUI/*.h" )
	file( GLOB INCLUDE_MISC_FILES "${MODULE_DIR}/Include/Misc/*.h" )
	
	# *.CPP
	file( GLOB SOURCE_FILES "${MODULE_DIR}/Source/*.cpp" )
	file( GLOB SOURCE_IMGUI_FILES "${MODULE_DIR}/Source/ImGUI/*.cpp" )
	file( GLOB SOURCE_MISC_FILES "${MODULE_DIR}/Source/Misc/*.cpp" )
	
	# Source groups
	source_group( "Engine/UI/Include" FILES ${INCLUDE_FILES} )
	source_group( "Engine/UI/Include/ImGUI" FILES ${INCLUDE_IMGUI_FILES} )
	source_group( "Engine/UI/Include/Misc" FILES ${INCLUDE_MISC_FILES} )
	source_group( "Engine/UI/Source" FILES ${SOURCE_FILES} )
	source_group( "Engine/UI/Source/ImGUI" FILES ${SOURCE_IMGUI_FILES} )
	source_group( "Engine/UI/Source/Misc" FILES ${SOURCE_MISC_FILES} )

	# Set include dirs
	set( INCLUDE_DIRS ${INCLUDE_DIRS} "${MODULE_DIR}/Include" PARENT_SCOPE )

	# Set all source files
	set( ALL_SOURCE_FILES ${ALL_SOURCE_FILES} 
		${INCLUDE_FILES} ${INCLUDE_IMGUI_FILES} ${INCLUDE_MISC_FILES} 
		${SOURCE_FILES} ${SOURCE_IMGUI_FILES} ${SOURCE_MISC_FILES} PARENT_SCOPE )
endfunction()