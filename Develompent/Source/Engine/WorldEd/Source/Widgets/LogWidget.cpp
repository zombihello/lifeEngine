#include "Logger/LoggerMacros.h"
#include "Containers/String.h"
#include "Core.h"
#include "Widgets/LogWidget.h"
#include "ui_LogWidget.h"

WeLogWidget::WeLogWidget( QWidget* InParent /* = nullptr */ )
	: QWidget( InParent )
	, ui( new Ui::WeLogWidget() )
{
	// Init Qt UI
	ui->setupUi( this );
}

WeLogWidget::~WeLogWidget()
{
	delete ui;
}

void WeLogWidget::Print( ELogType InLogType, const tchar* InMessage )
{
	std::wstring		beginColorMessage;
	std::wstring		endColorMessage = TEXT( "<\\font>" );
	switch ( InLogType )
	{
	case LT_Warning:
		beginColorMessage = TEXT( "<font color = orange>" );
		break;

	case LT_Error:
		beginColorMessage = TEXT( "<font color = red>" );
		break;

	default:
		beginColorMessage = TEXT( "" );
		endColorMessage = TEXT( "" );
		break;
	}
	
	ui->textBrowser->append( QString::fromStdWString( FString::Format( ( beginColorMessage + TEXT( "%s" ) + endColorMessage ).c_str(), InMessage ) ) );
}