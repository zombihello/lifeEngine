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
#include "Reflection/Function.h"
#include "Scripts/ScriptFileContext.h"
#include "Scripts/ScriptTokenStream.h"

/**
 * @ingroup WorldEd
 * @brief Enumeration of script stub flags
 */
enum EScriptStubFlags
{
	SSF_Native		= 1 << 0		/**< Is a native stub */
};

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
 * @brief Script function parameter stub
 */
class CScriptFunctionParamStub : public CScriptBaseStub
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InContext		Parameter context
	 * @param InParamName	Parameter name
	 * @param InTypeContext	Type context
	 * @param InTypeName	Type name
	 */
	CScriptFunctionParamStub( const ScriptFileContext& InContext, const std::wstring& InParamName, const ScriptFileContext& InTypeContext, const std::wstring& InTypeName );

	/**
	 * @brief Get parameter type context
	 * @return Return parameter type context
	 */
	FORCEINLINE const ScriptFileContext* GetTypeContext() const
	{
		return &typeContext;
	}

	/**
	 * @brief Get parameter type name
	 * @return Return parameter type name
	 */
	FORCEINLINE const std::wstring& GetTypeName() const
	{
		return typeName;
	}

private:
	ScriptFileContext		typeContext;		/**< Return type context */
	std::wstring			typeName;			/**< Return type name */
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
	 * @param InFlags					Flags (look EScriptStubFlags)
	 */
	CScriptFunctionStub( const ScriptFileContext& InContext, const std::wstring& InFunctionName, const ScriptFileContext& InReturnTypeContext, const std::wstring& InReturnTypeName, uint32 InFlags );

	/**
	 * @brief Add a new function parameter
	 * @param InFunctionParam	A new function parameter
	 */
	FORCEINLINE void AddParam( const TSharedPtr<CScriptFunctionParamStub>& InFunctionParam )
	{
		parameters.push_back( InFunctionParam );
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
	 * @brief Get array of function parameters
	 * @return Return array of function parameters
	 */
	FORCEINLINE const std::vector<TSharedPtr<CScriptFunctionParamStub>>& GetParams() const
	{
		return parameters;
	}

	/**
	 * @brief Is the function native
	 * @return Return TRUE if the function is native
	 */
	FORCEINLINE bool IsNative() const
	{
		return bIsNative;
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

private:
	bool												bIsNative;				/**< Is the function native */
	ScriptScopeStub										scope;					/**< Scope of the function */
	CScriptTokenStream									code;					/**< Function code */
	ScriptFileContext									returnTypeContext;		/**< Return type context */
	std::wstring										returnTypeName;			/**< Return type name */
	std::vector<TSharedPtr<CScriptFunctionParamStub>>	parameters;				/**< Array of function parameters */
	CFunction*											createdFunction;		/**< Created function for script environment */
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
	 * @param InFlags				Flags (look EScriptStubFlags)
	 */
	CScriptClassStub( const ScriptFileContext& InContext, const std::wstring& InClassName, const ScriptFileContext& InSuperClassContext, const std::wstring& InSuperClassName, uint32 InFlags );

	/**
	 * @brief Add a new function
	 * @param InFunction	A new function
	 */
	FORCEINLINE void AddFunction( const TSharedPtr<CScriptFunctionStub>& InFunction )
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
	 * @brief Get array of functions
	 * @return Return array of functions
	 */
	FORCEINLINE const std::vector<TSharedPtr<CScriptFunctionStub>>& GetFunctions() const
	{
		return functions;
	}

	/**
	 * @brief Is the class native
	 * @return Return TRUE if the class is native
	 */
	FORCEINLINE bool IsNative() const
	{
		return bIsNative;
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

private:
	bool											bIsNative;			/**< Is the class native */
	ScriptScopeStub									scope;				/**< Scope of the class */
	ScriptFileContext								superClassContext;	/**< Super class context */
	std::wstring									superClassName;		/**< Super class name */
	std::vector<TSharedPtr<CScriptFunctionStub>>	functions;			/**< Functions */
	CClass*											createdClass;		/**< Created class for script environment */
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

private:
	/**
	 * @brief Constructor of copy
	 * @param InOther	Other script system stub
	 */
	CScriptSystemStub( const CScriptSystemStub& InOther );

	std::vector<TSharedPtr<CScriptClassStub>>							classes;	/**< Top level classes */
	std::unordered_map<std::wstring, TSharedPtr<CScriptClassStub>>		classesMap;	/**< Top level classes for find by name */
};

#endif // !SCRIPTFIELDSTUBS_H