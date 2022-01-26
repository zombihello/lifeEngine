macro( InitQt5 )
	# Finding Qt library here, because for set AUTOMOC, AUTORCC and AUTOUIC need seting their before create build target
	find_package( Qt5 COMPONENTS Widgets Core Svg REQUIRED )
	set( CMAKE_AUTOUIC ON )
	set( CMAKE_AUTOMOC ON )
	set( CMAKE_AUTORCC ON )
endmacro()

function( IncludeExternals MODULE_NAME )
	# SDL2
	find_package( SDL2 REQUIRED )
	if ( SDL2_FOUND )
	    include_directories( ${SDL2_INCLUDE} )
		target_link_libraries( ${MODULE_NAME} ${SDL2_LIB} ${SDL2MAIN_LIB} )	
	else()
		message( SEND_ERROR "Failed to find SDL2" )
	endif()
	
	# RapidJSON
	find_package( Rapidjson REQUIRED )
	if ( RAPIDJSON_FOUND )
		include_directories( ${RAPIDJSON_INCLUDE} )
	else()
		message( SEND_ERROR "Failed to find RapidJSON" )
	endif()
	
	# LuaJIT
	find_package( LuaJIT REQUIRED )
	if ( LUAJIT_FOUND )
	    include_directories( ${LUAJIT_INCLUDE} )
		target_link_libraries( ${MODULE_NAME} ${LUAJIT_LIB} )			
	else()
		message( SEND_ERROR "Failed to find LuaJIT" )
	endif()
	
	# LuaBridge
	find_package( LuaBridge REQUIRED )
	if ( LUABRIDGE_FOUND )
		include_directories( ${LUABRIDGE_INCLUDE} )
	else()
		message( SEND_ERROR "Failed to find LuaBridge" )
	endif()
	
	# GLM
	find_package( GLM REQUIRED )
	if ( GLM_FOUND )
		include_directories( ${GLM_INCLUDE} )
	else()
		message( SEND_ERROR "Failed to find GLM" )
	endif()
	
	# Zlib
	find_package( Zlib REQUIRED )
	if ( ZLIB_FOUND )
		include_directories( ${ZLIB_INCLUDE} )
		target_link_libraries( ${MODULE_NAME} ${ZLIB_LIB} )
	else()
		message( SEND_ERROR "Failed to find ZLIB" )
	endif()
	
	#
	# Externals for WorldEd
	#
	if ( WITH_EDITOR )
		# STB
		find_package( STB REQUIRED )
		if ( STB_FOUND )
			include_directories( ${STB_INCLUDE} )
		else()
			message( SEND_ERROR "Failed to find STB" )
		endif()
	
		# Assimp
		find_package( Assimp REQUIRED )
		if ( ASSIMP_FOUND )
			include_directories( ${ASSIMP_INCLUDE} )
			target_link_libraries( ${MODULE_NAME} ${ASSIMP_LIB} )
		else()
			message( SEND_ERROR "Failed to find Assimp" )
		endif()
	
		# Qt5
		target_link_libraries( ${MODULE_NAME} Qt5::Widgets Qt5::Core Qt5::Svg )	
	endif()
	
	if ( PLATFORM_WINDOWS )
		target_link_libraries( ${MODULE_NAME} d3d11 d3d9 dxgi dxguid d3dcompiler )
	endif()
endfunction()

function( InstallExternals INSTALL_DIR )
	set( PLATFORM_BIN_DIR "Unknown" )
	set( PLATFORM_DLL_EXTENSION "Unknown" )
	
	if ( PLATFORM_WINDOWS )
		if ( PLATFORM_64BIT )
			set( PLATFORM_BIN_DIR "Win64" )	
		elseif( PLATFORM_32BIT )
			set( PLATFORM_BIN_DIR "Win32" )
		endif()
		
		set( PLATFORM_DLL_EXTENSION ".dll" )
	else()
		message( SEND_ERROR "Unknown platform" )
	endif()
		
	set( BINARES "${SDL2_PATH}/lib/${PLATFORM_BIN_DIR}/SDL2${PLATFORM_DLL_EXTENSION}" )	
		
	if ( WITH_EDITOR )
		set( BINARES ${BINARES}
			 "${QT5_PATH}/bin/Qt5Cored${PLATFORM_DLL_EXTENSION}" 							"${QT5_PATH}/bin/Qt5Core${PLATFORM_DLL_EXTENSION}"
			 "${QT5_PATH}/bin/Qt5Guid${PLATFORM_DLL_EXTENSION}" 							"${QT5_PATH}/bin/Qt5Gui${PLATFORM_DLL_EXTENSION}"
			 "${QT5_PATH}/bin/Qt5Widgetsd${PLATFORM_DLL_EXTENSION}" 						"${QT5_PATH}/bin/Qt5Widgets${PLATFORM_DLL_EXTENSION}"
			 "${QT5_PATH}/bin/Qt5Svgd${PLATFORM_DLL_EXTENSION}" 							"${QT5_PATH}/bin/Qt5Svg${PLATFORM_DLL_EXTENSION}"
			 "${ASSIMP_PATH}/bin/${PLATFORM_BIN_DIR}/assimp${PLATFORM_DLL_EXTENSION}" 		"${ASSIMP_PATH}/bin/${PLATFORM_BIN_DIR}/assimpd${PLATFORM_DLL_EXTENSION}" )
	endif()
	
	if ( WITH_EDITOR )
		install( DIRECTORY "${QT5_PATH}/plugins/platforms" DESTINATION ${QT_PLUGIN_DIR} )
		install( DIRECTORY "${QT5_PATH}/plugins/styles" DESTINATION ${QT_PLUGIN_DIR} )
	endif()
	install( FILES ${BINARES} DESTINATION ${INSTALL_DIR} )
endfunction()