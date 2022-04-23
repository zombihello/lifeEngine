/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EDITORLEVELVIEWPORTCLIENT_H
#define EDITORLEVELVIEWPORTCLIENT_H

#include "Math/Rotator.h"
#include "Render/Viewport.h"
#include "Render/Scene.h"

/**
 * @ingroup WorldEd
 * @brief Enumeration of viewport type
 */
enum ELevelViewportType
{
	LVT_None = -1,			/**< None */
	LVT_OrthoXY,			/**< Ortho XY */
	LVT_OrthoXZ,			/**< Ortho XZ */
	LVT_OrthoYZ,			/**< Ortho YZ */
	LVT_Perspective			/**< Perspective */
};

/**
 * @ingroup WorldEd
 * @brief Editor level viewport client
 */
class FEditorLevelViewportClient : public FViewportClient
{
public:
	/**
	 * @brief Constructor
	 */
	FEditorLevelViewportClient();

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
	FORCEINLINE void SetViewportType( ELevelViewportType InViewportType )
	{
		viewportType = InViewportType;
	}

	/**
	 * @brief Get viewport type
	 * @return Return viewport type
	 */
	FORCEINLINE ELevelViewportType GetViewportType() const
	{
		return viewportType;
	}

protected:
	/**
	 * @brief Calculate scene view
	 * 
	 * @warning Need delete allocated pointer
	 * @param InViewport		Viewport
	 * @return Return scene view
	 */
	class FSceneView* CalcSceneView( FViewport* InViewport );

	/**
	 * @brief Get background color
	 * @return Return background color
	 */
	FColor GetBackgroundColor() const;

	bool					bSetListenerPosition;		/**< Is need sets the listener position */
	ELevelViewportType		viewportType;				/**< Viewport type */	
	FVector					viewLocation;				/**< Viewport location */	
	FRotator				viewRotation;				/**< Viewport orientation. Valid only for perspective projections */
	float					viewFOV;					/**< Viewport horizontal field of view */
	EShowFlags				showFlags;					/**< Show flags */
};

#endif // !EDITORLEVELVIEWPORTCLIENT_H