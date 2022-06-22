/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EXPLOERLEVELMODEL_H
#define EXPLOERLEVELMODEL_H

#include <qabstractitemmodel.h>

#include "Misc/Types.h"
#include "WorldEd.h"

/**
 * @ingroup WorldEd
 * @brief Model view for exploer level
 */
class WeExploerLevelModel : public QAbstractItemModel
{
public:
	/**
	 * @brief Constructor
	 * @param InParent		Parent
	 */
	WeExploerLevelModel( QObject* InParent = nullptr );

	/**
	 * @brief Destructor
	 */
	~WeExploerLevelModel();

	/**
	 * @brief Get index of item
	 */
	virtual QModelIndex index( int InRow, int InColumn, const QModelIndex& InParent = QModelIndex() ) const override;

	/**
	 * @brief Get index of parent
	 */
	virtual QModelIndex parent( const QModelIndex& InIndex ) const override;

	/**
	 * @brief Get row count
	 */
	virtual int rowCount( const QModelIndex& InParent = QModelIndex() ) const override;

	/**
	 * @brief Get column count
	 */
	virtual int columnCount( const QModelIndex& InParent = QModelIndex() ) const override;

	/**
	 * @brief Get data
	 */
	virtual QVariant data( const QModelIndex& InIndex, int InRole ) const override;

	/**
	 * @brief Insert rows
	 */
	virtual bool insertRows( int InRow, int InCount, const QModelIndex& InParent = QModelIndex() ) override;

	/**
	 * @brief Remove rows
	 */
	virtual bool removeRows( int InRow, int InCount, const QModelIndex& InParent = QModelIndex() ) override;

private:
	/**
	 * @brief Event called when actors spawned
	 */
	void OnActorsSpawned( const std::vector<ActorRef_t>& InActors );

	/**
	 * @brief Event called when actors destroyed
	 */
	void OnActorsDestroyed( const std::vector<ActorRef_t>& InActors );

	uint32													numRows;				/**< Number created rows */
	SEditorDelegates::COnActorsSpawned::DelegateType_t*		actorsSpawnedHandle;	/**< Handle delegate of actors spawned */
	SEditorDelegates::COnActorsDestroyed::DelegateType_t*	actorsDestroyedHandle;	/**< Handle delegate of actors destroyed */
};

#endif // !EXPLOERLEVELMODEL_H