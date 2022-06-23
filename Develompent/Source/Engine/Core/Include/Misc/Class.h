/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CLASS_H
#define CLASS_H

#include <string>
#include <unordered_map>

#include "Core.h"
#include "System/Archive.h"
#include "Scripts/ScriptEngine.h"

/**
 * @ingroup Core
 * @brief Class description for reflection
 */
class CClass
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE										CClass() :
		ClassConstructor( nullptr ),
		superClass( nullptr )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param[in] InClassName Class name
	 * @param[in] InClassConstructor Pointer to class constructor
	 * @param[in] InSuperClass Pointer to super class
	 */
	FORCEINLINE										CClass( const std::wstring& InClassName, class CObject*( *InClassConstructor )(), CClass* InSuperClass = nullptr ) :
		ClassConstructor( InClassConstructor ),
		superClass( InSuperClass ),
		name( InClassName )
	{}

	/**
	 * @brief Get class name
	 * @return Return class name
	 */
	FORCEINLINE const std::wstring&					GetName() const
	{
		return name;
	}

	/**
	 * @brief Get super class
	 * @return Return pointer to super class. If it is not, it will return nullptr
	 */
	FORCEINLINE CClass*								GetSuperClass() const
	{
		return superClass;
	}

	/**
	 * @brief Create instance of class object
	 * @return Return pointer to instance of class object
	 */
	FORCEINLINE class CObject*						CreateObject() const
	{
		check( ClassConstructor );
		return ClassConstructor();
	}

	/**
	 * @brief Create instance of class object
	 * @return Return pointer to instance of class object
	 */
	template< typename TObject >
	FORCEINLINE TObject*							CreateObject() const
	{
		return ( TObject* )CreateObject();
	}

	/**
	 * @brief Register class in table
	 * @param[in] InClass Class
	 */
	static FORCEINLINE void							StaticRegisterClass( const CClass* InClass )
	{
		check( InClass );
		classesTable.insert( std::make_pair( InClass->GetName(), InClass ) );
	}

	/**
	 * @brief Find class by name
	 * @param[in] InClassName Class name
	 * 
	 * @return Return pointer to class. If not found returning nullptr
	 */
	static FORCEINLINE CClass*					StaticFindClass( const tchar* InClassName )
	{
		auto		itClass = classesTable.find( InClassName );
		if ( itClass == classesTable.end() )
		{
			return nullptr;
		}

		return ( CClass* )itClass->second;
	}

	/**
	 * @brief Get array of all registered classes
	 * @return Return array of all registered classes
	 */
	static FORCEINLINE const std::unordered_map<std::wstring, const CClass*>& StaticGetRegisteredClasses()
	{
		return classesTable;
	}

	/**
	 * @brief Is a class
	 * 
	 * @param InClass	Checking class
	 * @return Return TRUE if object is a class InClass, else returning FALSE
	 */
	FORCEINLINE bool IsA( CClass* InClass ) const
	{
		// If class is not valid, we return false
		if ( !InClass )
		{
			return false;
		}

		// Iterate over each class ancestor
		for ( const CClass* tempClass = this; tempClass; tempClass = tempClass->superClass )
		{
			// Success! This class contains the class being checked
			if ( tempClass == InClass )
			{
				return true;
			}
		}

		// Otherwise the class does not contain the class being checked 
		return false;
	}

private:
	class CObject*( *ClassConstructor )();											/**< Pointer to constructor of class */

	CClass*														superClass;			/**< Pointer to super class */
	std::wstring												name;				/**< Class name */	
	static std::unordered_map<std::wstring, const CClass*>		classesTable;		/**< Table of all classes in system */
};

#endif // !CLASS_H