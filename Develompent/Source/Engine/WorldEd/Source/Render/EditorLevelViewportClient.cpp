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
#include "System/Gizmo.h"
#include "System/BaseWindow.h"
#include "ImGUI/ImGUIEngine.h"
#include "Actors/Actor.h"
#include "EngineLoop.h"
#include "EngineDefines.h"

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
static const Vector			GDefaultPerspectiveViewLocation( 495.166962, 167.584518, -400.f );

/** Default camera orientation for level editor perspective viewports */
static const Quaternion		GDefaultPerspectiveViewRotationQuat( SMath::quaternionZero );
static const Vector			GDefaultPerspectiveViewRotationEuler( SMath::vectorZero );

/** Show flags for each viewport type */
static const ShowFlags_t		GShowFlags[ LVT_Max ] =
{
	SHOW_DefaultEditor | SHOW_Wireframe,		// LVT_OrthoXY
	SHOW_DefaultEditor | SHOW_Wireframe,		// LVT_OrthoXZ
	SHOW_DefaultEditor | SHOW_Wireframe,		// LVT_OrthoYZ
	SHOW_DefaultEditor							// LVT_Perspective
};

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
	, gizmo( nullptr )
{
	SetViewportType( InViewportType );
}

void CEditorLevelViewportClient::Tick( float InDeltaSeconds )
{
	// If we tracking mouse and this is perspective viewport - change view location
	if ( trackingType == MT_View && viewportType == LVT_Perspective )
	{
		Vector		targetDirection = SMath::vectorForward	* viewRotationQuat;
		Vector		axisUp			= SMath::vectorUp		* viewRotationQuat;
		Vector		axisRight		= SMath::CrossVector( targetDirection, axisUp );

		if ( cameraMoveFlags & CMV_MoveForward )		viewLocation +=	targetDirection	* cameraSpeed;
		if ( cameraMoveFlags & CMV_MoveBackward )		viewLocation +=	targetDirection	* -cameraSpeed;
		if ( cameraMoveFlags & CMV_MoveLeft )			viewLocation +=	axisRight		* -cameraSpeed;
		if ( cameraMoveFlags & CMV_MoveRight )			viewLocation +=	axisRight		* cameraSpeed;
	}
}

void CEditorLevelViewportClient::Draw( CViewport* InViewport )
{
	check( InViewport );
	CSceneView*		sceneView = CalcSceneView( InViewport->GetSizeX(), InViewport->GetSizeY() );

	// Update audio listener spatial if allowed
	if ( viewportType == LVT_Perspective && bSetListenerPosition )
	{
		GAudioDevice.SetListenerSpatial( viewLocation, SMath::vectorForward * viewRotationQuat, SMath::vectorUp * viewRotationQuat );
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

void CEditorLevelViewportClient::Draw_RenderThread( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView )
{
	check( IsInRenderingThread() );
	CBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	CScene*						scene = ( CScene* )GWorld->GetScene();
	CSceneRenderer				sceneRenderer( InSceneView, scene );
	sceneRenderer.BeginRenderViewTarget( InViewportRHI );
	
	// Draw grid
	drawHelper.DrawGrid( InSceneView, viewportType, scene );
	
	// Draw gizmo
	if ( gizmo && gizmo->IsEnabled() )
	{
		gizmo->Draw_RenderThread( InViewportRHI, InSceneView, scene, viewportType );
	}

	// Draw scene
	sceneRenderer.Render( InViewportRHI );

	// Finishing render and delete scene view
	sceneRenderer.FinishRenderViewTarget( InViewportRHI );
	delete InSceneView;
}

#if ENABLE_HITPROXY
void CEditorLevelViewportClient::DrawHitProxies( CViewport* InViewport, EHitProxyLayer InHitProxyLayer /* = HPL_World */ )
{ 
	check( InViewport );
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

void CEditorLevelViewportClient::DrawHitProxies_RenderThread( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView, EHitProxyLayer InHitProxyLayer /* = HPL_World */ )
{
	check( IsInRenderingThread() );
	CBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	CScene*						scene = ( CScene* )GWorld->GetScene();
	CSceneRenderer				sceneRenderer( InSceneView, scene );

	// Draw hit proxies
	GRHI->SetViewport( immediateContext, 0, 0, 0.f, InViewportRHI->GetWidth(), InViewportRHI->GetHeight(), 1.f );
	sceneRenderer.BeginRenderHitProxiesViewTarget( InViewportRHI );

	// Draw gizmo
	if ( gizmo && gizmo->IsEnabled() )
	{
		gizmo->Draw_RenderThread( InViewportRHI, InSceneView, scene, viewportType );
	}

	// Draw scene
	sceneRenderer.RenderHitProxies( InViewportRHI, InHitProxyLayer );
	
	sceneRenderer.FinishRenderHitProxiesViewTarget( InViewportRHI );
	delete InSceneView;
}

CHitProxyId CEditorLevelViewportClient::GetHitProxyId( uint32 InX, uint32 InY ) const
{
	CHitProxyId		result;
	UNIQUE_RENDER_COMMAND_THREEPARAMETER( CGetHitProxyIdCommand,
										  CHitProxyId&, hitProxyId, result,
										  uint32, x, InX,
										  uint32, y, InY,
										  {
											  // Getting hit proxy texture and lock him for read
											  CBaseDeviceContextRHI*	deviceContext	= GRHI->GetImmediateContext();
											  Texture2DRHIRef_t			hitProxyTexture = GSceneRenderTargets.GetHitProxyTexture();
											  SLockedData				lockedData;
											  GRHI->LockTexture2D( deviceContext, hitProxyTexture, 0, false, lockedData );

											  // Getting pointer to we interested data in texture
											  CColor*		data = ( CColor* )lockedData.data + x + y * ( lockedData.pitch / GPixelFormats[ hitProxyTexture->GetFormat() ].blockBytes );

											  // If color is not black (black is color of clear texture), then hit proxy is valid
											  if ( *data != CColor::black )
											  {
												  hitProxyId.SetIndex( *data );
											  }

											  // Unlock texture
											  GRHI->UnlockTexture2D( deviceContext, hitProxyTexture, 0, lockedData );
										  } );

	FlushRenderingCommands();
	return result;
}
#endif // ENABLE_HITPROXY

void CEditorLevelViewportClient::SetViewportType( ELevelViewportType InViewportType )
{
	bSetListenerPosition	= false;
	viewportType			= InViewportType;
	showFlags				= GShowFlags[ InViewportType ];

	// Init for perspective mode default location and rotation
	if ( viewportType == LVT_Perspective )
	{
		bSetListenerPosition = true;
		viewLocation		= GDefaultPerspectiveViewLocation;
		viewRotationEuler	= GDefaultPerspectiveViewRotationEuler;
		viewRotationQuat	= GDefaultPerspectiveViewRotationQuat;
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

Vector CEditorLevelViewportClient::WorldToScreen( const Vector& InWorldPoint, uint32 InViewportSizeX, uint32 InViewportSizeY )
{
	CSceneView*		sceneView = CalcSceneView( InViewportSizeX, InViewportSizeY );
	Vector			result = sceneView->WorldToScreen( InWorldPoint );
	delete sceneView;
	return result;
}

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
			GImGUIEngine->SetShowCursor( true );
		}
		else
		{
			GWindow->ShowCursor();
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
				GImGUIEngine->SetShowCursor( false );
			}
			else
			{
				GWindow->HideCursor();
			}

			trackingType = MT_View;
		}
		else if ( gizmo && InWindowEvent.events.mouseButton.code == BC_MouseLeft && gizmo->IsEnabled() && gizmo->GetCurrentAxis() > A_None )
		{
			trackingType = MT_Gizmo;
		}
		break;

		// Event of mouse released
	case SWindowEvent::T_MouseReleased:
		if ( trackingType == MT_View && InWindowEvent.events.mouseButton.code == BC_MouseRight )
		{
			// Show cursor
			if ( InWindowEvent.imGUILayer )
			{
				GImGUIEngine->SetShowCursor( true );
			}
			else
			{
				GWindow->ShowCursor();
			}

			trackingType		= MT_None;
			cameraMoveFlags		= 0x0;
		}
		else if ( trackingType == MT_Gizmo && InWindowEvent.events.mouseButton.code == BC_MouseLeft )
		{
			trackingType = MT_None;
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
			if ( InWindowEvent.events.mouseWheel.y < 0.f )
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
				moveDelta.x		= InWindowEvent.events.mouseMove.xDirection * GInputSystem->GetMouseSensitivity();
				moveDelta.y		= InWindowEvent.events.mouseMove.yDirection * GInputSystem->GetMouseSensitivity();
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
					check( false );		// Not supported viewport type
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
					viewRotationEuler.y += moveDelta.x;
					if ( viewRotationEuler.y < -360.f || viewRotationEuler.y > 360.f )
					{
						viewRotationEuler.y = 0.f;
					}
				}

				// Update Pitch axis
				if ( moveDelta.y != 0.f )
				{
					viewRotationEuler.x -= moveDelta.y;
					if ( viewRotationEuler.x > 90.f )
					{
						viewRotationEuler.x = 90.f;
					} 
					else if ( viewRotationEuler.x < -90.f )
					{
						viewRotationEuler.x = -90.f;
					}

				}

				viewRotationQuat = SMath::AnglesToQuaternionZXY( viewRotationEuler );
			}
		}
		
		// Apply transformation for object if we traking mouse for MT_Gizmo
		else if ( gizmo && trackingType == MT_Gizmo )
		{
			Vector		drag;
			Quaternion	rotator;
			Vector		scale;

			for ( uint32 axis = 0; axis < 2; ++axis )
			{
				Vector2D	delta( 0.f, 0.f );
				delta[ axis ] = moveDelta[ axis ];
				if ( delta.x != 0.f || delta.y != 0.f )
				{
					// Convert movement mouse to drag/rotate/scale
					ConvertMovementDeltaToDragRot( delta, drag, rotator, scale );

					// Apply transform to actors
					const std::vector<ActorRef_t>&	selectedActors = GWorld->GetSelectedActors();
					switch ( gizmo->GetType() )
					{
					// Translate gizmo
					case GT_Translate:
						for ( uint32 index = 0, count = selectedActors.size(); index < count; ++index )
						{
							ActorRef_t		actor = selectedActors[ index ];
							actor->AddActorLocation( drag );
						}

						CGizmo::OnUpdateAllGizmo().Broadcast( gizmo->IsEnabled(), gizmo->GetLocation() + drag );
						break;

					// Rotate gizmo
					case GT_Rotate:
						for ( uint32 index = 0, count = selectedActors.size(); index < count; ++index )
						{
							ActorRef_t		actor = selectedActors[ index ];
							actor->AddActorRotation( rotator );
						}
						break;

					// Scale gizmo
					case GT_Scale:
					case GT_ScaleNonUniform:
						for ( uint32 index = 0, count = selectedActors.size(); index < count; ++index )
						{
							ActorRef_t		actor = selectedActors[ index ];
							actor->AddActorScale( scale );
						}
						break;
					}
				}
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

void CEditorLevelViewportClient::ConvertMovementDeltaToDragRot( const Vector2D& InDragDelta, Vector& OutDrag, Quaternion& OutRotation, Vector& OutScale, class CSceneView* InSceneView /* = nullptr */ )
{
	if ( !gizmo )
	{
		return;
	}

	Vector2D	axisEnd;
	uint32		currentAxis = gizmo->GetCurrentAxis();
	Vector2D	dragDelta	= InDragDelta;

	OutDrag		= SMath::vectorZero;
	OutRotation = SMath::quaternionZero;
	OutScale	= SMath::vectorOne;

	// We apply an increase in offset depending on the distance of the camera to gizmo point (for viewport type LVT_Perspective) 
	if ( gizmo->GetType() == GT_Translate )
	{
		if ( viewportType == LVT_Perspective )
		{
			dragDelta *= SMath::DistanceVector( viewLocation, gizmo->GetLocation() ) / viewFOV / 4.f;
		}
		else
		{
			dragDelta *= orthoZoom / CAMERA_ZOOM_DIV;
		}
	}

	// Screen space Y axis is inverted
	dragDelta.y *= -1.f;

	// To move objects in perspective on a plane, we consider offsets relative to the camera
	if ( viewportType == LVT_Perspective && gizmo->GetType() == GT_Translate )
	{
		switch ( currentAxis )
		{
		case A_X | A_Y:
		case A_X | A_Z:
		case A_Y | A_Z:
		{
			CViewport*		viewport = GetViewport();
			if ( viewport )
			{
				CSceneView*	sceneView		= CalcSceneView( viewport->GetSizeX(), viewport->GetSizeY() );
				Matrix		cameraToWorld	= SMath::InverseMatrix( sceneView->GetViewMatrix() );
				OutDrag		= cameraToWorld * Vector4D( dragDelta.x, dragDelta.y, 0.f, 0.f );
				
				switch ( currentAxis )
				{
				case A_X | A_Y:		OutDrag.z = 0.f;	break;
				case A_X | A_Z:		OutDrag.y = 0.f;	break;
				case A_Y | A_Z:		OutDrag.x = 0.f;	break;
				}

				delete sceneView;
			}
			return;
		}
		}
	}

	// Get the end of the axis (in screen space) based on which axis is being pulled
	switch ( currentAxis )
	{
	case A_X:					axisEnd = gizmo->GetAxisXEnd();												break;
	case A_Y:					axisEnd = gizmo->GetAxisYEnd();												break;
	case A_Z:					axisEnd = gizmo->GetAxisZEnd();												break;
	case A_X | A_Y:				axisEnd = dragDelta.x != 0 ? gizmo->GetAxisXEnd() : gizmo->GetAxisYEnd();	break;
	case A_X | A_Z:				axisEnd = dragDelta.x != 0 ? gizmo->GetAxisXEnd() : gizmo->GetAxisZEnd();	break;
	case A_Y | A_Z:				axisEnd = dragDelta.x != 0 ? gizmo->GetAxisYEnd() : gizmo->GetAxisZEnd();	break;
	case A_X | A_Y | A_Z:		axisEnd = dragDelta.x != 0 ? gizmo->GetAxisYEnd() : gizmo->GetAxisZEnd();	break;
	default:																								break;
	}

	// Get the directions of the axis (on the screen)
	Vector2D	axisDir = axisEnd - gizmo->GetScreenLocation();

	// Use the most dominant axis the mouse is being dragged along
	int32		idx = 0;
	if ( SMath::Abs( dragDelta.x ) < SMath::Abs( dragDelta.y ) )
	{
		idx = 1;
	}
	float		value = dragDelta[ idx ];

	// If the axis dir is negative, it is pointing in the negative screen direction.  In this situation, the mouse
	// drag must be inverted so that you are still dragging in the right logical direction.
	//
	// For example, if the X axis is pointing left and you drag left, this will ensure that the widget moves left.
	// Only valid for single axis movement.  For planar movement, this widget gets caught up at the origin and oscillates
	if ( axisDir[ idx ] < 0.f && ( currentAxis == A_X || currentAxis == A_Y || currentAxis == A_Z ) )
	{
		value *= -1.f;
	}

	switch ( gizmo->GetType() )
	{
	// Translate gizmo
	case GT_Translate:
		switch ( currentAxis )
		{
		case A_X:			OutDrag = Vector( value, 0.f, 0.f );													break;
		case A_Y:			OutDrag = Vector( 0.f, value, 0.f );													break;
		case A_Z:			OutDrag = Vector( 0.f, 0.f, value );													break;
		case A_X | A_Y:		OutDrag = dragDelta.x != 0 ? Vector( -value, 0.f, 0.f ) : Vector( 0.f, value, 0.f );	break;
		case A_X | A_Z:		OutDrag = dragDelta.x != 0 ? Vector( -value, 0.f, 0.f ) : Vector( 0.f, 0.f, value );	break;
		case A_Y | A_Z:		OutDrag = dragDelta.x != 0 ? Vector( 0.f, 0.f, value ) : Vector( 0.f, value, 0.f );		break;
		}
		break;

	// Rotate gizmo
	case GT_Rotate:
		switch ( currentAxis )
		{
		case A_X:		OutRotation = SMath::AnglesToQuaternionXYZ( Vector( value, 0.f, 0.f ) );	break;
		case A_Y:		OutRotation = SMath::AnglesToQuaternionXYZ( Vector( 0.f, value, 0.f ) );	break;
		case A_Z:		OutRotation = SMath::AnglesToQuaternionXYZ( Vector( 0.f, 0.f, value ) );	break;
		default:		checkMsg( false, TEXT( "Axis not correctly set while rotating!" ) );		break;
		}
		break;

	// Scale gizmo
	case GT_Scale:
		OutScale = Vector( 0.005f, 0.005f, 0.005f ) * value;
		break;

	case GT_ScaleNonUniform:
		switch ( currentAxis )
		{
		case A_X:				OutScale = Vector( 0.005f * value, 0.f, 0.f );																break;
		case A_Y:				OutScale = Vector( 0.f, 0.005f * value, 0.f );																break;
		case A_Z:				OutScale = Vector( 0.f, 0.f, 0.005f * value );																break;
		case A_X | A_Y:			OutScale = dragDelta.x != 0 ? Vector( 0.005f * value, 0.f, 0.f )	: Vector( 0.f, 0.005f * value, 0.f );	break;
		case A_X | A_Z:			OutScale = dragDelta.x != 0 ? Vector( 0.005f * value, 0.f, 0.f )	: Vector( 0.f, 0.f, -0.005f * value );	break;
		case A_Y | A_Z:			OutScale = dragDelta.x != 0 ? Vector( 0.f, 0.f, -0.005f * value )	: Vector( 0.f, 0.005f * value, 0.f );	break;														break;
		}
	}
}

CSceneView* CEditorLevelViewportClient::CalcSceneView( uint32 InSizeX, uint32 InSizeY )
{
	// Calculate projection matrix
	Matrix		projectionMatrix;
	if ( viewportType == LVT_Perspective )
	{
		projectionMatrix = glm::perspective( SMath::DegreesToRadians( viewFOV ), ( float )InSizeX / InSizeY, 0.01f, ( float )WORLD_MAX );
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
		targetDirection		= SMath::vectorForward	* viewRotationQuat ;
		axisUp				= SMath::vectorUp		* viewRotationQuat;
		break;

	case LVT_OrthoXZ:
		targetDirection		= -SMath::vectorUp		* viewRotationQuat;
		axisUp				= SMath::vectorForward	* viewRotationQuat;
		break;

	case LVT_OrthoYZ:
		targetDirection		= SMath::vectorRight	* viewRotationQuat;
		axisUp				= SMath::vectorUp		* viewRotationQuat;
		break;

	default:
		check( false );		// Unknown viewport type
		break;
	}

	viewMatrix = glm::lookAt( viewLocation, viewLocation + targetDirection, axisUp );

	CSceneView*		sceneView = new CSceneView( viewLocation, projectionMatrix, viewMatrix, InSizeX, InSizeY, GetBackgroundColor(), showFlags );
	return sceneView;
}

CColor CEditorLevelViewportClient::GetBackgroundColor() const
{
	if ( viewportType == LVT_Perspective )
	{
		return CColor::black;
	}
	else
	{
		return CColor( 26, 27, 28 );
	}
}