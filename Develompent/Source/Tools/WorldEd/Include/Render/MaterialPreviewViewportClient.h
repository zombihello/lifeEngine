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

#ifndef MATERIALPREVIEWVIEWPORTCLIENT_H
#define MATERIALPREVIEWVIEWPORTCLIENT_H

#include "Engine/Render/Material.h"
#include "Engine/Render/Scene.h"
#include "Engine/Components/SphereComponent.h"
#include "Engine/Components/PointLightComponent.h"
#include "Render/EditorLevelViewportClient.h"

/**
 * @ingroup WorldEd
 * @brief Material preview viewport client
 */
class CMaterialPreviewViewportClient : public CEditorLevelViewportClient
{
public:
	/**
	 * @brief Constructor
	 * @param InMaterial	Material
	 */
	CMaterialPreviewViewportClient( const TSharedPtr<CMaterial>& InMaterial );

	/**
	 * @brief Destructor
	 */
	~CMaterialPreviewViewportClient();

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
	 * @brief Set preview material
	 * @param InMaterial	Material
	 */
	FORCEINLINE void SetMaterial( const TSharedPtr<CMaterial>& InMaterial )
	{
		material = InMaterial;
		sphereComponent->SetMaterial( InMaterial->GetAssetHandle() );
	}

private:
	CScene*								scene;					/**< Scene for preview material */
	TSharedPtr<CMaterial>				material;				/**< Material */
	TRefCountPtr<CSphereComponent> 		sphereComponent;		/**< Sphere component */
	TRefCountPtr<CPointLightComponent>	pointLightComponent;	/**< Point light component */
};

#endif // !MATERIALPREVIEWVIEWPORTCLIENT_H