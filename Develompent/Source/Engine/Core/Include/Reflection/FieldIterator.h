/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef FIELDITERATOR_H
#define FIELDITERATOR_H

#include "Reflection/Struct.h"

/**
 * @ingroup Core
 * @brief Iterator through a linked list of fields
 */
template<class TClass>
class TFieldIterator
{
public:
	/**
	 * @brief Constructor
	 * @param InStruct				The object where will be used to search a field
	 * @param InIsIncludeSuper		Should include super class while iterating by list of fields
	 */
	TFieldIterator( const CStruct* InStruct, bool InIsIncludeSuper = true )
		: theStruct( InStruct )
		, field( InStruct ? InStruct->GetChildrenField() : nullptr )
		, bShouldIncludeSuper( InIsIncludeSuper )
	{
		IterateToNext();
	}

	/**
	 * @brief Override operator cast to bool
	 * @return Return TRUE if iterator is valid, otherwise returns FALSE
	 */
	FORCEINLINE operator bool() const
	{
		return field;
	}

	/**
	 * @brief Override operator !
	 * @return Return TRUE if iterator isn't valid, otherwise returns FALSE
	 */
	FORCEINLINE bool operator !() const
	{
		return !operator bool();
	}

	/**
	 * @brief Override operator ++
	 */
	FORCEINLINE void operator++()
	{
		Assert( field );
		field = field->GetNextField();
		IterateToNext();
	}

	/**
	 * @brief Override operator *
	 * @return Return current field
	 */
	FORCEINLINE TClass* operator*() const
	{
		return ( TClass* )field;
	}

	/**
	 * @brief Override operator ->
	 * @return Return current field
	 */
	FORCEINLINE TClass* operator->() const
	{
		return ( TClass* )field;
	}

	/**
	 * @brief Get current struct where used to search a field
	 * @return Return pointer to struct
	 */
	FORCEINLINE const CStruct* GetStruct() const
	{
		return theStruct;
	}

private:
	/**
	 * @brief Iterate to the next field
	 */
	FORCEINLINE void IterateToNext()
	{
		CField*			currentField	= field;
		const CStruct*	currentStruct	= theStruct;
		while ( currentStruct )
		{
			// Iterate over all fields in the struct
			while ( currentField )
			{
				if ( currentField->GetClass()->HasAllCastFlags( TClass::StaticClassCastFlags ) )
				{
					theStruct	= currentStruct;
					field		= currentField;
					return;
				}
				currentField	= currentField->GetNextField();
			}

			// If we arrived at the end go to super list of fields, of course if it allowed
			if ( bShouldIncludeSuper )
			{
				currentStruct = currentStruct->GetSuperStruct();
				if ( currentStruct )
				{
					currentField = currentStruct->GetChildrenField();
				}
			}
			// Otherwise exit from loop
			else
			{
				currentStruct = nullptr;
			}
		}

		theStruct	= currentStruct;
		field		= currentField;
	}

	const CStruct*	theStruct;				/**< The object where will be used to search a field */
	CField*			field;					/**< The current location in the list of fields */
	bool			bShouldIncludeSuper;	/**< Should include super class while iterating by list of fields */
};

#endif // !FIELDITERATOR_H