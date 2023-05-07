#include "Components/LightComponent.h"
#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Render/Scene.h"

IMPLEMENT_CLASS( CLightComponent )

/*
==================
CLightComponent::CLightComponent
==================
*/
CLightComponent::CLightComponent()
	: bEnabled( true )
	, scene( nullptr )
	, lightColor( CColor::white )
	, specularColor( CColor::white )
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