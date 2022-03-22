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

 /**
  * @ingroup Engine
  * @brief Component for work with static mesh
  */
class LStaticMeshComponent : public LPrimitiveComponent
{
	DECLARE_CLASS( LStaticMeshComponent, LPrimitiveComponent )

public:
    /**
     * @brief Destructor
     */
    virtual ~LStaticMeshComponent();

	/**
	 * @brief Add primitive to draw list
	 *
	 * @param InScene Scene
     * @param InSceneView Current view of scene
	 */
    virtual void AddToDrawList( class FScene* InScene, const class FSceneView& InSceneView ) override;

    /**
     * @brief Set material
     *
     * @param InIndex Index of material layer in mesh
     * @param InMaterial Material
     */
    FORCEINLINE void SetMaterial( uint32 InIndex, class FMaterial* InMaterial )
    {
        check( InIndex < materials.size() );
        materials[ InIndex ] = InMaterial;
    }

	/**
	 * @brief Set static mesh
	 * @param InNewStaticMesh New static mesh
	 */
	FORCEINLINE void SetStaticMesh( class FStaticMesh* InNewStaticMesh )
	{
		staticMesh = InNewStaticMesh;
        materials = staticMesh->GetMaterials();
	}

	/**
	 * @brief Get static mesh
	 * @return Return pointer to static mesh. If not valid returning nullptr
	 */
	FORCEINLINE FStaticMeshRef GetStaticMesh() const
	{
		return staticMesh;
	}

    /**
     * @brief Get material in mesh
     *
     * @param InIndex Index of material layer in mesh
     * @return Return pointer to material. If material not exist returning nullptr
     */
    FORCEINLINE FMaterialRef GetMaterial( uint32 InIndex ) const
    {
        check( InIndex < materials.size() );
        return materials[ InIndex ];
    }

private:
    FStaticMeshRef                      staticMesh;     /**< Static mesh */
    std::vector< FMaterialRef >         materials;      /**< Override materials */
};

#endif // !STATICMESHCOMPONENT_H
