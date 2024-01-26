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
	DECLARE_CLASS( CStaticMeshComponent, CPrimitiveComponent, 0, 0, TEXT( "Engine" ) )

public:
	/**
	 * @brief Constructor
	 */
	CStaticMeshComponent();

#if WITH_EDITOR
	/**
	 * @brief Function called by the editor when property is changed
	 * @param InPropertyChangedEvenet    Property changed event
	 */
	virtual void PostEditChangeProperty( const PropertyChangedEvenet& InPropertyChangedEvenet ) override;
#endif // WITH_EDITOR

	/**
	 * @brief Do any object-specific cleanup required immediately after loading an object
	 * @note This is not called for newly-created objects, and by default will always execute on the game thread
	 */
	virtual void PostLoad() override;

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
