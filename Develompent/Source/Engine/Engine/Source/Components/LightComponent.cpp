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
CLightComponent::StaticInitializeClass
==================
*/
void CLightComponent::StaticInitializeClass()
{
	new CBoolProperty( staticClass, TEXT( "bEnabled" ), TEXT( "Light" ), TEXT( "Is enabled light" ), CPP_PROPERTY( bEnabled ), 0 );
	new CColorProperty( staticClass, TEXT( "Color" ), TEXT( "Light" ), TEXT( "Light color" ), CPP_PROPERTY( lightColor ), 0 );
	new CFloatProperty( staticClass, TEXT( "Intensivity" ), TEXT( "Light" ), TEXT( "Light intensivity" ), CPP_PROPERTY( intensivity ), 0 );
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