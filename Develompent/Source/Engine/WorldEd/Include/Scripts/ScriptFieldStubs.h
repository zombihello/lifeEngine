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

#include "Misc/SharedPointer.h"
#include "Reflection/Class.h"
#include "Reflection/Function.h"
#include "Reflection/Struct.h"
#include "Reflection/Enum.h"
#include "Scripts/ScriptFileContext.h"
#include "Scripts/ScriptTokenStream.h"

/**
 * @ingroup WorldEd
 * @brief Shared pointer of CScriptClassStub
 */
typedef TSharedPtr<class CScriptClassStub>		ScriptClassStubPtr_t;

/**
 * @ingroup WorldEd
 * @brief Shared pointer of CScriptFunctionStub
 */
typedef TSharedPtr<class CScriptFunctionStub>	ScriptFunctionStubPtr_t;

/**
 * @ingroup WorldEd
 * @brief Shared pointer of CScriptStructStub
 */
typedef TSharedPtr<class CScriptStructStub>		ScriptStructStubPtr_t;

/**
 * @ingroup WorldEd
 * @brief Shared pointer of CScriptEnumStub
 */
typedef TSharedPtr<class CScriptEnumStub>		ScriptEnumStubPtr_t;

/**
 * @ingroup WorldEd
 * @brief Script scope of a stub
 */
struct ScriptScopeStub
{
	ScriptFileContext	startContext;	/**< Context of the start context */
	ScriptFileContext	endContext;		/**< Context of the end context */
};

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
	 * @param InContext	Stub context
	 * @param InName	Stub name
	 */
	CScriptBaseStub( const ScriptFileContext& InContext, const std::wstring& InName );

	/**
	 * @brief Get stub context
	 * @return Return stub context. If isn't exist returns NULL
	 */
	FORCEINLINE const ScriptFileContext& GetContext() const
	{
		return context;
	}

	/**
	 * @brief Get stub name
	 * @return Return sub name
	 */
	FORCEINLINE const std::wstring& GetName() const
	{
		return name;
	}

protected:
	ScriptFileContext		context;	/**< Context */
	std::wstring			name;		/**< Name */
};

/**
 * @ingroup WorldEd
 * @brief Script function stub
 */
class CScriptFunctionStub : public CScriptBaseStub
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InContext					Function context
	 * @param InFunctionName			Function name
	 * @param InReturnTypeContext		Return type context
	 * @param InReturnTypeName			Return type name
	 * @param InFlags					Flags (look EFunctionFlags)
	 */
	CScriptFunctionStub( const ScriptFileContext& InContext, const std::wstring& InFunctionName, const ScriptFileContext& InReturnTypeContext, const std::wstring& InReturnTypeName, uint32 InFlags );

	/**
	 * @brief Get function flags
	 * @return Return function flags (see EFunctionFlags)
	 */
	FORCEINLINE uint32 GetFlags() const
	{
		return flags;
	}

	/**
	 * @brief Has any function flags
	 *
	 * @param InCheckFlags		Function flags to check for
	 * @return Return TRUE if the passed in flag is set (including no flag passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAnyFlags( uint32 InCheckFlags ) const
	{
		return ( flags & InCheckFlags ) != 0;
	}

	/**
	 * @brief Has all function flags
	 *
	 * @param InCheckFlags		Function flags to check for
	 * @return Return TRUE if all of the passed in flags are set (including no flags passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAllFlags( uint32 InCheckFlags ) const
	{
		return ( flags & InCheckFlags ) == InCheckFlags;
	}

	/**
	 * @brief Get the function scope
	 * @return Return reference to the function scope
	 */
	FORCEINLINE ScriptScopeStub& GetScope()
	{
		return scope;
	}

	/**
	 * @brief Get return type context
	 * @return Return return type context
	 */
	FORCEINLINE const ScriptFileContext* GetReturnTypeContext() const
	{
		return &returnTypeContext;
	}

	/**
	 * @brief Get return type name
	 * @return Return return type name
	 */
	FORCEINLINE const std::wstring& GetReturnTypeName() const
	{
		return returnTypeName;
	}

	/**
	 * @brief Get function code
	 * @return Return function code
	 */
	FORCEINLINE CScriptTokenStream& GetCode()
	{
		return code;
	}

	/**
	 * @brief Get function code
	 * @return Return function code
	 */
	FORCEINLINE const CScriptTokenStream& GetCode() const
	{
		return code;
	}

	/**
	 * @brief Set created function
	 * @param InCreatedFunction		Created function for script environment
	 */
	FORCEINLINE void SetCreatedFunction( CFunction* InCreatedFunction )
	{
		createdFunction = InCreatedFunction;
	}

	/**
	 * @brief Get created function
	 * @return Return pointer to created function. If isn't created returns NULL
	 */
	FORCEINLINE CFunction* GetCreatedFunction() const
	{
		return createdFunction;
	}

	/**
	 * @brief Set has function body or not
	 * @param InHasBody		Has function body or not
	 */
	FORCEINLINE void SetHasBody( bool InHasBody )
	{
		bHasBody = InHasBody;
	}

	/**
	 * @brief Has function body or not
	 * @return Return TRUE if function has body, otherwise FALSE
	 */
	FORCEINLINE bool HasBody() const
	{
		return bHasBody;
	}

private:
	bool				bHasBody;				/**< Has function body */
	uint32				flags;					/**< Function flags (see EFunctionFlags) */
	ScriptScopeStub		scope;					/**< Scope of the function */
	CScriptTokenStream	code;					/**< Function code */
	ScriptFileContext	returnTypeContext;		/**< Return type context */
	std::wstring		returnTypeName;			/**< Return type name */
	CFunction*			createdFunction;		/**< Created function for script environment */
};

/**
 * @ingroup WorldEd
 * @brief Script enum stub
 */
class CScriptEnumStub : public CScriptBaseStub
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InContext			Enum context
	 * @param InEnumName		Enum name
	 * @param InIsIntrinsic		Has LifeScript counter-part or not
	 */
	CScriptEnumStub( const ScriptFileContext& InContext, const std::wstring& InEnumName, bool InIsIntrinsic = false );

	/**
	 * @brief Has LifeScript counter-part or not
	 * @return Return TRUE if the enum has LifeScript counter-part, otherwise FALSE
	 */
	FORCEINLINE bool IsIntrinsic() const
	{
		return bIntrinsic;
	}

	/**
	 * @brief Set created enum
	 * @param InCreatedEnum		Created CEnum
	 */
	FORCEINLINE void SetCreatedEnum( CEnum* InCreatedEnum )
	{
		createdEnum = InCreatedEnum;
	}

	/**
	 * @brief Get created enum
	 * @return Return pointer to created enum. If isn't created returns NULL
	 */
	FORCEINLINE CEnum* GetCreatedEnum() const
	{
		return createdEnum;
	}

private:
	bool		bIntrinsic;		/**< Has no LifeScript counter-part */
	CEnum*		createdEnum;		/**< Created enum for script environment */
};

/**
 * @ingroup WorldEd
 * @brief Script struct stub
 */
class CScriptStructStub : public CScriptBaseStub
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InContext			Struct context
	 * @param InStructName		Struct name
	 * @param InIsIntrinsic		Has LifeScript counter-part or not
	 */
	CScriptStructStub( const ScriptFileContext& InContext, const std::wstring& InStructName, bool InIsIntrinsic = false );

	/**
	 * @brief Has LifeScript counter-part or not
	 * @return Return TRUE if the struct has LifeScript counter-part, otherwise FALSE
	 */
	FORCEINLINE bool IsIntrinsic() const
	{
		return bIntrinsic;
	}

	/**
	 * @brief Set created struct
	 * @param InCreatedStruct	Created CStruct
	 */
	FORCEINLINE void SetCreatedStruct( CStruct* InCreatedStruct )
	{
		createdStruct = InCreatedStruct;
	}

	/**
	 * @brief Get created struct
	 * @return Return pointer to created struct. If isn't created returns NULL
	 */
	FORCEINLINE CStruct* GetCreatedStruct() const
	{
		return createdStruct;
	}

private:
	bool		bIntrinsic;			/**< Has no LifeScript counter-part */
	CStruct*	createdStruct;		/**< Created struct for script environment */
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
	 * @param InContext				Class context
	 * @param InClassName			Class name
	 * @param InFlags				Flags (see EClassFlags)
	 */
	CScriptClassStub( const ScriptFileContext& InContext, const std::wstring& InClassName, uint32 InFlags );

	/**
	 * @brief Add a new function
	 * @param InFunction	A new function
	 */
	FORCEINLINE void AddFunction( const ScriptFunctionStubPtr_t& InFunction )
	{
		functions.push_back( InFunction );
	}

	/**
	 * @brief Get the class scope
	 * @return Return reference to the class scope
	 */
	FORCEINLINE ScriptScopeStub& GetScope()
	{
		return scope;
	}

	/**
	 * @brief Set super class
	 * 
	 * @param InSuperClassContext	Super class context
	 * @param InSuperClassName		Super class name
	 */
	FORCEINLINE void SetSuperClass( const ScriptFileContext& InSuperClassContext, const std::wstring& InSuperClassName )
	{
		superClassContext	= InSuperClassContext;
		superClassName		= InSuperClassName;
	}

	/**
	 * @brief Get super class context
	 * @return Return super class context
	 */
	FORCEINLINE const ScriptFileContext* GetSuperClassContext() const
	{
		return &superClassContext;
	}

	/**
	 * @brief Get super class name
	 * @return Return super class name
	 */
	FORCEINLINE const std::wstring& GetSuperClassName() const
	{
		return superClassName;
	}

	/**
	 * @brief Add class flag
	 * @param InNewFlag		New flag (see EClassFlags)
	 */
	FORCEINLINE void AddFlag( uint32 InNewFlag )
	{
		flags |= InNewFlag;
	}

	/**
	 * @brief Remove class flag
	 * @param InFlag	Flag to remove (see EClassFlags)
	 */
	FORCEINLINE void RemoveFlag( uint32 InFlag )
	{
		flags &= ~InFlag;
	}

	/**
	 * @brief Get class flags
	 * @return Return class flags (see EClassFlags)
	 */
	FORCEINLINE uint32 GetFlags() const
	{
		return flags;
	}

	/**
	 * @brief Has any class flags
	 *
	 * @param InCheckFlags		Class flags to check for
	 * @return Return TRUE if the passed in flag is set (including no flag passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAnyFlags( uint32 InCheckFlags ) const
	{
		return ( flags & InCheckFlags ) != 0;
	}

	/**
	 * @brief Has all class flags
	 *
	 * @param InCheckFlags		Class flags to check for
	 * @return Return TRUE if all of the passed in flags are set (including no flags passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAllFlags( uint32 InCheckFlags ) const
	{
		return ( flags & InCheckFlags ) == InCheckFlags;
	}

	/**
	 * @brief Set created class
	 * @param InCreatedClass	Created CClass
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
	 * @brief Add C++ text that will be placed into header (only for native classes)
	 * @param InCppText		++ code that will be placed into header
	 */
	void AddCppText( const std::wstring& InCppText );

	/**
	 * @brief Get C++ text that will be placed into header
	 * @return Return C++ text that will be placed into header
	 */
	FORCEINLINE const std::wstring& GetCppText() const
	{
		return cppText;
	}

	/**
	 * @brief Get array of functions
	 * @return Return array of functions
	 */
	FORCEINLINE const std::vector<ScriptFunctionStubPtr_t>& GetFunctions() const
	{
		return functions;
	}

	/**
	 * @brief Set within class
	 *
	 * @param InWithinClassContext	Within class context
	 * @param InWithinClassName		Within class name
	 */
	FORCEINLINE void SetWithinClass( const ScriptFileContext& InWithinClassContext, const std::wstring& InWithinClassName )
	{
		withinClassContext	= InWithinClassContext;
		withinClassName		= InWithinClassName;
	}

	/**
	 * @brief Get within class context
	 * @return Return within class context
	 */
	FORCEINLINE const ScriptFileContext* GetWithinClassContext() const
	{
		return &withinClassContext;
	}

	/**
	 * @brief Get within class name
	 * @return Return within class name
	 */
	FORCEINLINE const std::wstring& GetWithinClassName() const
	{
		return withinClassName;
	}

	/**
	 * @brief Set native class group
	 *
	 * @param InNativeClassGroupContext		Native class group context
	 * @param InNativeClassGroup			Native class group
	 */
	FORCEINLINE void SetNativeClassGroup( const ScriptFileContext& InNativeClassGroupContext, const std::wstring& InNativeClassGroup )
	{
		nativeClassGroupContext = InNativeClassGroupContext;
		nativeClassGroup		= InNativeClassGroup;
	}

	/**
	 * @brief Get native class group context
	 * @return Return native class group context
	 */
	FORCEINLINE const ScriptFileContext* GetNativeClassGroupContext() const
	{
		return &nativeClassGroupContext;
	}

	/**
	 * @brief Get native class group
	 * @return Return native class group
	 */
	FORCEINLINE const std::wstring& GetNativeClassGroup() const
	{
		return nativeClassGroup;
	}

private:
	uint32									flags;						/**< Class flags (see EClassFlags) */
	ScriptScopeStub							scope;						/**< Scope of the class */
	ScriptFileContext						superClassContext;			/**< Super class context */
	std::wstring							superClassName;				/**< Super class name */
	ScriptFileContext						withinClassContext;			/**< Within class context */
	std::wstring							withinClassName;			/**< Within class name */
	std::wstring							cppText;					/**< C++ code that will be placed into header (only for native classes) */
	ScriptFileContext						nativeClassGroupContext;	/**< Native class group context (only for native classes) */
	std::wstring							nativeClassGroup;			/**< Native class group (only for native classes) */
	CClass*									createdClass;				/**< Created class for script environment */
	std::vector<ScriptFunctionStubPtr_t>	functions;					/**< Functions */
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
	 */
	CScriptSystemStub();

	/**
	 * @brief Add a class
	 * @param InClassStub	Class stub
	 */
	FORCEINLINE void AddClass( const ScriptClassStubPtr_t& InClassStub )
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
	FORCEINLINE ScriptClassStubPtr_t FindClass( const std::wstring& InClassName ) const
	{
		auto	it = classesMap.find( InClassName );
		return it != classesMap.end() ? it->second : nullptr;
	}

	/**
	 * @brief Add a struct
	 * @param InStructStub	Struct stub
	 */
	FORCEINLINE void AddStruct( const ScriptStructStubPtr_t& InStructStub )
	{
		structs.push_back( InStructStub );
		structsMap.insert( std::make_pair( InStructStub->GetName(), InStructStub ) );
	}

	/**
	 * @brief Find struct stub by name
	 *
	 * @param InStructName	Struct name
	 * @return Return found struct stub. If not found returns NULL
	 */
	FORCEINLINE ScriptStructStubPtr_t FindStruct( const std::wstring& InStructName ) const
	{
		auto	it = structsMap.find( InStructName );
		return it != structsMap.end() ? it->second : nullptr;
	}

	/**
	 * @brief Add a enum
	 * @param InEnumStub	Enum stub
	 */
	FORCEINLINE void AddEnum( const ScriptEnumStubPtr_t& InEnumStub )
	{
		enums.push_back( InEnumStub );
		enumsMap.insert( std::make_pair( InEnumStub->GetName(), InEnumStub ) );
	}

	/**
	 * @brief Find enum stub by name
	 *
	 * @param InEnumName	Enum name
	 * @return Return found enum stub. If not found returns NULL
	 */
	FORCEINLINE ScriptEnumStubPtr_t FindEnum( const std::wstring& InEnumName ) const
	{
		auto	it = enumsMap.find( InEnumName );
		return it != enumsMap.end() ? it->second : nullptr;
	}

	/**
	 * @brief Get array of all classes
	 * @return Return array of all classes
	 */
	FORCEINLINE const std::vector<ScriptClassStubPtr_t>& GetClasses() const
	{
		return classes;
	}

	/**
	 * @brief Get array of all classes
	 * @return Return array of all classes
	 */
	FORCEINLINE std::vector<ScriptClassStubPtr_t>& GetClasses()
	{
		return classes;
	}

	/**
	 * @brief Get array of all structs
	 * @return Return array of all structs
	 */
	FORCEINLINE const std::vector<ScriptStructStubPtr_t>& GetStructs() const
	{
		return structs;
	}

	/**
	 * @brief Get array of all structs
	 * @return Return array of all structs
	 */
	FORCEINLINE std::vector<ScriptStructStubPtr_t>& GetStructs()
	{
		return structs;
	}

	/**
	 * @brief Get array of all enums
	 * @return Return array of all enums
	 */
	FORCEINLINE const std::vector<ScriptEnumStubPtr_t>& GetEnums() const
	{
		return enums;
	}

	/**
	 * @brief Get array of all enums
	 * @return Return array of all enums
	 */
	FORCEINLINE std::vector<ScriptEnumStubPtr_t>& GetEnums()
	{
		return enums;
	}

private:
	/**
	 * @brief Constructor of copy
	 * @param InOther	Other script system stub
	 */
	CScriptSystemStub( const CScriptSystemStub& InOther );

	std::vector<ScriptClassStubPtr_t>							classes;	/**< Classes */
	std::unordered_map<std::wstring, ScriptClassStubPtr_t>		classesMap;	/**< Classes for find by name */
	std::vector<ScriptStructStubPtr_t>							structs;	/**< Structs */
	std::unordered_map<std::wstring, ScriptStructStubPtr_t>		structsMap;	/**< Structs for find by name */
	std::vector<ScriptEnumStubPtr_t>							enums;		/**< Enums */
	std::unordered_map<std::wstring, ScriptEnumStubPtr_t>		enumsMap;	/**< Enums for find by name */
};

#endif // !SCRIPTFIELDSTUBS_H