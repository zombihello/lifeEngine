#include "ui_MainWindow.h"
#include "Windows/MainWindow.h"

#include "Containers/String.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/LaunchGlobals.h"
#include "System/EditorEngine.h"
#include "EngineLoop.h"

WeMainWindow::WeMainWindow( QWidget* InParent /* = nullptr */ ) : 
	QMainWindow( InParent ),
	ui( new Ui::MainWindow() )
{
	// Init Qt UI
	ui->setupUi( this );
	setWindowTitle( QString::fromStdWString( GEditorEngine->GetEditorName() ) );

	// Update text in menu action 'About'
	ui->actionAbout->setText( QString::fromWCharArray( FString::Format( TEXT( "About %s" ), GEditorEngine->GetEditorName().c_str() ) ) );

	// Start timer for tick engine
	connect( &timerTick, &QTimer::timeout, this, &WeMainWindow::OnTickLE );
	timerTick.start( 1.f );
}

WeMainWindow::~WeMainWindow()
{
	delete ui;
}

void WeMainWindow::OnTickLE()
{
	if ( !GIsRequestingExit )
	{
		GEngineLoop->Tick();
	}
}