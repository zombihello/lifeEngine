#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/AudioGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/LaunchGlobals.h"
#include "Misc/UIGlobals.h"
#include "Render/EditorLevelViewportClient.h"
#include "Render/Scene.h"
#include "Render/SceneRendering.h"
#include "Render/RenderingThread.h"
#include "Render/SceneUtils.h"
#include "Render/SceneRenderTargets.h"
#include "Render/RenderUtils.h"
#include "System/AudioDevice.h"
#include "System/EditorEngine.h"
#include "System/InputSystem.h"
#include "System/World.h"
#include "System/BaseWindow.h"
#include "ImGUI/ImGUIEngine.h"
#include "Actors/Actor.h"
#include "EngineLoop.h"
#include "EngineDefines.h"
#include "LEBuild.h"

/**
 * @ingroup WorldEd
 * Macro of camera zoom dampen
 */
#define CAMERA_ZOOM_DAMPEN			200.f

/**
 * @ingroup WorldEd
 * Macro of camera zoom divide
 */
#define CAMERA_ZOOM_DIV				15000.0f

/**
 * @ingroup WorldEd
 * Macro of min camera speed
 */
#define MIN_CAMERA_SPEED			0.5f

/**
 * @ingroup WorldEd
 * Macro of max camera speed
 */
#define MAX_CAMERA_SPEED			1000.f

/** Default camera position for level editor perspective viewports */
static const Vector				s_DefaultPerspectiveViewLocation( 495.166962, 167.584518, -400.f );

/** Default camera orientation for level editor perspective viewports */
static const Quaternion			s_DefaultPerspectiveViewRotationQuat( SMath::quaternionZero );
static const Vector				s_DefaultPerspectiveViewRotationEuler( SMath::vectorZero );

/** Show flags for each viewport type */
static const ShowFlags_t		s_ShowFlags[ LVT_Max ] =
{
#if !ENGINE_2D
	SHOW_DefaultEditor | SHOW_Wireframe,		// LVT_OrthoXY
#else 
	SHOW_DefaultEditor,							// LVT_OrthoXY
#endif // !ENGINE_2D

	SHOW_DefaultEditor | SHOW_Wireframe,		// LVT_OrthoXZ
	SHOW_DefaultEditor | SHOW_Wireframe,		// LVT_OrthoYZ
	SHOW_DefaultEditor							// LVT_Perspective
};

/*
==================
CEditorLevelViewportClient::CEditorLevelViewportClient
==================
*/
CEditorLevelViewportClient::CEditorLevelViewportClient( ELevelViewportType InViewportType /* = LVT_Perspective */ )
	: bSetListenerPosition( true )
	, trackingType( CEditorLevelViewportClient::MT_None )
	, bIgnoreInput( false )
	, bAllowContextMenu( true )
	, viewportType( InViewportType )
	, viewLocation( SMath::vectorZero )
	, viewRotationEuler( SMath::vectorZero )
	, viewRotationQuat( SMath::quaternionZero )
	, viewFOV( 90.f )
	, orthoZoom( 10000.f )
	, cameraSpeed( MIN_CAMERA_SPEED )
	, showFlags( SHOW_DefaultEditor )
	, cameraMoveFlags( 0x0 )
{
	SetViewportType( InViewportType );
}

/*
==================
CEditorLevelViewportClient::Tick
==================
*/
void CEditorLevelViewportClient::Tick( float InDeltaSeconds )
{
	// If we tracking mouse and this is perspective viewport - change view location
	if ( trackingType == MT_View && viewportType == LVT_Perspective )
	{
		Vector		targetDirection = viewRotationQuat * SMath::vectorForward;
		Vector		axisUp			= viewRotationQuat * SMath::vectorUp;
		Vector		axisRight		= SMath::CrossVector( targetDirection, axisUp );

		if ( cameraMoveFlags & CMV_MoveForward )		viewLocation +=	targetDirection	* cameraSpeed;
		if ( cameraMoveFlags & CMV_MoveBackward )		viewLocation +=	targetDirection	* -cameraSpeed;
		if ( cameraMoveFlags & CMV_MoveLeft )			viewLocation +=	axisRight		* -cameraSpeed;
		if ( cameraMoveFlags & CMV_MoveRight )			viewLocation +=	axisRight		* cameraSpeed;
	}
}

/*
==================
CEditorLevelViewportClient::Draw
==================
*/
void CEditorLevelViewportClient::Draw( CViewport* InViewport )
{
	Assert( InViewport );
	CSceneView*		sceneView = CalcSceneView( InViewport->GetSizeX(), InViewport->GetSizeY() );

	// Update audio listener spatial if allowed
	if ( viewportType == LVT_Perspective && bSetListenerPosition )
	{
		g_AudioDevice.SetListenerSpatial( viewLocation, viewRotationQuat * SMath::vectorForward, viewRotationQuat * SMath::vectorUp );
	}

	// Draw viewport
	UNIQUE_RENDER_COMMAND_THREEPARAMETER( CViewportRenderCommand,
										  CEditorLevelViewportClient*, viewportClient, this,
										  ViewportRHIRef_t, viewportRHI, InViewport->GetViewportRHI(),
										  CSceneView*, sceneView, sceneView,
										  {
											  viewportClient->Draw_RenderThread( viewportRHI, sceneView );
										  } );
}

/*
==================
CEditorLevelViewportClient::Draw_RenderThread
==================
*/
void CEditorLevelViewportClient::Draw_RenderThread( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView )
{
	Assert( IsInRenderingThread() );
	CBaseDeviceContextRHI*		immediateContext = g_RHI->GetImmediateContext();
	CScene*						scene = ( CScene* )g_World->GetScene();
	CSceneRenderer				sceneRenderer( InSceneView, scene );
	sceneRenderer.BeginRenderViewTarget( InViewportRHI );
	
	// Draw grid
	drawHelper.DrawGrid( InSceneView, viewportType, scene );

	// Draw scene
	sceneRenderer.Render( InViewportRHI );

	// Finishing render and delete scene view
	sceneRenderer.FinishRenderViewTarget( InViewportRHI );
	delete InSceneView;
}

#if ENABLE_HITPROXY
/*
==================
CEditorLevelViewportClient::DrawHitProxies
==================
*/
void CEditorLevelViewportClient::DrawHitProxies( CViewport* InViewport, EHitProxyLayer InHitProxyLayer /* = HPL_World */ )
{ 
	Assert( InViewport );
	CSceneView*		sceneView = CalcSceneView( InViewport->GetSizeX(), InViewport->GetSizeY() );

	// Draw viewport
	UNIQUE_RENDER_COMMAND_FOURPARAMETER( CViewportRenderCommand,
										 CEditorLevelViewportClient*, viewportClient, this,
										 ViewportRHIRef_t, viewportRHI, InViewport->GetViewportRHI(),
										 CSceneView*, sceneView, sceneView,
										 EHitProxyLayer, hitProxyLayer, InHitProxyLayer,
										 {
											 viewportClient->DrawHitProxies_RenderThread( viewportRHI, sceneView, hitProxyLayer );
										 } );
}

/*
==================
CEditorLevelViewportClient::DrawHitProxies_RenderThread
==================
*/
void CEditorLevelViewportClient::DrawHitProxies_RenderThread( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView, EHitProxyLayer InHitProxyLayer /* = HPL_World */ )
{
	Assert( IsInRenderingThread() );
	CBaseDeviceContextRHI*		immediateContext = g_RHI->GetImmediateContext();
	CScene*						scene = ( CScene* )g_World->GetScene();
	CSceneRenderer				sceneRenderer( InSceneView, scene );

	// Draw hit proxies
	g_RHI->SetViewport( immediateContext, 0, 0, 0.f, InViewportRHI->GetWidth(), InViewportRHI->GetHeight(), 1.f );
	sceneRenderer.BeginRenderHitProxiesViewTarget( InViewportRHI );

	// Draw scene
	sceneRenderer.RenderHitProxies( InViewportRHI, InHitProxyLayer );
	
	sceneRenderer.FinishRenderHitProxiesViewTarget( InViewportRHI );
	delete InSceneView;
}

/*
==================
CEditorLevelViewportClient::GetHitProxyId
==================
*/
CHitProxyId CEditorLevelViewportClient::GetHitProxyId( uint32 InX, uint32 InY ) const
{
	CHitProxyId		result;
	UNIQUE_RENDER_COMMAND_THREEPARAMETER( CGetHitProxyIdCommand,
										  CHitProxyId&, hitProxyId, result,
										  uint32, x, InX,
										  uint32, y, InY,
										  {
											  // Getting hit proxy texture and lock him for read
											  CBaseDeviceContextRHI*	deviceContext	= g_RHI->GetImmediateContext();
											  Texture2DRHIRef_t			hitProxyTexture = g_SceneRenderTargets.GetHitProxyTexture();
											  SLockedData				lockedData;
											  g_RHI->LockTexture2D( deviceContext, hitProxyTexture, 0, false, lockedData );

											  // Getting pointer to we interested data in texture
											  CColor*		data = ( CColor* )lockedData.data + x + y * ( lockedData.pitch / g_PixelFormats[ hitProxyTexture->GetFormat() ].blockBytes );

											  // If color is not black (black is color of clear texture), then hit proxy is valid
											  if ( *data != CColor::black )
											  {
												  hitProxyId.SetIndex( *data );
											  }

											  // Unlock texture
											  g_RHI->UnlockTexture2D( deviceContext, hitProxyTexture, 0, lockedData );
										  } );

	FlushRenderingCommands();
	return result;
}
#endif // ENABLE_HITPROXY

/*
==================
CEditorLevelViewportClient::SetViewportType
==================
*/
void CEditorLevelViewportClient::SetViewportType( ELevelViewportType InViewportType )
{
	bSetListenerPosition	= false;
	viewportType			= InViewportType;
	showFlags				= s_ShowFlags[ InViewportType ];

	// Init for perspective mode default location and rotation
	if ( viewportType == LVT_Perspective )
	{
		bSetListenerPosition = true;
		viewLocation		= s_DefaultPerspectiveViewLocation;
		viewRotationEuler	= s_DefaultPerspectiveViewRotationEuler;
		viewRotationQuat	= s_DefaultPerspectiveViewRotationQuat;
	}
	// Else we init default location of axis up for other viewport types
	else
	{
		switch ( viewportType )
		{
			// In XZ case Y is up
		case LVT_OrthoXZ:
			viewLocation.y = HALF_WORLD_MAX1/2;
			break;

			// In XY case Z is up
		case LVT_OrthoXY:
			viewLocation.z = -HALF_WORLD_MAX1/2;
			break;

			// In YZ case X is up
		case LVT_OrthoYZ:
			viewLocation.x = -HALF_WORLD_MAX1/2;
			break;
		}
	}
}

/*
==================
CEditorLevelViewportClient::ScreenToWorld
==================
*/
Vector CEditorLevelViewportClient::ScreenToWorld( const Vector2D& InScreenPoint, uint32 InViewportSizeX, uint32 InViewportSizeY )
{
	// Convert screen point to screen space
	Vector2D		screenSpace = InScreenPoint;

	// For non perspective viewport need apply zoom factor
	if ( viewportType != LVT_Perspective )
	{
		const float		zoom = orthoZoom / ( InViewportSizeX * 15.0f );
		screenSpace.x	= zoom * ( screenSpace.x - InViewportSizeX / 2.f );
		screenSpace.y	= zoom * ( screenSpace.y - InViewportSizeY / 2.f );
	}

	// Convert screen to world space for each viewport type
	switch ( viewportType )
	{
		// For perspective type we use CSceneView::ScreenToWorld (use inverted matrices)
	case LVT_Perspective:
	{
		Vector		worldOrigin;
		Vector		worldDirection;

		CSceneView*		sceneView = CalcSceneView( InViewportSizeX, InViewportSizeY );
		sceneView->ScreenToWorld( screenSpace, worldOrigin, worldDirection );
		delete sceneView;

		float	t		= -worldOrigin.y / worldDirection.y;
		float	worldX	= worldOrigin.x + worldDirection.x * t;
		float	worldZ	= worldOrigin.z + worldDirection.z * t;
		return Vector( worldX, 0.f, worldZ );
	}

		// In XZ case Y is up
	case LVT_OrthoXZ:		return Vector( viewLocation.x - screenSpace.x, 0.f, viewLocation.z - screenSpace.y );

		// In XY case Z is up
	case LVT_OrthoXY:		return Vector( viewLocation.x - screenSpace.x, viewLocation.y - screenSpace.y, 0.f );
		
		// In YZ case X is up
	case LVT_OrthoYZ:		return Vector( 0.f, viewLocation.y - screenSpace.y, viewLocation.z - screenSpace.y );

		// In default case we return empty vector
	default:
		return SMath::vectorZero;
	}
}

/*
==================
CEditorLevelViewportClient::WorldToScreen
==================
*/
Vector CEditorLevelViewportClient::WorldToScreen( const Vector& InWorldPoint, uint32 InViewportSizeX, uint32 InViewportSizeY )
{
	CSceneView*		sceneView = CalcSceneView( InViewportSizeX, InViewportSizeY );
	Vector			result = sceneView->WorldToScreen( InWorldPoint );
	delete sceneView;
	return result;
}

/*
==================
CEditorLevelViewportClient::ProcessEvent
==================
*/
void CEditorLevelViewportClient::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	// We ignore events when bIgnoreInput is TRUE or when ImGUI layer is unhovered
	bool						bIsNotUnhoveredEvent = InWindowEvent.type != SWindowEvent::T_ImGUILayerUnHovered;
	TSharedPtr<CImGUILayer>		imGUILayer = InWindowEvent.imGUILayer.Pin();
	if ( bIgnoreInput || ( bIsNotUnhoveredEvent && imGUILayer && !imGUILayer->IsHovered() ) )
	{
		return;
	}

	switch ( InWindowEvent.type )
	{
		// Event of unhovered ImGUI layer
	case SWindowEvent::T_ImGUILayerUnHovered:
		// Show cursor
		if ( InWindowEvent.imGUILayer )
		{
			g_ImGUIEngine->SetShowCursor( true );
		}
		else
		{
			g_Window->ShowCursor();
		}

		trackingType	= MT_None;
		cameraMoveFlags = 0x0;
		break;

		// Event of mouse pressed
	case SWindowEvent::T_MousePressed:
		bAllowContextMenu = true;

		if ( InWindowEvent.events.mouseButton.code == BC_MouseRight )
		{
			// Hide cursor
			if ( InWindowEvent.imGUILayer )
			{
				g_ImGUIEngine->SetShowCursor( false );
			}
			else
			{
				g_Window->HideCursor();
			}

			trackingType = MT_View;
		}
		break;

		// Event of mouse released
	case SWindowEvent::T_MouseReleased:
		if ( trackingType == MT_View && InWindowEvent.events.mouseButton.code == BC_MouseRight )
		{
			// Show cursor
			if ( InWindowEvent.imGUILayer )
			{
				g_ImGUIEngine->SetShowCursor( true );
			}
			else
			{
				g_Window->ShowCursor();
			}

			trackingType		= MT_None;
			cameraMoveFlags		= 0x0;
		}
		break;

		// Event of mouse wheel moved
	case SWindowEvent::T_MouseWheel:
	{
		// For ortho viewports we change zoom
		if ( viewportType != LVT_Perspective )
		{
			// Scrolling the mousewheel up/down zooms the orthogonal viewport in/out
			int32		delta = 25;
			if ( InWindowEvent.events.mouseWheel.y > 0.f )
			{
				delta *= -1.f;
			}

			orthoZoom += orthoZoom / CAMERA_ZOOM_DAMPEN * delta;
			orthoZoom = Clamp<float>( orthoZoom, MIN_ORTHOZOOM, MAX_ORTHOZOOM );
		}

		// For perspective viewports we change camera speed
		else
		{
			cameraSpeed += 1.f * ( InWindowEvent.events.mouseWheel.y > 0.f ? 1.f : -1.f );
			cameraSpeed = Clamp( cameraSpeed, MIN_CAMERA_SPEED, MAX_CAMERA_SPEED );
		}
		break;
	}

		// Event of mouse move
	case SWindowEvent::T_MouseMove:
	{
		Vector2D		moveDelta = Vector2D( InWindowEvent.events.mouseMove.xDirection, InWindowEvent.events.mouseMove.yDirection );

		// Update view if we tracking mouse for MT_View
		if ( trackingType == MT_View )
		{
			if ( viewportType != LVT_Perspective )
			{
				moveDelta.x		= InWindowEvent.events.mouseMove.xDirection * ( orthoZoom / CAMERA_ZOOM_DIV );
				moveDelta.y		= InWindowEvent.events.mouseMove.yDirection * ( orthoZoom / CAMERA_ZOOM_DIV );
			}
			else
			{
				moveDelta.x		= InWindowEvent.events.mouseMove.xDirection * g_InputSystem->GetMouseSensitivity();
				moveDelta.y		= InWindowEvent.events.mouseMove.yDirection * g_InputSystem->GetMouseSensitivity();
			}

			// We moved mouse when press right mouse button, in this case we not allow opening context menu after release
			bAllowContextMenu = false;

			// For ortho viewports its move
			if ( viewportType != LVT_Perspective )
			{
				float*		dX = nullptr;
				float*		dY = nullptr;
				switch ( viewportType )
				{
				case LVT_OrthoXY:
					dX = &viewLocation.x;
					dY = &viewLocation.y;
					break;

				case LVT_OrthoXZ:
					dX = &viewLocation.x;
					dY = &viewLocation.z;
					break;

				case LVT_OrthoYZ:
					moveDelta.x = -moveDelta.x;
					dX = &viewLocation.z;
					dY = &viewLocation.y;
					break;

				default:
					Assert( false );		// Not supported viewport type
					break;
				}

				*dX	= Clamp<float>( *dX + moveDelta.x, -HALF_WORLD_MAX1, HALF_WORLD_MAX1 );
				*dY	= Clamp<float>( *dY + moveDelta.y, -HALF_WORLD_MAX1, HALF_WORLD_MAX1 );
			}

			// For perspective viewports its rotate camera
			else
			{
				// Update Yaw axis
				if ( moveDelta.x != 0.f )
				{
					viewRotationEuler.y -= moveDelta.x;
					if ( viewRotationEuler.y < -360.f || viewRotationEuler.y > 360.f )
					{
						viewRotationEuler.y = 0.f;
					}
				}

				// Update Pitch axis
				if ( moveDelta.y != 0.f )
				{
					viewRotationEuler.x += moveDelta.y;
					if ( viewRotationEuler.x > 90.f )
					{
						viewRotationEuler.x = 90.f;
					} 
					else if ( viewRotationEuler.x < -90.f )
					{
						viewRotationEuler.x = -90.f;
					}

				}

				viewRotationQuat = SMath::AnglesToQuaternion( viewRotationEuler );
			}
		}
		break;
	}

		// Event of key press
	case SWindowEvent::T_KeyPressed:
		if ( trackingType == MT_View && viewportType == LVT_Perspective )
		{
			switch ( InWindowEvent.events.key.code )
			{
			case BC_KeyW:		cameraMoveFlags |= CMV_MoveForward;			break;
			case BC_KeyS:		cameraMoveFlags |= CMV_MoveBackward;		break;
			case BC_KeyA:		cameraMoveFlags |= CMV_MoveLeft;			break;
			case BC_KeyD:		cameraMoveFlags |= CMV_MoveRight;			break;
			}
		}
		break;

		// Event of release key
	case SWindowEvent::T_KeyReleased:
		if ( trackingType == MT_View && viewportType == LVT_Perspective )
		{
			switch ( InWindowEvent.events.key.code )
			{
			case BC_KeyW:		cameraMoveFlags &= ~CMV_MoveForward;		break;
			case BC_KeyS:		cameraMoveFlags &= ~CMV_MoveBackward;		break;
			case BC_KeyA:		cameraMoveFlags &= ~CMV_MoveLeft;			break;
			case BC_KeyD:		cameraMoveFlags &= ~CMV_MoveRight;			break;
			}
		}
		break;
	}
}

/*
==================
CEditorLevelViewportClient::CalcSceneView
==================
*/
CSceneView* CEditorLevelViewportClient::CalcSceneView( uint32 InSizeX, uint32 InSizeY )
{
	// Calculate projection matrix
	Matrix		projectionMatrix;
	if ( viewportType == LVT_Perspective )
	{
		projectionMatrix = glm::perspective( SMath::DegreesToRadians( viewFOV ), ( float )InSizeX / InSizeY, 0.01f, ( float )HALF_WORLD_MAX );
	}
	else
	{
		const float		zoom		= orthoZoom / ( InSizeX * 15.0f );
		float			halfWidth	= zoom * InSizeX / 2.f;
		float			halfHeight	= zoom * InSizeY / 2.f;
		projectionMatrix = glm::ortho( -halfWidth, halfWidth, -halfHeight, halfHeight, ( float )-HALF_WORLD_MAX, ( float )HALF_WORLD_MAX );
	}

	// Calculate view matrix
	Matrix		viewMatrix;
	Vector		targetDirection;
	Vector		axisUp;
	switch ( viewportType )
	{
	case LVT_Perspective:
	case LVT_OrthoXY:
		targetDirection		= viewRotationQuat * SMath::vectorForward;
		axisUp				= viewRotationQuat * SMath::vectorUp;
		break;

	case LVT_OrthoXZ:
		targetDirection		= viewRotationQuat * ( -SMath::vectorUp );
		axisUp				= viewRotationQuat * SMath::vectorForward;
		break;

	case LVT_OrthoYZ:
		targetDirection		= viewRotationQuat * SMath::vectorRight;
		axisUp				= viewRotationQuat * SMath::vectorUp;
		break;

	default:
		Assert( false );		// Unknown viewport type
		break;
	}

	viewMatrix = glm::lookAt( viewLocation, viewLocation + targetDirection, axisUp );

	CSceneView*		sceneView = new CSceneView( viewLocation, projectionMatrix, viewMatrix, InSizeX, InSizeY, GetBackgroundColor(), showFlags );
	return sceneView;
}

/*
==================
CEditorLevelViewportClient::GetBackgroundColor
==================
*/
CColor CEditorLevelViewportClient::GetBackgroundColor() const
{
	if ( viewportType == LVT_Perspective 
#if ENGINE_2D
		 || viewportType == LVT_OrthoXY
#endif // ENGINE_2D
		 )
	{
		return CColor::black;
	}
	else
	{
		return CColor( 26, 27, 28 );
	}
}