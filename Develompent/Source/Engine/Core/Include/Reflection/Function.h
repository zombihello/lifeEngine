/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef FUNCTION_H
#define FUNCTION_H

#include <vector>

#include "Reflection/Struct.h"

/**
 * @ingroup Core
 * @brief Macro to define a function for scripts
 *
 * @param Class			Name of the class where the function is
 * @param FunctionName	Function name
 */
#define SCRIPT_MAP_FUNC( Class, FunctionName ) \
	{ #Class #FunctionName, ( ScriptFn_t )&Class::FunctionName },

/**
 * @ingroup Core
 * @brief Type of a callable function by reflection system
 */
typedef void ( CObject::*ScriptFn_t )( ScriptFrame& InStack );

/**
 * @ingroup Core
 * @brief Description of a callable function by reflection system
 */
struct ReflectionFunctionDesc
{
	const achar*		name;		/**< Function name */
	ScriptFn_t		FunctionFn;	/**< Pointer to function */
};

/**
 * @ingroup Core
 * @brief Enum function flags
 */
enum EFunctionFlags
{
	FUNC_Native = 1 << 0		/**< Native function */
};

/**
 * @ingroup Core
 * @brief Function description for reflection
 */
class CFunction : public CStruct
{
	DECLARE_CLASS_INTRINSIC( CFunction, CStruct, 0, 0, TEXT( "Core" ) )
	DECLARE_WITHIN_CLASS( CClass )

public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE CFunction()
		: functionFlags( 0 )
		, FunctionFn( nullptr )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InFunctionName		Function name
	 * @param InFunctionFlags		Class flags
	 * @param InSuperFunction		Pointer to super function
	 */
	CFunction( uint32 InFunctionFlags, CFunction* InSuperFunction = nullptr );

	/**
	 * @brief Serialize object
	 * @param InArchive     Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Bind function
	 */
	virtual void Bind() override;

	/**
	 * @brief Get function to execute script opcode
	 *
	 * @param InOpcode  Opcode to execute
	 * @return Return function to execute script opcode
	 */
	static FORCEINLINE ScriptFn_t StaticGetOpcodeFunc( uint32 InOpcode )
	{
		return OpcodeFunctions[InOpcode];
	}

	/**
	 * @brief Set super function
	 * @param InSuperFunction	Super function
	 */
	FORCEINLINE void SetSuperFunction( CFunction* InSuperFunction )
	{
		SetSuperStruct( ( CStruct* )InSuperFunction );
	}

	/**
	 * @brief Set bytecode
	 * @param InBytecode	Bytecode
	 */
	FORCEINLINE void SetBytecode( const std::vector<byte>& InBytecode )
	{
		bytecode = InBytecode;
	}

	/**
	 * @brief Get function bytecode (only for script function)
	 * @return Return function's bytecode. If this function is native returns empty array
	 */
	FORCEINLINE const std::vector<byte>& GetBytecode() const
	{
		return bytecode;
	}

	/**
	 * @brief Get super function
	 * @return Return pointer to super function. If it is not, it will return nullptr
	 */
	FORCEINLINE CFunction* GetSuperFunction() const
	{
		return ( CFunction* )GetSuperStruct();
	}

	/**
	 * @brief Has any function flags
	 *
	 * @param InCheckFlags		Function flags to check for
	 * @return Return TRUE if the passed in flag is set (including no flag passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAnyFunctionFlags( uint32 InCheckFlags ) const
	{
		return ( functionFlags & InCheckFlags ) != 0;
	}

	/**
	 * @brief Has all function flags
	 *
	 * @param InCheckFlags		Function flags to check for
	 * @return Return TRUE if all of the passed in flags are set (including no flags passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAllFunctionFlags( uint32 InCheckFlags ) const
	{
		return ( functionFlags & InCheckFlags ) == InCheckFlags;
	}

	/**
	 * @brief Get pointer to function
	 * @return Return pointer to function. If the function isn't bind returns NULL
	 */
	FORCEINLINE ScriptFn_t GetFunction() const
	{
		return FunctionFn;
	}

private:
	static void ( CObject::*OpcodeFunctions[OP_Num] )( ScriptFrame& InStack );       /**< Native functions to execute script opcodes */
	uint32					functionFlags;	/**< Function flags */
	ScriptFn_t				FunctionFn;		/**< Function to call */
	std::vector<byte>		bytecode;		/**< Script bytecode */
};

#endif // !FUNCTION_H