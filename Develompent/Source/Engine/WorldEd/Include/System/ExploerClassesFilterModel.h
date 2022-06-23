/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EXPLOERCLASSESFILTERMODEL_H
#define EXPLOERCLASSESFILTERMODEL_H

#include <qsortfilterproxymodel.h>
#include "Core.h"

 /**
  * @ingroup WorldEd
  * @brief Model view for exploer classes
  */
class WeExploerClassesFilterModel : public QSortFilterProxyModel
{
public:
	/**
	 * @brief Constructor
	 * @param InParent		Parent
	 */
	WeExploerClassesFilterModel( QObject* InParent = nullptr );

	/**
	 * @brief Destructor
	 */
	virtual ~WeExploerClassesFilterModel();

	/**
	 * @brief Set filter pattern
	 * @param InFilterPattern	Filter pattern
	 */
	FORCEINLINE void SetFilterPattern( const QString& InFilterPattern )
	{
		filterPatter.setPattern( InFilterPattern );
		invalidateFilter();
	}

	/**
	 * @brief Get filter pattern
	 * @return Return current filter pattern
	 */
	FORCEINLINE const QString& GetFilterPattern() const
	{
		return filterPatter.pattern();
	}

protected:
	/**
	 * @brief Filter accepts column
	 * 
	 * @param InSourceColumn	Column index
	 * @param InSourceParent	Parent
	 * @return Return TRUE if filter accepts, else return FALSE
	 */
	bool filterAcceptsColumn( int InSourceColumn, const QModelIndex& InSourceParent ) const;

	/**
	 * @brief Filter accepts row
	 * 
	 * @param InSourceRow		Row index
	 * @param InSourceParent	Parent
	 * @return Return TRUE if filter accepts, else return FALSE
	 */
	bool filterAcceptsRow( int InSourceRow, const QModelIndex& InSourceParent ) const;

	QRegExp		filterPatter;		/**< Filter pattern */
};

#endif // !EXPLOERCLASSESFILTERMODEL_H