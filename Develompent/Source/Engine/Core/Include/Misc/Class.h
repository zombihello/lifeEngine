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
#include "System/Name.h"

/**
 * @ingroup Core
 * @brief Enumeration of flags describing a class
 */
enum EClassFlags
{
	CLASS_None			= 0,		/**< None */
	CLASS_Deprecated	= 1 << 0,	/**< Class is deprecated */
	CLASS_Abstract		= 1 << 1,	/**< Class is abstract and can't be instantiated directly  */
};

/**
 * @ingroup Core
 * @brief Enumeration of flags used for quickly casting classes of certain types; all class cast flags are inherited
 */
enum EClassCastFlags
{
	CASTCLASS_None					= 0,		/**< None */
	CASTCLASS_CProperty				= 1 << 0,	/**< Cast to CProperty */
	CASTCLASS_CByteProperty			= 1 << 1,	/**< Cast to CByteProperty */
	CASTCLASS_CIntProperty			= 1 << 2,	/**< Cast to CIntProperty */
	CASTCLASS_CFloatProperty		= 1 << 3,	/**< Cast to CFloatProperty */
	CASTCLASS_CBoolProperty			= 1 << 4,	/**< Cast to CBoolProperty */
	CASTCLASS_CColorProperty		= 1 << 5,	/**< Cast to CColorProperty */
	CASTCLASS_CComponentProperty	= 1 << 6,	/**< Cast to CComponentProperty */
	CASTCLASS_CVectorProperty		= 1 << 7,	/**< Cast to CVectorProperty */
	CASTCLASS_CRotatorProperty		= 1 << 8,	/**< Cast to CRotatorProperty */
	CASTCLASS_CAssetProperty		= 1 << 9,	/**< Cast to CAssetProperty */
};

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
	FORCEINLINE CClass() 
		: ClassConstructor( nullptr )
		, classFlags( CLASS_None )
		, classCastFlags( CASTCLASS_None )
		, superClass( nullptr )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param InClassName			Class name
	 * @param InClassFlags			Class flags
	 * @param InClassCastFlags		Class cast flags
	 * @param InClassConstructor	Pointer to class constructor
	 * @param InSuperClass			Pointer to super class
	 */
	FORCEINLINE CClass( const CName& InClassName, uint32 InClassFlags, uint32 InClassCastFlags, class CObject*( *InClassConstructor )(), CClass* InSuperClass = nullptr ) 
		: ClassConstructor( InClassConstructor )
		, classFlags( InClassFlags )
		, classCastFlags( InClassCastFlags )
		, superClass( InSuperClass )
		, name( InClassName )
	{}

	/**
	 * @brief Get class name
	 * @return Return class name
	 */
	FORCEINLINE std::wstring GetName() const
	{
		std::wstring	retName;
		name.ToString( retName );
		return retName;
	}

	/**
	 * @brief Get class name
	 * @param OutName	Output class name
	 */
	FORCEINLINE void GetName( std::wstring& OutName ) const
	{
		name.ToString( OutName );
	}

	/**
	 * @brief Get class name
	 * @return Return class name (CName)
	 */
	FORCEINLINE const CName& GetCName() const
	{
		return name;
	}

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
	 * @return Return pointer to instance of class object
	 */
	FORCEINLINE class CObject* CreateObject() const
	{
		Assert( ClassConstructor );
		return ClassConstructor();
	}

	/**
	 * @brief Create instance of class object
	 * @return Return pointer to instance of class object
	 */
	template< typename TObject >
	FORCEINLINE TObject* CreateObject() const
	{
		return ( TObject* )CreateObject();
	}

	/**
	 * @brief Register class in table
	 * @param[in] InClass Class
	 */
	static FORCEINLINE void StaticRegisterClass( const CClass* InClass )
	{
		Assert( InClass );
		classesTable.insert( std::make_pair( InClass->GetName(), InClass ) );
	}

	/**
	 * @brief Find class by name
	 * @param[in] InClassName Class name
	 * 
	 * @return Return pointer to class. If not found returning nullptr
	 */
	static FORCEINLINE CClass* StaticFindClass( const tchar* InClassName )
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

	/**
	 * @brief Add class property
	 * @param InProperty	Property
	 */
	FORCEINLINE void AddProperty( class CProperty* InProperty )
	{
		properties.push_back( InProperty );
	}

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
	 * @brief Find property
	 *
	 * @param InName		Property name
	 * @return Return pointer to class property, if not found return NULL
	 */
	class CProperty* InternalFindProperty( const CName& InName ) const;
	
	class CObject*( *ClassConstructor )();											/**< Pointer to constructor of class */
	uint32														classFlags;			/**< Class flags */
	uint32														classCastFlags;		/**< Class cast flags */
	CClass*														superClass;			/**< Pointer to super class */
	CName														name;				/**< Class name */
	std::vector<class CProperty*>								properties;			/**< Class properties */
	static std::unordered_map<std::wstring, const CClass*>		classesTable;		/**< Table of all classes in system */
};

#endif // !CLASS_H