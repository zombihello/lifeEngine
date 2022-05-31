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
#include "Render/EditorCommonDrawHelper.h"
#include "System/WindowEvent.h"

/**
 * @ingroup WorldEd
 * @brief Editor level viewport client
 */
class CEditorLevelViewportClient : public CViewportClient
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InViewportType		Viewport type
	 */
	CEditorLevelViewportClient( ELevelViewportType InViewportType = LVT_Perspective );

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
	 * @brief Process event
	 *
	 * @param InWindowEvent			Window event
	 */
	virtual void ProcessEvent( struct SWindowEvent& InWindowEvent ) override;

	/**
	 * @brief Set viewport type
	 * @param InViewportType		Viewport type
	 */
	virtual void SetViewportType( ELevelViewportType InViewportType );

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
	 * @brief Enumeration of move camera flags
	 */
	enum ECameraMoveVector
	{
		CMV_MoveForward		= 1 << 0,		/**< Move forward */
		CMV_MoveBackward	= 1 << 1,		/**< Move backward */
		CMV_MoveLeft		= 1 << 2,		/**< Move left */
		CMV_MoveRight		= 1 << 3		/**< Move right */
	};

	/**
	 * @brief Calculate scene view
	 * 
	 * @warning Need delete allocated pointer
	 * @param InViewport		Viewport
	 * @return Return scene view
	 */
	virtual class CSceneView* CalcSceneView( CViewport* InViewport );

	/**
	 * @brief Get background color
	 * @return Return background color
	 */
	virtual ÑColor GetBackgroundColor() const;

	bool						bSetListenerPosition;	/**< Is need sets the listener position */
	bool						bIsTracking;			/**< Is mouse tracking */
	bool						bIgnoreInput;			/**< Is need ignore input events */
	ELevelViewportType			viewportType;			/**< Viewport type */	
	Vector						viewLocation;			/**< Viewport location */	
	CRotator					viewRotation;			/**< Viewport orientation. Valid only for perspective projections */
	float						viewFOV;				/**< Viewport horizontal field of view */
	float						orthoZoom;				/**< Zoom in ortho viewport type */
	float						cameraSpeed;			/**< Camera speed (only for perspective projections) */
	ShowFlags_t					showFlags;				/**< Show flags */
	CEditorCommonDrawHelper		drawHelper;				/**< Draw helper */
	byte						cameraMoveFlags;		/**< Camera move flags */
};

#endif // !EDITORLEVELVIEWPORTCLIENT_H