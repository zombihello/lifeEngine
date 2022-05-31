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
class CStaticMeshComponent : public CPrimitiveComponent
{
	DECLARE_CLASS( CStaticMeshComponent, CPrimitiveComponent )

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
        check( InIndex < overrideMaterials.size() );
		overrideMaterials[ InIndex ] = InMaterial;
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
				overrideMaterials.resize( staticMeshRef->GetNumMaterials() );
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
        check( InIndex < overrideMaterials.size() );
		
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
	std::vector< TAssetHandle<CMaterial> >					overrideMaterials;				/**< Override materials */
	TSharedPtr<CStaticMesh::SElementDrawingPolicyLink>		elementDrawingPolicyLink;		/**< Element drawing policy link of current static mesh */
};

#endif // !STATICMESHCOMPONENT_H
