# *.H
file( GLOB INCLUDE_FILES "${PROJECT_ENGINE}/Include/*.h" )
file( GLOB INCLUDE_MISC_FILES "${PROJECT_ENGINE}/Include/Misc/*.h" )
file( GLOB INCLUDE_RENDER_FILES "${PROJECT_ENGINE}/Include/Render/*.h" )
file( GLOB INCLUDE_RENDER_SHADERS_FILES "${PROJECT_ENGINE}/Include/Render/Shaders/*.h" )
file( GLOB INCLUDE_RHI_FILES "${PROJECT_ENGINE}/Include/RHI/*.h" )
file( GLOB INCLUDE_SCRIPTS_FILES "${PROJECT_ENGINE}/Include/Scripts/*.h" )
file( GLOB INCLUDE_SYSTEM_FILES "${PROJECT_ENGINE}/Include/System/*.h" )
file( GLOB INCLUDE_COMPONENTS_FILES "${PROJECT_ENGINE}/Include/Components/*.h" )
file( GLOB INCLUDE_ACTORS_FILES "${PROJECT_ENGINE}/Include/Actors/*.h" )
file( GLOB INCLUDE_RENDER_VERTEXFACTORY_FILES "${PROJECT_ENGINE}/Include/Render/VertexFactory/*.h" )

# *.CPP
file( GLOB SOURCE_FILES "${PROJECT_ENGINE}/Source/*.cpp" )
file( GLOB SOURCE_MISC_FILES "${PROJECT_ENGINE}/Source/Misc/*.cpp" )
file( GLOB SOURCE_RENDER_FILES "${PROJECT_ENGINE}/Source/Render/*.cpp" )
file( GLOB SOURCE_RENDER_SHADERS_FILES "${PROJECT_ENGINE}/Source/Render/Shaders/*.cpp" )
file( GLOB SOURCE_SCRIPTS_FILES "${PROJECT_ENGINE}/Source/Scripts/*.cpp" )
file( GLOB SOURCE_SYSTEM_FILES "${PROJECT_ENGINE}/Source/System/*.cpp" )
file( GLOB SOURCE_COMPONENTS_FILES "${PROJECT_ENGINE}/Source/Components/*.cpp" )
file( GLOB SOURCE_ACTORS_FILES "${PROJECT_ENGINE}/Source/Actors/*.cpp" )
file( GLOB SOURCE_RENDER_VERTEXFACTORY_FILES "${PROJECT_ENGINE}/Source/Render/VertexFactory/*.cpp" )

# Source groups
source_group( "Engine/Engine/Include" FILES ${INCLUDE_FILES} )
source_group( "Engine/Engine/Include/Misc" FILES ${INCLUDE_MISC_FILES} )
source_group( "Engine/Engine/Include/Render" FILES ${INCLUDE_RENDER_FILES} )
source_group( "Engine/Engine/Include/Render/Shaders" FILES ${INCLUDE_RENDER_SHADERS_FILES} )
source_group( "Engine/Engine/Include/RHI" FILES ${INCLUDE_RHI_FILES} )
source_group( "Engine/Engine/Include/Scripts" FILES ${INCLUDE_SCRIPTS_FILES} )
source_group( "Engine/Engine/Include/System" FILES ${INCLUDE_SYSTEM_FILES} )
source_group( "Engine/Engine/Include/Components" FILES ${INCLUDE_COMPONENTS_FILES} )
source_group( "Engine/Engine/Include/Actors" FILES ${INCLUDE_ACTORS_FILES} )
source_group( "Engine/Engine/Include/Render/VertexFactory" FILES ${INCLUDE_RENDER_VERTEXFACTORY_FILES} )

source_group( "Engine/Engine/Source" FILES ${SOURCE_FILES} )
source_group( "Engine/Engine/Source/Misc" FILES ${SOURCE_MISC_FILES} )
source_group( "Engine/Engine/Source/Render" FILES ${SOURCE_RENDER_FILES} )
source_group( "Engine/Engine/Source/Render/Shaders" FILES ${SOURCE_RENDER_SHADERS_FILES} )
source_group( "Engine/Engine/Source/Scripts" FILES ${SOURCE_SCRIPTS_FILES} )
source_group( "Engine/Engine/Source/System" FILES ${SOURCE_SYSTEM_FILES} )
source_group( "Engine/Engine/Source/Components" FILES ${SOURCE_COMPONENTS_FILES} )
source_group( "Engine/Engine/Source/Actors" FILES ${SOURCE_ACTORS_FILES} )
source_group( "Engine/Engine/Source/Render/VertexFactory" FILES ${SOURCE_RENDER_VERTEXFACTORY_FILES} )

# Set include dirs
set( INCLUDE_DIRS ${INCLUDE_DIRS} "${PROJECT_ENGINE}/Include" )

# Set all source files
set( ALL_SOURCE_FILES ${ALL_SOURCE_FILES}  
	${INCLUDE_FILES} ${INCLUDE_MISC_FILES} ${INCLUDE_RENDER_FILES} ${INCLUDE_RENDER_SHADERS_FILES} ${INCLUDE_RHI_FILES} ${INCLUDE_SCRIPTS_FILES} ${INCLUDE_SYSTEM_FILES} ${INCLUDE_COMPONENTS_FILES} ${INCLUDE_ACTORS_FILES} ${INCLUDE_RENDER_VERTEXFACTORY_FILES}
	${SOURCE_FILES} ${SOURCE_MISC_FILES} ${SOURCE_RENDER_FILES} ${SOURCE_RENDER_SHADERS_FILES} ${SOURCE_SCRIPTS_FILES} ${SOURCE_SYSTEM_FILES} ${SOURCE_COMPONENTS_FILES} ${SOURCE_ACTORS_FILES} ${SOURCE_RENDER_VERTEXFACTORY_FILES} )