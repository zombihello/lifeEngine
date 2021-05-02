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
			return TypeIDToString( typeID ) + " " + name;
		}

		std::string		name;		/**< Name of param */
		int32			typeID;		/**< AngelScript type id */
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
	void				CompileModule( class CScriptBuilder* InScriptBuilder, const tchar* InNameModule, const tchar* InPathToModuleDir, const tchar* InOutputPath  );

	/**
	 * @brief Generate C++ headers for module
	 * @warning This method only available with enabled macro WITH_EDITOR
	 * 
	 * @param[in] InScriptModule Pointer to script module
	 * @param[in] InOutputPath Path to directory for create C++ header
	 */
	void				GenerateHeadersForModule( class asIScriptModule* InScriptModule, const tchar* InOutputPath );

	/**
	 * @brief Generate C++ code of function
	 * @warning This method only available with enabled macro WITH_EDITOR
	 * 
	 * @param[in] InScriptModule Pointer to script module
	 * @param[in] InIndexFunction Index of function in script module
	 * @return Code of C++ function
	 */
	std::string			GenerateCPPFunction( class asIScriptModule* InScriptModule, uint32 InIndexFunction );

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
	 * @return Converted AngelScript type ID to C++ name
	 */
	static std::string	TypeIDToString( int32 InTypeID );

	/**
	 * @brief Register name of type from AngelScript to C++
	 * @warning This method only available with enabled macro WITH_EDITOR
	 * 
	 * @param[in] InASType Name of type in AngelScript
	 * @param[in] InCPPType Name of type in C++
	 */
	void				RegisterTypeASToCPP( const std::string& InASType, const std::string& InCPPType );

	std::unordered_map< int32, std::string >				tableTypesASToCPP;			/**< Table of types for convert from AngelScript to C++ */
#endif // WITH_EDITOR

	class asIScriptEngine*									asScriptEngine;				/**< Point to AngelScript engine */
};

#endif // !SCRIPTENGINE_H
