/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CATEGORYPROPERTYNODE_H
#define CATEGORYPROPERTYNODE_H

#include "System/PropertyNode.h"

/**
 * @ingroup WorldEd
 * @brief Category property node
 * sed for the highest level categories
 */
class CCategoryPropertyNode : public CPropertyNode
{
public:
	/**
	 * @brief Constructor
	 */
	CCategoryPropertyNode();

	/**
	 * @brief Destructor
	 */
	virtual ~CCategoryPropertyNode();

	/**
	 * @brief Method tick interface of a property node
	 * @param InItemWidthSpacing	Item spacing by width
	 */
	virtual void Tick( float InItemWidthSpacing = 0.f ) override;

	/**
	 * @brief Set category name
	 * @param InCategoryName	Category name
	 */
	FORCEINLINE void SetCategoryName( const CName& InCategoryName )
	{
		categoryName = InCategoryName;
	}

	/**
	 * @brief Get category name
	 * @return Return category name
	 */
	FORCEINLINE const CName& GetCategoryName() const
	{
		return categoryName;
	}

	/**
	 * @brief Interface function to get at the derived CCategoryPropertyNode class
	 * @return Return interface CCategoryPropertyNode if this class derived from CCategoryPropertyNode
	 */
	virtual CCategoryPropertyNode* GetCategoryNode() override;

protected:
	/**
	 * @brief Interface function for Creating Child Nodes
	 */
	virtual void InitChildNodes() override;

	CName		categoryName;	/**< Stored category name */
};

#endif // !CATEGORYPROPERTYNODE_H