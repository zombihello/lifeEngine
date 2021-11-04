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
class LClass
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE										LClass() :
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
	FORCEINLINE										LClass( const std::wstring& InClassName, class LObject*( *InClassConstructor )(), LClass* InSuperClass = nullptr ) :
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
	FORCEINLINE LClass*								GetSuperClass() const
	{
		return superClass;
	}

	/**
	 * @brief Create instance of class object
	 * @return Return pointer to instance of class object
	 */
	FORCEINLINE class LObject*						CreateObject() const
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
	static FORCEINLINE void							StaticRegisterClass( const LClass* InClass )
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
	static FORCEINLINE LClass*					StaticFindClass( const tchar* InClassName )
	{
		auto		itClass = classesTable.find( InClassName );
		if ( itClass == classesTable.end() )
		{
			return nullptr;
		}

		return ( LClass* )itClass->second;
	}

private:
	class LObject*( *ClassConstructor )();											/**< Pointer to constructor of class */

	LClass*														superClass;			/**< Pointer to super class */
	std::wstring												name;				/**< Class name */	
	static std::unordered_map< std::wstring, const LClass* >	classesTable;		/**< Table of all classes in system */
};

#endif // !CLASS_H