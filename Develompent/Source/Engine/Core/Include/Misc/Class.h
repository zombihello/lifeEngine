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

#include "Misc/Struct.h"

/**
 * @ingroup Core
 * @brief Class description for reflection
 */
class CClass : public CStruct
{
	DECLARE_CLASS( CClass, CStruct, 0, 0 )

public:
	friend CObject;

	/**
	 * @brief Constructor
	 */
	FORCEINLINE CClass() 
		: ClassConstructor( nullptr )
		, classFlags( CLASS_None )
		, classCastFlags( CASTCLASS_None )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param InClassName			Class name
	 * @param InClassFlags			Class flags
	 * @param InClassCastFlags		Class cast flags
	 * @param InPropertiesSize		Properties size in bytes
	 * @param InMinAlignment		Minimum alignment for the class
	 * @param InClassConstructor	Pointer to class constructor
	 * @param InSuperClass			Pointer to super class
	 */
	FORCEINLINE CClass( const CName& InClassName, uint32 InClassFlags, uint32 InClassCastFlags, uint32 InPropertiesSize, uint32 InMinAlignment, class CObject*( *InClassConstructor )( void* InPtr ), CClass* InSuperClass = nullptr )
		: CStruct( InClassName, InPropertiesSize, InMinAlignment, InSuperClass )
		, ClassConstructor( InClassConstructor )
		, classFlags( InClassFlags )
		, classCastFlags( InClassCastFlags )
	{}

	/**
	 * @brief Get super class
	 * @return Return pointer to super class. If it is not, it will return nullptr
	 */
	FORCEINLINE CClass* GetSuperClass() const
	{
		return ( CClass* )GetSuperStruct();
	}

	/**
	 * @brief Get class flags
	 * @return Return class flags
	 */
	FORCEINLINE uint32 GetClassFlags() const
	{
		return classFlags;
	}

	/**
	 * @brief Has any class flags
	 * 
	 * @param InCheckFlags		Class flags to check for
	 * @return Return TRUE if the passed in flag is set (including no flag passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAnyClassFlags( uint32 InCheckFlags ) const
	{
		return ( classFlags & InCheckFlags ) != 0;
	}

	/**
	 * @brief Has all class flags
	 *
	 * @param InCheckFlags		Class flags to check for
	 * @return Return TRUE if all of the passed in flags are set (including no flags passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAllClassFlags( uint32 InCheckFlags ) const
	{
		return ( classFlags & InCheckFlags ) == InCheckFlags;
	}

	/**
	 * @brief Has any cast flags
	 *
	 * @param InCheckFlags		Class flags to check for
	 * @return Return TRUE if the passed in flag is set (including no flag passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAnyCastFlags( uint32 InCheckFlags ) const
	{
		return ( classCastFlags & InCheckFlags ) != 0;
	}

	/**
	 * @brief Has all cast flags
	 *
	 * @param InCheckFlags		Class flags to check for
	 * @return Return TRUE if all of the passed in flags are set (including no flags passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAllCastFlags( uint32 InCheckFlags ) const
	{
		return ( classCastFlags & InCheckFlags ) == InCheckFlags;
	}

	/**
	 * @brief Get class cast flags
	 * @return Return class cast flags
	 */
	FORCEINLINE uint32 GetClassCastFlags() const
	{
		return classCastFlags;
	}

	/**
	 * @brief Create instance of class object
	 * 
	 * @param InOuter	Object this object resides in
	 * @param InName	Name object
	 * @return Return pointer to instance of class object
	 */
	FORCEINLINE class CObject* CreateObject( class CObject* InOuter = nullptr, const CName& InName = NAME_None ) const
	{
		return CObject::StaticConstructObject( ( CClass* )this, InOuter, InName );
	}

	/**
	 * @brief Create instance of class object
	 * 
	 * @param InOuter	Object this object resides in
	 * @param InName	Name object
	 * @return Return pointer to instance of class object
	 */
	template<typename TObject>
	FORCEINLINE TObject* CreateObject( class CObject* InOuter = nullptr, const CName& InName = NAME_None ) const
	{
		return ( TObject* )CreateObject( InOuter, InName );
	}

	/**
	 * @brief Register class in table
	 * @param[in] InClass Class
	 */
	static FORCEINLINE void StaticRegisterClass( const CClass* InClass )
	{
		Assert( InClass );
		StaticRegisteredClassesInternal().insert( std::make_pair( InClass->GetCName(), InClass ) );
	}

	/**
	 * @brief Find class by name
	 * @param[in] InClassName Class name
	 * 
	 * @return Return pointer to class. If not found returning nullptr
	 */
	static FORCEINLINE CClass* StaticFindClass( const tchar* InClassName )
	{
		const std::unordered_map<CName, const CClass*, CName::HashFunction>&	classesTable = StaticRegisteredClasses();
		auto	itClass = classesTable.find( InClassName );
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
	static FORCEINLINE const std::unordered_map<CName, const CClass*, CName::HashFunction>& StaticRegisteredClasses()
	{
		return StaticRegisteredClassesInternal();
	}

private:
	/**
	 * @brief Get array of all registered classes
	 * @return Return array of all registered classes
	 */
	static FORCEINLINE std::unordered_map<CName, const CClass*, CName::HashFunction>& StaticRegisteredClassesInternal()
	{
		static std::unordered_map<CName, const CClass*, CName::HashFunction>		classesTable;
		return classesTable;
	}

	class CObject*( *ClassConstructor )( void* InPtr );										/**< Pointer to constructor of class */
	uint32		classFlags;					/**< Class flags */
	uint32		classCastFlags;				/**< Class cast flags */
};

#endif // !CLASS_H