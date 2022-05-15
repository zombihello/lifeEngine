#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>

#include "Misc/Misc.h"
#include "Misc/WorldEdGlobals.h"
#include "Containers/StringConv.h"
#include "System/Package.h"
#include "System/EditorEngine.h"
#include "Widgets/SelectAssetWidget.h"
#include "Widgets/ContentBrowserWidget.h"
#include "Windows/MainWindow.h"

WeSelectAssetWidget::WeSelectAssetWidget( QWidget* InParent /* = nullptr */ )
	: QWidget( InParent )
	, horizontalLayout( nullptr )
	, lineEdit_assetReference( nullptr )
	, toolButton_pasteAsset( nullptr )
	, toolButton_browseAsset( nullptr )
	, toolButton_removeAsset( nullptr )
{
	InitUI();
}

WeSelectAssetWidget::~WeSelectAssetWidget()
{}

void WeSelectAssetWidget::InitUI()
{
	horizontalLayout		= new QHBoxLayout( this );
	lineEdit_assetReference = new QLineEdit( this );
	toolButton_pasteAsset	= new QToolButton( this );
	toolButton_browseAsset	= new QToolButton( this );
	toolButton_removeAsset	= new QToolButton( this );

	lineEdit_assetReference->setText( "None" );
	toolButton_pasteAsset->setToolTip( "Use asset selected in content browser" );
	toolButton_browseAsset->setToolTip( "Show asset in content browser" );
	toolButton_removeAsset->setToolTip( "Remove reference to asset" );

	toolButton_pasteAsset->setIcon( QIcon( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/CSW_PasteAsset.png" ) ).c_str() ) ) );
	toolButton_browseAsset->setIcon( QIcon( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/CSW_ShowAsset.png" ) ).c_str() ) ) );
	toolButton_removeAsset->setIcon( QIcon( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/CSW_RemoveAsset.png" ) ).c_str() ) ) );

	horizontalLayout->setContentsMargins( 0, 3, 0, 3 );
	horizontalLayout->setSpacing( 1 );
	horizontalLayout->addWidget( lineEdit_assetReference );
	horizontalLayout->addWidget( toolButton_pasteAsset );
	horizontalLayout->addWidget( toolButton_browseAsset );
	horizontalLayout->addWidget( toolButton_removeAsset );

	connect( lineEdit_assetReference, SIGNAL( returnPressed() ), this, SLOT( OnLineEditAssetReferenceReturnPressed() ) );
	connect( toolButton_removeAsset, SIGNAL( clicked() ), this, SLOT( OnRemoveAsset() ) );
	connect( toolButton_pasteAsset, SIGNAL( clicked() ), this, SLOT( OnPasteAssetFromContentBrowser() ) );
	connect( toolButton_browseAsset, SIGNAL( clicked() ), this, SLOT( OnShowAssetInContentBrowser() ) );
}

void WeSelectAssetWidget::ClearAssetReference( bool InIsEmitEvent /* = true */ )
{
	lineEdit_assetReference->setText( "None" );
	if ( InIsEmitEvent )
	{
		emit OnAssetReferenceChanged( TEXT( "" ) );
	}
}

void WeSelectAssetWidget::SetAssetReference( const std::wstring& InAssetReference, bool InIsEmitEvent /* = true */ )
{
	if ( InAssetReference.empty() )
	{
		ClearAssetReference( InIsEmitEvent );
		return;
	}

	lineEdit_assetReference->setText( QString::fromStdWString( InAssetReference ) );

	if ( InIsEmitEvent )
	{
		emit OnAssetReferenceChanged( InAssetReference );
	}
}

void WeSelectAssetWidget::OnLineEditAssetReferenceReturnPressed()
{
	emit OnAssetReferenceChanged( lineEdit_assetReference->text().toStdWString() );
}

void WeSelectAssetWidget::OnRemoveAsset()
{
	ClearAssetReference();
}

void WeSelectAssetWidget::OnPasteAssetFromContentBrowser()
{
	SetAssetReference( GEditorEngine->GetMainWindow()->GetContentBrowserWidget()->GetSelectedAssetReference() );
}

void WeSelectAssetWidget::OnShowAssetInContentBrowser()
{
	GEditorEngine->GetMainWindow()->GetContentBrowserWidget()->ShowAsset( lineEdit_assetReference->text().toStdWString() );
}