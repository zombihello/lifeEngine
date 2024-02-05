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

#include "Core/System/Package.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/Actors/Audio.h"
#include "Engine/System/World.h"
#include "Audio/System/AudioBuffer.h"

IMPLEMENT_CLASS( AAudio )

/*
==================
AAudio::AAudio
==================
*/
AAudio::AAudio()
{
	audioComponent = CreateComponent<CAudioComponent>( TEXT( "AudioComponent0" ) );

#if WITH_EDITOR
	gizmoComponent = CreateComponent<CSpriteComponent>( TEXT( "GizmoComponent0" ), true );
	gizmoComponent->SetGizmo( true );
	gizmoComponent->SetType( ST_Rotating );
	gizmoComponent->SetSpriteSize( Vector2D( 64.f, 64.f ) );
	gizmoComponent->SetMaterial( g_PackageManager->FindAsset( TEXT( "Material'EditorMaterials:AAudio_Gizmo_Mat" ), AT_Material ) );
#endif // WITH_EDITOR
}

/*
==================
AAudio::~AAudio
==================
*/
AAudio::~AAudio()
{}

/*
==================
AAudio::StaticInitializeClass
==================
*/
void AAudio::StaticInitializeClass()
{
	new( staticClass, TEXT( "Audio Component" ) ) CObjectProperty( TEXT( "Audio" ), TEXT( "Audio component" ), STRUCT_OFFSET( ThisClass, audioComponent ), CPF_Edit, CAudioComponent::StaticClass() );
}

#if WITH_EDITOR
/*
==================
AAudio::GetActorIcon
==================
*/
std::wstring AAudio::GetActorIcon() const
{
	return TEXT( "Engine/Editor/Icons/Asset_Audio.png" );
}

/*
==================
AAudio::SpawnActorAsset
==================
*/
ActorRef_t AAudio::SpawnActorAsset( const TSharedPtr<CAsset>& InAsset, const Vector& InLocation, const Quaternion& InRotation )
{
	// If asset is not valid or not audio bank asset, we do nothing
	if ( !InAsset || InAsset->GetType() != AT_AudioBank )
	{
		return nullptr;
	}

	// Spawn new actor
	TRefCountPtr<AAudio>		audioActor = g_World->SpawnActor<AAudio>( InLocation, InRotation );
	if ( !audioActor )
	{
		return nullptr;
	}

	// Init asset in new audio actor
	audioActor->SetAudioBank( InAsset->GetAssetHandle() );
	return audioActor;
}
#endif // WITH_EDITOR