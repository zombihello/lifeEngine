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
#include "System/AudioDevice.h"
#include "System/EditorEngine.h"
#include "System/InputSystem.h"
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
static const FVector		GDefaultPerspectiveViewLocation( 495.166962, 167.584518, -400.f );

/** Default camera orientation for level editor perspective viewports */
static const FRotator		GDefaultPerspectiveViewRotation( 0, 0, 0 );

/** Show flags for each viewport type */
static const EShowFlags		GShowFlags[ LVT_Max ] =
{
	SHOW_DefaultEditor | SHOW_Wireframe,		// LVT_OrthoXY
	SHOW_DefaultEditor | SHOW_Wireframe,		// LVT_OrthoXZ
	SHOW_DefaultEditor | SHOW_Wireframe,		// LVT_OrthoYZ
	SHOW_DefaultEditor							// LVT_Perspective
};


FEditorLevelViewportClient::FEditorLevelViewportClient( ELevelViewportType InViewportType /* = LVT_Perspective */ )
	: bSetListenerPosition( true )
	, bIsTracking( false )
	, viewportType( InViewportType )
	, viewLocation( FMath::vectorZero )
	, viewRotation( FMath::rotatorZero )
	, viewFOV( 90.f )
	, orthoZoom( 10000.f )
	, cameraSpeed( MIN_CAMERA_SPEED )
	, showFlags( SHOW_DefaultEditor )
	, cameraMoveFlags( 0x0 )
{
	SetViewportType( InViewportType );
}

void FEditorLevelViewportClient::Tick( float InDeltaSeconds )
{
	// If we tracking mouse and this is perspective viewport - change view location
	if ( bIsTracking && viewportType == LVT_Perspective )
	{
		FVector		targetDirection = viewRotation.RotateVector( FMath::vectorForward );
		FVector		axisUp			= viewRotation.RotateVector( FMath::vectorUp );
		FVector		axisRight		= FMath::CrossVector( targetDirection, axisUp );

		if ( cameraMoveFlags & CMV_MoveForward )		viewLocation +=	targetDirection	* cameraSpeed;
		if ( cameraMoveFlags & CMV_MoveBackward )		viewLocation +=	targetDirection	* -cameraSpeed;
		if ( cameraMoveFlags & CMV_MoveLeft )			viewLocation +=	axisRight		* -cameraSpeed;
		if ( cameraMoveFlags & CMV_MoveRight )			viewLocation +=	axisRight		* cameraSpeed;
	}
}

void FEditorLevelViewportClient::Draw( FViewport* InViewport )
{
	FSceneView*		sceneView = CalcSceneView( InViewport );

	// Update audio listener spatial if allowed
	if ( viewportType == LVT_Perspective && bSetListenerPosition )
	{
		GAudioDevice.SetListenerSpatial( viewLocation, viewRotation.RotateVector( FMath::vectorForward ), viewRotation.RotateVector( FMath::vectorUp ) );
	}

	// Draw viewport
	UNIQUE_RENDER_COMMAND_THREEPARAMETER( FViewportRenderCommand,
										  FEditorLevelViewportClient*, viewportClient, this,
										  FViewportRHIRef, viewportRHI, InViewport->GetViewportRHI(),
										  FSceneView*, sceneView, sceneView,
										  {
											  viewportClient->Draw_RenderThread( viewportRHI, sceneView );
										  } );
}

void FEditorLevelViewportClient::Draw_RenderThread( FViewportRHIRef InViewportRHI, class FSceneView* InSceneView )
{
	check( IsInRenderingThread() );
	FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	FSceneRenderer				sceneRenderer( InSceneView );
	sceneRenderer.BeginRenderViewTarget( InViewportRHI );
	
	// Draw grid
	drawHelper.DrawGrid( InSceneView, viewportType );
	
	// Draw scene
	sceneRenderer.Render( InViewportRHI );
	
	// Finishing render and delete scene view
	sceneRenderer.FinishRenderViewTarget( InViewportRHI );
	delete InSceneView;
}

void FEditorLevelViewportClient::SetViewportType( ELevelViewportType InViewportType )
{
	bSetListenerPosition	= false;
	viewportType			= InViewportType;
	showFlags				= GShowFlags[ InViewportType ];

	if ( viewportType == LVT_Perspective )
	{
		bSetListenerPosition = true;
		viewLocation = GDefaultPerspectiveViewLocation;
		viewRotation = GDefaultPerspectiveViewRotation;
	}
}

void FEditorLevelViewportClient::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	switch ( InWindowEvent.type )
	{
		// Event of mouse pressed
	case SWindowEvent::T_MousePressed:
		if ( InWindowEvent.events.mouseButton.code == BC_MouseRight )
		{
			QApplication::setOverrideCursor( QCursor( Qt::BlankCursor ) );
			bIsTracking			= true;
		}
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

FSceneView* FEditorLevelViewportClient::CalcSceneView( FViewport* InViewport )
{
	// Calculate projection matrix
	FMatrix		projectionMatrix;
	float		viewportSizeX	= InViewport->GetSizeX();
	float		viewportSizeY	= InViewport->GetSizeY();
	if ( viewportType == LVT_Perspective )
	{
		projectionMatrix = glm::perspective( FMath::DegreesToRadians( viewFOV ), viewportSizeX / viewportSizeY, 0.01f, ( float )WORLD_MAX );
	}
	else
	{
		const float		zoom		= orthoZoom / ( viewportSizeX * 15.0f );
		float			halfWidth	= zoom * viewportSizeX / 2.f;
		float			halfHeight	= zoom * viewportSizeY / 2.f;
		projectionMatrix = glm::ortho( -halfWidth, halfWidth, -halfHeight, halfHeight, ( float )-HALF_WORLD_MAX, ( float )HALF_WORLD_MAX );
	}

	// Calculate view matrix
	FMatrix		viewMatrix;
	FVector		targetDirection;
	FVector		axisUp;
	switch ( viewportType )
	{
	case LVT_Perspective:
	case LVT_OrthoXY:
		targetDirection		= viewRotation.RotateVector( FMath::vectorForward );
		axisUp				= viewRotation.RotateVector( FMath::vectorUp );
		break;

	case LVT_OrthoXZ:
		targetDirection		= viewRotation.RotateVector( -FMath::vectorUp );
		axisUp				= viewRotation.RotateVector( FMath::vectorForward );
		break;

	case LVT_OrthoYZ:
		targetDirection		= viewRotation.RotateVector( FMath::vectorRight );
		axisUp				= viewRotation.RotateVector( FMath::vectorUp );
		break;

	default:
		check( false );		// Unknown viewport type
		break;
	}

	viewMatrix = glm::lookAt( viewLocation, viewLocation + targetDirection, axisUp );

	FSceneView*		sceneView = new FSceneView( projectionMatrix, viewMatrix, InViewport->GetSizeX(), InViewport->GetSizeY(), GetBackgroundColor(), showFlags );
	return sceneView;
}

FColor FEditorLevelViewportClient::GetBackgroundColor() const
{
	if ( viewportType == LVT_Perspective )
	{
		return FColor::black;
	}
	else
	{
		return FColor( 163, 163, 163 );
	}
}