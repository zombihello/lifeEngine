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

#ifndef STATICMESHCOMPONENT_H
#define STATICMESHCOMPONENT_H

#include "Engine/Components/PrimitiveComponent.h"
#include "Engine/Render/StaticMesh.h"
#include "Engine/Render/Material.h"
#include "Engine/Render/Scene.h"

 /**
  * @ingroup Engine
  * @brief Component for work with static mesh
  */
class CStaticMeshComponent : public CPrimitiveComponent
{
	DECLARE_CLASS( CStaticMeshComponent, CPrimitiveComponent, 0, 0 )

public:
	/**
	 * @brief Constructor
	 */
	CStaticMeshComponent();

    /**
     * @brief Destructor
     */
    virtual ~CStaticMeshComponent();

	/**
	 * @brief Serialize component
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

#if WITH_EDITOR
	/**
	 * @brief Function called by the editor when property is changed
	 * @param InPropertyChangedEvenet    Property changed event
	 */
	virtual void PostEditChangeProperty( const PropertyChangedEvenet& InPropertyChangedEvenet ) override;
#endif // WITH_EDITOR

	/**
	 * @brief Adds mesh batches for draw in scene
	 *
	 * @param InSceneView Current view of scene
	 */
	virtual void AddToDrawList( const class CSceneView& InSceneView ) override;

    /**
     * @brief Set material
     *
     * @param InIndex Index of material layer in mesh
     * @param InMaterial Material
     */
    FORCEINLINE void SetMaterial( uint32 InIndex, const TAssetHandle<CMaterial>& InMaterial )
    {
        Assert( InIndex < overrideMaterials.size() );
		TAssetHandle<CMaterial>		material = InMaterial;
		TSharedPtr<CStaticMesh>		staticMeshRef = staticMesh.ToSharedPtr();
		if ( staticMeshRef && !material.IsValid() )
		{
			material = staticMeshRef->GetMaterial( InIndex );
		}
	
		overrideMaterials[InIndex] = material;
		bIsDirtyDrawingPolicyLink = true;
    }

	/**
	 * @brief Set static mesh
	 * @param InNewStaticMesh New static mesh
	 */
	FORCEINLINE void SetStaticMesh( const TAssetHandle<CStaticMesh>& InNewStaticMesh )
	{
		staticMesh = InNewStaticMesh;
		{
			TSharedPtr<CStaticMesh>		staticMeshRef = InNewStaticMesh.ToSharedPtr();
			if ( staticMeshRef )
			{
				uint32		numMaterials = staticMeshRef->GetNumMaterials();
				overrideMaterials.resize( numMaterials );
				for ( uint32 index = 0; index < numMaterials; ++index )
				{
					overrideMaterials[index] = staticMeshRef->GetMaterial( index );
				}
			}
		}
		bIsDirtyDrawingPolicyLink = true;
	}

	/**
	 * @brief Get static mesh
	 * @return Return pointer to static mesh. If not valid returning nullptr
	 */
	FORCEINLINE TAssetHandle<CStaticMesh> GetStaticMesh() const
	{
		return staticMesh;
	}

    /**
     * @brief Get material in mesh
     *
     * @param InIndex Index of material layer in mesh
     * @return Return pointer to material. If material not exist or static mesh is not valid returning nullptr
     */
    FORCEINLINE TAssetHandle<CMaterial> GetMaterial( uint32 InIndex ) const
    {
		// If mesh is not valid, we return NULL
		TSharedPtr<CStaticMesh>		staticMeshRef = staticMesh.ToSharedPtr();
		if ( !staticMeshRef )
		{
			return nullptr;
		}
        Assert( InIndex < overrideMaterials.size() );
		
		TAssetHandle<CMaterial>		material = overrideMaterials[ InIndex ];
        return material.IsValid() ? material : staticMeshRef->GetMaterial( InIndex );
    }

private:
	/**
	 * @brief Adds a draw policy link in SDGs
	 */
	virtual void LinkDrawList() override;

	/**
	 * @brief Removes a draw policy link from SDGs
	 */
	virtual void UnlinkDrawList() override;

	TAssetHandle<CStaticMesh>								staticMesh;						/**< Static mesh */
	TAssetHandle<CStaticMesh>								drawStaticMesh;					/**< Static mesh which drawing now */
	std::vector< TAssetHandle<CMaterial> >					overrideMaterials;				/**< Override materials */
	TSharedPtr<CStaticMesh::ElementDrawingPolicyLink>		elementDrawingPolicyLink;		/**< Element drawing policy link of current static mesh */
};

#endif // !STATICMESHCOMPONENT_H
