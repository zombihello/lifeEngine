function( EngineBuild MODULE_DIR ALL_SOURCE_FILES INCLUDE_DIRS )
	# *.H
	file( GLOB INCLUDE_FILES "${MODULE_DIR}/Include/*.h" )
	file( GLOB INCLUDE_MISC_FILES "${MODULE_DIR}/Include/Misc/*.h" )
	file( GLOB INCLUDE_RENDER_FILES "${MODULE_DIR}/Include/Render/*.h" )
	file( GLOB INCLUDE_RENDER_SHADERS_FILES "${MODULE_DIR}/Include/Render/Shaders/*.h" )
	file( GLOB INCLUDE_RHI_FILES "${MODULE_DIR}/Include/RHI/*.h" )
	file( GLOB INCLUDE_SCRIPTS_FILES "${MODULE_DIR}/Include/Scripts/*.h" )
	file( GLOB INCLUDE_SYSTEM_FILES "${MODULE_DIR}/Include/System/*.h" )
	
	# *.CPP
	file( GLOB SOURCE_FILES "${MODULE_DIR}/Source/*.cpp" )
	file( GLOB SOURCE_MISC_FILES "${MODULE_DIR}/Source/Misc/*.cpp" )
	file( GLOB SOURCE_RENDER_FILES "${MODULE_DIR}/Source/Render/*.cpp" )
	file( GLOB SOURCE_RENDER_SHADERS_FILES "${MODULE_DIR}/Source/Render/Shaders/*.cpp" )
	file( GLOB SOURCE_SCRIPTS_FILES "${MODULE_DIR}/Source/Scripts/*.cpp" )
	file( GLOB SOURCE_SYSTEM_FILES "${MODULE_DIR}/Source/System/*.cpp" )
	
	# Source groups
	source_group( "Engine/Engine/Include" FILES ${INCLUDE_FILES} )
	source_group( "Engine/Engine/Include/Misc" FILES ${INCLUDE_MISC_FILES} )
	source_group( "Engine/Engine/Include/Render" FILES ${INCLUDE_RENDER_FILES} )
	source_group( "Engine/Engine/Include/Render/Shaders" FILES ${INCLUDE_RENDER_SHADERS_FILES} )
	source_group( "Engine/Engine/Include/RHI" FILES ${INCLUDE_RHI_FILES} )
	source_group( "Engine/Engine/Include/Scripts" FILES ${INCLUDE_SCRIPTS_FILES} )
	source_group( "Engine/Engine/Include/System" FILES ${INCLUDE_SYSTEM_FILES} )

	source_group( "Engine/Engine/Source" FILES ${SOURCE_FILES} )
	source_group( "Engine/Engine/Source/Misc" FILES ${SOURCE_MISC_FILES} )
	source_group( "Engine/Engine/Source/Render" FILES ${SOURCE_RENDER_FILES} )
	source_group( "Engine/Engine/Source/Render/Shaders" FILES ${SOURCE_RENDER_SHADERS_FILES} )
	source_group( "Engine/Engine/Source/Scripts" FILES ${SOURCE_SCRIPTS_FILES} )
	source_group( "Engine/Engine/Source/System" FILES ${SOURCE_SYSTEM_FILES} )

	# Set include dirs
	set( INCLUDE_DIRS ${INCLUDE_DIRS} "${MODULE_DIR}/Include" PARENT_SCOPE )

	# Set all source files
	set( ALL_SOURCE_FILES ${ALL_SOURCE_FILES}  
		${INCLUDE_FILES} ${INCLUDE_MISC_FILES} ${INCLUDE_RENDER_FILES} ${INCLUDE_RENDER_SHADERS_FILES} ${INCLUDE_RHI_FILES} ${INCLUDE_SCRIPTS_FILES} ${INCLUDE_SYSTEM_FILES}
		${SOURCE_FILES} ${SOURCE_MISC_FILES} ${SOURCE_RENDER_FILES} ${SOURCE_RENDER_SHADERS_FILES} ${SOURCE_SCRIPTS_FILES} ${SOURCE_SYSTEM_FILES} PARENT_SCOPE )
endfunction()