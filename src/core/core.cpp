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
#include "core/core_private.h"

/**
 * @ingroup core
 * @brief Build number counter
 */
class CBuildNumber
{
public:
	/**
	 * @brief Constructor
	 */
	CBuildNumber()
		: buildNumber( 0 )
	{
		ComputeBuildNumber();
	}

	/**
	 * @brief Get build number
	 * @return Return build number
	 */
	FORCEINLINE uint32 GetBuildNumber() const
	{
		return buildNumber;
	}

private:
	/**
	 * @brief Compute build number
	 */
	void ComputeBuildNumber()
	{
		const char* pDate			= __DATE__;
		const char* month[12]		= { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		const char	month_days[12]	= { 31,    28,    31,    30,    31,    30,    31,    31,    30,    31,    30,    31 };

		buildNumber		= 0;
		uint32	months	= 0;
		uint32	days	= 0;
		uint32	years	= 0;

		// Compute count days and years
		for ( months = 0; months < 11; ++months )
		{
			if ( L_Strncmp( &pDate[0], month[months], 3 ) == 0 )
			{
				break;
			}
			days += month_days[months];
		}
		days		+= L_Atoi( &pDate[4] ) - 1;
		years		= L_Atoi( &pDate[7] ) - 1900;
		
		// Compute build number
		buildNumber = days + ( uint32 )( ( years - 1 ) * 365.25f );
		if ( ( years % 4 == 0 ) && months > 1 )
		{
			++buildNumber;
		}

		buildNumber -= 44964; // Feb 09 2024 (lifeEngine development start)
	}

	uint32		buildNumber;	/**< Build number */
};

/*
==================
Sys_Error
==================
*/
void Sys_Error( const achar* pFormat, ... )
{
	// Get formated string
	va_list			params;
	va_start( params, pFormat );
	std::string		message = L_Vsprintf( pFormat, params );
	va_end( params );

	// Print message and show message box
	Error( message.c_str() );
	if ( Sys_IsDebuggerPresent() )
	{
		Sys_DebugBreak();
	}
	Sys_ShowMessageBox( "Engine Error", message.c_str(), MESSAGE_BOX_ERROR );

	// Shutdown application
	Sys_RequestExit( true );
}

/*
==================
Sys_IsRequestingExit
==================
*/
bool Sys_IsRequestingExit()
{
	return g_bRequestingExit;
}

/*
==================
Sys_BuildNumber
==================
*/
uint32 Sys_BuildNumber()
{
	static CBuildNumber		s_BuildNumber;
	return s_BuildNumber.GetBuildNumber();
}