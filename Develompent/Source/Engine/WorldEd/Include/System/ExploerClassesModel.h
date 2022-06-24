/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EXPLOERCLASSESMODEL_H
#define EXPLOERCLASSESMODEL_H

#include <qabstractitemmodel.h>
#include <list>

#include "Misc/Class.h"

 /**
  * @ingroup WorldEd
  * @brief Model view for exploer classes
  */
class WeExploerClassesModel : public QAbstractItemModel
{
public:
	/**
	 * @brief Constructor
	 * @param InBaseClass	Show all child classes of InBaseClass class
	 * @param InParent		Parent
	 */
	WeExploerClassesModel( CClass* InBaseClass, QObject* InParent = nullptr );

	/**
	 * @brief Destructor
	 */
	~WeExploerClassesModel();

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

	/**
	 * @brief Get class by index
	 * 
	 * @param InIndex	Index
	 * @return Return pointer to class if valid index, else returning NULL
	 */
	FORCEINLINE CClass* GetClass( const QModelIndex& InIndex ) const
	{
		// If index is not valid, we returning NULL
		if ( !InIndex.isValid() )
		{
			return nullptr;
		}

		// Else we getting from index class node, and return class from him
		WeClassNode*	classNode = ( WeClassNode* )InIndex.internalPointer();
		return classNode ? classNode->GetClass() : nullptr;
	}

private:
	/**
	 * @brief Container of class node
	 */
	class WeClassNode
	{
	public:
		/**
		 * @brief Constructor
		 * 
		 * @param InClass	Class
		 */
		WeClassNode( CClass* InClass = nullptr );

		/**
		 * @brief Destructor
		 */
		~WeClassNode();

		/**
		 * @brief Add child node
		 * @param InNode	Node
		 */
		void AddChild( WeClassNode* InNode );

		/**
		 * @brief Remove child node
		 * @param InNode				Node
		 * @param InIsDeleteNode		Is need delete child node
		 */
		void RemoveChild( WeClassNode* InNode, bool InIsDeleteNode = false );

		/**
		 * @brief Find node by class
		 * 
		 * @param InClass			Class
		 * @param InIsRecursive		Is need find node in childs
		 * @return Return class node if succeed finded, else returning NULL
		 */
		WeClassNode* Find( CClass* InClass, bool InIsRecursive = true ) const;

		/**
		 * @brief Get class
		 * @return Return class, if not setted returning NULL
		 */
		FORCEINLINE CClass* GetClass() const
		{
			return lclass;
		}

		/**
		 * @brief Get child node by index
		 * 
		 * @param InIndex	Index
		 * @return Return child class node
		 */
		WeClassNode* GetChild( uint32 InIndex ) const;

		/**
		 * @brief Get ID node in parent list
		 * @return Return ID node in parent list
		 */
		FORCEINLINE uint32 GetID() const
		{
			return id;
		}

		/**
		 * @brief Get parent node
		 * @return Return parent node. If not exist returning NULL
		 */
		FORCEINLINE WeClassNode* GetParent() const
		{
			return parent;
		}

		/**
		 * @brief Get number childs
		 * @return Return number of childs
		 */
		FORCEINLINE uint32 GetNumChilds() const
		{
			return childs.size();
		}

	private:
		uint32					id;			/**< ID node in parent list */
		CClass*					lclass;		/**< Class */
		WeClassNode*			parent;		/**< Parent node */
		std::list<WeClassNode*>	childs;		/**< List of child nodes */
	};

	/**
	 * @brief Get class node from index
	 *
	 * @param InIndex	Index
	 * @return Return class node. If index is not valid return root node, if internal pointer in index is not valid returning NULL
	 */
	FORCEINLINE WeClassNode* GetClassNode( const QModelIndex& InIndex ) const
	{
		// If index is not valid we return root node
		if ( !InIndex.isValid() )
		{
			return root;
		}

		// If in index valid pointer to class node, returning it
		WeClassNode*		classNode = ( WeClassNode* )InIndex.internalPointer();
		if ( classNode )
		{
			return classNode;
		}

		// Else we return NULL
		return nullptr;
	}

	WeClassNode*		root;	/**< Root node */
};

#endif // !EXPLOERCLASSESMODEL_H