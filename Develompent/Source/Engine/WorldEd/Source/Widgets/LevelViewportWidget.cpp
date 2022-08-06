#include <QMenu>

#include "Misc/EngineGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "System/World.h"
#include "System/EditorEngine.h"
#include "System/Package.h"
#include "System/ActorFactory.h"
#include "System/InputSystem.h"
#include "System/Gizmo.h"
#include "Widgets/LevelViewportWidget.h"
#include "Render/RenderingThread.h"

#include "Windows/MainWindow.h"

// Widgets
#include "Widgets/ActorClassesWidget.h"
#include "Widgets/ContentBrowserWidget.h"

WeLevelViewportWidget::WeLevelViewportWidget( QWidget* InParent /*= nullptr*/, CEditorLevelViewportClient* InViewportClient /*= nullptr*/, bool InDeleteViewportClient /*= false*/ )
	: WeViewportWidget( InParent, InViewportClient, InDeleteViewportClient )
{
	setContextMenuPolicy( Qt::CustomContextMenu );

	// Connect to slots
	connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( OnCustomContextMenuRequested( const QPoint& ) ) );

#if ENABLE_HITPROXY
	connect( &timerRenderHitProxyGizmo, &QTimer::timeout, this, &WeLevelViewportWidget::OnRenderHitProxyGizmo );
#endif // ENABLE_HITPROXY
}

WeLevelViewportWidget::~WeLevelViewportWidget()
{}

#if ENABLE_HITPROXY
void WeLevelViewportWidget::mousePressEvent( QMouseEvent* InEvent )
{
	WeViewportWidget::mousePressEvent( InEvent );

	if ( InEvent->button() == Qt::MouseButton::LeftButton )
	{
		CGizmo&		gizmo = GEditorEngine->GetGizmo();
		
		// If gizmo is disabled or not exist current axis, we try select actor
		if ( !gizmo.IsEnabled() || gizmo.GetCurrentAxis() == A_None )
		{
			// Update hit proxies id in all actors
			GWorld->UpdateHitProxiesId();

			// Render hit proxies to render target
			CEditorLevelViewportClient*		viewportClient = ( CEditorLevelViewportClient* ) GetViewport().GetViewportClient();
			check( viewportClient );
			viewportClient->DrawHitProxies( const_cast< CViewport* >( &GetViewport() ) );

			// Wait until we rendering scene
			FlushRenderingCommands();

			QPoint		cursorPosition	= mapFromGlobal( cursor().pos() );
			CHitProxyId	hitProxyId		= viewportClient->GetHitProxyId( cursorPosition.x(), cursorPosition.y() );

			GWorld->UnselectAllActors();
			gizmo.SetEnable( false );
			UpdateTimerRenderHitProxyGizmo( false );

			if ( hitProxyId.IsValid() )
			{
				uint32			index = hitProxyId.GetIndex();
				ActorRef_t		actor = GWorld->GetActor( index > 0 ? index - 1 : index );

				GWorld->SelectActor( actor );
				gizmo.SetEnable( true );
				gizmo.SetLocation( actor->GetActorLocation() );
				UpdateTimerRenderHitProxyGizmo( true );

				LE_LOG( LT_Log, LC_Editor, TEXT( "(%i;%i) Selected actor '%s'" ), cursorPosition.x(), cursorPosition.y(), actor->GetName() );
			}
		}
	}
}

void WeLevelViewportWidget::enterEvent( QEvent* InEvent )
{
	WeViewportWidget::enterEvent( InEvent );
	UpdateTimerRenderHitProxyGizmo( true );
}

void WeLevelViewportWidget::leaveEvent( QEvent* InEvent )
{
	WeViewportWidget::leaveEvent( InEvent );
	UpdateTimerRenderHitProxyGizmo( false );
}

void WeLevelViewportWidget::SetEnabled( bool InIsEnabled )
{
	WeViewportWidget::SetEnabled( InIsEnabled );
	UpdateTimerRenderHitProxyGizmo( InIsEnabled );
}

void WeLevelViewportWidget::UpdateTimerRenderHitProxyGizmo( bool InNeedStart )
{
	CGizmo&		gizmo = GEditorEngine->GetGizmo();
	if ( gizmo.IsEnabled() && InNeedStart )
	{
		timerRenderHitProxyGizmo.start( 1.f );
	}
	else
	{
		timerRenderHitProxyGizmo.stop();
	}
}

void WeLevelViewportWidget::OnRenderHitProxyGizmo()
{
	CGizmo&		gizmo = GEditorEngine->GetGizmo();
	if ( !gizmo.IsEnabled() || gizmo.GetType() == GT_None )
	{
		return;
	}

	// Getting current cursor position and check on condition cursor on widget
	QPoint		cursorPosition = mapFromGlobal( cursor().pos() );
	if ( cursorPosition.x() < 0 || cursorPosition.y() < 0 || cursorPosition.x() >= width() || cursorPosition.y() >= height() )
	{
		return;
	}

	// Getting hit proxy id from last hit proxy rendering
	CEditorLevelViewportClient*		viewportClient	= ( CEditorLevelViewportClient* )GetViewport().GetViewportClient();
	check( viewportClient );
	if ( viewportClient->GetMouseTrackingType() == CEditorLevelViewportClient::MT_Gizmo )
	{
		return;
	}

	// Render hit proxies to render target
	viewportClient->DrawHitProxies( const_cast< CViewport* >( &GetViewport() ), HPL_UI );

	// Wait until we rendering scene
	FlushRenderingCommands();

	CHitProxyId		hitProxyId = viewportClient->GetHitProxyId( cursorPosition.x(), cursorPosition.y() );
	if ( hitProxyId.IsValid() )
	{
		gizmo.SetCurrentAxis( hitProxyId.GetIndex() );
	}
	else
	{
		gizmo.SetCurrentAxis( A_None );
	}
}
#endif // ENABLE_HITPROXY

void WeLevelViewportWidget::OnCustomContextMenuRequested( const QPoint& InPoint )
{
	// Save cursor position for spawn objects on this place
	contextMenuCursorPosition = mapFromGlobal( cursor().pos() );

	// Check if we allow open context menu or no
	CEditorLevelViewportClient*		viewportClient = ( CEditorLevelViewportClient* )GetViewport().GetViewportClient();
	check( viewportClient );
	if ( !viewportClient->IsAllowContextMenu() )
	{
		return;
	}

	// If yes we getting current actor class and build menu
	CClass*				currentActorClass = GEditorEngine->GetMainWindow()->GetActorClassesWidget()->GetSelectedClass();
	check( currentActorClass );

	// Get asset name for showing in context menu
	std::wstring		assetReference = GEditorEngine->GetMainWindow()->GetContentBrowserWidget()->GetSelectedAssetReference();

	// Parse reference for check asset type. If not supported for spawning, we not showing this action
	{
		std::wstring		packageName;
		std::wstring		assetName;
		EAssetType			assetType;
		if ( !ParseReferenceToAsset( assetReference, packageName, assetName, assetType ) || !GActorFactory.IsRegistered( assetType ) )
		{
			assetReference = TEXT( "" );
		}
	}

	// Main menu
	QMenu			contextMenu( this );
	QAction			actionActorAdd( QString::fromStdWString( ÑString::Format( TEXT( "Add %s Here" ), currentActorClass->GetName().c_str() ) ), this );
	QAction			actionAssetAdd( QString::fromStdWString( ÑString::Format( TEXT( "Add %s Here" ), assetReference.c_str() ) ), this );
	contextMenu.addAction( &actionActorAdd );
	
	// If asset reference not empty, we add action for spawn asset in world
	if ( !assetReference.empty() )
	{
		contextMenu.addAction( &actionAssetAdd );
	}

	// Connect to slots
	connect( &actionActorAdd, SIGNAL( triggered() ), this, SLOT( OnActorAdd() ) );
	connect( &actionAssetAdd, SIGNAL( triggered() ), this, SLOT( OnAssetAdd() ) );

	// Execute him
	contextMenu.exec( QCursor::pos() );
}

void WeLevelViewportWidget::OnActorAdd()
{
	// Getting viewport client
	CEditorLevelViewportClient*		viewportClient = ( CEditorLevelViewportClient* )GetViewport().GetViewportClient();
	check( viewportClient );

	// Convert from screen space to world space
	Vector		location = viewportClient->ScreenToWorld( Vector2D( contextMenuCursorPosition.x(), contextMenuCursorPosition.y() ), width(), height() );

	// Spawn new actor
	GWorld->SpawnActor( GEditorEngine->GetMainWindow()->GetActorClassesWidget()->GetSelectedClass(), location );
}

void WeLevelViewportWidget::OnAssetAdd()
{
	// Getting asset reference
	std::wstring		assetReference = GEditorEngine->GetMainWindow()->GetContentBrowserWidget()->GetSelectedAssetReference();

	// Parse reference for get asset type. If failed in parsing, do nothing
	std::wstring		packageName;
	std::wstring		assetName;
	EAssetType			assetType;
	if ( !ParseReferenceToAsset( assetReference, packageName, assetName, assetType ) )
	{
		return;
	}

	// Find asset. If failed, we exit from method
	TAssetHandle<CAsset>		asset = GPackageManager->FindAsset( assetReference, assetType );
	if ( !asset.IsAssetValid() )
	{
		return;
	}

	// Getting viewport client
	CEditorLevelViewportClient*		viewportClient = ( CEditorLevelViewportClient* )GetViewport().GetViewportClient();
	check( viewportClient );

	// Convert from screen space to world space
	Vector		location = viewportClient->ScreenToWorld( Vector2D( contextMenuCursorPosition.x(), contextMenuCursorPosition.y() ), width(), height() );

	// Spawn new actor
	GActorFactory.Spawn( asset, location );
}