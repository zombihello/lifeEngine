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

#include "Engine/Components/ArrowComponent.h"
#include "Engine/Render/Scene.h"
#include "Engine/System/World.h"

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