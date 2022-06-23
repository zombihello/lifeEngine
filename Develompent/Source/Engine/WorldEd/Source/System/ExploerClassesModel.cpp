#include "Containers/StringConv.h"
#include "System/ExploerClassesModel.h"

WeExploerClassesModel::WeClassNode::WeClassNode( CClass* InClass /*= nullptr*/ )
	: id( 0 )
	, lclass( InClass )
	, parent( nullptr )
{}

WeExploerClassesModel::WeClassNode::~WeClassNode()
{
	// Delete all child nodes
	for ( auto itNode = childs.begin(), itNodeEnd = childs.end(); itNode != itNodeEnd; ++itNode )
	{
		delete *itNode;
	}
}

void WeExploerClassesModel::WeClassNode::AddChild( WeClassNode* InNode )
{
	check( InNode );

	// If parent this node, do nothing
	if ( InNode->parent == this )
	{
		return;
	}

	// If parent exist, we remove from parent this node
	if ( InNode->parent )
	{
		InNode->parent->RemoveChild( InNode );
	}

	// Add to current node this child
	InNode->parent	= this;
	InNode->id		= childs.size();
	childs.push_back( InNode );
}

void WeExploerClassesModel::WeClassNode::RemoveChild( WeClassNode* InNode, bool InIsDeleteNode /*= false*/ )
{
	check( InNode );

	// If parent not this node, do nothing
	if ( InNode->parent != this )
	{
		return;
	}

	// Find node and remove he
	for ( auto itNode = childs.begin(), itNodeEnd = childs.end(); itNode != itNodeEnd; ++itNode )
	{
		if ( *itNode == InNode )
		{
			childs.erase( itNode );
			break;
		}
	}

	// If need delete child node, do it!
	if ( InIsDeleteNode )
	{
		delete InNode;
	}
	// Else we reset parent in InNode
	else
	{
		InNode->parent	= nullptr;
		InNode->id		= 0;
	}
}

WeExploerClassesModel::WeClassNode* WeExploerClassesModel::WeClassNode::Find( CClass* InClass, bool InIsRecursive /*= true*/ ) const
{
	if ( lclass == InClass )
	{
		return const_cast<WeClassNode*>( this );
	}

	// Find class node in child elements
	WeClassNode*		result = nullptr;
	for ( auto itNode = childs.begin(), itNodeEnd = childs.end(); itNode != itNodeEnd && !result; ++itNode )
	{
		WeClassNode*	node = *itNode;

		// We've found the right node!
		if ( node->lclass == InClass )
		{
			result = node;
			break;
		}

		// Else if we find node recusrive - look in child nodes
		if ( InIsRecursive )
		{
			result = node->Find( InClass, InIsRecursive );
		}
	}

	return result;
}

WeExploerClassesModel::WeClassNode* WeExploerClassesModel::WeClassNode::GetChild( uint32 InIndex ) const
{
	check( InIndex >= 0 && InIndex < childs.size() );

	uint32		index = 0;
	for ( auto itNode = childs.begin(), itNodeEnd = childs.end(); itNode != itNodeEnd; ++itNode, ++index )
	{
		if ( index == InIndex )
		{
			return *itNode;
		}
	}

	return nullptr;
}


WeExploerClassesModel::WeExploerClassesModel( CClass* InBaseClass, QObject* InParent /*= nullptr*/ )
	: QAbstractItemModel( InParent )
	, root( new WeClassNode() )
{
	// Add base class to tree
	WeClassNode*		baseClassNode = new WeClassNode( InBaseClass );
	root->AddChild( baseClassNode );

	// Build tree of child classes of InBaseClass
	const std::unordered_map<std::wstring, const CClass*>&		classesTable = CClass::StaticGetRegisteredClasses();
	for ( auto itClass = classesTable.begin(), itClassEnd = classesTable.end(); itClass != itClassEnd; ++itClass )
	{
		CClass*		lclass = const_cast<CClass*>( itClass->second );

		// If current class is no child of InBaseClass, we skip it
		if ( !lclass->IsA( InBaseClass ) )
		{
			continue;
		}

		// Iterate over all class ancestors and add nodes
		WeClassNode*	newClassNode = nullptr;
		for ( CClass* tempClass = lclass; tempClass; tempClass = tempClass->GetSuperClass() )
		{
			WeClassNode*	classNode = baseClassNode->Find( tempClass );
			
			// If node already exist in tree, skip
			if ( classNode && classNode->GetClass() == lclass )
			{
				break;
			}
			// If we find parent node, add child to him
			else if ( classNode )
			{
				classNode->AddChild( newClassNode );
				break;
			}
			// Else we make new node for tempClass
			else
			{
				WeClassNode*	tempNewClassNode = newClassNode;
				newClassNode = new WeClassNode( tempClass );
				if ( tempNewClassNode )
				{
					newClassNode->AddChild( tempNewClassNode );
				}
			}
		}
	}
}

WeExploerClassesModel::~WeExploerClassesModel()
{
	delete root;
}

QModelIndex WeExploerClassesModel::index( int InRow, int InColumn, const QModelIndex& InParent /*= QModelIndex()*/ ) const
{
	// Get child node by index row from parent
	WeClassNode*	parentNode	= GetClassNode( InParent );
	WeClassNode*	childNode	= parentNode->GetNumChilds() > 0 ? parentNode->GetChild( InRow ) : nullptr;

	// If exist child node, we create and return new index
	if ( childNode )
	{
		return createIndex( InRow, InColumn, childNode );
	}
	// Else we returning invalid index
	else
	{
		return QModelIndex();
	}
}

QModelIndex WeExploerClassesModel::parent( const QModelIndex& InIndex ) const
{
	// If index is node valid, return invalid model index
	if ( !InIndex.isValid() )
	{
		return QModelIndex();
	}

	// Get parent node from child
	WeClassNode*	childNode	= GetClassNode( InIndex );
	WeClassNode*	parentNode	= childNode->GetParent();

	// If parent node is root, we returning invalid index
	if ( parentNode == root )
	{
		return QModelIndex();
	}

	// Else we returning new index
	return createIndex( parentNode->GetID(), 0, parentNode );
}

int WeExploerClassesModel::rowCount( const QModelIndex& InParent /*= QModelIndex()*/ ) const
{
	WeClassNode*	parentNode = GetClassNode( InParent );
	check( parentNode );
	return parentNode->GetNumChilds();
}

int WeExploerClassesModel::columnCount( const QModelIndex& InParent /*= QModelIndex()*/ ) const
{
	return 1;
}

QVariant WeExploerClassesModel::data( const QModelIndex& InIndex, int InRole ) const
{
	// If index is not valid, we return empty QVariant
	if ( !InIndex.isValid() )
	{
		return QVariant();
	}

	// Else we getting from index class node
	WeClassNode*	classNode = GetClassNode( InIndex );
	check( classNode );

	// Getting meta info about class
	CClass*			lclass = classNode->GetClass();
	check( lclass );

	switch ( InRole )
	{
	case Qt::DisplayRole:
	case Qt::EditRole:
		return TCHAR_TO_ANSI( lclass->GetName().c_str() );
	}

	return QVariant();
}

bool WeExploerClassesModel::insertRows( int InRow, int InCount, const QModelIndex& InParent /*= QModelIndex()*/ )
{
	beginInsertRows( InParent, InRow, InRow + InCount - 1 );
	endInsertRows();
	return true;
}

bool WeExploerClassesModel::removeRows( int InRow, int InCount, const QModelIndex& InParent /*= QModelIndex()*/ )
{
	if ( InCount <= 0 )
	{
		return false;
	}

	beginRemoveRows( InParent, InRow, InRow + InCount - 1 );
	endRemoveRows();
	return true;
}