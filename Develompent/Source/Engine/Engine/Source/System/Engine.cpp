#include "Scripts/ScriptFrame.h"
#include "Actors/StaticMesh.h"
#include "Actors/Audio.h"
#include "Actors/BoxCollision.h"
#include "Actors/Character.h"
#include "Actors/Sprite.h"
#include "Actors/DirectionalLight.h"
#include "Actors/SpotLight.h"
#include "Actors/PointLight.h"
#include "Actors/PlayerController.h"
#include "Actors/PlayerStart.h"
#include "Components/SphereComponent.h"
#include "System/GameEngine.h"
#include "System/World.h"

// Native classes
#define WITH_REGISTRANTS
#include "Classes/EngineClasses.h"
#undef WITH_REGISTRANTS

/*
==================
InitRegistrantsModule_Engine
Register all native classes in the system
==================
*/
void InitRegistrantsModule_Engine()
{
	InitRegistrants_Engine();
}