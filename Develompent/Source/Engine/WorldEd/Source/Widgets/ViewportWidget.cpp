#include <QDebug>
#include <QtGui/qevent.h>
#include <QMessageBox>
#include <QDateTime>

#include "Misc/WorldEdGlobals.h"
#include "Containers/String.h"
#include "System/EditorEngine.h"
#include "System/WindowEvent.h"
#include "System/InputKeyConv.h"
#include "Widgets/ViewportWidget.h"
#include "EngineDefines.h"

WeViewportWidget::WeViewportWidget( QWidget* InParent /* = nullptr */, CViewportClient* InViewportClient /* = nullptr */, bool InDeleteViewportClient /* = false */ )
	: QWidget( InParent )
	, bEnabled( false )
	, bInTick( false )
{
	SetViewportClient( InViewportClient, InDeleteViewportClient );
	InitViewport();
}

WeViewportWidget::~WeViewportWidget()
{
	if ( bInTick )
	{
		viewport.Update( true, 0, 0, nullptr );
		SetEnabled( false );

		if ( bDeleteViewportClient )
		{
			delete viewportClient;
			viewportClient = nullptr;
		}

		// Wait while viewport RHI is not deleted
		while ( viewport.IsValid() )
		{
			appSleep( 0.1f );
		}

		bInTick = false;
	}
}

void WeViewportWidget::InitViewport()
{
	QPalette		Palette = palette();
	Palette.setColor( QPalette::Base, Qt::black );
	setAutoFillBackground( true );
	setPalette( Palette );

	setFocusPolicy( Qt::StrongFocus );
	setAttribute( Qt::WA_NativeWindow );

	// Setting these attributes to our widget and returning nullptr on paintEngine event 
	// tells Qt that we'll handle all drawing and updating the widget ourselves.
	setAttribute( Qt::WA_PaintOnScreen );
	setAttribute( Qt::WA_NoSystemBackground );

	QSize		Size = size();
	viewport.SetViewportClient( viewportClient );
	viewport.Update( false, Size.width(), Size.height(), reinterpret_cast< HWND >( winId() ) );
}

void WeViewportWidget::showEvent( QShowEvent* InEvent )
{
	SetEnabled( bEnabled );
	QWidget::showEvent( InEvent );
}

void WeViewportWidget::SetEnabled( bool InIsEnabled )
{
	bEnabled = InIsEnabled;

	if ( bEnabled && !bInTick )
	{
		GEditorEngine->AddViewport( &viewport );
		bInTick = true;
	}
	else if ( !bEnabled && bInTick )
	{
		GEditorEngine->RemoveViewport( &viewport );
		bInTick = false;
	}
}

QPaintEngine* WeViewportWidget::paintEngine() const
{
	return nullptr;
}

void WeViewportWidget::paintEvent( QPaintEvent* InEvent )
{}

void WeViewportWidget::resizeEvent( QResizeEvent* InEvent )
{
	SWindowEvent		windowEvent;
	windowEvent.events.windowResize.width = InEvent->size().width();
	windowEvent.events.windowResize.height = InEvent->size().height();
	viewport.Update( false, windowEvent.events.windowResize.width, windowEvent.events.windowResize.height, reinterpret_cast< HWND >( winId() ) );
	
	// Process event resize in other sections
	GEditorEngine->ProcessEvent( windowEvent );
	QWidget::resizeEvent( InEvent );
}

void WeViewportWidget::wheelEvent( QWheelEvent* InEvent )
{
	if ( !viewportClient )
	{
		return;
	}

	QPoint				angleDelta = InEvent->angleDelta();
	SWindowEvent		windowEvent;
	windowEvent.type					= SWindowEvent::T_MouseWheel;
	windowEvent.events.mouseWheel.x		= angleDelta.x();
	windowEvent.events.mouseWheel.y		= angleDelta.y();
	viewportClient->ProcessEvent( windowEvent );
}

void WeViewportWidget::mousePressEvent( QMouseEvent* InEvent )
{
	if ( !viewportClient )
	{
		return;
	}

	SWindowEvent		windowEvent;
	windowEvent.type						= SWindowEvent::T_MousePressed;
	windowEvent.events.mouseButton.x		= InEvent->x();
	windowEvent.events.mouseButton.y		= InEvent->y();
	windowEvent.events.mouseButton.code		= appQtMouseButtonToButtonCode( InEvent->button() );

	mousePosition = mapFromGlobal( cursor().pos() );
	viewportClient->ProcessEvent( windowEvent );
}

void WeViewportWidget::mouseReleaseEvent( QMouseEvent* InEvent )
{
	if ( !viewportClient )
	{
		return;
	}

	SWindowEvent		windowEvent;
	windowEvent.type						= SWindowEvent::T_MouseReleased;
	windowEvent.events.mouseButton.x		= InEvent->x();
	windowEvent.events.mouseButton.y		= InEvent->y();
	windowEvent.events.mouseButton.code		= appQtMouseButtonToButtonCode( InEvent->button() );

	mousePosition.setX( 0 );
	mousePosition.setY( 0 );
	viewportClient->ProcessEvent( windowEvent );
}

void WeViewportWidget::mouseMoveEvent( QMouseEvent* InEvent )
{
	if ( !viewportClient )
	{
		return;
	}

	SWindowEvent		windowEvent;
	windowEvent.type							= SWindowEvent::T_MouseMove;
	windowEvent.events.mouseMove.x				= InEvent->x();
	windowEvent.events.mouseMove.y				= InEvent->y();
	windowEvent.events.mouseMove.xDirection		= windowEvent.events.mouseMove.x - mousePosition.x();
	windowEvent.events.mouseMove.yDirection		= windowEvent.events.mouseMove.y - mousePosition.y();

	mousePosition.setX( InEvent->x() );
	mousePosition.setY( InEvent->y() );
	viewportClient->ProcessEvent( windowEvent );
}

void WeViewportWidget::keyPressEvent( QKeyEvent* InEvent )
{
	if ( !viewportClient )
	{
		return;
	}

	SWindowEvent		windowEvent;
	uint32				modifiers		= InEvent->modifiers();
	windowEvent.type					= SWindowEvent::T_KeyPressed;
	windowEvent.events.key.isAlt		= modifiers & Qt::AltModifier;
	windowEvent.events.key.isCapsLock	= modifiers & Qt::GroupSwitchModifier;
	windowEvent.events.key.isControl	= modifiers & Qt::ControlModifier;
	windowEvent.events.key.isNumLock	= modifiers & Qt::KeypadModifier;
	windowEvent.events.key.isShift		= modifiers & Qt::ShiftModifier;
	windowEvent.events.key.isSuper		= modifiers & Qt::MetaModifier;
	windowEvent.events.key.code			= appQtKeyToButtonCode( ( Qt::Key ) InEvent->key(), modifiers );
	viewportClient->ProcessEvent( windowEvent );
}

void WeViewportWidget::keyReleaseEvent( QKeyEvent* InEvent )
{
	if ( !viewportClient )
	{
		return;
	}

	SWindowEvent		windowEvent;
	uint32				modifiers		= InEvent->modifiers();
	windowEvent.type					= SWindowEvent::T_KeyReleased;
	windowEvent.events.key.isAlt		= modifiers & Qt::AltModifier;
	windowEvent.events.key.isCapsLock	= modifiers & Qt::GroupSwitchModifier;
	windowEvent.events.key.isControl	= modifiers & Qt::ControlModifier;
	windowEvent.events.key.isNumLock	= modifiers & Qt::KeypadModifier;
	windowEvent.events.key.isShift		= modifiers & Qt::ShiftModifier;
	windowEvent.events.key.isSuper		= modifiers & Qt::MetaModifier;
	windowEvent.events.key.code			= appQtKeyToButtonCode( ( Qt::Key ) InEvent->key(), modifiers );
	viewportClient->ProcessEvent( windowEvent );
}