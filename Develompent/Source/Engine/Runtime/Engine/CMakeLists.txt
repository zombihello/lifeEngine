#
#   --- Set values ---
#

set( MODULE_NAME Engine )
set( SDL2_PATH ${EXTLIBS_DIR}/SDL2 CACHE PATH "Path to SDL2" )

#
#   --- Project settings ---
#

add_library( ${MODULE_NAME} STATIC ${ALL_SOURCE_FILES} )

set_target_properties (${MODULE_NAME} PROPERTIES FOLDER Runtime )
set_target_properties( ${MODULE_NAME} PROPERTIES PREFIX "" )

include_directories( Include )
include_directories( ${CORE_INCLUDE_DIR} )
include_directories( ${PLATFORM_INCLUDE_DIR} )

#
#   --- Find and include extlibs ---
#

#---------------
#   SDL2

find_package( SDL2 REQUIRED )
if( NOT SDL2_FOUND )
    message( SEND_ERROR "Failed to find SDL2" )
    return()
else()
    include_directories( ${SDL2_INCLUDE} )
    target_link_libraries( ${MODULE_NAME} ${SDL2_LIB} ${SDL2MAIN_LIB} )
endif()