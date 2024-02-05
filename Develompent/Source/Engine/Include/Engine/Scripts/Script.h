/**
 * @file
 * @addtogroup Engine Engine
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>

#include "Core/Logger/LoggerMacros.h"
#include "Core/Containers/StringConv.h"
#include "Core/System/Package.h"
#include "Engine/Scripts/ScriptEngine.h"

/**
 * @ingroup Engine
 * @brief Class for work with scripts
 */
class CScript : public CAsset
{
public:
	/**
	 * @brief Constructor
	 */
	CScript();

	/**
	 * @brief Destructor
	 */
	~CScript();

	/**
	 * @brief Serialize script
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Execute function
	 * 
	 * @param[in] InFunctionName Function name
	 * @param[in] InArgs Arguments of function
	 */
	template< typename... TArgs >
	FORCEINLINE void Execute( const achar* InFunctionName, TArgs... InArgs )
	{
		luabridge::LuaRef		luaRef = luabridge::getGlobal( luaVM, InFunctionName );
		if ( luaRef && luaRef.isFunction() )
		{
			luaRef( InArgs... );
		}
		else
		{
			Warnf( TEXT( "Script function [%s] not found in script\n" ), ANSI_TO_TCHAR( InFunctionName ) );
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
	FORCEINLINE TReturnType Execute( const achar* InFunctionName, TArgs... InArgs )
	{
		luabridge::LuaRef		luaRef = luabridge::getGlobal( luaVM, InFunctionName );
		if ( luaRef && luaRef.isFunction() )
		{
			return luaRef( InArgs... );
		}

		Warnf( TEXT( "Script function [%s] not found in script\n" ), ANSI_TO_TCHAR( InFunctionName ) );
		return TReturnType();
	}

	/**
	 * @brief Set name of script
	 * @param[in] InName
	 */
	FORCEINLINE void SetName( const achar* InName )
	{
		name = InName;
	}

	/**
	 * @brief Set byte code
	 * @param[in] InByteCode Byte code of script
	 * @param[in] InType Byte code type
	 */
	FORCEINLINE void SetByteCode( const std::vector< byte >& InByteCode )
	{
		SetByteCode( InByteCode.data(), ( uint32 )InByteCode.size() );
	}

	/**
	 * @brief Set byte code
	 * @param[in] InByteCode Byte code of script
	 * @param[in] InSize Size of byte code
	 * @param[in] InType Byte code type
	 */
	void SetByteCode( const byte* InByteCode, uint32 InSize );

	/**
	 * @brief Get variable
	 * @param[in] InVariableName Variable name
	 * 
	 * @return Return script variable
	 */
	template< typename TType >
	FORCEINLINE TType GetVariable( const achar* InVariableName ) const
	{
		luabridge::LuaRef		luaRef = luabridge::getGlobal( luaVM, InVariableName );
		if ( luaRef && !luaRef.isFunction() )
		{
			return luaRef;
		}

		Warnf( TEXT( "Script variable [%s] not found in script\n" ), ANSI_TO_TCHAR( InVariableName ) );
		return TType();
	}

	/**
	 * @brief Get name of script
	 * @return Return name of script
	 */
	FORCEINLINE const achar* GetName() const
	{
		return name.c_str();
	}

private:
	struct lua_State*		luaVM;				/**< Pointer to lua virtual machine */
	std::vector< byte >		byteCode;			/**< Byte code */
	std::string				name;				/**< Name of script */
};

#endif // !SCRIPT_H
