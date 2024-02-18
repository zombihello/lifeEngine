/**
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

#include "pch_core.h"
#include "core/debug.h"

/*
 ==================
 DefaultLogOutput
 ==================
 */
static void DefaultLogOutput( const achar* pMsg )
{
#if ENABLE_LOGGING
	// Print message to console
	printf( pMsg );

	// Print message to debug output
	if ( Sys_IsDebuggerPresent() )
	{
		Sys_DebugMessage( pMsg );
	}
#endif // ENABLE_LOGGING
}

/**
 * @ingroup core
 * @brief Log output function
 */
static LogOutputFn_t	s_LogOutputFn = &DefaultLogOutput;

/*
==================
Sys_SetLogOutputFunc
==================
*/
void Sys_SetLogOutputFunc( LogOutputFn_t pFunc )
{
	s_LogOutputFn = pFunc ? pFunc : Sys_GetDefaultLogOutput();
}

/*
==================
Sys_GetLogOutputFunc
==================
*/
LogOutputFn_t Sys_GetLogOutputFunc()
{
	return s_LogOutputFn;
}

/*
==================
Sys_GetDefaultLogOutput
==================
*/
LogOutputFn_t Sys_GetDefaultLogOutput()
{
	return &DefaultLogOutput;
}

#if ENABLE_LOGGING
	/*
	==================
	Msg
	==================
	*/
	void Msg( const achar* pFormat, ... )
	{
		va_list			params;
		va_start( params, pFormat );
		VMsg( pFormat, params );
		va_end( params );
	}

	/*
	==================
	VMsg
	==================
	*/
	void VMsg( const achar* pFormat, va_list params )
	{
		s_LogOutputFn( L_sprintf( "Msg: %s\n", L_vsprintf( pFormat, params ).c_str() ).c_str() );
	}

	/*
	==================
	Warning
	==================
	*/
	void Warning( const achar* pFormat, ... )
	{
		va_list			params;
		va_start( params, pFormat );
		VWarning( pFormat, params );
		va_end( params );
	}

	/*
	==================
	VWarning
	==================
	*/
	void VWarning( const achar* pFormat, va_list params )
	{
		s_LogOutputFn( L_sprintf( "Warning: %s\n", L_vsprintf( pFormat, params ).c_str() ).c_str() );
	}

	/*
	==================
	Error
	==================
	*/
	void Error( const achar* pFormat, ... )
	{
		va_list			params;
		va_start( params, pFormat );
		VError( pFormat, params );
		va_end( params );
	}

	/*
	==================
	VError
	==================
	*/
	void VError( const achar* pFormat, va_list params )
	{
		s_LogOutputFn( L_sprintf( "Error: %s\n", L_vsprintf( pFormat, params ).c_str() ).c_str() );
	}
#endif // ENABLE_LOGGING

#if ENABLE_ASSERT
	/*
	==================
	Sys_FailAssertFunc
	==================
	*/
	void Sys_FailAssertFunc( const achar* pExpr, const achar* pFile, int32 line, const achar* pFormat /*= "" */, ... )
	{
		// Get final message
		va_list			params;
		va_start( params, pFormat );
		std::string		finalMessage = L_sprintf( "Assertion failed: %s [File: %s] [Line: %i]\n\n%s\n\nStack:\n%s", pExpr, pFile, line, L_vsprintf( pFormat, params ).c_str(), Sys_DumpCallStack().c_str() );
		va_end( params );

		// Print message and show message box
		s_LogOutputFn( finalMessage.c_str() );
		if ( Sys_IsDebuggerPresent() )
		{
			Sys_DebugBreak();
		}
		Sys_ShowMessageBox( "Engine Error", finalMessage.c_str(), MESSAGE_BOX_ERROR );

		// Shutdown application
		Sys_RequestExit( true );
	}
#endif // ENABLE_ASSERT