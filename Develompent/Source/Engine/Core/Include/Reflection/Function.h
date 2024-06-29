/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef FUNCTION_H
#define FUNCTION_H

#include "Reflection/Struct.h"
#include "Scripts/ScriptMacros.h"
#include "System/MemoryArchive.h"

/**
 * @ingroup Core
 * @brief Function description for reflection
 */
class CFunction : public CStruct
{
	DECLARE_CLASS_INTRINSIC( CFunction, CStruct, 0, CASTCLASS_CFunction, TEXT( "Core" ) )
	DECLARE_WITHIN_CLASS( CClass )

public:
	/**
	 * @brief Constructor
	 */
	CFunction();

	/**
	 * @brief Constructor
	 *
	 * @param InFunctionFlags		Function flags (see EFunctionFlags)
	 * @param InSuperFunction		Pointer to super function
	 */
	CFunction( uint32 InFunctionFlags, CFunction* InSuperFunction = nullptr );

	/**
	 * @brief Serialize object
	 * @param InArchive     Archive for serialize
	 */
	virtual void Serialize( CArchive& InArchive ) override;

	/**
	 * @brief Bind function
	 */
	virtual void Bind() override;

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
	 * @brief Set function parameters size
	 * @param InParamsSize		Function parameters size
	 */
	FORCEINLINE void SetParamsSize( uint32 InParamsSize )
	{
		paramsSize = InParamsSize;
	}

	/**
	 * @brief Set number of function parameters
	 * @param InNumParams	Number of function parameters
	 */
	FORCEINLINE void SetNumParams( uint32 InNumParams )
	{
		numParams = InNumParams;
	}

	/**
	 * @brief Get super function
	 * @return Return pointer to super function. If it is not return NULL
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

	/**
	 * @brief Get function parameters size
	 * @return Return function parameters size
	 */
	FORCEINLINE uint32 GetParamsSize() const
	{
		return paramsSize;
	}

	/**
	 * @brief Get number of function parameters
	 * @return Return number of function parameters
	 */
	FORCEINLINE uint32 GetNumParams() const
	{
		return numParams;
	}

private:
	/**
	 * @brief Serialize opcode
	 * @param InOutByteCodeIndex	Bytecode index. After serialization will be shifted on size of a type
	 * @param InArchive				Archive
	 */
	void SerializeOpcode( uint32& InOutByteCodeIndex, CArchive& InArchive );

	/**
	 * @brief Serialize opcode parameter
	 * @param InOutByteCodeIndex		Bytecode index. After serialization will be shifted on size of a type
	 * @param InArchive					Archive
	 */
	template<typename TType>
	FORCEINLINE void SerializeOpcodeParam( uint32& InOutByteCodeIndex, CArchive& InArchive )
	{
		TType		value;
		if ( InArchive.IsSaving() )
		{
			Memory::Memcpy( &value, &bytecode[InOutByteCodeIndex], sizeof( TType ) );
		}
		InArchive << value;
		if ( InArchive.IsLoading() )
		{
			Memory::Memcpy( &bytecode[InOutByteCodeIndex], &value, sizeof( TType ) );
		}

		InOutByteCodeIndex += sizeof( TType );
	}

	/**
	 * @brief Serialize opcode parameter (for string constants)
	 * @param InOutByteCodeIndex		Bytecode index. After serialization will be shifted on size of a type
	 * @param InArchive					Archive
	 */
	FORCEINLINE void SerializeOpcodeParamStr( uint32& InOutByteCodeIndex, CArchive& InArchive )
	{
		do
		{
			SerializeOpcodeParam<achar>( InOutByteCodeIndex, InArchive );
		}
		while ( bytecode[InOutByteCodeIndex-1] );
	}

	/**
	 * @brief Serialize opcode parameter (for pointer types, e.g: CFunction*, CClass*)
	 * @param InOutByteCodeIndex		Bytecode index. After serialization will be shifted on size of a type
	 * @param InArchive					Archive
	 */
	template<typename TType>
	FORCEINLINE void SerializeOpcodeParamPtr( uint32& InOutByteCodeIndex, CArchive& InArchive )
	{
		TType*		value = nullptr;
		if ( InArchive.IsSaving() )
		{
			Memory::Memcpy( &value, &bytecode[InOutByteCodeIndex], sizeof( uptrint ) );
		}
		InArchive << value;
		if ( InArchive.IsLoading() )
		{
			Memory::Memcpy( &bytecode[InOutByteCodeIndex], &value, sizeof( uptrint ) );
		}

		InOutByteCodeIndex += sizeof( uptrint );
	}

	uint32				functionFlags;	/**< Function flags */
	uint32				paramsSize;		/**< Function parameters size in bytes */
	uint32				numParams;		/**< Number of function parameters */
	ScriptFn_t			FunctionFn;		/**< Function to call */
	std::vector<byte>	bytecode;		/**< Script bytecode */
};

#endif // !FUNCTION_H