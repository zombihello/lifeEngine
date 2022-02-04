#include <qmessagebox.h>
#include <QKeyEvent>
#include <qlabel.h>
#include <qdebug.h>

#include "ui_MainWindow.h"
#include "ADS/SectionWidget.h"
#include "Windows/MainWindow.h"
#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/LaunchGlobals.h"
#include "System/EditorEngine.h"
#include "System/InputKeyConv.h"
#include "System/WindowEvent.h"
#include "EngineLoop.h"

// Widgets
#include "Widgets/ViewportWidget.h"
#include "Widgets/LogWidget.h"
#include "Widgets/ActorPropertiesWidget.h"
#include "Widgets/ExploerLevelWidget.h"
#include "Widgets/ContentBrowserWidget.h"

/**
 * @ingroup WorldEd
 * Create widget in ADS dock panel
 * 
 * @param InContainerWidget ADS container widget
 * @param InDropArea Area for drop created dock panel
 * @param InWidget Addable widget in dock panel
 * @param InUniqueName Unique name of dock panel for ID
 * @param InTitle Title of dock panel
 * @param InSectionWidget Add dock panel to other dock panel. If InSectionWidget is nullptr - dock panel not adding to other panel
 * @return Return pointer to created section widget of new dock panel
 */
FORCEINLINE ADS_NS::SectionWidget* CreateWidget( ADS_NS::ContainerWidget* InContainerWidget, ADS_NS::DropArea InDropArea, QWidget* InWidget, const QString& InUniqueName, const QString& InTitle, ADS_NS::SectionWidget* InSectionWidget = nullptr )
{
	ADS_NS::SectionContent::RefPtr			refPtr = ADS_NS::SectionContent::newSectionContent( InUniqueName, InContainerWidget, new QLabel( InTitle ), InWidget );
	return InContainerWidget->addSectionContent( refPtr, InSectionWidget, InDropArea );
}

WeMainWindow::WeMainWindow( QWidget* InParent /* = nullptr */ ) 
	: QMainWindow( InParent )
	, ui( new Ui::MainWindow() )
	, logWidget( nullptr )
{
	// Init Qt UI
	ui->setupUi( this );
	setWindowTitle( QString::fromStdWString( GEditorEngine->GetEditorName() ) );
	setCentralWidget( &containerWidget );

	// Create widgets of editor
	logWidget = new WeLogWidget();
	ADS_NS::SectionWidget*		sectionWidget = nullptr;

	sectionWidget = CreateWidget( &containerWidget, ADS_NS::CenterDropArea, new WeViewportWidget(), "#viewport", "Scene" );
	sectionWidget = CreateWidget( &containerWidget, ADS_NS::RightDropArea, new WeExploerLevelWidget(), "#exploer_level", "Exploer level" );
	sectionWidget = CreateWidget( &containerWidget, ADS_NS::BottomDropArea, new WeActorPropertiesWidget(), "#actor_properties", "Actor properties", sectionWidget );
	sectionWidget = CreateWidget( &containerWidget, ADS_NS::CenterDropArea, new WeContentBrowserWidget( FString::Format( TEXT( "%s/Content/" ), appBaseDir().c_str() ).c_str() ), "#content_browser", "Content browser", sectionWidget);
	sectionWidget = CreateWidget( &containerWidget, ADS_NS::CenterDropArea, logWidget, "#log", "Logs", sectionWidget );

	// Update text in menu action 'About'
	ui->actionAbout->setText( QString::fromStdWString( FString::Format( TEXT( "About %s" ), GEditorEngine->GetEditorName().c_str() ) ) );

	// Start timer for tick engine
	connect( &timerTick, &QTimer::timeout, this, &WeMainWindow::OnTickLE );
	timerTick.start( 1.f );
}

WeMainWindow::~WeMainWindow()
{
	delete ui;
	delete logWidget;
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