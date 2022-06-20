/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PACKAGEMODEL_H
#define PACKAGEMODEL_H

#include <qabstractitemmodel.h>
#include "System/Package.h"

/**
 * @ingroup WorldEd
 * Qt package model
 */
class WePackageModel : public QAbstractItemModel
{
public:
	/**
	 * Constructor
	 */
	WePackageModel( QObject* InParent = nullptr );

	/**
	 * Destructor
	 */
	~WePackageModel();

	/**
	 * Get index of item
	 */
	virtual QModelIndex index( int InRow, int InColumn, const QModelIndex& InParent = QModelIndex() ) const override;

	/**
	 * Get index of parent
	 */
	virtual QModelIndex parent( const QModelIndex& InIndex ) const override;

	/**
	 * Get row count
	 */
	virtual int rowCount( const QModelIndex& InParent = QModelIndex() ) const override;

	/**
	 * Get column count
	 */
	virtual int columnCount( const QModelIndex& InParent = QModelIndex() ) const override;

	/**
	 * Get data
	 */
	virtual QVariant data( const QModelIndex& InIndex, int InRole ) const override;
	
	/**
	 * Insert rows
	 */
	virtual bool insertRows( int InRow, int InCount, const QModelIndex& InParent = QModelIndex() ) override;
	
	/**
	 * Remove rows
	 */
	virtual bool removeRows( int InRow, int InCount, const QModelIndex& InParent = QModelIndex() ) override;

	/**
	 * Refresh package model
	 */
	void Refresh();

	/**
	 * Set selected package
	 * @param InPackage Package
	 */
	void SetPackage( CPackage* InPackage );

	/**
	 * Get selected package
	 * @return Return selected package. If not selected returning nullptr
	 */
	FORCEINLINE PackageRef_t GetPackage() const
	{
		return package;
	}

private:
	uint32				numItems;		/**< Number created items */
	PackageRef_t		package;		/**< Current package */
};

#endif // !PACKAGEMODEL_H