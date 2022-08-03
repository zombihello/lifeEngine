#include <QCursor>
#include <QApplication>

#include "Misc/EngineGlobals.h"
#include "Misc/AudioGlobals.h"
#include "Misc/WorldEdGlobals.h"
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
static const Vector		GDefaultPerspectiveViewLocation( 495.166962, 167.584518, -400.f );

/** Default camera orientation for level editor perspective viewports */
static const CRotator		GDefaultPerspectiveViewRotation( 0, 0, 0 );

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
	, bIsTracking( false )
	, bIgnoreInput( false )
	, bAllowContextMenu( true )
	, viewportType( InViewportType )
	, viewLocation( SMath::vectorZero )
	, viewRotation( SMath::rotatorZero )
	, viewFOV( 90.f )
	, orthoZoom( 10000.f )
	, cameraSpeed( MIN_CAMERA_SPEED )
	, showFlags( SHOW_DefaultEditor )
	, cameraMoveFlags( 0x0 )
{
	SetViewportType( InViewportType );
}

void CEditorLevelViewportClient::Tick( float InDeltaSeconds )
{
	// If we tracking mouse and this is perspective viewport - change view location
	if ( bIsTracking && viewportType == LVT_Perspective )
	{
		Vector		targetDirection = viewRotation.RotateVector( SMath::vectorForward );
		Vector		axisUp			= viewRotation.RotateVector( SMath::vectorUp );
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
		GAudioDevice.SetListenerSpatial( viewLocation, viewRotation.RotateVector( SMath::vectorForward ), viewRotation.RotateVector( SMath::vectorUp ) );
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
	CSceneRenderer				sceneRenderer( InSceneView, ( CScene* )GWorld->GetScene() );
	sceneRenderer.BeginRenderViewTarget( InViewportRHI );
	
	// Draw grid
	drawHelper.DrawGrid( InSceneView, viewportType, ( CScene* )GWorld->GetScene() );
	
	// Draw scene
	sceneRenderer.Render( InViewportRHI );
	
	// Finishing render and delete scene view
	sceneRenderer.FinishRenderViewTarget( InViewportRHI );
	delete InSceneView;
}

#if ENABLE_HITPROXY
void CEditorLevelViewportClient::DrawHitProxies( CViewport* InViewport )
{ 
	check( InViewport );
	CSceneView*		sceneView = CalcSceneView( InViewport->GetSizeX(), InViewport->GetSizeY() );

	// Draw viewport
	UNIQUE_RENDER_COMMAND_THREEPARAMETER( CViewportRenderCommand,
										  CEditorLevelViewportClient*, viewportClient, this,
										  ViewportRHIRef_t, viewportRHI, InViewport->GetViewportRHI(),
										  CSceneView*, sceneView, sceneView,
										  {
											  viewportClient->DrawHitProxies_RenderThread( viewportRHI, sceneView );
										  } );
}

void CEditorLevelViewportClient::DrawHitProxies_RenderThread( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView )
{
	check( IsInRenderingThread() );
	CBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	CSceneRenderer				sceneRenderer( InSceneView, ( CScene* )GWorld->GetScene() );

	// Draw hit proxies
	sceneRenderer.RenderHitProxies( InViewportRHI );
	
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
		viewLocation = GDefaultPerspectiveViewLocation;
		viewRotation = GDefaultPerspectiveViewRotation;
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
	if ( bIgnoreInput )
	{
		return;
	}

	switch ( InWindowEvent.type )
	{
		// Event of mouse pressed
	case SWindowEvent::T_MousePressed:
		if ( InWindowEvent.events.mouseButton.code == BC_MouseRight )
		{
			QApplication::setOverrideCursor( QCursor( Qt::BlankCursor ) );
			bIsTracking			= true;
		}

		bAllowContextMenu		= true;
		break;

		// Event of mouse released
	case SWindowEvent::T_MouseReleased:
		if ( InWindowEvent.events.mouseButton.code == BC_MouseRight )
		{
			QApplication::restoreOverrideCursor();
			bIsTracking			= false;
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
		if ( bIsTracking )
		{
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
					dX = &viewLocation.z;
					dY = &viewLocation.y;
					break;

				default:
					check( false );		// Not supported viewport type
					break;
				}

				*dX			+= InWindowEvent.events.mouseMove.xDirection * ( orthoZoom / CAMERA_ZOOM_DIV );
				*dY			+= InWindowEvent.events.mouseMove.yDirection * ( orthoZoom / CAMERA_ZOOM_DIV );
				*dX			= Clamp<float>( *dX, -HALF_WORLD_MAX1, HALF_WORLD_MAX1 );
				*dY			= Clamp<float>( *dY, -HALF_WORLD_MAX1, HALF_WORLD_MAX1 );
			}

			// For perspective viewports its rotate camera
			else
			{
				// Update Yaw axis
				if ( InWindowEvent.events.mouseMove.xDirection != 0.f )
				{
					viewRotation.yaw += InWindowEvent.events.mouseMove.xDirection * GInputSystem->GetMouseSensitivity();
					if ( viewRotation.yaw < -360.f || viewRotation.yaw > 360.f )
					{
						viewRotation.yaw = 0.f;
					}
				}

				// Update Pitch axis
				if ( InWindowEvent.events.mouseMove.yDirection != 0.f )
				{
					viewRotation.pitch -= InWindowEvent.events.mouseMove.yDirection * GInputSystem->GetMouseSensitivity();
					if ( viewRotation.pitch > 90.f )
					{
						viewRotation.pitch = 90.f;
					}
					else if ( viewRotation.pitch < -90.f )
					{
						viewRotation.pitch = -90.f;
					}
				}
			}
		}		
		break;

		// Event of key press
	case SWindowEvent::T_KeyPressed:
		if ( bIsTracking && viewportType == LVT_Perspective )
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
		if ( bIsTracking && viewportType == LVT_Perspective )
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
		targetDirection		= viewRotation.RotateVector( SMath::vectorForward );
		axisUp				= viewRotation.RotateVector( SMath::vectorUp );
		break;

	case LVT_OrthoXZ:
		targetDirection		= viewRotation.RotateVector( -SMath::vectorUp );
		axisUp				= viewRotation.RotateVector( SMath::vectorForward );
		break;

	case LVT_OrthoYZ:
		targetDirection		= viewRotation.RotateVector( SMath::vectorRight );
		axisUp				= viewRotation.RotateVector( SMath::vectorUp );
		break;

	default:
		check( false );		// Unknown viewport type
		break;
	}

	viewMatrix = glm::lookAt( viewLocation, viewLocation + targetDirection, axisUp );

	CSceneView*		sceneView = new CSceneView( projectionMatrix, viewMatrix, InSizeX, InSizeY, GetBackgroundColor(), showFlags );
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
		return CColor( 163, 163, 163 );
	}
}