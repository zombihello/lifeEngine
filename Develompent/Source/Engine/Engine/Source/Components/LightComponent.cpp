#include "Components/LightComponent.h"
#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Render/Scene.h"

IMPLEMENT_CLASS( CLightComponent )

// WorldEd reflection
BEGIN_DATADESC( CLightComponent, CSceneComponent )
	DEFINE_FIELD( bEnabled, "Light", "Is enabled light", FT_Boolean )
	DEFINE_FIELD( lightColor, "Light", "Light color", FT_Color )
	DEFINE_FIELD( intensivity, "Light", "Light intensivity", FT_Float )
END_DATADESC()

/*
==================
CLightComponent::CLightComponent
==================
*/
CLightComponent::CLightComponent()
	: bEnabled( true )
	, scene( nullptr )
	, lightColor( CColor::white )
	, intensivity( 22400.f )
{}

/*
==================
CLightComponent::~CLightComponent
==================
*/
CLightComponent::~CLightComponent()
{
	if ( scene )
	{
		scene->RemoveLight( this );
	}
}

/*
==================
CLightComponent::Serialize
==================
*/
void CLightComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << bEnabled;
	if ( InArchive.Ver() < VER_NewSeriallizeDataInLightComponents )
	{
		return;
	}

	InArchive << lightColor;
	InArchive << intensivity;
}

/*
==================
CLightComponent::Spawned
==================
*/
void CLightComponent::Spawned()
{
	Super::Spawned();
	g_World->GetScene()->AddLight( this );
}

/*
==================
CLightComponent::Destroyed
==================
*/
void CLightComponent::Destroyed()
{
	Super::Destroyed();
	g_World->GetScene()->RemoveLight( this );
}

/*
==================
CLightComponent::GetLightType
==================
*/
ELightType CLightComponent::GetLightType() const
{
	return LT_Unknown;
}