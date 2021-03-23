#
#   --- Set values ---
#

set( MODULE_NAME Core )
set( ALL_SOURCE_FILES Include/CoreDefines.h Include/Core.h Include/Types.h Source/Core.cpp Include/Object.h)

#
#   --- Project settings ---
#

add_library( ${MODULE_NAME} STATIC ${ALL_SOURCE_FILES} )

set_target_properties (${MODULE_NAME} PROPERTIES FOLDER Runtime )
set_target_properties( ${MODULE_NAME} PROPERTIES PREFIX "" )

include_directories( Include )
include_directories( ${ENGINE_INCLUDE_DIR} )
include_directories( ${PLATFORM_INCLUDE_DIR} )