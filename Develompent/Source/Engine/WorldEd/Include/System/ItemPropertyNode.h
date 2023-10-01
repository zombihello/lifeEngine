/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ITEMPROPERTYNODE_H
#define ITEMPROPERTYNODE_H

#include "System/PropertyNode.h"

/**
 * @ingroup WorldEd
 * @brief Item property node
 * Used for other non-category/object nodes
 */
class CItemPropertyNode : public CPropertyNode
{
public:
	/**
	 * @brief Constructor
	 */
	CItemPropertyNode();

	/**
	 * @brief Destructor
	 */
	virtual ~CItemPropertyNode();

	/**
	 * @brief Method tick interface of a property node
	 * @param InItemWidthSpacing	Item spacing by width
	 */
	virtual void Tick( float InItemWidthSpacing = 0.f ) override;

	/**
	 * @brief Get base address of the property
	 * Calculates the memory address for the data associated with this item's property.
	 * This is typically the value of a CProperty or a CObject address
	 *
	 * @param InBaseAddress		The base address to use as the starting point for the calculation. Typically the address of the object that contains this property
	 * @return Return a pointer to a CProperty value or CObject. For arrays, you'd cast this value to an std::vector<byte>*
	 */
	virtual byte* GetValueBaseAddress( byte* InBaseAddress ) const override;

	/**
	 * @brief Get base address of the property
	 * Calculates the memory address for the data associated with this item's property.
	 * For most properties, identical to GetValueBaseAddress, for items corresponding to dynamic array elements,
	 * the pointer returned will be the location for that element's data
	 *
	 * @param InBaseAddress		The base address to use as the starting point for the calculation. Typically the address of the object that contains this property or allocated array data
	 * @return Return a pointer to a CProperty value or CObject. For arrays, you'd cast this value to whatever type is the 'inner' for the dynamic array
	 */
	virtual byte* GetValueAddress( byte* InBaseAddress ) const override;

protected:
	/**
	 * @brief Interface function for Creating Child Nodes
	 */
	virtual void InitChildNodes() override;
};

#endif // !ITEMPROPERTYNODE_H