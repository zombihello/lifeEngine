cmake_minimum_required( VERSION 3.10 )
project( lifeEngine )

include( ../CMake/InstallExtlibs.cmake )
set_property( GLOBAL PROPERTY USE_FOLDERS ON )

# Get architecture type
if ( ${CMAKE_SIZEOF_VOID_P} MATCHES 8 )
	set( ARCH_TYPE 64 )
else()
	set( ARCH_TYPE 32 )
endif()

if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
    if ( MINGW )
        set( CMAKE_RC_COMPILER_INIT windres )
        enable_language( RC )
        set( CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> <DEFINES> -i <SOURCE> -o <OBJECT>" )
        set( CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -static )
        set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -static )
    elseif( MSVC )
    endif()

    if( ${ARCH_TYPE} EQUAL 64 )
        set( PLATFORM_NAME "Win64" )
    else()
        set( PLATFORM_NAME "Win32" )
    endif()
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Android" )
    set( PLATFORM_NAME "Android" )
else()
    message( SEND_ERROR "Unknow platform '" ${CMAKE_SYSTEM_NAME} "'" )
endif()

#
#   --- Modules list ---
#

set( MODULE_CORE ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Runtime/Core )
set( CORE_INCLUDE_DIR ${MODULE_CORE}/Include )

set( MODULE_ENGINE ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Runtime/Engine )
set( ENGINE_INCLUDE_DIR ${MODULE_ENGINE}/Include )

set( MODULE_GAME ${CMAKE_CURRENT_SOURCE_DIR}/Games/Testbed CACHE PATH "Path to game module" )
set( GAME_NAME TestbedGame CACHE STRING "Game name" )
set( GAME_INCLUDE_DIR ${MODULE_GAME}/Include )

if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
    set( MODULE_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Runtime/Platforms/Windows )
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Android" )
    set( MODULE_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Runtime/Platforms/Android )
endif()

set( PLATFORM_INCLUDE_DIR ${MODULE_PLATFORM}/Include )

#
#   --- Build settings ---
#

set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/../CMake" )
set( EXTLIBS_DIR "${PROJECT_SOURCE_DIR}/../Extlibs" )
set( BUILD_BIN_DIR Binaries/${PLATFORM_NAME} )
set( BUILD_LIB_DIR Libs )
set( BUILD_ENGINE_DIR Engine )
set( BUILD_DIR ${CMAKE_INSTALL_PREFIX} )

include_directories( ${EXTLIBS_DIR} )
InstallExtlibs( ${EXTLIBS_DIR} ${BUILD_DIR}/${BUILD_BIN_DIR} )

option( LE_DEBUG "Enable debug mode" OFF )
option( DO_CHECK "Enable checks" OFF )
option( SHIPPING_BUILD "Is shipping build" OFF )

# Enable debug mode
if( LE_DEBUG )
    message( STATUS "Debug mode enabled" )
    add_definitions( -DLE_DEBUG=1 )
else()
    add_definitions( -DLE_DEBUG=0 )
endif()

# Enable checks
if ( DO_CHECK )
    message( STATUS "Checks enabled" )
    add_definitions( -DDO_CHECK=1 )
else()
    add_definitions( -DDO_CHECK=0 )
endif()

# Is shipping build?
if ( SHIPPING_BUILD )
    message( STATUS "Shipping build enabled" )
    add_definitions( -DSHIPPING_BUILD=1 )
else()
    add_definitions( -DSHIPPING_BUILD=0 )
endif()

#
# Add subprojects
#
add_subdirectory( ${MODULE_PLATFORM} )
add_subdirectory( ${MODULE_CORE} )
#add_subdirectory( ${MODULE_ENGINE} )
add_subdirectory( ${MODULE_GAME} )