#include <qpixmap.h>

#include "System/PackageModel.h"
#include "Containers/StringConv.h"

WePackageModel::WePackageModel( QObject* InParent /* = nullptr */ )
	: QAbstractItemModel( InParent )
	, numItems( 0 )
{}

WePackageModel::~WePackageModel()
{}

QModelIndex WePackageModel::index( int InRow, int InColumn, const QModelIndex& InParent /* = QModelIndex() */ ) const
{
	return createIndex( InRow, InColumn, nullptr );
}

QModelIndex WePackageModel::parent( const QModelIndex& InIndex ) const
{
	return QModelIndex();
}

int WePackageModel::rowCount( const QModelIndex& InParent /* = QModelIndex() */ ) const
{
	return numItems;
}

int WePackageModel::columnCount( const QModelIndex& InParent /* = QModelIndex() */ ) const
{
	return 1;
}

QVariant WePackageModel::data( const QModelIndex& InIndex, int InRole ) const
{
	if ( !InIndex.isValid() )
	{
		return QVariant();
	}

	FAssetInfo		assetInfo;
	package->GetAssetInfo( InIndex.row(), assetInfo );

	switch ( InRole )
	{
	case Qt::DisplayRole:
	case Qt::EditRole:
		if ( assetInfo.data && ( FAsset* )assetInfo.data->IsDirty() )		// If asset loaded we check is changed in memory. In successed case mark this asset by star
		{
			return TCHAR_TO_ANSI( ( assetInfo.name + TEXT( "*" ) ).c_str() );
		}
		else
		{
			return TCHAR_TO_ANSI( assetInfo.name.c_str() );
		}

	case Qt::DecorationRole:
		switch ( assetInfo.type )
		{
		case AT_Texture2D:		return QPixmap( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/Asset_Texture.png" ) ).c_str() ) );			
		case AT_StaticMesh:		return QPixmap( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/Asset_StaticMesh.png" ) ).c_str() ) );
		case AT_Material:		return QPixmap( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/Asset_Material.png" ) ).c_str() ) );
		case AT_Script:			return QPixmap( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/Asset_Script.png" ) ).c_str() ) );
		default:				return QPixmap( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Icons/Asset_Unknown.png" ) ).c_str() ) );
		}
	}

	return QVariant();
}

bool WePackageModel::insertRows( int InRow, int InCount, const QModelIndex& InParent /* = QModelIndex() */ )
{
	beginInsertRows( InParent, InRow, InRow + InCount - 1 );
	endInsertRows();
	return true;
}

bool WePackageModel::removeRows( int InRow, int InCount, const QModelIndex& InParent /* = QModelIndex() */ )
{
	if ( InCount <= 0 )
	{
		return false;
	}

	beginRemoveRows( InParent, InRow, InRow + InCount - 1 );
	endRemoveRows();
	return true;
}

void WePackageModel::Refresh()
{
	removeRows( 0, numItems, QModelIndex() );
	numItems = package ? package->GetNumAssets() : 0;
	
	if ( !package || numItems <= 0 )
	{
		return;
	}

	insertRows( 0, numItems, QModelIndex() );
}

void WePackageModel::SetPackage( FPackage* InPackage )
{
	if ( package && !package->IsEmpty() )
	{
		removeRows( 0, numItems, QModelIndex() );
	}

	package = InPackage;
	numItems = package ? package->GetNumAssets() : 0;

	if ( package && !package->IsEmpty() )
	{
		insertRows( 0, package->GetNumAssets(), QModelIndex() );
	}
}