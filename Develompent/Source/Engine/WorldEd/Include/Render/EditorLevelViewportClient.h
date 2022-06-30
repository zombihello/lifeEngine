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
	 * Screen to world space
	 *
	 * @param InScreenPoint		Screen point
	 * @param InViewportSizeX	Viewport size by X
	 * @param InViewportSizeY	Viewport size by Y
	 * @return Return point in world space
	 */
	Vector ScreenToWorld( const Vector2D& InScreenPoint, uint32 InViewportSizeX, uint32 InViewportSizeY );

	/**
	 * World space to screen
	 *
	 * @param InWorldPoint		World point
	 * @param InViewportSizeX	Viewport size by X
	 * @param InViewportSizeY	Viewport size by Y
	 * @return Return point in screen space
	 */
	Vector WorldToScreen( const Vector& InWorldPoint, uint32 InViewportSizeX, uint32 InViewportSizeY );

	/**
	 * @brief Get viewport type
	 * @return Return viewport type
	 */
	FORCEINLINE ELevelViewportType GetViewportType() const
	{
		return viewportType;
	}

	/**
	 * @brief Is allow open context menu
	 * @return Return TRUE if allow open context menu, else returning FALSE
	 */
	FORCEINLINE bool IsAllowContextMenu() const
	{
		return bAllowContextMenu;
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
	 * @param InSizeX	Size X
	 * @param InSizeY	Size Y
	 * @return Return scene view
	 */
	virtual class CSceneView* CalcSceneView( uint32 InSizeX, uint32 InSizeY );

	/**
	 * @brief Get background color
	 * @return Return background color
	 */
	virtual �Color GetBackgroundColor() const;

	bool						bSetListenerPosition;	/**< Is need sets the listener position */
	bool						bIsTracking;			/**< Is mouse tracking */
	bool						bIgnoreInput;			/**< Is need ignore input events */
	bool						bAllowContextMenu;		/**< Is allow open context menu when pressed right mouse button */
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