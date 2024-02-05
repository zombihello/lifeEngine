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

#ifndef OBJECTPROPERTYNODE_H
#define OBJECTPROPERTYNODE_H

#include <unordered_set>
#include "System/PropertyNode.h"

/**
 * @ingroup WorldEd
 * @brief Object property node
 * Used for the root and various sub-nodes
 */
class CObjectPropertyNode : public CPropertyNode
{
public:
	/**
	 * @brief Constructor
	 */
	CObjectPropertyNode();

	/**
	 * @brief Destructor
	 */
	virtual ~CObjectPropertyNode();

	/**
	 * @brief Add a new object to the list
	 * @param InObject		A new object
	 */
	FORCEINLINE void AddObject( CObject* InObject )
	{
		objects.insert( InObject );
	}

	/**
	 * @brief Remove an object from the list
	 * @param InObject		A new object
	 */
	FORCEINLINE void RemoveObject( CObject* InObject )
	{
		objects.erase( InObject );
	}

	/**
	 * @brief Removes all objects from the list
	 */
	FORCEINLINE void RemoveAllObjects()
	{
		objects.clear();
	}

	/**
	 * @brief Interface function to get at the derived CObjectPropertyNode class
	 * @return Return interface CObjectPropertyNode if this class derived from CObjectPropertyNode
	 */
	virtual CObjectPropertyNode* GetObjectNode() override;

	/**
	 * @brief Get address for all available objects
	 *
	 * @param InNode		The property to get objects from
	 * @param OutAddresses	Storage array for all of the objects' addresses
	 * @return Return TRUE if successful filled addresses, otherwise returns FALSE
	 */
	virtual bool GetReadAddress( CPropertyNode* InNode, std::vector<byte*>& OutAddresses ) const override;

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
	 * @brief Get object by index from the list
	 * 
	 * @param InIndex	Object index
	 * @return Return object at index 'InIndex' of the objects array
	 */
	CObject* GetObject( uint32 InIndex ) const;

	/**
	 * @brief Get number of objects in the list
	 * @return Return number of objects in the list
	 */
	FORCEINLINE uint32 GetNumObjects() const
	{
		return objects.size();
	}

	/**
	 * @brief Get the base-est base class for objects in this list
	 * @return Return the base-est base class for objects in this list
	 */
	FORCEINLINE CClass* GetObjectBaseClass() const
	{
		return baseClass;
	}

protected:
	/**
	 * @brief Interface function for init node
	 */
	virtual void InitNodeInternal() override;

	/**
	 * @brief Interface function for Creating Child Nodes
	 */
	virtual void InitChildNodes() override;

	/**
	 * @brief Looks at the objects array and creates the best base class
	 */
	void SetBestBaseClass();

	std::unordered_set<CObject*>	objects;	/**< The list of objects we are editing properties for */
	CClass*							baseClass;	/**< The lowest level base class for objects in the list */
};

#endif // !OBJECTPROPERTYNODE_H