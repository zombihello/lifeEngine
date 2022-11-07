# *.H
file( GLOB INCLUDE_FILES "${PROJECT_WORLDED}/Include/*.h" )
file( GLOB INCLUDE_COMMANDLETS_FILES "${PROJECT_WORLDED}/Include/Commandlets/*.h" )
file( GLOB INCLUDE_MISC_FILES "${PROJECT_WORLDED}/Include/Misc/*.h" )
file( GLOB INCLUDE_SYSTEM_FILES "${PROJECT_WORLDED}/Include/System/*.h" )
file( GLOB INCLUDE_WINDOWS_FILES "${PROJECT_WORLDED}/Include/Windows/*.h" )
file( GLOB INCLUDE_WIDGETS_FILES "${PROJECT_WORLDED}/Include/Widgets/*.h" )
file( GLOB INCLUDE_RENDER_FILES "${PROJECT_WORLDED}/Include/Render/*.h" )
file( GLOB INCLUDE_RENDER_SHADERS_FILES "${PROJECT_WORLDED}/Include/Render/Shaders/*.h" )
file( GLOB INCLUDE_RENDER_VERTEXFACTORY_FILES "${PROJECT_WORLDED}/Include/Render/VertexFactory/*.h" )
file( GLOB INCLUDE_COMPONENTS_FILES "${PROJECT_WORLDED}/Include/Components/*.h" )
file( GLOB INCLUDE_DIALOGS_FILES "${PROJECT_WORLDED}/Include/Dialogs/*.h" )

# *.CPP
file( GLOB SOURCE_FILES "${PROJECT_WORLDED}/Source/*.cpp" )
file( GLOB SOURCE_COMMANDLETS_FILES "${PROJECT_WORLDED}/Source/Commandlets/*.cpp" )
file( GLOB SOURCE_MISC_FILES "${PROJECT_WORLDED}/Source/Misc/*.cpp" )
file( GLOB SOURCE_SYSTEM_FILES "${PROJECT_WORLDED}/Source/System/*.cpp" )
file( GLOB SOURCE_WINDOWS_FILES "${PROJECT_WORLDED}/Source/Windows/*.cpp" )
file( GLOB SOURCE_WIDGETS_FILES "${PROJECT_WORLDED}/Source/Widgets/*.cpp" )
file( GLOB SOURCE_RENDER_FILES "${PROJECT_WORLDED}/Source/Render/*.cpp" )
file( GLOB SOURCE_RENDER_SHADERS_FILES "${PROJECT_WORLDED}/Source/Render/Shaders/*.cpp" )
file( GLOB SOURCE_RENDER_VERTEXFACTORY_FILES "${PROJECT_WORLDED}/Source/Render/VertexFactory/*.cpp" )
file( GLOB SOURCE_COMPONENTS_FILES "${PROJECT_WORLDED}/Source/Components/*.cpp" )
file( GLOB SOURCE_DIALOGS_FILES "${PROJECT_WORLDED}/Source/Dialogs/*.cpp" )

set( ALL_CPP_FILES ${SOURCE_FILES} ${SOURCE_DIALOGS_FILES} ${SOURCE_WIDGETS_FILES} ${SOURCE_COMPONENTS_FILES} ${SOURCE_RENDER_VERTEXFACTORY_FILES} ${SOURCE_RENDER_SHADERS_FILES} ${SOURCE_RENDER_FILES} ${SOURCE_COMMANDLETS_FILES} ${SOURCE_MISC_FILES} ${SOURCE_SYSTEM_FILES} ${SOURCE_WINDOWS_FILES} )
set( ALL_H_FILES ${INCLUDE_FILES} ${INCLUDE_DIALOGS_FILES} ${INCLUDE_WIDGETS_FILES} ${INCLUDE_COMPONENTS_FILES} ${INCLUDE_RENDER_VERTEXFACTORY_FILES} ${INCLUDE_RENDER_SHADERS_FILES} ${INCLUDE_RENDER_FILES} ${INCLUDE_COMMANDLETS_FILES} ${INCLUDE_MISC_FILES} ${INCLUDE_SYSTEM_FILES} ${INCLUDE_WINDOWS_FILES} )

# Source groups
source_group( "Engine/WorldEd/Include" FILES ${INCLUDE_FILES} )
source_group( "Engine/WorldEd/Include/Commandlets" FILES ${INCLUDE_COMMANDLETS_FILES} )
source_group( "Engine/WorldEd/Include/Misc" FILES ${INCLUDE_MISC_FILES} )
source_group( "Engine/WorldEd/Include/System" FILES ${INCLUDE_SYSTEM_FILES} )
source_group( "Engine/WorldEd/Include/Windows" FILES ${INCLUDE_WINDOWS_FILES} )
source_group( "Engine/WorldEd/Include/Widgets" FILES ${INCLUDE_WIDGETS_FILES} )
source_group( "Engine/WorldEd/Include/Render" FILES ${INCLUDE_RENDER_FILES} )
source_group( "Engine/WorldEd/Include/Render/VertexFactory" FILES ${INCLUDE_RENDER_VERTEXFACTORY_FILES} )
source_group( "Engine/WorldEd/Include/Render/Shaders" FILES ${INCLUDE_RENDER_SHADERS_FILES} )
source_group( "Engine/WorldEd/Include/Components" FILES ${INCLUDE_COMPONENTS_FILES} )
source_group( "Engine/WorldEd/Include/Dialogs" FILES ${INCLUDE_DIALOGS_FILES} )

source_group( "Engine/WorldEd/Source" FILES ${SOURCE_FILES} )
source_group( "Engine/WorldEd/Source/Commandlets" FILES ${SOURCE_COMMANDLETS_FILES} )
source_group( "Engine/WorldEd/Source/Misc" FILES ${SOURCE_MISC_FILES} )
source_group( "Engine/WorldEd/Source/System" FILES ${SOURCE_SYSTEM_FILES} )
source_group( "Engine/WorldEd/Source/Windows" FILES ${SOURCE_WINDOWS_FILES} )
source_group( "Engine/WorldEd/Source/Widgets" FILES ${SOURCE_WIDGETS_FILES} )
source_group( "Engine/WorldEd/Source/Render" FILES ${SOURCE_RENDER_FILES} )
source_group( "Engine/WorldEd/Source/Render/VertexFactory" FILES ${SOURCE_RENDER_VERTEXFACTORY_FILES} )
source_group( "Engine/WorldEd/Source/Render/Shaders" FILES ${SOURCE_RENDER_SHADERS_FILES} )
source_group( "Engine/WorldEd/Source/Components" FILES ${SOURCE_COMPONENTS_FILES} )
source_group( "Engine/WorldEd/Source/Dialogs" FILES ${SOURCE_DIALOGS_FILES} )

# Set include dirs
set( INCLUDE_DIRS 	${INCLUDE_DIRS} 
					"${PROJECT_WORLDED}/Include" )

# Set all source files
set( ALL_SOURCE_FILES 	${ALL_SOURCE_FILES}
						${ALL_CPP_FILES}
						${ALL_H_FILES} )