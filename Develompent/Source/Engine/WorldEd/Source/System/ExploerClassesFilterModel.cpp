#include "System/ExploerClassesFilterModel.h"

WeExploerClassesFilterModel::WeExploerClassesFilterModel( QObject* InParent /*= nullptr*/ )
	: QSortFilterProxyModel( InParent )
{
	filterPatter.setPattern( "*" );
	filterPatter.setPatternSyntax( QRegExp::Wildcard );
    setRecursiveFilteringEnabled( true );
}

WeExploerClassesFilterModel::~WeExploerClassesFilterModel()
{}

bool WeExploerClassesFilterModel::filterAcceptsColumn( int InSourceColumn, const QModelIndex& InSourceParent ) const
{
	return true;
}

bool WeExploerClassesFilterModel::filterAcceptsRow( int InSourceRow, const QModelIndex& InSourceParent ) const
{
    // Get source-model index for current row
    QModelIndex         sourceIndex = sourceModel()->index( InSourceRow, filterKeyColumn(), InSourceParent );
    if ( sourceIndex.isValid() )
    {
        // Check current index itself
        QString     key = sourceModel()->data( sourceIndex, filterRole() ).toString();
        return filterPatter.exactMatch( key );
    }

    return QSortFilterProxyModel::filterAcceptsRow( InSourceRow, InSourceParent );
}