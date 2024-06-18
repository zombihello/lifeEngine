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
	 * @param InSuperClassContext	Super class context
	 * @param InSuperClassName		Super class name
	 * @param InFlags				Flags (see EClassFlags)
	 */
	CScriptClassStub( const ScriptFileContext& InContext, const std::wstring& InClassName, const ScriptFileContext& InSuperClassContext, const std::wstring& InSuperClassName, uint32 InFlags );

	/**
	 * @brief Constructor
	 *
	 * @param InContext				Class context
	 * @param InClassName			Class name
	 * @param InSuperClassContext	Super class context
	 * @param InSuperClassName		Super class name
	 * @param InWithinClassContext	Within class context
	 * @param InWithinClassName		Within class name
	 * @param InFlags				Flags (see EClassFlags)
	 */
	CScriptClassStub( const ScriptFileContext& InContext, const std::wstring& InClassName, const ScriptFileContext& InSuperClassContext, const std::wstring& InSuperClassName, const ScriptFileContext& InWithinClassContext, const std::wstring& InWithinClassName, uint32 InFlags );

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

private:
	uint32									flags;				/**< Class flags (see EClassFlags) */
	ScriptScopeStub							scope;				/**< Scope of the class */
	ScriptFileContext						superClassContext;	/**< Super class context */
	std::wstring							superClassName;		/**< Super class name */
	ScriptFileContext						withinClassContext;	/**< Within class context */
	std::wstring							withinClassName;	/**< Within class name */
	std::wstring							cppText;			/**< C++ code that will be placed into header (only for native classes) */
	CClass*									createdClass;		/**< Created class for script environment */
	std::vector<ScriptFunctionStubPtr_t>	functions;			/**< Functions */
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

private:
	/**
	 * @brief Constructor of copy
	 * @param InOther	Other script system stub
	 */
	CScriptSystemStub( const CScriptSystemStub& InOther );

	std::vector<ScriptClassStubPtr_t>							classes;	/**< Top level classes */
	std::unordered_map<std::wstring, ScriptClassStubPtr_t>		classesMap;	/**< Top level classes for find by name */
};

#endif // !SCRIPTFIELDSTUBS_H