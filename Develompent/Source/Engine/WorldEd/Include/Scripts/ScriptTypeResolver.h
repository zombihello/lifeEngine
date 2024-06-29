/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTTYPERESOLVER_H
#define SCRIPTTYPERESOLVER_H

#include <unordered_map>

#include "System/Name.h"
#include "Reflection/Property.h"
#include "Scripts/ScriptTypeDummy.h"

/**
 * @ingroup WorldEd
 * @brief Pointer to function to create CProperty
 */
typedef CProperty*( *CreatePropertyFn_t )( const struct ScriptTypeResolveParams& InParams );

/**
 * @ingroup WorldEd
 * @brief Script type resolve parameters
 */
struct ScriptTypeResolveParams
{
	/**
	 * @brief Constructor
	 * 
	 * @param InOuter			The object to create this property within
	 * @param InName			The name to give the property
	 * @param InObjectFlags		The object flags (see EObjectFlags)
	 * @param InDummyType		Dummy type to resolve
	 * @param InOffset			Offset to property
	 * @param InPropertyFlags	Property flags (see EPropertyFlags)
	 * @param InCategory		Property category
	 * @param InDescription		Property description
	 */
	ScriptTypeResolveParams( CObject* InOuter, const CName& InName, uint32 InObjectFlags, const CScriptTypeDummy& InDummyType, uint32 InOffset, uint32 InPropertyFlags, const CName& InCategory = NAME_None, const std::wstring& InDescription = TEXT( "" ) );

	CScriptTypeDummy	dummyType;		/**< Dummy type to resolve */
	CObject*			outer;			/**< The object to create this property within */
	CName				name;			/**< The name to give the property */
	ObjectFlags_t		objectFlags;	/**< The object flags (see EObjectFlags) */
	uint32				offset;			/**< Offset to property */
	uint32				propertyFlags;	/**< Property flags (see EPropertyFlags) */
	CName				category;		/**< Property category */
	std::wstring		description;	/**< Property description */
};

/**
 * @ingroup WorldEd
 * @brief Built-in script type
 */
struct ScriptBuiltInType
{
	CName					name;					/**< Real name */
	CName					scriptName;				/**< Name in script */
	CName					stackFrameGetPostfix;	/**< Postfix of macro STACKFRAME_GET */
	CreatePropertyFn_t		CreatePropertyFn;		/**< Function to create CProperty */
};

/**
 * @ingroup WorldEd
 * @brief Resolve dummy types into usable types
 */
class CScriptTypeResolver
{
public:
	/**
	 * @brief Create CProperty from dummy type
	 * 
	 * @param InParams	Params to resolve dummy type
	 */
	static CProperty* Resolve( const ScriptTypeResolveParams& InParams );

	/**
	 * @brief Convert script type into real type
	 * @param InDummyType	Dummy type
	 * @return Return real type if was found, otherwise returns empty string
	 */
	static std::wstring Resolve( const CScriptTypeDummy& InDummyType );

	/**
	 * @brief Convert script type into STACKFRAME_GET_<ParamType>
	 * @param InDummyType	Dummy type
	 * @return Return type name of STACKFRAME_GET_<ParamType> if was found, otherwise returns empty string
	 */
	static std::wstring ResolveStackFrameGetMacro( const CScriptTypeDummy& InDummyType );

	/**
	 * @brief Translate built-in type into real C++ types
	 * @param InName	Script type
	 * @return Return real type if was found, otherwise returns script type
	 */
	static std::wstring TranslateBuiltInTypes( const std::wstring& InName );

	/**
	 * @brief Is built-in type
	 * @param InName	Type name
	 * @return Return TRUE if this is type built-in, otherwise FALSE
	 */
	static FORCEINLINE bool IsBuiltInType( const CName& InName )
	{
		InitBuiltInTypes();
		auto	it = builtInTypes.find( InName );
		return it != builtInTypes.end();
	}

private:
	/**
	 * @brief Initialize built-in types
	 */
	static void InitBuiltInTypes();

	/**
	 * @brief Add built-in type into map
	 * @param InName					Real name
	 * @param InScriptName				Name in script
	 * @param InStackFrameGetPostfix	Postfix of macro STACKFRAME_GET
	 * @param InCreatePropertyFn		Function to create CProperty
	 */
	static FORCEINLINE void AddBuiltInType( const std::wstring& InName, const std::wstring& InScriptName, const std::wstring& InStackFrameGetPostfix, CreatePropertyFn_t InCreatePropertyFn )
	{
		builtInTypes.insert( std::make_pair( InName, ScriptBuiltInType{ InName, InScriptName, InStackFrameGetPostfix, InCreatePropertyFn } ) );
	}

	static bool																	bBuiltInTypesInited;	/**< Is initialized built-in types */
	static std::unordered_map<CName, ScriptBuiltInType, CName::HashFunction>	builtInTypes;			/**< Built-in types */
};

#endif // !SCRIPTTYPERESOLVER_H