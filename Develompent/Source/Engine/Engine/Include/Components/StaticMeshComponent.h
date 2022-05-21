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
    FORCEINLINE void SetMaterial( uint32 InIndex, const TWeakPtr<FMaterial>& InMaterial )
    {
        check( InIndex < materials.size() );
        materials[ InIndex ] = InMaterial;
		bIsDirtyDrawingPolicyLink = true;
    }

	/**
	 * @brief Set static mesh
	 * @param InNewStaticMesh New static mesh
	 */
	FORCEINLINE void SetStaticMesh( const TWeakPtr<FStaticMesh>& InNewStaticMesh )
	{
		staticMesh = InNewStaticMesh;
		{
			TSharedPtr<FStaticMesh>		staticMeshRef = InNewStaticMesh.Pin();
			if ( staticMeshRef )
			{
				materials = staticMeshRef->GetMaterials();
			}
		}
		bIsDirtyDrawingPolicyLink = true;
	}

	/**
	 * @brief Get static mesh
	 * @return Return pointer to static mesh. If not valid returning nullptr
	 */
	FORCEINLINE TWeakPtr<FStaticMesh> GetStaticMesh() const
	{
		return staticMesh;
	}

    /**
     * @brief Get material in mesh
     *
     * @param InIndex Index of material layer in mesh
     * @return Return pointer to material. If material not exist returning nullptr
     */
    FORCEINLINE TWeakPtr<FMaterial> GetMaterial( uint32 InIndex ) const
    {
        check( InIndex < materials.size() );
        return materials[ InIndex ];
    }

private:
	/**
	 * @brief Typedef of drawing policy link
	 */
	typedef FMeshDrawList< FStaticMeshDrawPolicy >::FDrawingPolicyLink			FDrawingPolicyLink;

	/**
	 * @brief Typedef of reference on drawing policy link in scene
	 */
	typedef FMeshDrawList< FStaticMeshDrawPolicy >::FDrawingPolicyLinkRef		FDrawingPolicyLinkRef;

	/**
	 * @brief Adds a draw policy link in SDGs
	 */
	virtual void LinkDrawList() override;

	/**
	 * @brief Removes a draw policy link from SDGs
	 */
	virtual void UnlinkDrawList() override;

	TWeakPtr<FStaticMesh>						staticMesh;						/**< Static mesh */
	std::vector< TWeakPtr<FMaterial> >			materials;						/**< Override materials */
	std::vector< FDrawingPolicyLinkRef >		drawingPolicyLinks;				/**< Array of reference to drawing policy link in scene */
	std::vector< const FMeshBatch* >			meshBatchLinks;					/**< Array of references to mesh batch in drawing policy link */
};

#endif // !STATICMESHCOMPONENT_H
