/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSMATERIAL_H
#define PHYSICSMATERIAL_H

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include "Misc/RefCounted.h"
#include "Misc/RefCountPtr.h"
#include "System/Package.h"
#include "Core.h"

/**
 * @ingroup Physics
 * @brief Reference to FPhysicsMaterial
 */
typedef TRefCountPtr< class FPhysicsMaterial >				FPhysicsMaterialRef;

/**
 * @ingroup Physics
 * @brief Class of physics material
 */
class FPhysicsMaterial : public FAsset
{
public:
	/**
	 * @brief Constructor
	 * @warning Before call must need initialized FPhysicsEngine
	 */
	FPhysicsMaterial();

	/**
	 * @brief Destructor
	 */
	~FPhysicsMaterial();

	/**
	 * Serialize
	 *
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class FArchive& InArchive ) override;

	/**
	 * @brief Set static friction
	 * @param InStaticFriction The coefficient of static friction
	 */
	FORCEINLINE void SetStaticFriction( float InStaticFriction )
	{
		pxMaterial->setStaticFriction( InStaticFriction );
		staticFriction = InStaticFriction;
	}

	/**
	 * @brief Set dynamic friction
	 * @param InDynamicFriction The coefficient of dynamic friction
	 */
	FORCEINLINE void SetDynamicFriction( float InDynamicFriction )
	{
		pxMaterial->setDynamicFriction( InDynamicFriction );
		dynamicFriction = InDynamicFriction;
	}

	/**
	 * @brief Set restitution
	 * @param InRestitution The coefficient of restitution
	 */
	FORCEINLINE void SetRestitution( float InRestitution )
	{
		pxMaterial->setRestitution( InRestitution );
		restitution = InRestitution;
	}

	/**
	 * @brief Get static friction
	 * @return Get static friction
	 */
	FORCEINLINE float GetStaticFriction() const
	{
		return staticFriction;
	}

	/**
	 * @brief Get dynamic friction
	 * @return Get dynamic friction
	 */
	FORCEINLINE float GetDynamicFriction() const
	{
		return dynamicFriction;
	}

	/**
	 * @brief Get restitution
	 * @return Get restitution
	 */
	FORCEINLINE float GetRestitution() const
	{
		return restitution;
	}

	/**
	 * @brief Get PhysX material
	 * @return Get PhysX material
	 */
	FORCEINLINE physx::PxMaterial* GetPhysXMaterial() const
	{
		return pxMaterial;
	}

private:
	float					staticFriction;		/**< The coefficient of static friction */
	float					dynamicFriction;	/**< The coefficient of dynamic friction */
	float					restitution;		/**< The coefficient of restitution */
	physx::PxMaterial*		pxMaterial;			/**< PhysX material */
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FPhysicsMaterialRef& InValue )
{
	FAssetRef			asset = InValue;
	InArchive << asset;

	if ( InArchive.IsLoading() )
	{
		InValue = asset;
	}
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FPhysicsMaterialRef& InValue )
{
	check( InArchive.IsSaving() );
	InArchive << ( FAssetRef )InValue;
	return InArchive;
}


#endif // !PHYSICSENGINE_H