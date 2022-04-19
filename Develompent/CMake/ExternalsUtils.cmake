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

		# WxWidgets
		find_package( WxWidgets REQUIRED )
		if ( WXWIDGETS_FOUND )
			include_directories( ${WXWIDGETS_INCLUDE} )
			target_link_libraries( ${MODULE_NAME} optimized ${WXWIDGETS_LIB} 					debug ${WXWIDGETS_DEBUG_LIB}
												  optimized ${WXWIDGETS_NET_LIB} 				debug ${WXWIDGETS_NET_DEBUG_LIB}
												  optimized ${WXWIDGETS_XML_LIB} 				debug ${WXWIDGETS_XML_DEBUG_LIB}
												  optimized ${WXWIDGETS_ADV_LIB} 				debug ${WXWIDGETS_ADV_DEBUG_LIB}
												  optimized ${WXWIDGETS_AUI_LIB} 				debug ${WXWIDGETS_AUI_DEBUG_LIB}
												  optimized ${WXWIDGETS_CORE_LIB} 				debug ${WXWIDGETS_CORE_DEBUG_LIB}
												  optimized ${WXWIDGETS_GL_LIB} 				debug ${WXWIDGETS_GL_DEBUG_LIB}
												  optimized ${WXWIDGETS_HTML_LIB} 				debug ${WXWIDGETS_HTML_DEBUG_LIB}
												  optimized ${WXWIDGETS_MEDIA_LIB} 				debug ${WXWIDGETS_MEDIA_DEBUG_LIB}
												  optimized ${WXWIDGETS_PROPGRID_LIB} 			debug ${WXWIDGETS_PROPGRID_DEBUG_LIB}
												  optimized ${WXWIDGETS_QA_LIB} 				debug ${WXWIDGETS_QA_DEBUG_LIB}
												  optimized ${WXWIDGETS_RIBBON_LIB} 			debug ${WXWIDGETS_RIBBON_DEBUG_LIB}
												  optimized ${WXWIDGETS_RICHTEXT_LIB} 			debug ${WXWIDGETS_RICHTEXT_DEBUG_LIB}
												  optimized ${WXWIDGETS_STC_LIB} 				debug ${WXWIDGETS_STC_DEBUG_LIB}
												  optimized ${WXWIDGETS_WEBVIEW_LIB} 			debug ${WXWIDGETS_WEBVIEW_DEBUG_LIB}
												  optimized ${WXWIDGETS_XRC_LIB} 				debug ${WXWIDGETS_XRC_DEBUG_LIB} )			
		else()
			message( SEND_ERROR "Failed to find WxWidgets" )
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
			set( PLATFORM_WXWIDGETS_BIN_DIR "${WXWIDGETS_PATH}/lib/vc_x64_dll" )
		elseif( PLATFORM_32BIT )
			set( PLATFORM_BIN_DIR "Win32" )
			set( PLATFORM_PHYSX_BIN_DIR "${PHYSX_PATH}/PhysX/bin/win.x86.vc142.md" )	
			set( PLATFORM_PHYSX_BIT_SUFIX "" )
			set( PLATFORM_WXWIDGETS_BIN_DIR "${WXWIDGETS_PATH}/lib/vc_dll" )
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
			 "${PLATFORM_WXWIDGETS_BIN_DIR}/wxbase315u_vc_custom${PLATFORM_DLL_EXTENSION}"			"${PLATFORM_WXWIDGETS_BIN_DIR}/wxbase315ud_vc_custom${PLATFORM_DLL_EXTENSION}"
			 "${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315u_core_vc_custom${PLATFORM_DLL_EXTENSION}"		"${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315ud_core_vc_custom${PLATFORM_DLL_EXTENSION}"
			 "${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315u_xrc_vc_custom${PLATFORM_DLL_EXTENSION}"		"${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315ud_xrc_vc_custom${PLATFORM_DLL_EXTENSION}"
			 "${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315u_webview_vc_custom${PLATFORM_DLL_EXTENSION}"	"${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315ud_webview_vc_custom${PLATFORM_DLL_EXTENSION}"
			 "${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315u_stc_vc_custom${PLATFORM_DLL_EXTENSION}"		"${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315ud_stc_vc_custom${PLATFORM_DLL_EXTENSION}"
			 "${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315u_richtext_vc_custom${PLATFORM_DLL_EXTENSION}"	"${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315ud_richtext_vc_custom${PLATFORM_DLL_EXTENSION}"
			 "${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315u_ribbon_vc_custom${PLATFORM_DLL_EXTENSION}"	"${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315ud_ribbon_vc_custom${PLATFORM_DLL_EXTENSION}"
			 "${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315u_qa_vc_custom${PLATFORM_DLL_EXTENSION}"		"${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315ud_qa_vc_custom${PLATFORM_DLL_EXTENSION}"
			 "${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315u_propgrid_vc_custom${PLATFORM_DLL_EXTENSION}"	"${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315ud_propgrid_vc_custom${PLATFORM_DLL_EXTENSION}"
			 "${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315u_media_vc_custom${PLATFORM_DLL_EXTENSION}"		"${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315ud_media_vc_custom${PLATFORM_DLL_EXTENSION}"
			 "${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315u_html_vc_custom${PLATFORM_DLL_EXTENSION}"		"${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315ud_html_vc_custom${PLATFORM_DLL_EXTENSION}"
			 "${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315u_gl_vc_custom${PLATFORM_DLL_EXTENSION}"		"${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315ud_gl_vc_custom${PLATFORM_DLL_EXTENSION}"
			 "${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315u_aui_vc_custom${PLATFORM_DLL_EXTENSION}"		"${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315ud_aui_vc_custom${PLATFORM_DLL_EXTENSION}"
			 "${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315u_adv_vc_custom${PLATFORM_DLL_EXTENSION}"		"${PLATFORM_WXWIDGETS_BIN_DIR}/wxmsw315ud_adv_vc_custom${PLATFORM_DLL_EXTENSION}"
			 "${PLATFORM_WXWIDGETS_BIN_DIR}/wxbase315u_xml_vc_custom${PLATFORM_DLL_EXTENSION}"		"${PLATFORM_WXWIDGETS_BIN_DIR}/wxbase315ud_xml_vc_custom${PLATFORM_DLL_EXTENSION}"
			 "${PLATFORM_WXWIDGETS_BIN_DIR}/wxbase315u_net_vc_custom${PLATFORM_DLL_EXTENSION}"		"${PLATFORM_WXWIDGETS_BIN_DIR}/wxbase315ud_net_vc_custom${PLATFORM_DLL_EXTENSION}"
			 "${ASSIMP_PATH}/bin/${PLATFORM_BIN_DIR}/assimp${PLATFORM_DLL_EXTENSION}" 				"${ASSIMP_PATH}/bin/${PLATFORM_BIN_DIR}/assimpd${PLATFORM_DLL_EXTENSION}" )
	endif()

	install( FILES ${BINARES} DESTINATION ${INSTALL_DIR} )
endfunction()