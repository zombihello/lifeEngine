/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Engine/Misc/EngineGlobals.h"
#include "Engine/Render/Scene.h"
#include "Engine/Render/SceneUtils.h"
#include "Engine/System/World.h"
#include "Engine/Components/SpotLightComponent.h"

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