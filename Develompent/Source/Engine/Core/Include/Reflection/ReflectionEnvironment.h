/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef REFLECTIONENVIRONMENT_H
#define REFLECTIONENVIRONMENT_H

#include <unordered_map>

#include "Reflection/Class.h"
#include "Reflection/Struct.h"
#include "Reflection/Enum.h"
#include "Reflection/Function.h"

/**
 * @ingroup Core
 * @brief Reflection environment
 * 
 * This class contains all registered native and script classes, enums and other
 */
class CReflectionEnvironment
{
public:
	/**
	 * @brief Typedef map of registered classes
	 */
	typedef std::unordered_map<CName, const CClass*, CName::HashFunction>				ClassesMap_t;

	/**
	 * @brief Typedef map of registered structs
	 */
	typedef std::unordered_map<CName, const CStruct*, CName::HashFunction>				StructsMap_t;

	/**
	 * @brief Typedef map of registered enums
	 */
	typedef std::unordered_map<CName, const CEnum*, CName::HashFunction>				EnumsMap_t;

	/**
	 * @brief Typedef map of registered native functions
	 */
	typedef std::unordered_map<CName, ReflectionFunctionDesc*, CName::HashFunction>		NativeFunctionsMap_t;

	/**
	 * @brief Add a new class into reflection environment
	 * @param InClass	A new class
	 */
	FORCEINLINE void AddClass( const CClass* InClass )
	{
		Assert( InClass );
		classesMap.insert( std::make_pair( InClass->GetCName(), InClass ) );
	}

	/**
	 * @brief Add a new struct into reflection environment
	 * @param InStruct	A new struct
	 */
	FORCEINLINE void AddStruct( const CStruct* InStruct )
	{
		Assert( InStruct );
		structsMap.insert( std::make_pair( InStruct->GetCName(), InStruct ) );
	}

	/**
	 * @brief Add a new enum into reflection environment
	 * @param InEnum	A new enum
	 */
	FORCEINLINE void AddEnum( const CEnum* InEnum )
	{
		Assert( InEnum );
		enumsMap.insert( std::make_pair( InEnum->GetCName(), InEnum ) );
	}

	/**
	 * @brief Add a new native function into reflection environment
	 * @warning The native functions table must have NULL item for terminate array
	 * 
	 * @param InClassName		Class name of owner the native function table
	 * @param InNativeFuncTable	The native function table
	 */
	FORCEINLINE void AddNativeFunctions( const CName& InClassName, ReflectionFunctionDesc* InNativeFuncTable )
	{
		Assert( InClassName != NAME_None && InNativeFuncTable );
		nativeFunctionsMap.insert( std::make_pair( InClassName, InNativeFuncTable ) );
	}

	/**
	 * @brief Find class by name
	 * 
	 * @param InClassName	Class name
	 * @return Return found class. If not returns NULL
	 */
	FORCEINLINE CClass* FindClass( const tchar* InClassName )
	{
		auto	itClass = classesMap.find( InClassName );
		if ( itClass == classesMap.end() )
		{
			return nullptr;
		}

		return ( CClass* )itClass->second;
	}

	/**
	 * @brief Find structs by name
	 *
	 * @param InStructName	Struct name
	 * @return Return found struct. If not returns NULL
	 */
	FORCEINLINE CStruct* FindStruct( const tchar* InStructName )
	{
		auto	itStruct = structsMap.find( InStructName );
		if ( itStruct == structsMap.end() )
		{
			return nullptr;
		}

		return ( CStruct* )itStruct->second;
	}

	/**
	 * @brief Find enum by name
	 *
	 * @param InEnumName	Enum name
	 * @return Return found enum. If not returns NULL
	 */
	FORCEINLINE CEnum* FindEnum( const tchar* InEnumName )
	{
		auto	itEnum = enumsMap.find( InEnumName );
		if ( itEnum == enumsMap.end() )
		{
			return nullptr;
		}

		return ( CEnum* )itEnum->second;
	}

	/**
	 * @brief Find native function by name
	 *
	 * @param InClassName		Class name
	 * @param InFunctionName	Function name
	 * @return Return found native function. If not returns NULL
	 */
	FORCEINLINE ScriptFn_t FindNativeFunction( const CName& InClassName, const achar* InFunctionName )
	{
		// If the class has a native function table, loop over the table and find the function
		auto	itNativeFuncTable = nativeFunctionsMap.find( InClassName );
		if ( itNativeFuncTable != nativeFunctionsMap.end() )
		{
			uint32	functionIndex = 0;
			while ( itNativeFuncTable->second[functionIndex].name )
			{
				if ( !strcmp( InFunctionName, itNativeFuncTable->second[functionIndex].name ) )
				{
					return itNativeFuncTable->second[functionIndex].FunctionFn;
				}
				++functionIndex;
			}
		}

		// We nothing to found, return NULL
		return nullptr;
	}

	/**
	 * @brief Get singleton instance
	 * @return Return singleton instance
	 */
	static FORCEINLINE CReflectionEnvironment& Get()
	{
		static CReflectionEnvironment	reflectionEnvironment;
		return reflectionEnvironment;
	}

	/**
	 * @brief Get map of all registered classes
	 * @return Return map of all registered classes
	 */
	FORCEINLINE const ClassesMap_t& GetAllClasses()
	{
		return classesMap;
	}

	/**
	 * @brief Get map of all registered structures
	 * @return Return map of all registered structures
	 */
	FORCEINLINE const StructsMap_t& GetAllStructs()
	{
		return structsMap;
	}

	/**
	 * @brief Get map of all registered enums
	 * @return Return map of all registered enums
	 */
	FORCEINLINE const EnumsMap_t& GetAllEnums()
	{
		return enumsMap;
	}

	/**
	 * @brief Get map of all registered native functions
	 * @return Return map of all registered native functions
	 */
	FORCEINLINE const NativeFunctionsMap_t& GetAllNativeFunctions()
	{
		return nativeFunctionsMap;
	}

private:
	ClassesMap_t			classesMap;			/**< Map of registered classes */
	StructsMap_t			structsMap;			/**< Map of registered structures */
	EnumsMap_t				enumsMap;			/**< Map of registered enums */
	NativeFunctionsMap_t	nativeFunctionsMap;	/**< Map of registered native functions */
};

#endif // !REFLECTIONENVIRONMENT_H