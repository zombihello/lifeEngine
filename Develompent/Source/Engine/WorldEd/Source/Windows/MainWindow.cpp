#include <qmessagebox.h>
#include <QKeyEvent>
#include <qlabel.h>
#include <qdebug.h>

#include "ui_MainWindow.h"
#include "Windows/MainWindow.h"
#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/LaunchGlobals.h"
#include "System/EditorEngine.h"
#include "System/InputKeyConv.h"
#include "System/WindowEvent.h"
#include "Render/EditorLevelViewportClient.h"
#include "EngineLoop.h"
#include "WorldEd.h"

// Widgets
#include "Widgets/ViewportWidget.h"
#include "Widgets/LogWidget.h"
#include "Widgets/ActorPropertiesWidget.h"
#include "Widgets/ExploerLevelWidget.h"
#include "Widgets/ContentBrowserWidget.h"
#include "Widgets/ActorClassesWidget.h"

WeMainWindow::WeMainWindow( QWidget* InParent /* = nullptr */ ) 
	: QMainWindow( InParent )
	, ui( new Ui::MainWindow() )
	, dockManager( nullptr )
	, logWidget( nullptr )
	, contentBrowserWidget( nullptr )
	, actorClassesWidget( nullptr )
{
	// Init Qt UI
	ui->setupUi( this );
	InitUI();

	// Start timer for tick engine
	connect( &timerTick, &QTimer::timeout, this, &WeMainWindow::OnTickLE );
	timerTick.start( 1.f );
}

void WeMainWindow::InitUI()
{
	// Update window title
	setWindowTitle( QString::fromStdWString( GEditorEngine->GetEditorName() ) );

	// Create the dock manager. Because the parent parameter is a QMainWindow
	// the dock manager registers itself as the central widget
	dockManager = new ads::CDockManager( this );
	dockManager->setConfigFlag( ads::CDockManager::MiddleMouseButtonClosesTab, true );

	QAction*				firstActionMenuView = ui->menuWindows->actions().at( 0 );
	ads::CDockWidget*		dockWidget = nullptr;

	// Create widgets of editor
	// Create viewports
	{
		// 3D perspective
		viewportWidgets[ LVT_Perspective ]		= new WeViewportWidget( this, new CEditorLevelViewportClient( LVT_Perspective ), true );
		ads::CDockAreaWidget*					perspectiveDockAreaWidget = CreateDockWidget( ads::CenterDockWidgetArea, "3D Perspective", viewportWidgets[ LVT_Perspective ], nullptr, false, &dockWidget );
		ui->menuViewports->addAction( dockWidget->toggleViewAction() );
		connect( dockWidget, &ads::CDockWidget::visibilityChanged, this, &WeMainWindow::On_PerspectiveDockWidget_VisibilityChanged );

		// 2D XY
		viewportWidgets[ LVT_OrthoXY ]			= new WeViewportWidget( this, new CEditorLevelViewportClient( LVT_OrthoXY ), true );
		ads::CDockAreaWidget*					xyDockAreaWidget = CreateDockWidget( ads::TopDockWidgetArea, "2D X/Y", viewportWidgets[ LVT_OrthoXY ], perspectiveDockAreaWidget, false, &dockWidget );
		ui->menuViewports->addAction( dockWidget->toggleViewAction() );
		connect( dockWidget, &ads::CDockWidget::visibilityChanged, this, &WeMainWindow::On_OrthoXYDockWidget_VisibilityChanged );

		// 2D XZ
		viewportWidgets[ LVT_OrthoXZ ]			= new WeViewportWidget( this, new CEditorLevelViewportClient( LVT_OrthoXZ ), true );
		ads::CDockAreaWidget*					xzDockAreaWidget = CreateDockWidget( ads::RightDockWidgetArea, "2D X/Z", viewportWidgets[ LVT_OrthoXZ ], xyDockAreaWidget, false, &dockWidget );
		ui->menuViewports->addAction( dockWidget->toggleViewAction() );
		connect( dockWidget, &ads::CDockWidget::visibilityChanged, this, &WeMainWindow::On_OrthoXZDockWidget_VisibilityChanged );

		// 2D YZ
		viewportWidgets[ LVT_OrthoYZ ]			= new WeViewportWidget( this, new CEditorLevelViewportClient( LVT_OrthoYZ ), true );
		ads::CDockAreaWidget*					yzDockAreaWidget = CreateDockWidget( ads::RightDockWidgetArea, "2D Y/Z", viewportWidgets[ LVT_OrthoYZ ], perspectiveDockAreaWidget, false, &dockWidget );
		ui->menuViewports->addAction( dockWidget->toggleViewAction() );
		connect( dockWidget, &ads::CDockWidget::visibilityChanged, this, &WeMainWindow::On_OrthoYZDockWidget_VisibilityChanged );
	}

	// Explorer level
	WeExploerLevelWidget*		exploerLevelWidget = new WeExploerLevelWidget( this );
	ads::CDockAreaWidget*		explorerLevelAreaWidget = CreateDockWidget( ads::RightDockWidgetArea, "Explorer Level", exploerLevelWidget, nullptr, false, &dockWidget );
	ui->menuWindows->insertAction( firstActionMenuView, dockWidget->toggleViewAction() );

	// Actor properties
	WeActorPropertiesWidget*	actorPropertiesWidget = new WeActorPropertiesWidget( this );
	ads::CDockAreaWidget*		actorPropertiesAreaWidget = CreateDockWidget( ads::BottomDockWidgetArea, "Actor Properties", actorPropertiesWidget, explorerLevelAreaWidget, false, &dockWidget );
	ui->menuWindows->insertAction( firstActionMenuView, dockWidget->toggleViewAction() );

	// Content browser
	contentBrowserWidget		= new WeContentBrowserWidget( WeContentBrowserWidget::RD_Game, this );
	ads::CDockAreaWidget*		contentBrowserAreaWidget = CreateDockWidget( ads::CenterDockWidgetArea, "Content Browser", contentBrowserWidget, actorPropertiesAreaWidget, false, &dockWidget );
	ui->menuWindows->insertAction( firstActionMenuView, dockWidget->toggleViewAction() );

	// Actor classes
	actorClassesWidget			= new WeActorClassesWidget( this );
	ads::CDockAreaWidget*		actorClassesAreaWidget = CreateDockWidget( ads::CenterDockWidgetArea, "Actor Classes", actorClassesWidget, actorPropertiesAreaWidget, false, &dockWidget );
	ui->menuWindows->insertAction( firstActionMenuView, dockWidget->toggleViewAction() );

	// Logger
	logWidget = new WeLogWidget( this );
	ads::CDockAreaWidget*		logAreaWidget = CreateDockWidget( ads::CenterDockWidgetArea, "Logs", logWidget, actorPropertiesAreaWidget, false, &dockWidget );
	ui->menuWindows->insertAction( firstActionMenuView, dockWidget->toggleViewAction() );

	// Update text in menu action 'About'
	ui->actionAbout->setText( QString::fromStdWString( ÑString::Format( TEXT( "About %s" ), GEditorEngine->GetEditorName().c_str() ) ) );

	// Set icons for actions
	ui->actionNewLevel->setIcon( QIcon( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/New.png" ) ).c_str() ) ) );
	ui->actionOpen->setIcon( QIcon( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/Open.png" ) ).c_str() ) ) );
	ui->actionSave->setIcon( QIcon( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/Save.png" ) ).c_str() ) ) );
	ui->actionSaveAll->setIcon( QIcon( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/SaveAll.png" ) ).c_str() ) ) );

	// Restore state of dock widgets from cache
	QFile           file( QString::fromStdWString( ÑString::Format( TEXT( "%s/EditorCache/UICache.dat" ), appGameDir().c_str() ) ) );
	QByteArray		cachedState;
	if ( file.open( QFile::ReadOnly ) )
	{
		cachedState = file.readAll();
		file.close();
	}

	if ( !cachedState.isEmpty() )
	{
		dockManager->restoreState( cachedState );
	}
}

WeMainWindow::~WeMainWindow()
{
	delete ui;
	delete logWidget;
	delete dockManager;
}

void WeMainWindow::keyPressEvent( QKeyEvent* InEvent )
{
	SWindowEvent		windowEvent;
	uint32				modifiers = InEvent->modifiers();
	windowEvent.type = SWindowEvent::T_KeyPressed;
	windowEvent.events.key.isAlt = modifiers & Qt::AltModifier;
	windowEvent.events.key.isCapsLock = modifiers & Qt::GroupSwitchModifier;
	windowEvent.events.key.isControl = modifiers & Qt::ControlModifier;
	windowEvent.events.key.isNumLock = modifiers & Qt::KeypadModifier;
	windowEvent.events.key.isShift = modifiers & Qt::ShiftModifier;
	windowEvent.events.key.isSuper = modifiers & Qt::MetaModifier;
	windowEvent.events.key.code = appQtKeyToButtonCode( ( Qt::Key ) InEvent->key(), modifiers );
	GEngineLoop->ProcessEvent( windowEvent );
}

void WeMainWindow::keyReleaseEvent( QKeyEvent* InEvent )
{
	SWindowEvent		windowEvent;
	uint32				modifiers = InEvent->modifiers();
	windowEvent.type = SWindowEvent::T_KeyReleased;
	windowEvent.events.key.isAlt = modifiers & Qt::AltModifier;
	windowEvent.events.key.isCapsLock = modifiers & Qt::GroupSwitchModifier;
	windowEvent.events.key.isControl = modifiers & Qt::ControlModifier;
	windowEvent.events.key.isNumLock = modifiers & Qt::KeypadModifier;
	windowEvent.events.key.isShift = modifiers & Qt::ShiftModifier;
	windowEvent.events.key.isSuper = modifiers & Qt::MetaModifier;
	windowEvent.events.key.code = appQtKeyToButtonCode( ( Qt::Key ) InEvent->key(), modifiers );
	GEngineLoop->ProcessEvent( windowEvent );
}

void WeMainWindow::mousePressEvent( QMouseEvent* InEvent )
{
	SWindowEvent		windowEvent;
	windowEvent.type						= SWindowEvent::T_MousePressed;
	windowEvent.events.mouseButton.x		= InEvent->x();
	windowEvent.events.mouseButton.y		= InEvent->y();
	windowEvent.events.mouseButton.code		= appQtMouseButtonToButtonCode( InEvent->button() );

	mousePosition = cursor().pos();
	GEngineLoop->ProcessEvent( windowEvent );
}

void WeMainWindow::mouseReleaseEvent( QMouseEvent* InEvent )
{
	SWindowEvent		windowEvent;
	windowEvent.type						= SWindowEvent::T_MouseReleased;
	windowEvent.events.mouseButton.x		= InEvent->x();
	windowEvent.events.mouseButton.y		= InEvent->y();
	windowEvent.events.mouseButton.code		= appQtMouseButtonToButtonCode( InEvent->button() );

	mousePosition.setX( 0 );
	mousePosition.setY( 0 );
	GEngineLoop->ProcessEvent( windowEvent );
}

void WeMainWindow::mouseMoveEvent( QMouseEvent* InEvent )
{
	QPoint				centerWindow( width() / 2, height() / 2 );
	QPoint				globalCenterWindow = mapToGlobal( centerWindow );
	SWindowEvent		windowEvent;

	windowEvent.type							= SWindowEvent::T_MouseMove;
	windowEvent.events.mouseMove.x				= InEvent->globalX();
	windowEvent.events.mouseMove.y				= InEvent->globalY();	
	windowEvent.events.mouseMove.xDirection		= windowEvent.events.mouseMove.x - mousePosition.x();
	windowEvent.events.mouseMove.yDirection		= windowEvent.events.mouseMove.y - mousePosition.y();

	mousePosition.setX( InEvent->globalX() );
	mousePosition.setY( InEvent->globalY() );
	GEngineLoop->ProcessEvent( windowEvent );
}

void WeMainWindow::closeEvent( QCloseEvent* InEvent )
{
	// Save to UI cache of state all dock widgets
	QDir	dirEditorCache( QString::fromStdWString( appGameDir() ) );
	if ( !dirEditorCache.exists( "EditorCache" ) )
	{
		dirEditorCache.mkdir( "EditorCache" );
	}
	dirEditorCache.cd( "EditorCache" );

	QFile           file( dirEditorCache.absolutePath() + "/UICache.dat" );
	if ( file.open( QFile::WriteOnly ) )
	{
		file.write( dockManager->saveState() );
		file.close();
	}

	QApplication::quit();		// BS yehor.pohuliaka - Hack for close WorldEd when exist floating dock widgets
}

void WeMainWindow::OnTickLE()
{
	if ( !GIsRequestingExit )
	{
		GEngineLoop->Tick();
	}
}

void WeMainWindow::on_actionAbout_Qt_triggered()
{
	QMessageBox::aboutQt( this, "About Qt" );
}

void WeMainWindow::on_actionAbout_triggered()
{
	QMessageBox::about( this, ui->actionAbout->text(), "Place about text" );
}

void WeMainWindow::On_PerspectiveDockWidget_VisibilityChanged( bool InVisible )
{
	WeViewportWidget*		viewportWidget = viewportWidgets[ LVT_Perspective ];
	if ( viewportWidget )
	{
		viewportWidget->SetEnabled( InVisible );
	}
}

void WeMainWindow::On_OrthoXYDockWidget_VisibilityChanged( bool InVisible )
{ 
	WeViewportWidget*		viewportWidget = viewportWidgets[ LVT_OrthoXY ];
	if ( viewportWidget )
	{
		viewportWidget->SetEnabled( InVisible );
	}
}

void WeMainWindow::On_OrthoXZDockWidget_VisibilityChanged( bool InVisible )
{ 
	WeViewportWidget*		viewportWidget = viewportWidgets[ LVT_OrthoXZ ];
	if ( viewportWidget )
	{
		viewportWidget->SetEnabled( InVisible );
	}
}

void WeMainWindow::On_OrthoYZDockWidget_VisibilityChanged( bool InVisible )
{ 
	WeViewportWidget*		viewportWidget = viewportWidgets[ LVT_OrthoYZ ];
	if ( viewportWidget )
	{
		viewportWidget->SetEnabled( InVisible );
	}
}