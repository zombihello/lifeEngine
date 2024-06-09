/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ACTORCLASSESWINDOW_H
#define ACTORCLASSESWINDOW_H

#include <list>

#include "Reflection/Class.h"
#include "ImGUI/ImGUIEngine.h"

/**
 * @ingroup WorldEd
 * @brief Window of actor classes
 */
class CActorClassesWindow : public CImGUILayer
{
public:
	friend class CClassNode;

	/**
	 * @brief Constructor
	 *
	 * @param InName	Window name
	 */
	CActorClassesWindow( const std::wstring& InName, CClass* InBaseClass = nullptr );

	/**
	 * @brief Destructor
	 */
	~CActorClassesWindow();

	/**
	 * @brief Get current class
	 * @return Return selected class. If isn't select will return root class
	 */
	FORCEINLINE CClass* GetCurrentClass() const
	{
		std::vector<CClass*>		classes;
		root->GetSelectedClasses( classes );
		return !classes.empty() ? classes[0] : root->GetClass();
	}

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

private:
	/**
	 * @brief Class node container
	 */
	class CClassNode
	{
	public:
		/**
		 * @brief Constructor
		 * 
		 * @param InOwner	Owner
		 * @param InClass	Class
		 */
		CClassNode( CActorClassesWindow* InOwner, CClass* InClass = nullptr );

		/**
		 * @brief Destructor
		 */
		~CClassNode();

		/**
		 * @brief Add child node
		 * @param InNode	Node
		 */
		void AddChild( CClassNode* InNode );

		/**
		 * @brief Remove child node
		 * @param InNode			Node
		 * @param InIsDeleteNode	Is need delete child node
		 */
		void RemoveChild( CClassNode* InNode, bool InIsDeleteNode = false );

		/**
		 * @brief Update tree node
		 */
		void Tick();

		/**
		 * @brief Find node by class
		 *
		 * @param InClass			Class
		 * @param InIsRecursive		Is need find node in children
		 * @return Return class node if succeed found, otherwise will return NULL
		 */
		CClassNode* Find( CClass* InClass, bool InIsRecursive = true ) const;

		/**
		 * @brief Set select
		 *
		 * @param InIsSelected			Is selected node
		 * @param InApplyToChildren		Is need apply to children
		 */
		FORCEINLINE void SetSelect( bool InIsSelected, bool InApplyToChildren = true )
		{
			bSelected = InIsSelected;
			if ( InApplyToChildren )
			{
				for ( auto it = children.begin(), itEnd = children.end(); it != itEnd; ++it )
				{
					( *it )->SetSelect( InIsSelected, InApplyToChildren );
				}
			}
		}

		/**
		 * @brief Is selected node
		 * @return Return TRUE if node is selected
		 */
		FORCEINLINE bool IsSelected() const
		{
			return bSelected;
		}

		/**
		 * @brief Get class
		 * @return Return class, if not set return NULL
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
		CClassNode* GetChild( uint32 InIndex ) const;

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
		 * @return Return parent node. If not exist return NULL
		 */
		FORCEINLINE CClassNode* GetParent() const
		{
			return parent;
		}

		/**
		 * @brief Get number children
		 * @return Return number of children
		 */
		FORCEINLINE uint32 GetNumChildren() const
		{
			return children.size();
		}

		/**
		 * @brief Get array of selected classes
		 *
		 * @param OutSelectedNodes		Output. Array of selected nodes
		 * @param InIsIgnoreChildren	Is need ignore children node if current not selected
		 */
		void GetSelectedClasses( std::vector<CClass*>& OutSelectedNodes, bool InIsIgnoreChildren = false ) const;

	private:
		/**
		 * @brief Process events
		 */
		void ProcessEvents();

		bool					bSelected;	/**< Is selected node */
		uint32					id;			/**< ID node in parent list */
		CClass*					lclass;		/**< Class */
		CClassNode*				parent;		/**< Parent node */
		std::list<CClassNode*>	children;	/**< List of child nodes */
		CActorClassesWindow*	owner;		/**< Owner */
	};

	CClassNode*					root;		/**< Root node */
};

#endif // !ACTORCLASSESWINDOW_H