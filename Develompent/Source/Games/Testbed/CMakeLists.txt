#
#   --- Set values ---
#

set( MODULE_NAME Game )
set( ALL_SOURCE_FILES Source/Main.cpp )

#
#   --- Settings project ---
#

add_library( ${MODULE_NAME} ${ALL_SOURCE_FILES} )

include_directories( Include )
include_directories( ${CORE_INCLUDE_DIR} )
include_directories( ${ENGINE_INCLUDE_DIR} )
include_directories( ${PLATFORM_INCLUDE_DIR} )