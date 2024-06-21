#include "Misc/PhysicsTypes.h"

// Native classes
#define WITH_REGISTRANTS
#include "Classes/PhysicsClasses.h"
#undef WITH_REGISTRANTS

/*
==================
InitRegistrantsModule_Physics
Register all native classes in the system
==================
*/
void InitRegistrantsModule_Physics()
{
	InitRegistrants_Physics();
}