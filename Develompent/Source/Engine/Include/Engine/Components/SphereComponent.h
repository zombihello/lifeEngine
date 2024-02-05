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

#ifndef SPHERECOMPONENT_H
#define SPHERECOMPONENT_H

#include "Engine/Components/ShapeComponent.h"
#include "Engine/Render/Scene.h"
#include "Engine/Render/Material.h"

 /**
  * @ingroup Engine
  * Sphere component
  */
class CSphereComponent : public CShapeComponent
{
	DECLARE_CLASS( CSphereComponent, CShapeComponent, 0, 0 )

public:
	/**
	 * Constructor
	 */
	CSphereComponent();

	/**
	 * @brief Serialize component
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Update the body setup parameters based on shape information
	 */
	virtual void UpdateBodySetup() override;

	/**
	 * @brief Adds mesh batches for draw in scene
	 *
	 * @param InSceneView Current view of scene
	 */
	virtual void AddToDrawList( const class CSceneView& InSceneView ) override;

#if WITH_EDITOR
	/**
	 * @brief Function called by the editor when property is changed
	 * @param InPropertyChangedEvenet    Property changed event
	 */
	virtual void PostEditChangeProperty( const PropertyChangedEvenet& InPropertyChangedEvenet ) override;
#endif // WITH_EDITOR

	/**
	 * @brief Set SDG level
	 * @param InSDGLevel	SDG level for draw
	 */
	FORCEINLINE void SetSDGLevel( ESceneDepthGroup InSDGLevel )
	{
		pendingSDGLevel = InSDGLevel;
		bIsDirtyDrawingPolicyLink = true;
	}

	/**
	 * @brief Set radius
	 * @param InRadius	Radius
	 */
	FORCEINLINE void SetRadius( float InRadius )
	{
		radius = InRadius;
		bIsDirtyDrawingPolicyLink = true;
	}

	/**
	 * @brief Set material
	 * @param InMaterial	Material
	 */
	FORCEINLINE void SetMaterial( const TAssetHandle<CMaterial>& InMaterial )
	{
		material = InMaterial;
		bIsDirtyDrawingPolicyLink = true;
	}

	/**
	 * @brief Get SDG level
	 * @return Return SDG level for draw
	 */
	FORCEINLINE ESceneDepthGroup GetSDGLevel() const
	{
		return SDGLevel;
	}

	/**
	 * @brief Get radius
	 * @return Return radius
	 */
	FORCEINLINE float GetRadius() const
	{
		return radius;
	}

	/**
	 * @brief Get material
	 * @return Return material, if not exist return NULL
	 */
	FORCEINLINE TAssetHandle<CMaterial> GetMaterial() const
	{
		return material;
	}

private:
	/**
	 * @brief Typedef of drawing policy link
	 */
	typedef CMeshDrawList<CMeshDrawingPolicy>::DrawingPolicyLink			DrawingPolicyLink_t;

	/**
	 * @brief Typedef of reference on drawing policy link in scene
	 */
	typedef CMeshDrawList<CMeshDrawingPolicy>::DrawingPolicyLinkRef_t		DrawingPolicyLinkRef_t;

	/**
	 * @brief Typedef of depth drawing policy link
	 */
	typedef CMeshDrawList<CDepthDrawingPolicy>::DrawingPolicyLink			DepthDrawingPolicyLink_t;

	/**
	 * @brief Typedef of reference on depth drawing policy link in scene
	 */
	typedef CMeshDrawList<CDepthDrawingPolicy>::DrawingPolicyLinkRef_t		DepthDrawingPolicyLinkRef_t;

	/**
	 * @brief Adds a draw policy link in SDGs
	 */
	virtual void LinkDrawList() override;

	/**
	 * @brief Removes a draw policy link from SDGs
	 */
	virtual void UnlinkDrawList() override;

	float								radius;						/**< Radius of sphere */
	ESceneDepthGroup					SDGLevel;					/**< Mesh on SDG level */
	ESceneDepthGroup					pendingSDGLevel;			/**< Pending SDG level */
	TAssetHandle<CMaterial>				material;					/**< Material */
	DrawingPolicyLinkRef_t				drawingPolicyLink;			/**< Drawing policy link in scene */
	DepthDrawingPolicyLinkRef_t			depthDrawingPolicyLink;		/**< Depth drawing policy link in scene */
	std::vector<const MeshBatch*>		meshBatchLinks;				/**< Reference to mesh batch in drawing policy link */
};

#endif // !SPHERECOMPONENT_H