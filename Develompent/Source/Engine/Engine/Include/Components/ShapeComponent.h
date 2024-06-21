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
class CShapeComponent : public CPrimitiveComponent
{
	DECLARE_CLASS_INTRINSIC( CShapeComponent, CPrimitiveComponent, 0, 0, TEXT( "Engine" ) )

public:
	/**
	 * @brief Constructor
	 */
	CShapeComponent();

	/**
	 * Begins Play for the component.
	 * Called when the owning Actor begins play or when the component is created if the Actor has already begun play.
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief Serialize component
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Update the body setup parameters based on shape information
	 */
	virtual void UpdateBodySetup() PURE_VIRTUAL( CShapeComponent::UpdateBodySetup, );

	/**
	 * @brief Set collision profile
	 * @param InName Name of collision profile
	 */
	FORCEINLINE void SetCollisionProfile( const std::wstring& InName )
	{
		CollisionProfile*		newCollisionProfile;
		newCollisionProfile = g_PhysicsEngine.FindCollisionProfile( InName );
		if ( newCollisionProfile )
		{
			collisionProfile = newCollisionProfile;
		}
	}

	/**
	 * @brief Set physics material
	 * @param InPhysMaterial Physics material
	 */
	FORCEINLINE void SetPhysMaterial( const TAssetHandle<CPhysicsMaterial>& InPhysMaterial )
	{
		physicsMaterial = InPhysMaterial.IsAssetValid() ? InPhysMaterial : g_PhysicsEngine.GetDefaultPhysMaterial();
	}

	/**
	 * @brief Get collision profile
	 * @return Return collision profile
	 */
	FORCEINLINE CollisionProfile* GetCollisionProfile() const
	{
		return collisionProfile;
	}

	/**
	 * @brief Get physics material
	 * @return Return physics material
	 */
	FORCEINLINE TAssetHandle<CPhysicsMaterial> GetPhysMaterial() const
	{
		return physicsMaterial;
	}

protected:
	CollisionProfile*				collisionProfile;		/**< Collision profile */
	TAssetHandle<CPhysicsMaterial>	physicsMaterial;		/**< Physics material */
};

#endif // !SHAPECOMPONENT_H