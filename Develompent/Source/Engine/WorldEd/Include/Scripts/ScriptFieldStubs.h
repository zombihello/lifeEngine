/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTFIELDSTUBS_H
#define SCRIPTFIELDSTUBS_H

#include <vector>
#include <unordered_map>

#include "Core.h"
#include "Misc/SharedPointer.h"
#include "Reflection/Class.h"
#include "Reflection/Property.h"
#include "Reflection/ObjectPackage.h"

/**
 * @ingroup WorldEd
 * @brief Script property types
 */
enum EScriptPropertyType
{
	SPT_Unknown,	/**< Unknown */
	SPT_Byte,		/**< Byte */
	SPT_Enum,		/**< Enum */
	SPT_Int,		/**< Integer */
	SPT_Bool,		/**< Boolean */
	SPT_Float,		/**< Float */
	SPT_Color,		/**< Color */
	SPT_Object,		/**< CObject */
	SPT_Vector,		/**< Vector */
	SPT_Rotator,	/**< Rotator */
	SPT_Asset,		/**< Asset */
	SPT_Array,		/**< Array */
	SPT_Struct,		/**< CStruct */
	SPT_String,		/**< String */
	SPT_Num,		/**< Properties number */
};

/**
 * @ingroup WorldEd
 * @brief Convert Text to EScriptPropertyType
 * 
 * @param InString	String
 * @return Return converted string into EScriptPropertyType. In fail case returns SPT_Unknown
 */
EScriptPropertyType ConvertTextToEScriptPropertyType( const std::wstring& InString );

/**
 * @ingroup WorldEd
 * @brief Is property type have sub type
 * 
 * @param InPropertyType	Property type
 * @return Return TRUE if the property type must have sub type, otherwise returns FALSE
 */
FORCEINLINE bool IsScriptPropertyTypeHaveSubType( EScriptPropertyType InPropertyType )
{
	switch ( InPropertyType )
	{
	case SPT_Enum:
	case SPT_Object:
	case SPT_Asset:
	case SPT_Array:
	case SPT_Struct:
		return true;

	default:
		return false;
	}
}

/**
 * @ingroup WorldEd
 * @brief Script base stub
 */
class CScriptBaseStub
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InName	Stub name
	 */
	CScriptBaseStub( const std::wstring& InName );

	/**
	 * @brief Get stub name
	 * @return Return sub name
	 */
	FORCEINLINE const std::wstring& GetName() const
	{
		return name;
	}

protected:
	std::wstring	name;		/**< Stub name */
};

/**
 * @ingroup WorldEd
 * @brief Script class property stub
 */
class CScriptPropertyStub : public CScriptBaseStub
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InPropertyName	Property name
	 * @param InPropertyType	Property type
	 * @param InPropertyFlags	Property flags (see EPropertyFlags)
	 * @param InPropertySubType	Property sub type, using for byte (enum), object, struct, asset and array
	 */
	CScriptPropertyStub( const std::wstring& InPropertyName, EScriptPropertyType InPropertyType, uint32 InFlags = CPF_None, const std::wstring& InPropertySubType = TEXT( "" ) );

	/**
	 * @brief Get property type
	 * @return Return property type
	 */
	FORCEINLINE EScriptPropertyType GetType() const
	{
		return type;
	}

	/**
	 * @brief Get property sub type
	 * @return Return property sub type. If isn't returns empty string
	 */
	FORCEINLINE const std::wstring& GetSubType() const
	{
		return subType;
	}

	/**
	 * @brief Set created property
	 * @param InCreatedProperty		Created property for class
	 */
	FORCEINLINE void SetCreatedProperty( CProperty* InCreatedProperty )
	{
		createdProperty = InCreatedProperty;
	}

	/**
	 * @brief Get created property
	 * @return Return pointer to created property. If isn't created returns NULL
	 */
	FORCEINLINE CProperty* GetCreatedProperty() const
	{
		return createdProperty;
	}

	/**
	 * @brief Set property flags
	 * @param InFlags	Property flags
	 */
	FORCEINLINE void SetFlags( uint32 InFlags )
	{
		flags = InFlags;
	}

	/**
	 * @brief Get property flags
	 * @return Return property flags
	 */
	FORCEINLINE uint32 GetFlags() const
	{
		return flags;
	}

	/**
	 * @brief Set count of persistent variables
	 * @param InArraySize	Count of persistent variables
	 */
	FORCEINLINE void SetArraySize( uint32 InArraySize )
	{
		arraySize = InArraySize;
	}

	/**
	 * @brief Get count of persistent variables
	 * @return Return count of persistent variables
	 */
	FORCEINLINE uint32 GetArraySize() const
	{
		return arraySize;
	}

	/**
	 * @brief Set category
	 * @param InCategory	Category
	 */
	FORCEINLINE void SetCategory( const std::wstring& InCategory )
	{
		category = InCategory;
	}

	/**
	 * @brief Set description
	 * @param InDescription		Description
	 */
	FORCEINLINE void SetDescription( const std::wstring& InDescription )
	{
		description = InDescription;
	}

	/**
	 * @brief Get category
	 * @return Return category
	 */
	FORCEINLINE const std::wstring& GetCategory() const
	{
		return category;
	}

	/**
	 * @brief Get description
	 * @return Return description
	 */
	FORCEINLINE const std::wstring& GetDescription() const
	{
		return description;
	}

private:
	EScriptPropertyType		type;				/**< Property type */
	std::wstring			subType;			/**< Property sub type, using for byte (enum), object, struct, asset and array */
	std::wstring			category;			/**< Category */
	std::wstring			description;		/**< Description */
	CProperty*				createdProperty;	/**< Created property for class */
	uint32					flags;				/**< Property flags (see EPropertyFlags) */
	uint32					arraySize;			/**< Count of persistent variables */
};

/**
 * @ingroup WorldEd
 * @brief Script class stub
 */
class CScriptClassStub : public CScriptBaseStub
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InClassName			Class name
	 * @param InSuperClassName		Super class name
	 * @param InFlags				Flags (see EClassFlags)
	 */
	CScriptClassStub( const std::wstring& InClassName, const std::wstring& InSuperClassName, uint32 InFlags = CLASS_None );

	/**
	 * @brief Get super class name
	 * @return Return super class name
	 */
	FORCEINLINE const std::wstring& GetSuperClassName() const
	{
		return superClassName;
	}

	/**
	 * @brief Set script code
	 * @param InScriptCode	Script code of the class
	 */
	FORCEINLINE void SetScriptCode( const std::string& InScriptCode )
	{
		scriptCode = InScriptCode;
	}

	/**
	 * @brief Get script code
	 * @return Return script code. If isn't returns empty string
	 */
	FORCEINLINE const std::string& GetScriptCode() const
	{
		return scriptCode;
	}

	/**
	 * @brief Set created class
	 * @param InCreatedClass	Created class for script environment
	 */
	FORCEINLINE void SetCreatedClass( CClass* InCreatedClass )
	{
		createdClass = InCreatedClass;
	}

	/**
	 * @brief Get created class
	 * @return Return pointer to created class. If isn't created returns NULL
	 */
	FORCEINLINE CClass* GetCreatedClass() const
	{
		return createdClass;
	}

	/**
	 * @brief Set properties
	 * @param InProperties	Properties
	 */
	FORCEINLINE void SetProperties( const std::vector<TSharedPtr<CScriptPropertyStub>>& InProperties )
	{
		properties = InProperties;
	}

	/**
	 * @brief Get properties
	 * @return Return properties
	 */
	FORCEINLINE const std::vector<TSharedPtr<CScriptPropertyStub>>& GetProperties() const
	{
		return properties;
	}

	/**
	 * @brief Set class flags
	 * @param InFlags	Class flags (see EClassFlags)
	 */
	FORCEINLINE void SetFlags( uint32 InFlags )
	{
		flags = InFlags;
	}

	/**
	 * @brief Get class flags
	 * @return Return class flags
	 */
	FORCEINLINE uint32 GetFlags() const
	{
		return flags;
	}

private:
	std::wstring										superClassName;		/**< Super class name */
	std::string											scriptCode;			/**< Script code */
	CClass*												createdClass;		/**< Created class for script environment */
	uint32												flags;				/**< Class flags (see EClassFlags) */
	std::vector<TSharedPtr<CScriptPropertyStub>>		properties;			/**< Properties */
};

/**
 * @ingroup WorldEd
 * @brief Script system stub
 */
class CScriptSystemStub
{
public:
	/**
	 * @brief Constructor
	 * @param InPackage		Package where will be placing CClass, CProperty and etc
	 */
	CScriptSystemStub( CObjectPackage* InPackage );

	/**
	 * @brief Add a class/structure
	 * @param InClassStub	Class stub
	 */
	FORCEINLINE void AddClass( const TSharedPtr<CScriptClassStub>& InClassStub )
	{
		classes.push_back( InClassStub );
		classesMap.insert( std::make_pair( InClassStub->GetName(), InClassStub ) );
	}

	/**
	 * @brief Find class stub by name
	 * 
	 * @param InClassName	Class name
	 * @return Return found class stub. If not found returns NULL
	 */
	FORCEINLINE TSharedPtr<CScriptClassStub> FindClass( const std::wstring& InClassName ) const
	{
		auto	it = classesMap.find( InClassName );
		return it != classesMap.end() ? it->second : nullptr;
	}

	/**
	 * @brief Get array of all classes
	 * @return Return array of all classes
	 */
	FORCEINLINE const std::vector<TSharedPtr<CScriptClassStub>>& GetClasses() const
	{
		return classes;
	}

	/**
	 * @brief Get array of all classes
	 * @return Return array of all classes
	 */
	FORCEINLINE std::vector<TSharedPtr<CScriptClassStub>>& GetClasses()
	{
		return classes;
	}

	/**
	 * @brief Get package
	 * @return Return package
	 */
	FORCEINLINE CObjectPackage* GetPackage() const
	{
		return package;
	}

private:
	CObjectPackage*														package;	/**< Package where will be placing CClass, CProperty and etc */
	std::vector<TSharedPtr<CScriptClassStub>>							classes;	/**< Top level classes */
	std::unordered_map<std::wstring, TSharedPtr<CScriptClassStub>>		classesMap;	/**< Top level classes for find by name */
};

#endif // !SCRIPTFIELDSTUBS_H