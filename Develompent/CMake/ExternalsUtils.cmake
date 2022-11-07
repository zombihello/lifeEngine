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
	
	# OpenAL
	find_package( OpenAL REQUIRED )
	if ( OPENAL_FOUND )
		include_directories( ${OPENAL_INCLUDE} )
		target_link_libraries( ${MODULE_NAME} ${OPENAL_LIB} )
	else()
		message( SEND_ERROR "Failed to find OpenAL" )
	endif()

	# Ogg
	find_package( Ogg REQUIRED )
	if ( OGG_FOUND )
		include_directories( ${OGG_INCLUDE} )
		target_link_libraries( ${MODULE_NAME} ${OGG_LIB} )
	else()
		message( SEND_ERROR "Failed to find Ogg" )
	endif()
	
	# Vorbis
	find_package( Vorbis REQUIRED )
	if ( VORBIS_FOUND )
		include_directories( ${VORBIS_INCLUDE} )
		target_link_libraries( ${MODULE_NAME} ${VORBIS_LIB} ${VORBISSENC_LIB} ${VORBISFILE_LIB} )
	else()
		message( SEND_ERROR "Failed to find Vorbis" )
	endif()
	
	# Theora
	if ( USE_THEORA_CODEC )
		find_package( Theora REQUIRED )
		if ( THEORA_FOUND )
			include_directories( ${THEORA_INCLUDE} )
			target_link_libraries( ${MODULE_NAME} optimized ${THEORA_LIB} debug ${THEORA_DEBUG_LIB} )
		else()
			message( SEND_ERROR "Failed to find Theora" )
		endif()
	endif()
	
	# For physics in 2D engine we use Box2D, else using PhysX
	if ( ENGINE_2D )
		# Box2D
		find_package( Box2D REQUIRED )
		if ( BOX2D_FOUND )
			include_directories( ${BOX2D_INCLUDE} )
			target_link_libraries( ${MODULE_NAME} optimized ${BOX2D_LIB} debug ${BOX2D_DEBUG_LIB} )
		else()
			message( SEND_ERROR "Failed to find Box2D" )
		endif()		
	else()
		# PhysX
		find_package( PhysX REQUIRED )
		if ( PHYSX_FOUND )
			include_directories( ${PHYSX_INCLUDE} )
			target_link_libraries( ${MODULE_NAME} optimized ${PHYSX_LIB} debug ${PHYSX_DEBUG_LIB}
												  optimized ${PHYSX_FOUNDATION_LIB} debug ${PHYSX_FOUNDATION_DEBUG_LIB}
												  optimized ${PHYSX_COMMON_LIB} debug ${PHYSX_COMMON_DEBUG_LIB}
												  optimized ${PHYSX_EXTENSIONS_LIB} debug ${PHYSX_EXTENSIONS_DEBUG_LIB} )
		else()
			message( SEND_ERROR "Failed to find PhysX" )
		endif()
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
	
		# TMXLite
		find_package( TMXLite REQUIRED )
		if ( TMXLITE_FOUND )
			include_directories( ${TMXLITE_INCLUDE} )
			target_link_libraries( ${MODULE_NAME} optimized ${TMXLITE_LIB} debug ${TMXLITE_DEBUG_LIB} )
		else()
			message( SEND_ERROR "Failed to find TMXLite" )
		endif()
		
		if ( NOT ENGINE_2D )
			# PhysX PVD
			target_link_libraries( ${MODULE_NAME} optimized ${PHYSX_PVDSDK_LIB} debug ${PHYSX_PVDSDK_DEBUG_LIB} )
		endif()
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
			set( PLATFORM_PHYSX_BIN_DIR "${PHYSX_PATH}/PhysX/bin/win.x86_64.vc142.md" )			
			set( PLATFORM_PHYSX_BIT_SUFIX "_64" )
		elseif( PLATFORM_32BIT )
			set( PLATFORM_BIN_DIR "Win32" )
			set( PLATFORM_PHYSX_BIN_DIR "${PHYSX_PATH}/PhysX/bin/win.x86.vc142.md" )	
			set( PLATFORM_PHYSX_BIT_SUFIX "" )
		endif()
		
		set( PLATFORM_DLL_EXTENSION ".dll" )
	else()
		message( SEND_ERROR "Unknown platform" )
	endif()
	
	if ( ENGINE_2D )
		set( PHYSICS_BINARES )
	else()
		set( PHYSICS_BINARES 	"${PLATFORM_PHYSX_BIN_DIR}/debug/PhysXCommonDEBUG${PLATFORM_PHYSX_BIT_SUFIX}${PLATFORM_DLL_EXTENSION}"
								"${PLATFORM_PHYSX_BIN_DIR}/debug/PhysXCookingDEBUG${PLATFORM_PHYSX_BIT_SUFIX}${PLATFORM_DLL_EXTENSION}"
								"${PLATFORM_PHYSX_BIN_DIR}/debug/PhysXDEBUG${PLATFORM_PHYSX_BIT_SUFIX}${PLATFORM_DLL_EXTENSION}" 
								"${PLATFORM_PHYSX_BIN_DIR}/debug/PhysXFoundationDEBUG${PLATFORM_PHYSX_BIT_SUFIX}${PLATFORM_DLL_EXTENSION}"
								"${PLATFORM_PHYSX_BIN_DIR}/release/PhysXCommon${PLATFORM_PHYSX_BIT_SUFIX}${PLATFORM_DLL_EXTENSION}"
								"${PLATFORM_PHYSX_BIN_DIR}/release/PhysXCooking${PLATFORM_PHYSX_BIT_SUFIX}${PLATFORM_DLL_EXTENSION}"		# BS yehor.pohuliaka - Maybe PhysXCooking need use only with WITH_EDITOR
								"${PLATFORM_PHYSX_BIN_DIR}/release/PhysX${PLATFORM_PHYSX_BIT_SUFIX}${PLATFORM_DLL_EXTENSION}" 
								"${PLATFORM_PHYSX_BIN_DIR}/release/PhysXFoundation${PLATFORM_PHYSX_BIT_SUFIX}${PLATFORM_DLL_EXTENSION}" )
	endif()
	
	set( BINARES "${SDL2_PATH}/lib/${PLATFORM_BIN_DIR}/SDL2${PLATFORM_DLL_EXTENSION}"
				 "${OPENAL_PATH}/bin/${PLATFORM_BIN_DIR}/OpenAL32${PLATFORM_DLL_EXTENSION}"
				 ${PHYSICS_BINARES} )	
		
	if ( WITH_EDITOR )
		set( BINARES ${BINARES}
			 "${ASSIMP_PATH}/bin/${PLATFORM_BIN_DIR}/assimp${PLATFORM_DLL_EXTENSION}" 						"${ASSIMP_PATH}/bin/${PLATFORM_BIN_DIR}/assimpd${PLATFORM_DLL_EXTENSION}" )
	endif()

	install( FILES ${BINARES} DESTINATION ${INSTALL_DIR} )
endfunction()