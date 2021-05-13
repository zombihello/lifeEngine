/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "Core.h"
#include "Misc/Types.h"

/**
 * @ingroup Engine
 * @brief Class for work with script engine
 */
class ScriptEngine
{
public:
	/**
	 * @brief Constructor
	 */
												ScriptEngine();

	/**
	 * @brief Destructor
	 */
												~ScriptEngine();

	/**
	 * @brief Initialize
	 */
	void										Init();

	/**
	 * @brief Load script modules
	 */
	void										LoadModules();

#if WITH_EDITOR
	/**
	 * @brief Compile scripts and generate headers for C++
	 * @warning This method only available with enabled macro WITH_EDITOR
	 * 
	 * @param[in] InCmdLine Command-line of arguments start
	 */
	void										Make( const tchar* InCmdLine );
#endif // WITH_EDITOR
	
	/**
	 * @brief Get pointer to AngelScript engine
	 * @return Pointer to AngelScript engine, if script engine not initialized return nullptr
	 */
	FORCEINLINE class asIScriptEngine*			GetASScriptEngine() const
	{
		return asScriptEngine;
	}

private:

#if WITH_EDITOR
	/**
	 * @brief Struct of declaration param of functions/methods
	 */
	struct SCPPParam
	{
		/**
		 * @brief Convert struct to string
		 * @return Return converted struct to string
		 */
		FORCEINLINE std::string			ToString() const
		{
			return TypeIDToString( typeID, flags ) + " " + name;
		}

		uint32				id;				/**< ID of param */
		std::string			name;			/**< Name of param */
		int32				typeID;			/**< AngelScript type id */
		int64				flags;			/**< Modifiers of type */
	};

	/**
	 * @brief Struct of declaration AngleScript type
	 */
	struct SASType
	{
		std::string				cppName;		/**< Name of type in C++ */
		class asITypeInfo*		asTypeInfo;		/**< AngleScript type info */
	};

	/**
	 * @brief Structure for describing the necessary parameters that must be initialized when the engine starts
	 */
	struct ModuleInitDesc
	{
		std::vector< std::string >		externDefine;			/**< Array for define an external values */
		std::vector< std::string >		globalVarInit;			/**< Array for initialize global values */
	};

	/**
	 * @brief Compile module script
	 * @warning This method only available with enabled macro WITH_EDITOR
	 *
	 * @param[in] InScriptBuilder Pointer to AngleScript addon 'ScriptBuilder'
	 * @param[in] InNameModule Name of module
	 * @param[in] InPathToModuleDir Path to directory with source scripts	 
	 * @param[in] InOutputPath Path to directory for save bytecode of module
	 */
	void				CompileModule( class CScriptBuilder* InScriptBuilder, const tchar* InNameModule, const tchar* InPathToModuleDir, const tchar* InOutputPath );

	/**
	 * @brief Generate C++ headers for module
	 * @warning This method only available with enabled macro WITH_EDITOR
	 * 
	 * @param[in] InScriptModule Pointer to script module
	 * @param[in] InOutputPath Path to directory for create C++ header
	 */
	void				GenerateHeadersForModule( class asIScriptModule* InScriptModule, const tchar* InOutputPath );

	/**
	 * @brief Generate C++ code of typedefs
	 * @warning This method only available with enabled macro WITH_EDITOR
	 * 
	 * @param[in] InScriptModule Pointer to script module
	 * @param[in] InIndexTypedef Index of typedef in script module
	 * @return Code of C++ typedef
	 */
	std::string			GenerateCPPTypedef( class asIScriptModule* InScriptModule, uint32 InIndexTypedef );

	/**
	 * @brief Generate C++ code of function
	 * @warning This method only available with enabled macro WITH_EDITOR
	 *
	 * @param[in] InScriptFunction Pointer to script function
	 * @param[in] InIndexFunction Index of function in script module or class 
	 * @param[in] InIsMethod Is method of script class?
	 * @return Code of C++ function
	 */
	std::string			GenerateCPPFunction( class asIScriptFunction* InScriptFunction, uint32 InIndexFunction, bool InIsMethod = false );

	/**
	 * @brief Generate C++ code of enum
	 * @warning This method only available with enabled macro WITH_EDITOR
	 * 
	 * @param[in] InEnumType Pointer to script enum
	 * @param[in] InIndexEnum Index of enum in script module
	 * @return Code of C++ enum
	 */
	std::string			GenerateCPPEnum( class asITypeInfo* InEnumType, uint32 InIndexEnum );

	/**
	 * @brief Generate C++ code of global value
	 * @warning This method only available with enabled macro WITH_EDITOR
	 * 
	 * @param[in] InScriptModule Pointer to script module
	 * @param[in] InIndexGlobalValue Index of global value in script module
	 * @param[in,out] InOutModuleInitDesc Pointer to descriptor for init module
	 * @return Code of C++ global value
	 */
	std::string			GenerateCPPGlobalValue( class asIScriptModule* InScriptModule, uint32 InIndexGlobalValue, ModuleInitDesc* InOutModuleInitDesc );

	/**
	 * @brief Generate C++ code of class
	 * @warning This method only available with enabled macro WITH_EDITOR
	 * 
	 * @param[in] InClassType Pointer to script class
	 * @param[in] InIndexClass Index of class in script module
	 * @return Code of C++ class
	 */
	std::string			GenerateCPPClass( class asITypeInfo* InClassType, uint32 InIndexClass );

	/**
	 * @brief Get C++ param from function
	 * @warning This method only available with enabled macro WITH_EDITOR
	 * 
	 * @param[in] InScriptFunction Pointer to script function
	 * @param[in] InIndexParam Index of param in function
	 * @return Return struct SCPPParam with declaration of param
	 */
	SCPPParam			GetCPPParamFromFunction( class asIScriptFunction* InScriptFunction, uint32 InIndexParam ) const;

	/**
	 * @brief Convert AngelScript type ID to C++ name
	 * @warning This method only available with enabled macro WITH_EDITOR
	 * 
	 * @param[in] InTypeID AngelScript type ID
	 * @param[in] InFlags Modifiers of type
	 * @param[out] OutIsPointer Is pointer?
	 * @return Converted AngelScript type ID to C++ name
	 */
	static std::string	TypeIDToString( int32 InTypeID, int64 InFlags = 0, bool* OutIsPointer = nullptr );

	/**
	 * @brief Register name of types from AngelScript to C++
	 * @warning This method only available with enabled macro WITH_EDITOR
	 * 
	 * @param[in] InScriptModule Pointer to script module
	 */
	void				RegisterTypesASToCPP( class asIScriptModule* InScriptModule );

	/**
	 * @brief Get C++ code for set arg of function/constructor/method
	 * @warning This method only available with enabled macro WITH_EDITOR
	 * 
	 * @param[in] InParam Param for set arg
	 */
	std::string			GetCallMethodOfScriptContext( const SCPPParam& InParam );

	std::unordered_map< int32, SASType >					tableTypesASToCPP;			/**< Table of types for convert from AngelScript to C++ */
#endif // WITH_EDITOR

	/**
	 * @brief Register native types for AngelScript
	 */
	void				RegisterNativeTypes();

	class asIScriptEngine*									asScriptEngine;				/**< Point to AngelScript engine */
};

#endif // !SCRIPTENGINE_H
