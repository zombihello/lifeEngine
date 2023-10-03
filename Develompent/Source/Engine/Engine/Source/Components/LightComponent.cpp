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
	new( staticClass, TEXT( "bEnabled" ) )		CBoolProperty( TEXT( "Light" ), TEXT( "Is enabled light" ), CPP_PROPERTY( bEnabled ), CPF_Edit );
	new( staticClass, TEXT( "Color" ) )			CColorProperty( TEXT( "Light" ), TEXT( "Light color" ), CPP_PROPERTY( lightColor ), CPF_Edit );
	new( staticClass, TEXT( "Intensivity" ) )	CFloatProperty( TEXT( "Light" ), TEXT( "Light intensivity" ), CPP_PROPERTY( intensivity ), CPF_Edit );
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