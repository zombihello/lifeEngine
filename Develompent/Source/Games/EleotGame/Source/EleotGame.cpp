#include "Actors/ELPlayerCharacter.h"
#include "Actors/ELPlayerController.h"

// Native classes
#define WITH_REGISTRANTS
#include "Classes/EleotGameClasses.h"
#undef WITH_REGISTRANTS

/*
==================
InitRegistrantsModule_EleotGame
Register all native classes in the system
==================
*/
void InitRegistrantsModule_EleotGame()
{
	InitRegistrants_EleotGame();
}