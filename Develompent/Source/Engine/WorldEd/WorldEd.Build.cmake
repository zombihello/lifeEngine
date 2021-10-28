function( WorldEdBuild MODULE_DIR ALL_SOURCE_FILES INCLUDE_DIRS )
	# *.H
	file( GLOB INCLUDE_FILES "${MODULE_DIR}/Include/*.h" )
	file( GLOB INCLUDE_COMMANDLETS_FILES "${MODULE_DIR}/Include/Commandlets/*.h" )
	file( GLOB INCLUDE_MISC_FILES "${MODULE_DIR}/Include/Misc/*.h" )
	file( GLOB INCLUDE_SYSTEM_FILES "${MODULE_DIR}/Include/System/*.h" )
	file( GLOB INCLUDE_WINDOWS_FILES "${MODULE_DIR}/Include/Windows/*.h" )
	file( GLOB INCLUDE_WIDGETS_FILES "${MODULE_DIR}/Include/Widgets/*.h" )
	
	# *.CPP
	file( GLOB SOURCE_FILES "${MODULE_DIR}/Source/*.cpp" )
	file( GLOB SOURCE_COMMANDLETS_FILES "${MODULE_DIR}/Source/Commandlets/*.cpp" )
	file( GLOB SOURCE_MISC_FILES "${MODULE_DIR}/Source/Misc/*.cpp" )
	file( GLOB SOURCE_SYSTEM_FILES "${MODULE_DIR}/Source/System/*.cpp" )
	file( GLOB SOURCE_WINDOWS_FILES "${MODULE_DIR}/Source/Windows/*.cpp" )
	file( GLOB SOURCE_WIDGETS_FILES "${MODULE_DIR}/Source/Widgets/*.cpp" )
	
	# *.UI
	file( GLOB FORMS_FILES "${MODULE_DIR}/Forms/*.ui" )
	file( GLOB FORMS_WINDOWS_FILES "${MODULE_DIR}/Forms/Windows/*.ui" )

	set( ALL_CPP_FILES ${SOURCE_FILES} ${SOURCE_COMMANDLETS_FILES} ${SOURCE_MISC_FILES} ${SOURCE_SYSTEM_FILES} ${SOURCE_WINDOWS_FILES} ${SOURCE_WIDGETS_FILES} )
	set( ALL_H_FILES ${INCLUDE_FILES} ${INCLUDE_COMMANDLETS_FILES} ${INCLUDE_MISC_FILES} ${INCLUDE_SYSTEM_FILES} ${INCLUDE_WINDOWS_FILES} ${INCLUDE_WIDGETS_FILES} )
	set( ALL_UI_FILES ${FORMS_FILES} ${FORMS_WINDOWS_FILES} )

	# Qt settings
	find_package( Qt6 COMPONENTS Widgets Core REQUIRED )
	qt_wrap_ui( UI_HEADERS ${FORMS_FILES} ${FORMS_WINDOWS_FILES} )
	qt_wrap_cpp( MOC_FILES ${ALL_H_FILES} )

	# Source groups
	source_group( "Engine/WorldEd/Include" FILES ${INCLUDE_FILES} )
	source_group( "Engine/WorldEd/Include/Commandlets" FILES ${INCLUDE_COMMANDLETS_FILES} )
	source_group( "Engine/WorldEd/Include/Misc" FILES ${INCLUDE_MISC_FILES} )
	source_group( "Engine/WorldEd/Include/System" FILES ${INCLUDE_SYSTEM_FILES} )
	source_group( "Engine/WorldEd/Include/Windows" FILES ${INCLUDE_WINDOWS_FILES} )
	source_group( "Engine/WorldEd/Include/Widgets" FILES ${INCLUDE_WIDGETS_FILES} )
	
	source_group( "Engine/WorldEd/Source" FILES ${SOURCE_FILES} )
	source_group( "Engine/WorldEd/Source/Commandlets" FILES ${SOURCE_COMMANDLETS_FILES} )
	source_group( "Engine/WorldEd/Source/Misc" FILES ${SOURCE_MISC_FILES} )
	source_group( "Engine/WorldEd/Source/System" FILES ${SOURCE_SYSTEM_FILES} )
	source_group( "Engine/WorldEd/Source/Windows" FILES ${SOURCE_WINDOWS_FILES} )
	source_group( "Engine/WorldEd/Moc" FILES ${MOC_FILES} )
	source_group( "Engine/WorldEd/Source/Widgets" FILES ${SOURCE_WIDGETS_FILES} )

	source_group( "Engine/WorldEd/Forms" FILES ${FORMS_FILES} )
	source_group( "Engine/WorldEd/Forms/Windows" FILES ${FORMS_WINDOWS_FILES} )
	source_group( "Engine/WorldEd/Forms/UIHeaders" FILES ${UI_HEADERS} )

	#qt_add_resources ( RCC_SOURCES ${RESOURCES} )
	
	# Set include dirs
	set( INCLUDE_DIRS ${INCLUDE_DIRS} "${MODULE_DIR}/Include" PARENT_SCOPE )

	# Set all source files
	set( ALL_SOURCE_FILES ${ALL_SOURCE_FILES}
		${ALL_CPP_FILES}
		${ALL_H_FILES} ${UI_HEADERS}
		${ALL_UI_FILES}
		${MOC_FILES} PARENT_SCOPE )
endfunction()