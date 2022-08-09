/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EDITORLEVELVIEWPORTCLIENT_H
#define EDITORLEVELVIEWPORTCLIENT_H

#include "Render/Viewport.h"
#include "Render/Scene.h"
#include "Render/EditorCommonDrawHelper.h"
#include "Render/HitProxies.h"
#include "System/WindowEvent.h"
#include "System/Gizmo.h"

/**
 * @ingroup WorldEd
 * @brief Editor level viewport client
 */
class CEditorLevelViewportClient : public CViewportClient
{
public:
	/**
	 * @brief Enumeration of type mouse tracking
	 */
	enum EMouseTracking
	{
		MT_None,		/**< No mouse tracking */
		MT_View,		/**< Tacking mouse for view */
		MT_Gizmo		/**< Tacking mouse for gizmo */
	};

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

#if ENABLE_HITPROXY
	/**
	 * @brief Draw hit proxies
	 *
	 * @param InViewport		Viewport
	 * @param InHitProxyLayer	Hit proxy layer
	 */
	virtual void DrawHitProxies( CViewport* InViewport, EHitProxyLayer InHitProxyLayer = HPL_World );

	/**
	 * @brief Draw hit proxies. Must be call in render thread
	 *
	 * @param InViewportRHI		Viewport RHI
	 * @param InSceneView		Scene view
	 * @param InHitProxyLayer	Hit proxy layer
	 */
	void DrawHitProxies_RenderThread( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView, EHitProxyLayer InHitProxyLayer = HPL_World );

	/**
	 * @brief Get hit proxy id by screen coord
	 * 
	 * @param InX	X
	 * @param InY	Y
	 * @return Return hit proxy id
	 */
	CHitProxyId GetHitProxyId( uint32 InX, uint32 InY ) const;
#endif // ENABLE_HITPROXY

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
	 * @brief Get mouse tracking type
	 * @return Return mouse tracking type
	 */
	FORCEINLINE EMouseTracking GetMouseTrackingType() const
	{
		return trackingType;
	}

	/**
	 * @brief Set ignore input
	 * @param InIsIgnoreInput		Is need ignore input
	 */
	FORCEINLINE void SetIgnoreInput( bool InIsIgnoreInput )
	{
		bIgnoreInput = InIsIgnoreInput;
	}

	/**
	 * @brief Is allow open context menu
	 * @return Return TRUE if allow open context menu, else returning FALSE
	 */
	FORCEINLINE bool IsAllowContextMenu() const
	{
		return bAllowContextMenu;
	}

	/**
	 * @brief Set gizmo
	 * @param InNewGizmo	New gizmo
	 */
	FORCEINLINE void SetGizmo( CGizmo* InNewGizmo )
	{
		gizmo = InNewGizmo;
	}

	/**
	 * @brief Get gizmo
	 * @return Return pointer to gizmo, if not exist returning NULL
	 */
	FORCEINLINE CGizmo* GetGizmo() const
	{
		return gizmo;
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
	virtual CColor GetBackgroundColor() const;

	/**
	 * Converts the delta movement to drag/rotation/scale based on gizmo axis
	 * 
	 * @param InDragDelta		Muse move delta
	 * @param OutDrag			Output delta of move
	 * @param OutRotation		Output delta of rotation
	 * @param OutScale			Output delta of scale
	 * @param InSceneView		Scene view. This parameter using only for perspective viewport type
	 */
	void ConvertMovementDeltaToDragRot( const Vector2D& InDragDelta, Vector& OutDrag, Quaternion& OutRotation, Vector& OutScale, class CSceneView* InSceneView = nullptr );

	bool						bSetListenerPosition;	/**< Is need sets the listener position */
	EMouseTracking				trackingType;			/**< Mouse tacking type */
	bool						bIgnoreInput;			/**< Is need ignore input events */
	bool						bAllowContextMenu;		/**< Is allow open context menu when pressed right mouse button */
	ELevelViewportType			viewportType;			/**< Viewport type */	
	Vector						viewLocation;			/**< Viewport location */	
	Vector						viewRotationEuler;		/**< Viewport orientation in euler angles. Valid only for perspective projections */
	Quaternion					viewRotationQuat;		/**< Viewport orientation in quaternion. Valid only for perspective projections */
	float						viewFOV;				/**< Viewport horizontal field of view */
	float						orthoZoom;				/**< Zoom in ortho viewport type */
	float						cameraSpeed;			/**< Camera speed (only for perspective projections) */
	ShowFlags_t					showFlags;				/**< Show flags */
	CEditorCommonDrawHelper		drawHelper;				/**< Draw helper */
	byte						cameraMoveFlags;		/**< Camera move flags */
	CGizmo*						gizmo;					/**< Gizmo for rendering pivot */
};

#endif // !EDITORLEVELVIEWPORTCLIENT_H