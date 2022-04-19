cmake_minimum_required( VERSION 3.19 )
project( lifeEngine LANGUAGES CXX )

#
# General settings
#
set_property( GLOBAL PROPERTY USE_FOLDERS ON )
set( EXTERNAL_DIR ${PROJECT_SOURCE_DIR}/../External )
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/../CMake" )
set( INSTALL_DIR ${PROJECT_SOURCE_DIR}/../.. )
set( INSTALL_BINARIES_DIR ${PROJECT_SOURCE_DIR}/../../Binaries )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( DEBUG_NAME Debug )
set( RELEASE_NAME Release )
set( SHIPPING_NAME Shipping )
set( PLATFORM_NAME Unknown )
set( CMAKE_CONFIGURATION_TYPES Debug Release Shipping )
		
# TODO BS yehor.pohuliaka - Need move to ToolChain files
set( CMAKE_CXX_FLAGS_SHIPPING "${CMAKE_CXX_FLAGS_RELEASE}" )
set( CMAKE_RC_FLAGS_SHIPPING "${CMAKE_RC_FLAGS_RELEASE}" )
set( CMAKE_EXE_LINKER_FLAGS_SHIPPING "${CMAKE_EXE_LINKER_FLAGS_RELEASE}" )
set( CMAKE_MODULE_LINKER_FLAGS_SHIPPING "${CMAKE_MODULE_LINKER_FLAGS_RELEASE}" )
set( CMAKE_SHARED_LINKER_FLAGS_SHIPPING "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}" )
set( CMAKE_STATIC_LINKER_FLAGS_SHIPPING "${CMAKE_STATIC_LINKER_FLAGS_RELEASE}" )

include( ${PROJECT_SOURCE_DIR}/../CMake/ExternalsUtils.cmake )

#
# Platform settings
#
set( PLATFORM_WINDOWS OFF )
set( PLATFORM_32BIT 0 )
set( PLATFORM_64BIT 0 )

if ( ${CMAKE_SIZEOF_VOID_P} EQUAL 8 )
        set( PLATFORM_64BIT 1 )
elseif( ${CMAKE_SIZEOF_VOID_P} EQUAL 4 )
        set( PLATFORM_32BIT 1 )
else()
	message( SEND_ERROR "Unknown bit platform" )
endif()

add_definitions( -DPLATFORM_64BIT=${PLATFORM_64BIT} )
add_definitions( -DPLATFORM_32BIT=${PLATFORM_32BIT} )

if ( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
	set( PLATFORM_WINDOWS ON )
	add_definitions( -D_CRT_SECURE_NO_DEPRECATE )
	add_definitions( -D_CRT_NONSTDC_NO_DEPRECATE )
	add_definitions( -D_UNICODE )
	add_definitions( -DUNICODE )
	
    if ( MINGW )
        set( CMAKE_RC_COMPILER_INIT windres )
        enable_language( RC )
        set( CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> <DEFINES> -i <SOURCE> -o <OBJECT>" )
        set( CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -static )
        set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -static )
	else( MSVC )
		# TODO BS yehor.pohuliaka - Need move to ToolChain files
		set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )
		set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DDEBUG=1 /DRELEASE=0 /DSHIPPING=0" )
		set( CMAKE_RC_FLAGS_DEBUG "${CMAKE_RC_FLAGS_DEBUG} /DDEBUG=1 /DRELEASE=0 /DSHIPPING=0" )
		set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DDEBUG=0 /DRELEASE=1 /DSHIPPING=0" )
		set( CMAKE_RC_FLAGS_RELEASE "${CMAKE_RC_FLAGS_RELEASE} /DDEBUG=0 /DRELEASE=1 /DSHIPPING=0" )
		set( CMAKE_CXX_FLAGS_SHIPPING "${CMAKE_CXX_FLAGS_SHIPPING} /DDEBUG=0 /DRELEASE=0 /DSHIPPING=1" )
		set( CMAKE_RC_FLAGS_SHIPPING "${CMAKE_RC_FLAGS_SHIPPING} /DDEBUG=0 /DRELEASE=0 /DSHIPPING=1" )
    endif()
	
	if ( PLATFORM_64BIT )
		set( INSTALL_BINARIES_DIR ${INSTALL_BINARIES_DIR}/Win64 )
		set( PLATFORM_NAME Win64 )
	elseif( PLATFORM_32BIT )
		set( INSTALL_BINARIES_DIR ${INSTALL_BINARIES_DIR}/Win32 )
		set( PLATFORM_NAME Win32 )
	endif()
else()
	message( SEND_ERROR "Unknow platform" )
endif()

set( CMAKE_DEBUG_POSTFIX "-${PLATFORM_NAME}-${DEBUG_NAME}" )
set( CMAKE_RELEASE_POSTFIX "-${PLATFORM_NAME}-${RELEASE_NAME}" )
set( CMAKE_SHIPPING_POSTFIX "-${PLATFORM_NAME}-${SHIPPING_NAME}" )

#
# Set path to externals
#
set( SDL2_PATH ${EXTERNAL_DIR}/SDL2-2.0.14 )
set( RAPIDJSON_PATH ${EXTERNAL_DIR}/rapidjson )
set( LUAJIT_PATH ${EXTERNAL_DIR}/LuaJIT-2.1 )
set( LUABRIDGE_PATH ${EXTERNAL_DIR}/LuaBridge-2.6 )
set( GLM_PATH ${EXTERNAL_DIR}/glm-0.9.9.8 )
set( ZLIB_PATH ${EXTERNAL_DIR}/zlib )
set( OPENAL_PATH ${EXTERNAL_DIR}/openal-soft-1.21.1 )
set( OGG_PATH ${EXTERNAL_DIR}/libogg-1.3.5 )
set( VORBIS_PATH ${EXTERNAL_DIR}/libvorbis-1.3.7 )
set( PHYSX_PATH ${EXTERNAL_DIR}/PhysX-4.1/physx/install )
set( BOX2D_PATH ${EXTERNAL_DIR}/box2d-2.4.1/install )

if ( PLATFORM_WINDOWS )
	set( PHYSX_PATH ${PHYSX_PATH}/vc15win64 )
endif()

if ( WITH_EDITOR )
	set( WXWIDGETS_PATH ${EXTERNAL_DIR}/wxwidgets-3.1.5 )
	set( STB_PATH ${EXTERNAL_DIR}/stb )
	set( ASSIMP_PATH ${EXTERNAL_DIR}/assimp-5.0.1 )
	set( TMXLITE_PATH ${EXTERNAL_DIR}/tmxlite/tmxlite )
endif()

#
# Build modules
#
set( PROJECT_CORE Engine/Core )
set( PROJECT_ENGINE Engine/Engine )
set( PROJECT_LAUNCH Engine/Launch )
set( PROJECT_UI Engine/UI )
set( PROJECT_WORLDED Engine/WorldEd )
set( PROJECT_AUDIO Engine/Audio )
set( PROJECT_PHYSICS Engine/Physics )

if ( PLATFORM_WINDOWS )
	set( PROJECT_PLATFORM Engine/Platforms/Windows )
	set( PROJECT_RHI Engine/RHI/D3D11RHI )
else()
	message( SEND_ERROR "Unknow platform" )
endif()

#
# Options of build
#
# Enable/Disable editor
option( WITH_EDITOR "Build with editor. Only for Windows platform" ON )
if ( WITH_EDITOR )
	add_definitions( -DWITH_EDITOR=1 )
else()
	add_definitions( -DWITH_EDITOR=0 )
endif()

# Enable/Disable ImGUI
option( WITH_IMGUI "Build with ImGUI" ON )
if ( WITH_IMGUI )
	add_definitions( -DWITH_IMGUI=1 )
else()
	add_definitions( -DWITH_IMGUI=0 )
endif()

# Game source path
set( GAME_SOURCE_PATH "" CACHE PATH "Path to source code of game with <NAME_GAME>.Build.cmake file" )
if ( NOT GAME_SOURCE_PATH )
	message( "Value GAME_SOURCE_PATH not setted" )
	set( GAME_SOURCE_PATH "${PROJECT_SOURCE_DIR}/Games/HeliumGame" )			# Default game
endif()

# Build engine for 2D game
option( ENGINE_2D "Build engine for 2D game" OFF )
if ( ENGINE_2D )
	add_definitions( -DENGINE_2D=1 )
else()
	add_definitions( -DENGINE_2D=0 )
endif()

#
# Add projects to build
#
set( INCLUDE_DIRS )
set( ALL_SOURCE_FILES )

# Add modules to build
include( ${PROJECT_SOURCE_DIR}/${PROJECT_CORE}/Core.Build.cmake )				# Core
include( ${PROJECT_SOURCE_DIR}/${PROJECT_ENGINE}/Engine.Build.cmake )			# Engine
include( ${PROJECT_SOURCE_DIR}/${PROJECT_LAUNCH}/Launch.Build.cmake )			# Launch
include( ${PROJECT_SOURCE_DIR}/${PROJECT_UI}/UI.Build.cmake )					# UI
include( ${PROJECT_SOURCE_DIR}/${PROJECT_AUDIO}/Audio.Build.cmake )				# Audio
include( ${PROJECT_SOURCE_DIR}/${PROJECT_PHYSICS}/Physics.Build.cmake )			# Physics
include( ${GAME_SOURCE_PATH}/Game.Build.cmake )									# Game

if ( WITH_EDITOR )
	include( ${PROJECT_SOURCE_DIR}/${PROJECT_WORLDED}/WorldEd.Build.cmake )		# Map editor
endif()

if ( PLATFORM_WINDOWS )
	include( ${PROJECT_SOURCE_DIR}/${PROJECT_PLATFORM}/Windows.Build.cmake )	# Windows
	include( ${PROJECT_SOURCE_DIR}/${PROJECT_RHI}/D3D11RHI.Build.cmake )		# D3D11RHI
endif()

add_definitions( -DGAMENAME="${GAME_NAME}" )

add_executable( ${GAME_NAME} WIN32 ${ALL_SOURCE_FILES} )
set_target_properties( ${GAME_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${INSTALL_BINARIES_DIR} )
set_target_properties( ${GAME_NAME} PROPERTIES 
									DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
									RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX}
									SHIPPING_POSTFIX ${CMAKE_SHIPPING_POSTFIX} )

# Include directories and libs
include_directories( ${INCLUDE_DIRS} )
include_directories( ../../Engine/Shaders )
IncludeExternals( ${GAME_NAME} )
target_link_libraries( ${GAME_NAME} ${CMAKE_DL_LIBS} )

# Install settings
InstallExternals( ${INSTALL_BINARIES_DIR} )
install( TARGETS ${GAME_NAME} DESTINATION ${INSTALL_BINARIES_DIR} )
