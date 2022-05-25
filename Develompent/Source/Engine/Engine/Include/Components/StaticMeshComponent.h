/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef STATICMESHCOMPONENT_H
#define STATICMESHCOMPONENT_H

#include "Components/PrimitiveComponent.h"
#include "Render/StaticMesh.h"
#include "Render/Material.h"
#include "Render/Scene.h"

 /**
  * @ingroup Engine
  * @brief Component for work with static mesh
  */
class LStaticMeshComponent : public LPrimitiveComponent
{
	DECLARE_CLASS( LStaticMeshComponent, LPrimitiveComponent )

public:
	/**
	 * @brief Constructor
	 */
	LStaticMeshComponent();

    /**
     * @brief Destructor
     */
    virtual ~LStaticMeshComponent();

	/**
	 * @brief Adds mesh batches for draw in scene
	 *
	 * @param InSceneView Current view of scene
	 */
	virtual void AddToDrawList( const class FSceneView& InSceneView ) override;

    /**
     * @brief Set material
     *
     * @param InIndex Index of material layer in mesh
     * @param InMaterial Material
     */
    FORCEINLINE void SetMaterial( uint32 InIndex, const TAssetHandle<FMaterial>& InMaterial )
    {
        check( InIndex < overrideMaterials.size() );
		overrideMaterials[ InIndex ] = InMaterial;
		bIsDirtyDrawingPolicyLink = true;
    }

	/**
	 * @brief Set static mesh
	 * @param InNewStaticMesh New static mesh
	 */
	FORCEINLINE void SetStaticMesh( const TAssetHandle<FStaticMesh>& InNewStaticMesh )
	{
		staticMesh = InNewStaticMesh;
		{
			TSharedPtr<FStaticMesh>		staticMeshRef = InNewStaticMesh.ToSharedPtr();
			if ( staticMeshRef )
			{
				overrideMaterials.resize( staticMeshRef->GetNumMaterials() );
			}
		}
		bIsDirtyDrawingPolicyLink = true;
	}

	/**
	 * @brief Get static mesh
	 * @return Return pointer to static mesh. If not valid returning nullptr
	 */
	FORCEINLINE TAssetHandle<FStaticMesh> GetStaticMesh() const
	{
		return staticMesh;
	}

    /**
     * @brief Get material in mesh
     *
     * @param InIndex Index of material layer in mesh
     * @return Return pointer to material. If material not exist or static mesh is not valid returning nullptr
     */
    FORCEINLINE TAssetHandle<FMaterial> GetMaterial( uint32 InIndex ) const
    {
		// If mesh is not valid, we return NULL
		TSharedPtr<FStaticMesh>		staticMeshRef = staticMesh.ToSharedPtr();
		if ( !staticMeshRef )
		{
			return nullptr;
		}
        check( InIndex < overrideMaterials.size() );
		
		TAssetHandle<FMaterial>		material = overrideMaterials[ InIndex ];
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

	TAssetHandle<FStaticMesh>								staticMesh;						/**< Static mesh */
	std::vector< TAssetHandle<FMaterial> >					overrideMaterials;				/**< Override materials */
	TSharedPtr<FStaticMesh::FElementDrawingPolicyLink>		elementDrawingPolicyLink;		/**< Element drawing policy link of current static mesh */
};

#endif // !STATICMESHCOMPONENT_H
