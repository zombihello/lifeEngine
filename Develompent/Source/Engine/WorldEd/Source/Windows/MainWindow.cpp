#include <qmessagebox.h>
#include <QKeyEvent>
#include <qdebug.h>

#include "ui_MainWindow.h"
#include "Windows/MainWindow.h"

#include "Containers/String.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/LaunchGlobals.h"
#include "System/EditorEngine.h"
#include "System/InputKeyConv.h"
#include "System/WindowEvent.h"
#include "EngineLoop.h"

WeMainWindow::WeMainWindow( QWidget* InParent /* = nullptr */ ) : 
	QMainWindow( InParent ),
	ui( new Ui::MainWindow() )
{
	// Init Qt UI
	ui->setupUi( this );
	setWindowTitle( QString::fromStdWString( GEditorEngine->GetEditorName() ) );

	// Update text in menu action 'About'
	ui->actionAbout->setText( QString::fromStdWString( FString::Format( TEXT( "About %s" ), GEditorEngine->GetEditorName().c_str() ) ) );

	// Start timer for tick engine
	connect( &timerTick, &QTimer::timeout, this, &WeMainWindow::OnTickLE );
	timerTick.start( 1.f );
}

WeMainWindow::~WeMainWindow()
{
	delete ui;
}

void WeMainWindow::keyPressEvent( QKeyEvent* InEvent )
{
	SWindowEvent		windowEvent;
	uint32				modifiers = InEvent->modifiers();
	windowEvent.type					= SWindowEvent::T_KeyPressed;
	windowEvent.events.key.isAlt		= modifiers & Qt::AltModifier;
	windowEvent.events.key.isCapsLock	= modifiers & Qt::GroupSwitchModifier;
	windowEvent.events.key.isControl	= modifiers & Qt::ControlModifier;
	windowEvent.events.key.isNumLock	= modifiers & Qt::KeypadModifier;
	windowEvent.events.key.isShift		= modifiers & Qt::ShiftModifier;
	windowEvent.events.key.isSuper		= modifiers & Qt::MetaModifier;
	windowEvent.events.key.code			= appQtKeyToButtonCode( ( Qt::Key )InEvent->key(), modifiers );
	GEngineLoop->ProcessEvent( windowEvent );
}

void WeMainWindow::keyReleaseEvent( QKeyEvent* InEvent )
{
	SWindowEvent		windowEvent;
	uint32				modifiers = InEvent->modifiers();
	windowEvent.type					= SWindowEvent::T_KeyReleased;
	windowEvent.events.key.isAlt		= modifiers & Qt::AltModifier;
	windowEvent.events.key.isCapsLock	= modifiers & Qt::GroupSwitchModifier;
	windowEvent.events.key.isControl	= modifiers & Qt::ControlModifier;
	windowEvent.events.key.isNumLock	= modifiers & Qt::KeypadModifier;
	windowEvent.events.key.isShift		= modifiers & Qt::ShiftModifier;
	windowEvent.events.key.isSuper		= modifiers & Qt::MetaModifier;
	windowEvent.events.key.code			= appQtKeyToButtonCode( ( Qt::Key )InEvent->key(), modifiers );
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