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

#include "Core/Logger/LoggerMacros.h"
#include "Core/System/Config.h"
#include "Core/Misc/CoreGlobals.h"
#include "Core/System/Package.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/System/World.h"
#include "Engine/Actors/StaticMesh.h"

IMPLEMENT_CLASS( AStaticMesh )

/*
==================
AStaticMesh::AStaticMesh
==================
*/
AStaticMesh::AStaticMesh()
{
	staticMeshComponent = CreateComponent< CStaticMeshComponent >( TEXT( "StaticMeshComponent0" ) );
}

/*
==================
AStaticMesh::~AStaticMesh
==================
*/
AStaticMesh::~AStaticMesh()
{}

/*
==================
AStaticMesh::StaticInitializeClass
==================
*/
void AStaticMesh::StaticInitializeClass()
{
	new( staticClass, TEXT( "Static Mesh Component" ) ) CObjectProperty( TEXT( "Drawing" ), TEXT( "Static mesh component" ), STRUCT_OFFSET( ThisClass, staticMeshComponent ), CPF_Edit, CStaticMeshComponent::StaticClass() );
}

/*
==================
AStaticMesh::BeginPlay
==================
*/
void AStaticMesh::BeginPlay()
{
	Super::BeginPlay();
}

#if WITH_EDITOR
/*
==================
AStaticMesh::SpawnActorAsset
==================
*/
ActorRef_t AStaticMesh::SpawnActorAsset( const TSharedPtr<CAsset>& InAsset, const Vector& InLocation, const Quaternion& InRotation )
{
	// If asset is not valid or not static mesh asset, we do nothing
	if ( !InAsset || InAsset->GetType() != AT_StaticMesh )
	{
		return nullptr;
	}

	// Spawn new actor
	TRefCountPtr<AStaticMesh>		staticMeshActor = g_World->SpawnActor<AStaticMesh>( InLocation, InRotation );
	if ( !staticMeshActor )
	{
		return nullptr;
	}

	// Init asset in new static mesh actor
	staticMeshActor->SetStaticMesh( InAsset->GetAssetHandle() );
	return staticMeshActor;
}

/*
==================
AStaticMesh::GetActorIcon
==================
*/
std::wstring AStaticMesh::GetActorIcon() const
{
	return TEXT( "Engine/Editor/Icons/Asset_StaticMesh.png" );
}
#endif // WITH_EDITOR