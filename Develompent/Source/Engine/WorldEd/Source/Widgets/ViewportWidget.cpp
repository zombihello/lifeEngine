#include <QDebug>
#include <QtGui/qevent.h>
#include <QMessageBox>
#include <QDateTime>

#include "Misc/WorldEdGlobals.h"
#include "System/EditorEngine.h"
#include "System/WindowEvent.h"
#include "Widgets/ViewportWidget.h"

WeViewportWidget::WeViewportWidget( QWidget* InParent /* = nullptr */ ) :
	QWidget( InParent ),
	isInTick( false )
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
	viewport.Update( false, Size.width(), Size.height(), reinterpret_cast< HWND >( winId() ) );
}

WeViewportWidget::~WeViewportWidget()
{
	if ( isInTick )
	{
		viewport.Update( true, 0, 0, nullptr );
		GEditorEngine->RemoveViewport( &viewport );

		// Wait while viewport RHI is not deleted
		while ( viewport.IsValid() )
		{
			appSleep( 0.1f );
		}

		isInTick = false;
	}
}

void WeViewportWidget::showEvent( QShowEvent* InEvent )
{
	if ( !isInTick )
	{
		GEditorEngine->AddViewport( &viewport );
		isInTick = true;
	}

	QWidget::showEvent( InEvent );
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