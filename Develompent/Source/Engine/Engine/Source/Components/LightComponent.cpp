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
CLightComponent::BeginDestroy
==================
*/
void CLightComponent::BeginDestroy()
{
	Super::BeginDestroy();
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
	new( staticClass, TEXT( "bEnabled" ), OBJECT_Public )		CBoolProperty( CPP_PROPERTY( ThisClass, bEnabled ), TEXT( "Light" ), TEXT( "Is enabled light" ), CPF_Edit );
	new( staticClass, TEXT( "Color" ), OBJECT_Public )			CColorProperty( CPP_PROPERTY( ThisClass, lightColor ), TEXT( "Light" ), TEXT( "Light color" ), CPF_Edit );
	new( staticClass, TEXT( "Intensivity" ), OBJECT_Public )	CFloatProperty( CPP_PROPERTY( ThisClass, intensivity ), TEXT( "Light" ), TEXT( "Light intensivity" ), CPF_Edit );
}

/*
==================
CLightComponent::Spawned
==================
*/
void CLightComponent::Spawned()
{
	Super::Spawned();
	GetWorld()->GetScene()->AddLight( this );
}

/*
==================
CLightComponent::Destroyed
==================
*/
void CLightComponent::Destroyed()
{
	Super::Destroyed();
	GetWorld()->GetScene()->RemoveLight( this );
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