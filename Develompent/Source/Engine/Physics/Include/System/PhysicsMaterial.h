/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSMATERIAL_H
#define PHYSICSMATERIAL_H

#include "Misc/RefCounted.h"
#include "Misc/RefCountPtr.h"
#include "System/Package.h"
#include "System/Delegate.h"
#include "PhysicsInterface.h"
#include "Core.h"

/**
 * @ingroup Physics
 * @brief Delegate for called event when physics materials is updated
 */
DECLARE_MULTICAST_DELEGATE( FOnPhysicsMaterialUpdate, class FPhysicsMaterial* )

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
		staticFriction = InStaticFriction;
		UpdateMaterial();
	}

	/**
	 * @brief Set dynamic friction
	 * @param InDynamicFriction The coefficient of dynamic friction
	 */
	FORCEINLINE void SetDynamicFriction( float InDynamicFriction )
	{
		dynamicFriction = InDynamicFriction;
		UpdateMaterial();
	}

	/**
	 * @brief Set restitution
	 * @param InRestitution The coefficient of restitution
	 */
	FORCEINLINE void SetRestitution( float InRestitution )
	{
		restitution = InRestitution;
		UpdateMaterial();
	}

	/**
	 * @brief Set density
	 * @param InDensity Density
	 */
	FORCEINLINE void SetDensity( float InDensity )
	{
		density = InDensity;
		
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
	 * @brief Get density
	 * @return Get density
	 */
	FORCEINLINE float GetDensity() const
	{
		return density;
	}

	/**
	 * @brief Get physics material handle
	 * @return Return physics material handle
	 */
	FORCEINLINE FPhysicsMaterialHandle GetMaterialHandle() const
	{
		return handle;
	}

	/**
	 * @brief Get delegate of physics material is update
	 * @return Return delegate of physics material update
	 */
	FORCEINLINE FOnPhysicsMaterialUpdate& OnPhysicsMaterialUpdate() const
	{
		return onPhysicsMaterialUpdate;
	}

private:
	/**
	 * @brief Update material
	 */
	FORCEINLINE void UpdateMaterial()
	{
		FPhysicsInterface::UpdateMaterial( handle, this );
		onPhysicsMaterialUpdate.Broadcast( this );
	}

	float									staticFriction;					/**< The coefficient of static friction */
	float									dynamicFriction;				/**< The coefficient of dynamic friction */
	float									restitution;					/**< The coefficient of restitution */
	float									density;						/**< Density */
	FPhysicsMaterialHandle					handle;							/**< Physics material handle */
	mutable FOnPhysicsMaterialUpdate		onPhysicsMaterialUpdate;		/**< Event called when physics material is updated */
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