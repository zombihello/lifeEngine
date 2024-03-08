/**
 * @file
 * @addtogroup engine engine
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

#ifndef ICONCMD_H
#define ICONCMD_H

#include "core/types.h"

/**
 * @ingroup engine
 * @brief IConVars/IConCmds are marked as having a particular DLL identifier
 */
typedef uint32 CVarDLLIdentifier_t;

/**
 * @ingroup engine
 * @brief IConVar and IConCmd flags
 */
enum ECVarFlags
{
	FCVAR_NONE		= 0,		/**< No flags */
	FCVAR_CHEAT		= 1 << 0,	/**< This cvar is a cheat */
	FCVAR_GAMEDLL	= 1 << 1,	/**< Defined by the game DLL */
	FCVAR_ARCHIVE	= 1 << 2	/**< Set to cause it to be saved to a file */
};

/**
 * @ingroup engine
 * @brief The base console invoked command/variable interface
 */
class IConCmdBase
{
public:
	/**
	 * @brief Is command
	 * @return Return TRUE if this interface is a command, otherwise returns FALSE
	 */
	virtual bool IsCommand() const = 0;

	/**
	 * @brief Is set a flag
	 * 
	 * @param flag	Flag to check (see ECVarFlags)
	 * @return Return TRUE if flag is set, otherwise returns FALSE
	 */
	virtual bool IsFlagSet( uint32 flag ) const = 0;

	/**
	 * @brief Get flags
	 * @return Return cvar flags (see ECVarFlags)
	 */
	virtual uint32 GetFlags() const = 0;

	/**
	 * @brief Add flags
	 * @param flags		Flags (see ECVarFlags)
	 */
	virtual void AddFlags( uint32 flags ) = 0;

	/**
	 * @brief Get name for cvar
	 * @return Return name
	 */
	virtual const achar* GetName() const = 0;

	/**
	 * @brief Set help text for cvar
	 * @param pHelpText		Help text. Should be static data
	 */
	virtual void SetHelpText( const achar* pHelpText ) = 0;

	/**
	 * @brief Get help text for cvar
	 * @return Return help text
	 */
	virtual const achar* GetHelpText() const = 0;

	/**
	 * @brief Set next cvar in the global list
	 * @warning Internal usage only!
	 * 
	 * @param pNext		Next cvar in the global list
	 */
	virtual void SetNext( IConCmdBase* pNext ) = 0;

	/**
	 * @brief Get next cvar in the global list
	 * @return Return pointer to the next cvar in the global list. If no exists returns NULL
	 */
	virtual IConCmdBase* GetNext() const = 0;

	/**
	 * @brief Set registered cvar
	 * @warning Internal usage only!
	 * 
	 * @param bRegistered	Is registered cvar
	 */
	virtual void SetRegistered( bool bRegistered ) = 0;

	/**
	 * @brief Has this cvar been registered
	 * @return Returns TRUE on success, FALSE on failure
	 */
	virtual bool IsRegistered() const = 0;

	/**
	 * @brief Get the DLL identifier
	 * @return Returns the DLL identifier
	 */
	virtual CVarDLLIdentifier_t GetDLLIdentifier() const = 0;
};

/**
 * @ingroup engine
 * @brief The console invoked command interface
 */
class IConCmd : public IConCmdBase
{
public:
	/**
	 * @brief Execute command
	 *
	 * @param argc	Argument count
	 * @param argv	Arguments
	 */
	virtual void Exec( uint32 argc, const achar** argv ) = 0;
};

#endif // !ICONCMD_H