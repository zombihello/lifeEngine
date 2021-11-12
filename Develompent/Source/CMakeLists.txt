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
set( PLATFORM_32BIT OFF )
set( PLATFORM_64BIT OFF )

if ( ${CMAKE_SIZEOF_VOID_P} EQUAL 8 )
	set( PLATFORM_64BIT ON )
elseif( ${CMAKE_SIZEOF_VOID_P} EQUAL 4 )
	set( PLATFORM_32BIT ON )
else()
	message( SEND_ERROR "Unknown bit platform" )
endif()

if ( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
	set( PLATFORM_WINDOWS ON )
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

if ( WITH_EDITOR )
	set( STB_PATH ${EXTERNAL_DIR}/stb )
	set( ASSIMP_PATH ${EXTERNAL_DIR}/assimp-5.0.1 )
	set( QT6_PATH ${EXTERNAL_DIR}/Qt-6.2.0/msvc2019_64 )
	set( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT6_PATH} )
	set( QT_PLUGIN_DIR ${INSTALL_BINARIES_DIR}/Plugins )
endif()

#
# Build modules
#
set( PROJECT_CORE Engine/Core )
set( PROJECT_ENGINE Engine/Engine )
set( PROJECT_LAUNCH Engine/Launch )
set( PROJECT_UI Engine/UI )
set( PROJECT_WORLDED Engine/WorldEd )

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
	set( QT_PLUGIN_DIR )
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
	message( SEND_ERROR "Value GAME_SOURCE_PATH not setted" )
endif()

#
# Add projects to build
#
# Include directories
set( INCLUDE_DIRS )

# Add modules to build
set( ALL_SOURCE_FILES )

# Add Core
include( ${PROJECT_SOURCE_DIR}/${PROJECT_CORE}/Core.Build.cmake )
CoreBuild( ${PROJECT_CORE} "${ALL_SOURCE_FILES}" "${INCLUDE_DIRS}" )

# Add Engine
include( ${PROJECT_SOURCE_DIR}/${PROJECT_ENGINE}/Engine.Build.cmake )
EngineBuild( ${PROJECT_ENGINE} "${ALL_SOURCE_FILES}" "${INCLUDE_DIRS}" )

# Add Launch
include( ${PROJECT_SOURCE_DIR}/${PROJECT_LAUNCH}/Launch.Build.cmake )
LaunchBuild( ${PROJECT_LAUNCH} "${ALL_SOURCE_FILES}" "${INCLUDE_DIRS}" )

# Add UI
include( ${PROJECT_SOURCE_DIR}/${PROJECT_UI}/UI.Build.cmake )
UIBuild( ${PROJECT_UI} "${ALL_SOURCE_FILES}" "${INCLUDE_DIRS}" )

# Add WorldEd
if ( WITH_EDITOR )
	include( ${PROJECT_SOURCE_DIR}/${PROJECT_WORLDED}/WorldEd.Build.cmake )
	WorldEdBuild( ${PROJECT_WORLDED} "${ALL_SOURCE_FILES}" "${INCLUDE_DIRS}" )
endif()

# Add Game module
include( ${GAME_SOURCE_PATH}/Game.Build.cmake )
GameBuild( ${GAME_SOURCE_PATH} "${ALL_SOURCE_FILES}" "${INCLUDE_DIRS}" )

# Add platform modules
if ( PLATFORM_WINDOWS )
	include( ${PROJECT_SOURCE_DIR}/${PROJECT_PLATFORM}/Windows.Build.cmake )
	WindowsBuild( ${PROJECT_PLATFORM} "${ALL_SOURCE_FILES}" "${INCLUDE_DIRS}" )
	
	include( ${PROJECT_SOURCE_DIR}/${PROJECT_RHI}/D3D11RHI.Build.cmake )
	D3D11RHIBuild( ${PROJECT_RHI} "${ALL_SOURCE_FILES}" "${INCLUDE_DIRS}" )
endif()

add_executable( ${GAME_NAME} WIN32 ${ALL_SOURCE_FILES} )
set_target_properties( ${GAME_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${INSTALL_BINARIES_DIR}
											   VS_DEBUGGER_ENVIRONMENT 		 "QT_PLUGIN_PATH=${QT_PLUGIN_DIR}" )
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