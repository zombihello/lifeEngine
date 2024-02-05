/**
 * @file
 * @addtogroup WorldEd WorldEd
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

#ifndef STATICMESHPREVIEWVIEWPORTCLIENT_H
#define STATICMESHPREVIEWVIEWPORTCLIENT_H

#include "Engine/Render/StaticMesh.h"
#include "Engine/Render/Scene.h"
#include "Engine/Components/StaticMeshComponent.h"
#include "Render/EditorLevelViewportClient.h"

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
	CScene*								scene;					/**< Scene for preview material */
	TSharedPtr<CStaticMesh>				staticMesh;				/**< Static mesh */
	TRefCountPtr<CStaticMeshComponent>	staticMeshComponent;	/**< Static mesh component */
};

#endif // !STATICMESHPREVIEWVIEWPORTCLIENT_H