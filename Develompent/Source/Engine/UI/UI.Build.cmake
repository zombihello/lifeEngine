# *.H
file( GLOB INCLUDE_FILES "${PROJECT_UI}/Include/*.h" )
file( GLOB INCLUDE_IMGUI_FILES "${PROJECT_UI}/Include/ImGUI/*.h" )
file( GLOB INCLUDE_MISC_FILES "${PROJECT_UI}/Include/Misc/*.h" )

# *.CPP
file( GLOB SOURCE_FILES "${PROJECT_UI}/Source/*.cpp" )
file( GLOB SOURCE_IMGUI_FILES "${PROJECT_UI}/Source/ImGUI/*.cpp" )
file( GLOB SOURCE_MISC_FILES "${PROJECT_UI}/Source/Misc/*.cpp" )

# Source groups
source_group( "Engine/UI/Include" FILES ${INCLUDE_FILES} )
source_group( "Engine/UI/Include/ImGUI" FILES ${INCLUDE_IMGUI_FILES} )
source_group( "Engine/UI/Include/Misc" FILES ${INCLUDE_MISC_FILES} )
source_group( "Engine/UI/Source" FILES ${SOURCE_FILES} )
source_group( "Engine/UI/Source/ImGUI" FILES ${SOURCE_IMGUI_FILES} )
source_group( "Engine/UI/Source/Misc" FILES ${SOURCE_MISC_FILES} )

# Set include dirs
set( INCLUDE_DIRS ${INCLUDE_DIRS} "${PROJECT_UI}/Include" )

# Set all source files
set( ALL_SOURCE_FILES ${ALL_SOURCE_FILES} 
	${INCLUDE_FILES} ${INCLUDE_IMGUI_FILES} ${INCLUDE_MISC_FILES} 
	${SOURCE_FILES} ${SOURCE_IMGUI_FILES} ${SOURCE_MISC_FILES} )