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
class CStaticMeshPreviewViewportClient : public CEditorLevelViewportClient
{
public:
	/**
	 * @brief Constructor
	 * @param InStaticMesh	Static mesh
	 */
	CStaticMeshPreviewViewportClient( const TSharedPtr<CStaticMesh>& InStaticMesh );

	/**
	 * @brief Destructor
	 */
	~CStaticMeshPreviewViewportClient();

	/**
	 * Update logic of viewport client
	 */
	virtual void Tick( float InDeltaSeconds ) override;

	/**
	 * @brief Draw viewport
	 *
	 * @param InViewport	Viewport
	 */
	virtual void Draw( CViewport* InViewport ) override;

	/**
	 * @brief Draw viewport. Must be call in render thread
	 *
	 * @param InViewportRHI		Viewport RHI
	 * @param InSceneView		Scene view
	 */
	void Draw_RenderThread( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView );

	/**
	 * @brief Set viewport type
	 * @param InViewportType		Viewport type
	 */
	virtual void SetViewportType( ELevelViewportType InViewportType ) override;

	/**
	 * @brief Set preview static mesh
	 * @param InStaticMesh	Static mesh
	 */
	FORCEINLINE void SetStaticMesh( const TSharedPtr<CStaticMesh>& InStaticMesh )
	{
		staticMesh = InStaticMesh;
		staticMeshComponent->SetStaticMesh( InStaticMesh->GetAssetHandle() );
	}

private:
	CScene*						scene;					/**< Scene for preview material */
	TSharedPtr<CStaticMesh>		staticMesh;				/**< Static mesh */
	CStaticMeshComponent*		staticMeshComponent;	/**< Static mesh component */
};

#endif // !STATICMESHPREVIEWVIEWPORTCLIENT_H