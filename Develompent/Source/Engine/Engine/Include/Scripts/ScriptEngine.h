/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include "LEBuild.h"
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
	 * @brief Enumeration of C++ types from AngelScript
	 */
	enum ECPPType
	{
		CPPT_Unknown,		/**< Unknown C++ type */
		CPPT_Void,			/**< Void */
		CPPT_Bool,			/**< Bool */
		CPPT_Int8,			/**< Signed int8 */
		CPPT_Int16,			/**< Signed int16 */
		CPPT_Int32,			/**< Signed int32 */
		CPPT_Int64,			/**< Signed int64 */
		CPPT_UInt8,			/**< Unsigned int8 */
		CPPT_UInt16,		/**< Unsigned int16 */
		CPPT_UInt32,		/**< Unsigned int32 */
		CPPT_UInt64,		/**< Unsigned int64 */
		CPPT_Float,			/**< Float */
		CPPT_Double			/**< Double */
	};

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
			return ECPPType_To_String( type ) + " " + name;
		}

		std::string		name;		/**< Name of param */
		ECPPType		type;		/**< Type of param */
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
	 * @brief Convert AngleScript TypeID to ECPPType
	 * @warning This method only available with enabled macro WITH_EDITOR
	 * 
	 * @param[in] InTypeID Type id from AngelScript
	 * @return Return type in C++ ECPPType. If type not supported return CPPT_Unknown
	 */
	static ECPPType		TypeID_To_ECPPType( int32 InTypeID );

	/**
	 * @brief Convert ECPPType to string
	 * @warning This method only available with enabled macro WITH_EDITOR
	 * 
	 * @param[in] InCPPType CPP type
	 * @return Converted enum to string
	 */
	static std::string	ECPPType_To_String( ECPPType InCPPType );

#endif // WITH_EDITOR

	class asIScriptEngine*		asScriptEngine;			/**< Point to AngelScript engine */
};

#endif // !SCRIPTENGINE_H
