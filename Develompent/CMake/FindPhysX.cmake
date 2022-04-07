# 	---------------------------------
#	[in] 	PHYSX_PATH							- root dir PhysX
#	[out] 	PHYSX_INCLUDE						- dir with includes
#	[out]	PHYSX_LIB							- lib PhysX
#	[out]	PHYSX_COMMON_LIB					- lib PhysX common
#	[out]	PHYSX_COOKING_LIB					- lib PhysX cooking
#	[out]	PHYSX_EXTENSIONS_LIB				- lib PhysX extensions
#	[out]	PHYSX_FOUNDATION_LIB				- lib PhysX foundation
#	[out]	PHYSX_CHARACTERKINEMATIC_LIB		- lib PhysX character kinematic
#	[out]	PHYSX_PVDSDK_LIB					- lib PhysX PVD SDK
#	[out]	PHYSX_TASK_LIB						- lib PhysX task
#	[out]	PHYSX_VEHICLE_LIB					- lib PhysX vehicle
#	[out]	PHYSX_DEBUG_LIB						- lib PhysX debug
#	[out]	PHYSX_COMMON_DEBUG_LIB				- lib PhysX common debug
#	[out]	PHYSX_COOKING_DEBUG_LIB				- lib PhysX cooking debug
#	[out]	PHYSX_EXTENSIONS_DEBUG_LIB			- lib PhysX extensions debug
#	[out]	PHYSX_FOUNDATION_DEBUG_LIB			- lib PhysX foundation debug
#	[out]	PHYSX_CHARACTERKINEMATIC_DEBUG_LIB	- lib PhysX character kinematic debug
#	[out]	PHYSX_PVDSDK_DEBUG_LIB				- lib PhysX PVD SDK debug
#	[out]	PHYSX_TASK_DEBUG_LIB				- lib PhysX task debug
#	[out]	PHYSX_VEHICLE_DEBUG_LIB				- lib PhysX vehicle debug
#	[out]	PHYSX_FOUND							- is found PhysX
# 	---------------------------------

set( PHYSX_FOUND OFF )
SET( PHYSX_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${PHYSX_PATH} )

#
# Set path suffixed to libs for current platform
#
if ( PLATFORM_WINDOWS )
	if ( PLATFORM_64BIT )
		set( PHYSX_LIB_PATH_SUFFIXES "PhysX/bin/win.x86_64.vc142.md" )
	elseif( PLATFORM_32BIT )
		set( PHYSX_LIB_PATH_SUFFIXES "PhysX/bin/win.x86.vc142.md" )
	endif()
else()
	message( SEND_ERROR "PhysX :: Unknown platform" )
	return()
endif()

#
# Find pathes
#
find_path( 		PHYSX_INCLUDE
				NAMES "PxPhysics.h"
				PATH_SUFFIXES PhysX/include
				PATHS ${PHYSX_SEARCH_PATHS} )
find_path( 		PHYSX_PXSHARED_INCLUDE
				NAMES "foundation/Px.h"
				PATH_SUFFIXES PxShared/include
				PATHS ${PHYSX_SEARCH_PATHS} )	


find_library( 	PHYSX_LIB 
                NAMES PhysX_64 PhysX
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/release
                PATHS ${PHYSX_SEARCH_PATHS} )
find_library( 	PHYSX_COMMON_LIB 
                NAMES PhysXCommon_64 PhysXCommon
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/release
                PATHS ${PHYSX_SEARCH_PATHS} )
find_library( 	PHYSX_COOKING_LIB 
                NAMES PhysXCooking_64 PhysXCooking
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/release
                PATHS ${PHYSX_SEARCH_PATHS} )
find_library( 	PHYSX_EXTENSIONS_LIB 
                NAMES PhysXExtensions_static_64 PhysXExtensions_static
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/release
                PATHS ${PHYSX_SEARCH_PATHS} )
find_library( 	PHYSX_FOUNDATION_LIB 
                NAMES PhysXFoundation_64 PhysXFoundation
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/release
                PATHS ${PHYSX_SEARCH_PATHS} )
find_library( 	PHYSX_CHARACTERKINEMATIC_LIB 
                NAMES PhysXCharacterKinematic_static_64 PhysXCharacterKinematic_static
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/release
                PATHS ${PHYSX_SEARCH_PATHS} )
find_library( 	PHYSX_PVDSDK_LIB 
                NAMES PhysXPvdSDK_static_64 PhysXPvdSDK_static
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/release
                PATHS ${PHYSX_SEARCH_PATHS} )
find_library( 	PHYSX_TASK_LIB 
                NAMES PhysXTask_static_64 PhysXTask_static
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/release
                PATHS ${PHYSX_SEARCH_PATHS} )
find_library( 	PHYSX_VEHICLE_LIB 
                NAMES PhysXVehicle_static_64 PhysXVehicle_static
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/release
                PATHS ${PHYSX_SEARCH_PATHS} )
				
find_library( 	PHYSX_DEBUG_LIB 
                NAMES PhysXDEBUG_64 PhysXDEBUG
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/debug
                PATHS ${PHYSX_SEARCH_PATHS} )
find_library( 	PHYSX_COMMON_DEBUG_LIB 
                NAMES PhysXCommonDEBUG_64 PhysXCommonDEBUG
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/debug
                PATHS ${PHYSX_SEARCH_PATHS} )
find_library( 	PHYSX_COOKING_DEBUG_LIB 
                NAMES PhysXCookingDEBUG_64 PhysXCookingDEBUG
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/debug
                PATHS ${PHYSX_SEARCH_PATHS} )
find_library( 	PHYSX_EXTENSIONS_DEBUG_LIB 
                NAMES PhysXExtensions_staticDEBUG_64 PhysXExtensions_staticDEBUG
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/debug
                PATHS ${PHYSX_SEARCH_PATHS} )
find_library( 	PHYSX_FOUNDATION_DEBUG_LIB 
                NAMES PhysXFoundationDEBUG_64 PhysXFoundationDEBUG
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/debug
                PATHS ${PHYSX_SEARCH_PATHS} )
find_library( 	PHYSX_CHARACTERKINEMATIC_DEBUG_LIB 
                NAMES PhysXCharacterKinematic_staticDEBUG_64 PhysXCharacterKinematic_staticDEBUG
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/debug
                PATHS ${PHYSX_SEARCH_PATHS} )
find_library( 	PHYSX_PVDSDK_DEBUG_LIB 
                NAMES PhysXPvdSDK_staticDEBUG_64 PhysXPvdSDK_staticDEBUG
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/debug
                PATHS ${PHYSX_SEARCH_PATHS} )
find_library( 	PHYSX_TASK_DEBUG_LIB 
                NAMES PhysXTask_staticDEBUG_64 PhysXTask_staticDEBUG
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/debug
                PATHS ${PHYSX_SEARCH_PATHS} )
find_library( 	PHYSX_VEHICLE_DEBUG_LIB 
                NAMES PhysXVehicle_staticDEBUG_64 PhysXVehicle_staticDEBUG
                PATH_SUFFIXES ${PHYSX_LIB_PATH_SUFFIXES}/debug
                PATHS ${PHYSX_SEARCH_PATHS} )
				
if ( NOT PHYSX_INCLUDE OR NOT PHYSX_PXSHARED_INCLUDE OR NOT 
	PHYSX_LIB OR NOT PHYSX_COMMON_LIB OR NOT PHYSX_COOKING_LIB OR NOT PHYSX_EXTENSIONS_LIB OR NOT PHYSX_FOUNDATION_LIB OR NOT PHYSX_CHARACTERKINEMATIC_LIB OR NOT PHYSX_PVDSDK_LIB OR NOT PHYSX_TASK_LIB OR NOT PHYSX_VEHICLE_LIB OR NOT
	PHYSX_DEBUG_LIB OR NOT PHYSX_COMMON_DEBUG_LIB OR NOT PHYSX_COOKING_DEBUG_LIB OR NOT PHYSX_EXTENSIONS_DEBUG_LIB OR NOT PHYSX_FOUNDATION_DEBUG_LIB OR NOT PHYSX_CHARACTERKINEMATIC_DEBUG_LIB OR NOT PHYSX_PVDSDK_DEBUG_LIB OR NOT PHYSX_TASK_DEBUG_LIB OR NOT PHYSX_VEHICLE_DEBUG_LIB )
    return()
else()
	set( PHYSX_INCLUDE ${PHYSX_INCLUDE} ${PHYSX_PXSHARED_INCLUDE} )
	set( PHYSX_FOUND ON )
endif()
