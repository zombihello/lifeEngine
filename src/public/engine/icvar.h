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

#ifndef ICVAR_H
#define ICVAR_H

#include "appframework/iappsystem.h"
#include "engine/iconcmd.h"
#include "engine/iconvar.h"
#include "stdlib/math/color.h"

/**
 * @ingroup engine
 * @brief Cvar query interface version
 */
#define CVAR_QUERY_INTERFACE_VERSION "LCvarQuery001"

/**
 * @ingroup engine
 * @brief Applications can implement this to modify behavior in ICvar
 */
class ICvarQuery : public IAppSystem
{
public:
	/**
	 * @brief Can these two IConVars be aliased
	 * 
	 * @param pChildVar		Child variable
	 * @param pParentVar	Parent variable
	 * @return Return TRUE if these two IConVars can be aliased, otherwise returns FALSE
	 */
	virtual bool AreConVarsLinkable( const IConVar* pChildVar, const IConVar* pParentVar ) = 0;
};

/**
 * @ingroup engine
 * @brief Console system interface version
 */
#define CVAR_INTERFACE_VERSION "LCvar001"

/**
 * @ingroup engine
 * @brief Used to display console messages
 */
class IConsoleDisplayFunc
{
public:
	/**
	 * @brief Print message into console
	 * @param pMsg	Message
	 */
	virtual void Print( const achar* pMsg ) = 0;

	/**
	 * @brief Print message into console
	 * 
	 * @param pMsg	Message
	 * @param color	Color;
	 */
	virtual void Print( const achar* pMsg, const CColor& color ) = 0;
};

/**
 * @ingroup engine
 * @brief The console system interface
 */
class ICvar : public IAppSystem
{
public:
	/**
	 * @brief Allocate a unique DLL identifier
	 * @return Return allocated a unique DLL identifier
	 */
	virtual CVarDLLIdentifier_t AllocateDLLIdentifier() = 0;

	/**
	 * @brief Register command
	 * @param pCommand	Console command
	 */
	virtual void RegisterCommand( IConCmdBase* pCommand ) = 0;

	/**
	 * @brief Unregister command
	 * @param pCommand	Console command
	 */
	virtual void UnregisterCommand( IConCmdBase* pCommand ) = 0;

	/**
	 * @brief Unregister commands
	 * @param dllIdentifier		DLL identifier
	 */
	virtual void UnregisterCommands( CVarDLLIdentifier_t dllIdentifier ) = 0;

	/**
	 * @brief Execute command
	 *
	 * @param pCommand		Command
	 * @return Return TRUE if command is successfully executed, otherwise returns FALSE
	 */
	virtual bool Exec( const achar* pCommand ) = 0;

	/**
	 * @brief Find command base by name
	 * 
	 * @param pName		Command name
	 * @return Return pointer to command base if found, otherwise returns NULL
	 */
	virtual IConCmdBase* FindCommandBase( const achar* pName ) const = 0;

	/**
	 * @brief Find command by name
	 *
	 * @param pName		Command name
	 * @return Return pointer to command if found, otherwise returns NULL
	 */
	virtual IConCmd* FindCommand( const achar* pName ) const = 0;

	/**
	 * @brief Find variable by name
	 *
	 * @param pName		Command name
	 * @return Return pointer to variable if found, otherwise returns NULL
	 */
	virtual IConVar* FindVar( const achar* pName ) const = 0;

	/**
	 * @brief Reset cvars which contain a specific flags
	 * Sets cvars containing the flags to their default value
	 * 
	 * @param flags		Cvar flags
	 */
	virtual void ResetFlaggedVars( uint32 flags ) = 0;

	/**
	 * @brief Get the first IConCmdBase to allow iteration over all IConCmd and IConVars
	 * @return Return the first IConCmdBase to allow iteration. If in the system isn't any commands returns NULL
	 */
	virtual IConCmdBase* GetCommands() const = 0;

	/**
	 * @brief Set global change callback of IConVars
	 * @note To be called when any IConVar changes
	 * 
	 * @param pChangeCallbackFn		Change callback
	 */
	virtual void SetGlobalChangeCallback( ConVarChangeCallbackFn_t pChangeCallbackFn ) = 0;

	/**
	 * @brief Call global change callback
	 * @param pConVar	Changed cvar
	 */
	virtual void CallGlobalChangeCallback( IConVar* pConVar ) = 0;

	/**
	 * @brief Set console display function
	 * @param pConsoleDisplayFunc	Console display function
	 */
	virtual void SetConsoleDisplayFunc( IConsoleDisplayFunc* pConsoleDisplayFunc ) = 0;

	/**
	 * @brief Print message into console
	 * 
	 * @param color		Message color
	 * @param pFormat	Message format
	 * @param ...		Other message parameters
	 */
	virtual void ConsolePrintf( const CColor& color, const achar* pFormat, ... ) = 0;

	/**
	 * @brief Print message into console
	 *
	 * @param pFormat	Message format
	 * @param ...		Other message parameters
	 */
	virtual void ConsolePrintf( const achar* pFormat, ... ) = 0;

	/**
	 * @brief Set ICvarQuery for the console system
	 * 
	 * Method allowing the engine ICvarQuery interface to take over
	 * A little hacky, owing to the fact the engine is loaded
	 * well after ICvar, so we can't use the standard connect pattern
	 * 
	 * @param pCvarQuery	Cvar query to set. If NULL ICvar set to default ICvarQuery
	 */
	virtual void SetCVarQuery( ICvarQuery* pCvarQuery ) = 0;
};

#endif // !ICVAR_H