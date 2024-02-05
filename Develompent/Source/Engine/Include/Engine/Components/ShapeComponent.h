/**
 * @file
 * @addtogroup Engine Engine
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

#ifndef SHAPECOMPONENT_H
#define SHAPECOMPONENT_H

#include "Core/Math/Transform.h"
#include "Physics/Misc/PhysicsGlobals.h"
#include "Physics/System/PhysicsEngine.h"
#include "Engine/Components/PrimitiveComponent.h"

 /**
  * @ingroup Engine
  * ShapeComponent is a PrimitiveComponent that is represented by a simple geometrical shape (sphere, capsule, box, etc).
  */
class CShapeComponent : public CPrimitiveComponent
{
	DECLARE_CLASS( CShapeComponent, CPrimitiveComponent, 0, 0 )

public:
	/**
	 * @brief Constructor
	 */
	CShapeComponent();

	/**
	 * @brief Destructor
	 */
	virtual ~CShapeComponent();

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