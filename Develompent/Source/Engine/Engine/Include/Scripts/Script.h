/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>

#include "Logger/LoggerMacros.h"
#include "Containers/StringConv.h"
#include "System/Package.h"

/**
 * @ingroup Engine
 * @brief Class for work with scripts
 */
class FScript : public FAsset
{
public:
	/**
	 * @brief Constructor
	 */
										FScript();

	/**
	 * @brief Destructor
	 */
										~FScript();

	/**
	 * @brief Serialize script
	 */
	virtual void						Serialize( class FArchive& InArchive ) override;

	/**
	 * @brief Execute function
	 * 
	 * @param[in] InFunctionName Function name
	 * @param[in] InArgs Arguments of function
	 */
	template< typename... TArgs >
	FORCEINLINE void					Execute( const achar* InFunctionName, TArgs... InArgs )
	{
		luabridge::LuaRef		luaRef = luabridge::getGlobal( luaVM, InFunctionName );
		if ( luaRef && luaRef.isFunction() )
		{
			luaRef( InArgs... );
		}
		else
		{
			LE_LOG( LT_Warning, LC_Script, TEXT( "Script function [%s] not found in script" ), ANSI_TO_TCHAR( InFunctionName ) );
		}
	}

	/**
	 * @brief Execute function
	 * 
	 * @param[in] InFunctionName Function name
	 * @param[in] InArgs Arguments of function
	 * @return Return result of script function
	 */
	template< typename TReturnType, typename... TArgs >
	FORCEINLINE TReturnType				Execute( const achar* InFunctionName, TArgs... InArgs )
	{
		luabridge::LuaRef		luaRef = luabridge::getGlobal( luaVM, InFunctionName );
		if ( luaRef && luaRef.isFunction() )
		{
			return luaRef( InArgs... );
		}

		LE_LOG( LT_Warning, LC_Script, TEXT( "Script function [%s] not found in script" ), ANSI_TO_TCHAR( InFunctionName ) );
		return TReturnType();
	}

	/**
	 * @brief Set name of script
	 * @param[in] InName
	 */
	FORCEINLINE void					SetName( const achar* InName )
	{
		name = InName;
	}

	/**
	 * @brief Set byte code
	 * @param[in] InByteCode Byte code of script
	 * @param[in] InType Byte code type
	 */
	FORCEINLINE void					SetByteCode( const std::vector< byte >& InByteCode )
	{
		SetByteCode( InByteCode.data(), ( uint32 )InByteCode.size() );
	}

	/**
	 * @brief Set byte code
	 * @param[in] InByteCode Byte code of script
	 * @param[in] InSize Size of byte code
	 * @param[in] InType Byte code type
	 */
	void								SetByteCode( const byte* InByteCode, uint32 InSize );

	/**
	 * @brief Get variable
	 * @param[in] InVariableName Variable name
	 * 
	 * @return Return script variable
	 */
	template< typename TType >
	FORCEINLINE TType					GetVariable( const achar* InVariableName ) const
	{
		luabridge::LuaRef		luaRef = luabridge::getGlobal( luaVM, InVariableName );
		if ( luaRef && !luaRef.isFunction() )
		{
			return luaRef;
		}

		LE_LOG( LT_Warning, LC_Script, TEXT( "Script variable [%s] not found in script" ), ANSI_TO_TCHAR( InVariableName ) );
		return TType();
	}

	/**
	 * @brief Get name of script
	 * @return Return name of script
	 */
	FORCEINLINE const achar*			GetName() const
	{
		return name.c_str();
	}

private:
	struct lua_State*		luaVM;				/**< Pointer to lua virtual machine */
	std::vector< byte >		byteCode;			/**< Byte code */
	std::string				name;				/**< Name of script */
};

#endif // !SCRIPT_H
