#include "Components/ArrowComponent.h"
#include "Render/Scene.h"
#include "System/World.h"

IMPLEMENT_CLASS( CArrowComponent )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CArrowComponent )

/*
==================
CArrowComponent::CArrowComponent
==================
*/
CArrowComponent::CArrowComponent()
	: length( 100.f )
{}

/*
==================
CArrowComponent::AddToDrawList
==================
*/
void CArrowComponent::AddToDrawList( const class CSceneView& InSceneView )
{
#if WITH_EDITOR
	CScene*				scene				= ( CScene* )g_World->GetScene();
	float				oneThirdLength		= length / 10.f;
	const CTransform&	componentTransform	= GetComponentTransform();
	Vector				direction			= componentTransform.GetUnitAxis( A_Forward );

	// Arrow body
	Vector		start	= GetComponentLocation();
	Vector		end		= start + direction * length;
	scene->GetSDG( SDG_WorldEdForeground ).simpleElements.AddLine( start, end, CColor::red );

	// First arrow side
	start				= end;
	Vector	arrowBase	= start - direction * oneThirdLength;
	end					= arrowBase + componentTransform.GetUnitAxis( A_Right ) * 2.f;
	scene->GetSDG( SDG_WorldEdForeground ).simpleElements.AddLine( start, end, CColor::red );

	// Second arrow side
	end = arrowBase - componentTransform.GetUnitAxis( A_Right ) * 2.f;
	scene->GetSDG( SDG_WorldEdForeground ).simpleElements.AddLine( start, end, CColor::red );

	// Third arrow side
	end = arrowBase + componentTransform.GetUnitAxis( A_Up ) * 2.f;
	scene->GetSDG( SDG_WorldEdForeground ).simpleElements.AddLine( start, end, CColor::red );
	
	// Fourth arrow side
	end = arrowBase - componentTransform.GetUnitAxis( A_Up ) * 2.f;
	scene->GetSDG( SDG_WorldEdForeground ).simpleElements.AddLine( start, end, CColor::red );
#endif // WITH_EDITOR
}