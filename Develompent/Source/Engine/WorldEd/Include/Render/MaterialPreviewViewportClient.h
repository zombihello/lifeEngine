/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef MATERIALPREVIEWVIEWPORTCLIENT_H
#define MATERIALPREVIEWVIEWPORTCLIENT_H

#include "Render/EditorLevelViewportClient.h"
#include "Render/Material.h"
#include "Render/Scene.h"
#include "Components/SphereComponent.h"

/**
 * @ingroup WorldEd
 * @brief Material preview viewport client
 */
class FMaterialPreviewViewportClient : public FEditorLevelViewportClient
{
public:
	/**
	 * @brief Constructor
	 * @param InMaterial	Material
	 */
	FMaterialPreviewViewportClient( const TSharedPtr<FMaterial>& InMaterial );

	/**
	 * @brief Destructor
	 */
	~FMaterialPreviewViewportClient();

	/**
	 * Update logic of viewport client
	 */
	virtual void Tick( float InDeltaSeconds ) override;

	/**
	 * @brief Draw viewport
	 *
	 * @param InViewport	Viewport
	 */
	virtual void Draw( FViewport* InViewport ) override;

	/**
	 * @brief Draw viewport. Must be call in render thread
	 *
	 * @param InViewportRHI		Viewport RHI
	 * @param InSceneView		Scene view
	 */
	void Draw_RenderThread( FViewportRHIRef InViewportRHI, class FSceneView* InSceneView );

	/**
	 * @brief Set viewport type
	 * @param InViewportType		Viewport type
	 */
	virtual void SetViewportType( ELevelViewportType InViewportType ) override;

	/**
	 * @brief Set preview material
	 * @param InMaterial	Material
	 */
	FORCEINLINE void SetMaterial( const TSharedPtr<FMaterial>& InMaterial )
	{
		material = InMaterial;
		sphereComponent->SetMaterial( InMaterial );
	}

private:
	FScene*								scene;				/**< Scene for preview material */
	TSharedPtr<FMaterial>				material;			/**< Material */
	TRefCountPtr< LSphereComponent >	sphereComponent;	/**< Sphere component */
};

#endif // !MATERIALPREVIEWVIEWPORTCLIENT_H