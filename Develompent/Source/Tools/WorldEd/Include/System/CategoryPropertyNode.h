/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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