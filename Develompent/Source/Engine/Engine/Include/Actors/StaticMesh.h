/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ASTATICMESH_H
#define ASTATICMESH_H

#include <string>

#include "Actors/Actor.h"
#include "Components/StaticMeshComponent.h"

 /**
  * @ingroup Engine
  * Actor of static mesh
  */
class AStaticMesh : public AActor
{
	DECLARE_CLASS_INTRINSIC( AStaticMesh, AActor, 0, 0, TEXT( "Engine" ) )

public:
	/**
	 * Constructor
	 */
	AStaticMesh();

	/**
	 * Overridable native event for when play begins for this actor
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief Set static mesh asset
	 * @param InStaticMesh		Static mesh asset
	 */
	FORCEINLINE void SetStaticMesh( const TAssetHandle<CStaticMesh>& InStaticMesh )
	{
		staticMeshComponent->SetStaticMesh( InStaticMesh );
	}

#if WITH_EDITOR
	/**
	 * @brief Spawn asset actor
	 * 
	 * @param InAsset       Asset
	 * @param InLocation    Location actor on spawn
	 * @param InRotation    Rotation actor on spawn
	 * @return Return spawned actor. If failed returning NULL
	 */
	static AActor* SpawnActorAsset( const TSharedPtr<CAsset>& InAsset, const Vector& InLocation, const Quaternion& InRotation );

	/**
	 * @brief Get path to icon of actor for exploer level in WorldEd
	 * @return Return path to actor icon from Sys_BaseDir()
	 */
	virtual std::wstring GetActorIcon() const override;
#endif // WITH_EDITOR

private:
	CStaticMeshComponent*			staticMeshComponent;		/**< Static mesh component */
};

#endif // !ASTATICMESH_H