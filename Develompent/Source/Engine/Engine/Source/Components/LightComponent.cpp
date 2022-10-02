#include "Components/LightComponent.h"
#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Render/Scene.h"

IMPLEMENT_CLASS( CLightComponent )

CLightComponent::CLightComponent()
	: bEnabled( true )
	, scene( nullptr )
	, lightColor( CColor::white )
	, specularColor( CColor::white )
	, intensivity( 22400.f )
{}

CLightComponent::~CLightComponent()
{
	if ( scene )
	{
		scene->RemoveLight( this );
	}
}

void CLightComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << bEnabled;
}

void CLightComponent::Spawned()
{
	Super::Spawned();
	GWorld->GetScene()->AddLight( this );
}

void CLightComponent::Destroyed()
{
	Super::Destroyed();
	GWorld->GetScene()->RemoveLight( this );
}

ELightType CLightComponent::GetLightType() const
{
	return LT_Unknown;
}