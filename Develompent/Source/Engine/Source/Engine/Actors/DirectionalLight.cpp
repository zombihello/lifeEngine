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

#include "Engine/Actors/DirectionalLight.h"

IMPLEMENT_CLASS( ADirectionalLight )

/*
==================
ADirectionalLight::ADirectionalLight
==================
*/
ADirectionalLight::ADirectionalLight()
{
	directionalLightComponent	= CreateComponent<CDirectionalLightComponent>( TEXT( "DirectionalLightComponent0" ) );

#if WITH_EDITOR
	gizmoComponent				= CreateComponent<CSpriteComponent>( TEXT( "GizmoComponent0" ), true );
	gizmoComponent->SetGizmo( true );
	gizmoComponent->SetType( ST_Rotating );
	gizmoComponent->SetSpriteSize( Vector2D( 64.f, 64.f ) );
	gizmoComponent->SetMaterial( g_PackageManager->FindAsset( TEXT( "Material'EditorMaterials:ADirectionalLight_Gizmo_Mat" ), AT_Material ) );

	arrowComponent				= CreateComponent<CArrowComponent>( TEXT( "ArrowComponent0" ), true );
#endif // WITH_EDITOR
}

/*
==================
ADirectionalLight::StaticInitializeClass
==================
*/
void ADirectionalLight::StaticInitializeClass()
{
	new( staticClass, TEXT( "Directional Light Component" ) ) CObjectProperty( TEXT( "Light" ), TEXT( "Directional light component" ), STRUCT_OFFSET( ThisClass, directionalLightComponent ), CPF_Edit, CDirectionalLightComponent::StaticClass() );
}

#if WITH_EDITOR
/*
==================
ADirectionalLight::GetActorIcon
==================
*/
std::wstring ADirectionalLight::GetActorIcon() const
{
	return TEXT( "Engine/Editor/Icons/Actor_DirectionalLight.png" );
}
#endif // WITH_EDITOR