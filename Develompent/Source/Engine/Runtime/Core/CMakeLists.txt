#
#   --- Set values ---
#

set( MODULE_NAME Core )
file( GLOB_RECURSE SOURCE_FILES "Include/*.h" "Source/*.cpp" )

#
#   --- Project settings ---
#

add_library( ${MODULE_NAME} STATIC ${SOURCE_FILES} )

set_target_properties (${MODULE_NAME} PROPERTIES FOLDER Runtime )
set_target_properties( ${MODULE_NAME} PROPERTIES PREFIX "" )

include_directories( Include )
include_directories( ${ENGINE_INCLUDE_DIR} )
include_directories( ${PLATFORM_INCLUDE_DIR} )