#include "Misc/EngineGlobals.h"
#include "Containers/StringConv.h"
#include "Actors/Actor.h"
#include "System/World.h"
#include "System/ExploerLevelModel.h"

WeExploerLevelModel::WeExploerLevelModel( QObject* InParent /*= nullptr*/ )
	: QAbstractItemModel( InParent )
	, numItems( 0 )
{
	// Subscribe to event when world is loaded for updating exploer level in UI
	worldLoadedHandle = SEditorDelegates::onWorldLoaded.Add( std::bind( &WeExploerLevelModel::OnWorldLoaded, this ) );
}

WeExploerLevelModel::~WeExploerLevelModel()
{
	// Unscribe from event when world is loaded
	SEditorDelegates::onWorldLoaded.Remove( worldLoadedHandle );
}

QModelIndex WeExploerLevelModel::index( int InRow, int InColumn, const QModelIndex& InParent /*= QModelIndex()*/ ) const
{
	return createIndex( InRow, InColumn, nullptr );
}

QModelIndex WeExploerLevelModel::parent( const QModelIndex& InIndex ) const
{
	return QModelIndex();
}

int WeExploerLevelModel::rowCount( const QModelIndex& InParent /*= QModelIndex()*/ ) const
{
	return numItems;
}

int WeExploerLevelModel::columnCount( const QModelIndex& InParent /*= QModelIndex()*/ ) const
{
	return 1;
}

QVariant WeExploerLevelModel::data( const QModelIndex& InIndex, int InRole ) const
{
	if ( !InIndex.isValid() )
	{
		return QVariant();
	}

	ActorRef_t		actor = GWorld->GetActor( InIndex.row() );

	switch ( InRole )
	{
	case Qt::DisplayRole:
	case Qt::EditRole:
		return TCHAR_TO_ANSI( actor->GetName() );

	case Qt::DecorationRole:
		return QPixmap( TCHAR_TO_ANSI( ( appBaseDir() + actor->GetActorIcon() ).c_str() ) );
	}

	return QVariant();
}

bool WeExploerLevelModel::insertRows( int InRow, int InCount, const QModelIndex& InParent /*= QModelIndex()*/ )
{
	beginInsertRows( InParent, InRow, InRow + InCount - 1 );
	endInsertRows();
	return true;
}

bool WeExploerLevelModel::removeRows( int InRow, int InCount, const QModelIndex& InParent /*= QModelIndex()*/ )
{
	if ( InCount <= 0 )
	{
		return false;
	}

	beginRemoveRows( InParent, InRow, InRow + InCount - 1 );
	endRemoveRows();
	return true;
}

void WeExploerLevelModel::OnWorldLoaded()
{
	removeRows( 0, numItems, QModelIndex() );
	
	numItems = GWorld->GetNumActors();
	if ( numItems <= 0 )
	{
		return;
	}

	insertRows( 0, numItems, QModelIndex() );
}