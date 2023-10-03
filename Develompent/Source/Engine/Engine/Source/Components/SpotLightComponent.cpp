#include "Misc/EngineGlobals.h"
#include "Render/Scene.h"
#include "Render/SceneUtils.h"
#include "System/World.h"
#include "Components/SpotLightComponent.h"

IMPLEMENT_CLASS( CSpotLightComponent )

/*
==================
CSpotLightComponent::CSpotLightComponent
==================
*/
CSpotLightComponent::CSpotLightComponent()
	: bNeedUpdateCutoff( true )
	, radius( 850.f )
	, height( 1500.f )
	, cutoff( 0.f )
{
	intensivity = 100400.f;
}

/*
==================
CSpotLightComponent::StaticInitializeClass
==================
*/
void CSpotLightComponent::StaticInitializeClass()
{
	new( staticClass, TEXT( "Radius" ) ) CFloatProperty( TEXT( "Light" ), TEXT( "Light radius" ), STRUCT_OFFSET( ThisClass, radius ), CPF_Edit );
	new( staticClass, TEXT( "Height" ) ) CFloatProperty( TEXT( "Light" ), TEXT( "Light height" ), STRUCT_OFFSET( ThisClass, height ), CPF_Edit );
}

/*
==================
CSpotLightComponent::Serialize
==================
*/
void CSpotLightComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	if ( InArchive.Ver() < VER_NewSeriallizeDataInLightComponents )
	{
		return;
	}

	InArchive << radius;
	InArchive << height;
	
	if ( InArchive.IsLoading() )
	{
		bNeedUpdateCutoff = true;
	}
}

#if WITH_EDITOR
/*
==================
CSpotLightComponent::PostEditChangeProperty
==================
*/
void CSpotLightComponent::PostEditChangeProperty( const PropertyChangedEvenet& InPropertyChangedEvenet )
{
	CProperty*		changedProperty = InPropertyChangedEvenet.property;
	if ( changedProperty )
	{
		if ( changedProperty->GetCName() == TEXT( "Radius" ) || changedProperty->GetCName() == TEXT( "Height" ) )
		{
			bNeedUpdateCutoff = true;
		}
	}

	Super::PostEditChangeProperty( InPropertyChangedEvenet );
}

/*
==================
CSpotLightComponent::DrawDebugComponent
==================
*/
void CSpotLightComponent::DrawDebugComponent()
{
	if ( GetOwner() && GetOwner()->IsSelected() )
	{
		std::vector<Vector>		verteces;
		DrawWireCone( ( ( CScene* )g_World->GetScene() )->GetSDG( SDG_WorldEdBackground ), GetComponentTransform().ToMatrix(), radius, height, 50, DEC_LIGHT, verteces );
	}
}
#endif // WITH_EDITOR

/*
==================
CSpotLightComponent::GetLightType
==================
*/
ELightType CSpotLightComponent::GetLightType() const
{
	return LT_Spot;
}