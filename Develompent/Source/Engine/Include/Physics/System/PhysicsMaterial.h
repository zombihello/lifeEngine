/**
 * @file
 * @addtogroup Physics Physics
 *
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

#ifndef PHYSICSMATERIAL_H
#define PHYSICSMATERIAL_H

#include "Core/Misc/RefCounted.h"
#include "Core/Misc/RefCountPtr.h"
#include "Core/System/Package.h"
#include "Core/Core.h"
#include "Physics/Misc/PhysicsTypes.h"
#include "Physics/System/PhysicsMaterialDelegates.h"
#include "Physics/PhysicsInterface.h"

/**
 * @ingroup Physics
 * @brief Class of physics material
 */
class CPhysicsMaterial : public CAsset
{
public:
	/**
	 * @brief Constructor
	 */
	CPhysicsMaterial();

	/**
	 * @brief Destructor
	 */
	~CPhysicsMaterial();

	/**
	 * Serialize
	 *
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Set static friction
	 * @param InStaticFriction The coefficient of static friction
	 */
	FORCEINLINE void SetStaticFriction( float InStaticFriction )
	{
		if ( staticFriction == InStaticFriction )
		{
			return;
		}

		staticFriction = InStaticFriction;

		MarkDirty();
		UpdateMaterial();
	}

	/**
	 * @brief Set dynamic friction
	 * @param InDynamicFriction The coefficient of dynamic friction
	 */
	FORCEINLINE void SetDynamicFriction( float InDynamicFriction )
	{
		if ( dynamicFriction == InDynamicFriction )
		{
			return;
		}

		dynamicFriction = InDynamicFriction;

		MarkDirty();
		UpdateMaterial();
	}

	/**
	 * @brief Set restitution
	 * @param InRestitution The coefficient of restitution
	 */
	FORCEINLINE void SetRestitution( float InRestitution )
	{
		if ( restitution == InRestitution )
		{
			return;
		}

		restitution = InRestitution;
		
		MarkDirty();
		UpdateMaterial();
	}

	/**
	 * @brief Set density
	 * @param InDensity Density
	 */
	FORCEINLINE void SetDensity( float InDensity )
	{
		if ( density == InDensity )
		{
			return;
		}

		density = InDensity;

		MarkDirty();
		UpdateMaterial();
	}

	/**
	 * @brief Set surface type
	 * @param InNewSurfaceType New surface type
	 */
	FORCEINLINE void SetSurfaceType( ESurfaceType InNewSurfaceType )
	{
		if ( surfaceType == InNewSurfaceType )
		{
			return;
		}

		surfaceType = InNewSurfaceType;
		MarkDirty();
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
	FORCEINLINE PhysicsMaterialHandle_t GetMaterialHandle() const
	{
		if ( !CPhysicsInterface::IsValidMaterial( handle ) )
		{
			handle = CPhysicsInterface::CreateMaterial( SharedThis( this ) );
		}
		return handle;
	}

	/**
	 * @brief Get surface type
	 * @return Return surface type
	 */
	FORCEINLINE ESurfaceType GetSurfaceType() const
	{
		return surfaceType;
	}

	/**
	 * @brief Get delegate of physics material is update
	 * @return Return delegate of physics material update
	 */
	FORCEINLINE COnPhysicsMaterialUpdate& OnPhysicsMaterialUpdate() const
	{
		return onPhysicsMaterialUpdate;
	}

	/**
	 * @brief Get delegate of physics material is destroyed
	 * @return Return delegate of physics material destroyed
	 */
	FORCEINLINE COnPhysicsMaterialDestroyed& OnPhysicsMaterialDestroyed() const
	{
		return onPhysicsMaterialDestroyed;
	}

private:
	/**
	 * @brief Update material
	 */
	FORCEINLINE void UpdateMaterial()
	{
		TSharedPtr<CPhysicsMaterial>		sharedThis = SharedThis( this );
		CPhysicsInterface::UpdateMaterial( handle, sharedThis );
		onPhysicsMaterialUpdate.Broadcast( sharedThis );
	}

	float									staticFriction;					/**< The coefficient of static friction */
	float									dynamicFriction;				/**< The coefficient of dynamic friction */
	float									restitution;					/**< The coefficient of restitution */
	float									density;						/**< Density */
	ESurfaceType							surfaceType;					/**< Surface type */
	mutable PhysicsMaterialHandle_t			handle;							/**< Physics material handle */
	mutable COnPhysicsMaterialUpdate		onPhysicsMaterialUpdate;		/**< Event called when physics material is updated */
	mutable COnPhysicsMaterialDestroyed		onPhysicsMaterialDestroyed;		/**< Event called when physics material is destroyed */
};

//
// Serialization
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, TAssetHandle<CPhysicsMaterial>& InValue )
{
	TAssetHandle<CAsset>	asset = InValue;
	InArchive << asset;

	if ( InArchive.IsLoading() )
	{
		InValue = asset;
	}
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const TAssetHandle<CPhysicsMaterial>& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive << ( TAssetHandle<CAsset> )InValue;
	return InArchive;
}


#endif // !PHYSICSENGINE_H