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

#include "Misc/Object.h"

/**
 * @ingroup Core
 * @brief Class description for reflection
 */
class CClass : public CObject
{
	DECLARE_CLASS( CClass, CObject, 0, 0 )

public:
	friend CObject;

	/**
	 * @brief Constructor
	 */
	FORCEINLINE CClass() 
		: ClassConstructor( nullptr )
		, classFlags( CLASS_None )
		, classCastFlags( CASTCLASS_None )
		, propertiesSize( 0 )
		, minAlignment( 1 )
		, superClass( nullptr )
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
		: CObject( InClassName )
		, ClassConstructor( InClassConstructor )
		, classFlags( InClassFlags )
		, classCastFlags( InClassCastFlags )
		, propertiesSize( InPropertiesSize )
		, minAlignment( InMinAlignment )
		, superClass( InSuperClass )
	{}

	/**
	 * @brief Get super class
	 * @return Return pointer to super class. If it is not, it will return nullptr
	 */
	FORCEINLINE CClass* GetSuperClass() const
	{
		return superClass;
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
	 * @brief Get properties size in bytes
	 * @return Return properties size in bytes
	 */
	FORCEINLINE uint32 GetPropertiesSize() const
	{
		return propertiesSize;
	}

	/**
	 * @brief Get minimum alignment for the class
	 * @return Return minimum alignment for the class
	 */
	FORCEINLINE uint32 GetMinAlignment() const
	{
		return minAlignment;
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

	/**
	 * @brief Add class property
	 * @param InProperty	Property
	 */
	virtual void AddProperty( class CProperty* InProperty ) override;

	/**
	 * @brief Get array of class properties
	 * 
	 * @param OutArrayProperties		Output array properties
	 * @param InPropertiesInParents		Take into account the properties of the parents
	 */
	void GetProperties( std::vector<class CProperty*>& OutArrayProperties, bool InPropertiesInParents = true ) const;

	/**
	 * @brief Get number properties in the class
	 * 
	 * @param InPropertiesInParents		Take into account the properties of the parents
	 * @return Return number properties in the class
	 */
	uint32 GetNumProperties( bool InPropertiesInParents = true ) const;

	/**
	 * @brief Find property
	 *
	 * @param InName		Property name
	 * @return Return pointer to class property, if not found return NULL
	 */
	template<typename TPropertyClass>
	FORCEINLINE TPropertyClass* FindProperty( const CName& InName ) const
	{
		return Cast<TPropertyClass>( InternalFindProperty( InName ) );
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

	/**
	 * @brief Find property
	 *
	 * @param InName		Property name
	 * @return Return pointer to class property, if not found return NULL
	 */
	class CProperty* InternalFindProperty( const CName& InName ) const;
	
	class CObject*( *ClassConstructor )( void* InPtr );										/**< Pointer to constructor of class */
	uint32														classFlags;					/**< Class flags */
	uint32														classCastFlags;				/**< Class cast flags */
	uint32														propertiesSize;				/**< Properties size in bytes */
	uint32														minAlignment;				/**< Minimum alignment for the class */
	CClass*														superClass;					/**< Pointer to super class */
	std::vector<class CProperty*>								properties;					/**< Class properties */
};

#endif // !CLASS_H