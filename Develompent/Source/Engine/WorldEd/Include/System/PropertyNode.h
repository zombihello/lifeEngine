/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PROPERTYNODE_H
#define PROPERTYNODE_H

#include <vector>
#include "Reflection/Property.h"

// Forward declarations
class CObjectPropertyNode;
class CCategoryPropertyNode;

/**
 * @ingroup WorldEd
 * @brief Base class for property node
 */
class CPropertyNode
{
public:
	/**
	 * @brief Constructor
	 */
	CPropertyNode();

	/**
	 * @brief Destructor
	 */
	virtual ~CPropertyNode();

	/**
	 * @brief Init tree node
	 * 
	 * @param InParentNode			Parent node in the hierarchy
	 * @param InProperty			The property this node is modifying
	 * @param InPropertyOffset		Property offset
	 * @param InArrayIndex			Array index
	 * @param InAllowChildren		Allows children to be created
	 */
	void InitNode( CPropertyNode* InParentNode, CProperty* InProperty, uint32 InPropertyOffset, uint32 InArrayIndex, bool InAllowChildren = true );

	/**
	 * @brief Rebuild children nodes
	 */
	void RebuildChildren();

	/**
	 * @brief Method tick interface of a property node
	 * @param InItemWidthSpacing	Item spacing by width
	 */
	virtual void Tick( float InItemWidthSpacing = 0.f );

	/**
	 * @brief Add child node
	 * @param InNode	Child node
	 */
	FORCEINLINE void AddChildNode( CPropertyNode* InNode )
	{
		childNodes.push_back( InNode );
	}

	/**
	 * @brief Find parent object item
	 * Follows the chain of items upwards until it finds the object node that houses this item
	 * 
	 * @return Return founded parent object item
	 */
	CObjectPropertyNode* FindObjectItemParent();

	/**
	 * @brief Interface function to get at the derived CObjectPropertyNode class
	 * @return Return interface CObjectPropertyNode if this class derived from CObjectPropertyNode
	 */
	virtual CObjectPropertyNode* GetObjectNode();

	/**
	 * @brief Interface function to get at the derived CCategoryPropertyNode class
	 * @return Return interface CCategoryPropertyNode if this class derived from CCategoryPropertyNode
	 */
	virtual CCategoryPropertyNode* GetCategoryNode();

	/**
	 * @brief Get address for all available objects
	 * 
	 * @param InNode		The property to get objects from
	 * @param OutAddresses	Storage array for all of the objects' addresses
	 * @return Return TRUE if successful filled addresses, otherwise returns FALSE
	 */
	virtual bool GetReadAddress( CPropertyNode* InNode, std::vector<byte*>& OutAddresses ) const;

	/**
	 * @brief Get base address of the property
	 * Calculates the memory address for the data associated with this item's property.
	 * This is typically the value of a CProperty or a CObject address
	 *
	 * @param InBaseAddress		The base address to use as the starting point for the calculation. Typically the address of the object that contains this property
	 * @return Return a pointer to a CProperty value or CObject. For arrays, you'd cast this value to an std::vector<byte>*
	 */
	virtual byte* GetValueBaseAddress( byte* InBaseAddress ) const;

	/**
	 * @brief Get base address of the property
	 * Calculates the memory address for the data associated with this item's property.
	 * For most properties, identical to GetValueBaseAddress, for items corresponding to dynamic array elements, 
	 * the pointer returned will be the location for that element's data
	 * 
	 * @param InBaseAddress		The base address to use as the starting point for the calculation. Typically the address of the object that contains this property or allocated array data
	 * @return Return a pointer to a CProperty value or CObject. For arrays, you'd cast this value to whatever type is the 'inner' for the dynamic array
	 */
	virtual byte* GetValueAddress( byte* InBaseAddress ) const;

	/**
	 * @brief Get parent node
	 * @return Return parent node. If isn't exist return NULL
	 */
	FORCEINLINE CPropertyNode* GetParentNode() const
	{
		return parentNode;
	}

	/**
	 * @brief Get property
	 * @return Return property of the node
	 */
	FORCEINLINE CProperty* GetProperty() const
	{
		return property;
	}

	/**
	 * @brief Get property offset
	 * @return Return offset to the properties data
	 */
	FORCEINLINE uint32 GetPropertyOffset() const
	{
		return propertyOffset;
	}

	/**
	 * @brief Get array index
	 * @return Return array index
	 */
	FORCEINLINE uint32 GetArrayIndex() const
	{
		return arrayIndex;
	}

	/**
	 * @brief Get number of child nodes
	 * @retrun Return number of child nodes
	 */
	FORCEINLINE uint32 GetNumChildNodes() const
	{
		return childNodes.size();
	}

	/**
	 * @brief Get child node by index
	 * 
	 * @param InIndex	Child node index
	 * @retrun Return child node
	 */
	FORCEINLINE CPropertyNode* GetChildNode( uint32 InIndex ) const
	{
		Assert( InIndex >= 0 && InIndex < childNodes.size() );
		return childNodes[InIndex];
	}

protected:
	/**
	 * @brief Interface function for init node (by default is empty)
	 */
	virtual void InitNodeInternal();

	/**
	 * @brief Interface function for creating child nodes
	 */
	virtual void InitChildNodes() = 0;

	/**
	 * @brief Destroys all node within the hierarchy
	 */
	void DestroyTree();

	/**
	 * @brief Method tick interface of a property node's children
	 * @param InItemWidthSpacing	Item spacing by width
	 */
	void TickChildren( float InItemWidthSpacing = 0.f );

	/**
	 * @brief Notify all objects about post change event of property
	 * @param InPropertyChangedEvent	Property changed event
	 */
	void NotifyPostChange( const PropertyChangedEvenet& InPropertyChangedEvent );

	CPropertyNode*					parentNode;		/**< Parent node */
	std::vector<CPropertyNode*>		childNodes;		/**< List of all child nodes */
	CProperty*						property;		/**< The property being displayed/edited */
	uint32							propertyOffset;	/**< Offset to the properties data */
	uint32							arrayIndex;		/**< Array index */
};

#endif // !PROPERTYNODE_H