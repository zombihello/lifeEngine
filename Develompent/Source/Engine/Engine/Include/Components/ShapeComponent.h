/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SHAPECOMPONENT_H
#define SHAPECOMPONENT_H

#include "Math/Transform.h"
#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "Components/PrimitiveComponent.h"

 /**
  * @ingroup Engine
  * ShapeComponent is a PrimitiveComponent that is represented by a simple geometrical shape (sphere, capsule, box, etc).
  */
class LShapeComponent : public LPrimitiveComponent
{
	DECLARE_CLASS( LShapeComponent, LPrimitiveComponent )

public:
	/**
	 * @brief Constructor
	 */
	LShapeComponent();

	/**
	 * @brief Destructor
	 */
	virtual ~LShapeComponent();

	/**
	 * Begins Play for the component.
	 * Called when the owning Actor begins play or when the component is created if the Actor has already begun play.
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief Serialize component
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class FArchive& InArchive ) override;

	/**
	 * @brief Update the body setup parameters based on shape information
	 */
	virtual void UpdateBodySetup() PURE_VIRTUAL( LShapeComponent::UpdateBodySetup, );

	/**
	 * @brief Set collision profile
	 * @param InName Name of collision profile
	 */
	FORCEINLINE void SetCollisionProfile( const std::wstring& InName )
	{
		FCollisionProfile*		newCollisionProfile;
		newCollisionProfile = GPhysicsEngine.FindCollisionProfile( InName );
		if ( newCollisionProfile )
		{
			collisionProfile = newCollisionProfile;
		}
	}

	/**
	 * @brief Set physics material
	 * @param InPhysMaterial Physics material
	 */
	FORCEINLINE void SetPhysMaterial( const TAssetHandle<FPhysicsMaterial>& InPhysMaterial )
	{
		physicsMaterial = InPhysMaterial.IsAssetValid() ? InPhysMaterial : GPhysicsEngine.GetDefaultPhysMaterial();
	}

	/**
	 * @brief Get collision profile
	 * @return Return collision profile
	 */
	FORCEINLINE FCollisionProfile* GetCollisionProfile() const
	{
		return collisionProfile;
	}

	/**
	 * @brief Get physics material
	 * @return Return physics material
	 */
	FORCEINLINE TAssetHandle<FPhysicsMaterial> GetPhysMaterial() const
	{
		return physicsMaterial;
	}

protected:
	FCollisionProfile*				collisionProfile;		/**< Collision profile */
	TAssetHandle<FPhysicsMaterial>	physicsMaterial;		/**< Physics material */
};

#endif // !SHAPECOMPONENT_H