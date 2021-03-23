#
#   --- Set values ---
#

set( MODULE_NAME ${GAME_NAME} )
set( ALL_SOURCE_FILES Include/WindowsPlatform.h Source/WindowsLaunch.cpp Include/WindowsWindow.h Source/WindowsWindow.cpp )

#
#   --- Project settings ---
#

add_executable( ${MODULE_NAME} WIN32 ${ALL_SOURCE_FILES} )
install( TARGETS ${MODULE_NAME} DESTINATION ${BUILD_DIR}/${BUILD_BIN_DIR} )

target_link_libraries( ${MODULE_NAME} ${CMAKE_DL_LIBS} )
target_link_libraries( ${MODULE_NAME} Core )
#target_link_libraries( ${MODULE_NAME} Engine )
target_link_libraries( ${MODULE_NAME} Platform )
target_link_libraries( ${MODULE_NAME} Game )

include_directories( Include )
include_directories( ${CORE_INCLUDE_DIR} )
include_directories( ${ENGINE_INCLUDE_DIR} )
include_directories( ${GAME_INCLUDE_DIR} )