#
#   --- Set values ---
#

set( MODULE_NAME AndroidPlatform )
set( ALL_SOURCE_FILES Include/AndroidPlatform.h Source/native-lib.cpp )

#
#   --- Project settings ---
#

add_library( ${MODULE_NAME} SHARED ${ALL_SOURCE_FILES} )
install( TARGETS ${MODULE_NAME} DESTINATION ${BUILD_DIR}/${BUILD_BIN_DIR} )

target_link_libraries( ${MODULE_NAME} Core )
#target_link_libraries( ${MODULE_NAME} Engine )
target_link_libraries( ${MODULE_NAME} Game )

include_directories( Include )
include_directories( ${CORE_INCLUDE_DIR} )
include_directories( ${ENGINE_INCLUDE_DIR} )
include_directories( ${GAME_INCLUDE_DIR} )

#
#   --- Find libreries ---
#

# ----------------
# Android NDK

find_library( log-lib log )
target_link_libraries( ${MODULE_NAME} ${log-lib} )