#include "Misc/EngineGlobals.h"
#include "Containers/StringConv.h"
#include "Actors/Actor.h"
#include "System/World.h"
#include "System/ExploerLevelModel.h"

WeExploerLevelModel::WeExploerLevelModel( QObject* InParent /*= nullptr*/ )
	: QAbstractItemModel( InParent )
	, bDirtyMapActorsIndex( true )
	, numRows( 0 )
	, actorsSpawnedHandle( nullptr )
	, actorsDestroyedHandle( nullptr )
{
	// Subscribe to event when actors spawned and destroyed
	actorsSpawnedHandle		= SEditorDelegates::onActorsSpawned.Add(	std::bind( &WeExploerLevelModel::OnActorsSpawned, this, std::placeholders::_1 )		);
	actorsDestroyedHandle	= SEditorDelegates::onActorsDestroyed.Add(	std::bind( &WeExploerLevelModel::OnActorsDestroyed, this, std::placeholders::_1 )	);
}

WeExploerLevelModel::~WeExploerLevelModel()
{
	// Unscribe from event when actors spawned and destroyed
	SEditorDelegates::onActorsSpawned.Remove( actorsSpawnedHandle );
	SEditorDelegates::onActorsDestroyed.Remove( actorsDestroyedHandle );
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
	return numRows;
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

QModelIndex WeExploerLevelModel::ActorToModelIndex( ActorRef_t InActor ) const
{
	// Update map for convert actor to index if need
	if ( bDirtyMapActorsIndex )
	{
		bDirtyMapActorsIndex = false;
		mapActorsIndex.clear();
		
		for ( uint32 index = 0, count = GWorld->GetNumActors(); index < count; ++index )
		{
			mapActorsIndex.insert( std::make_pair( GWorld->GetActor( index ), index ) );
		}
	}

	auto	itFind = mapActorsIndex.find( InActor );
	if ( itFind != mapActorsIndex.end() )
	{
		return index( itFind->second, 0, QModelIndex() );
	}

	return QModelIndex();
}

void WeExploerLevelModel::OnActorsSpawned( const std::vector<ActorRef_t>& InActors )
{
	if ( !InActors.empty() )
	{
		bDirtyMapActorsIndex			= true;
		uint32		numSpawnedActors	= InActors.size();
		insertRows( numRows, numSpawnedActors, QModelIndex() );
		numRows += numSpawnedActors;
	}
}

void WeExploerLevelModel::OnActorsDestroyed( const std::vector<ActorRef_t>& InActors )
{
	if ( !InActors.empty() )
	{
		bDirtyMapActorsIndex				= true;
		uint32		numDestroyedActors		= InActors.size();
		removeRows( 0, numDestroyedActors, QModelIndex() );
		numRows -= numDestroyedActors;

		// Remove from map references to destroyed actors
		for ( uint32 index = 0, count = InActors.size(); index < count; ++index )
		{
			mapActorsIndex.erase( InActors[index] );
		}
	}
}