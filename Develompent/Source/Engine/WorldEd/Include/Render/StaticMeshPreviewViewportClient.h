/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef STATICMESHPREVIEWVIEWPORTCLIENT_H
#define STATICMESHPREVIEWVIEWPORTCLIENT_H

#include "Render/EditorLevelViewportClient.h"
#include "Render/StaticMesh.h"
#include "Render/Scene.h"
#include "Components/StaticMeshComponent.h"

/**
 * @ingroup WorldEd
 * @brief Static mesh preview viewport client
 */
class FStaticMeshPreviewViewportClient : public FEditorLevelViewportClient
{
public:
	/**
	 * @brief Constructor
	 * @param InStaticMesh	Static mesh
	 */
	FStaticMeshPreviewViewportClient( const TSharedPtr<FStaticMesh>& InStaticMesh );

	/**
	 * @brief Destructor
	 */
	~FStaticMeshPreviewViewportClient();

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
	 * @brief Set preview static mesh
	 * @param InStaticMesh	Static mesh
	 */
	FORCEINLINE void SetStaticMesh( const TSharedPtr<FStaticMesh>& InStaticMesh )
	{
		staticMesh = InStaticMesh;
		staticMeshComponent->SetStaticMesh( InStaticMesh->GetAssetHandle() );
	}

private:
	FScene*								scene;					/**< Scene for preview material */
	TSharedPtr<FStaticMesh>				staticMesh;				/**< Static mesh */
	TRefCountPtr<LStaticMeshComponent>	staticMeshComponent;	/**< Static mesh component */
};

#endif // !STATICMESHPREVIEWVIEWPORTCLIENT_H