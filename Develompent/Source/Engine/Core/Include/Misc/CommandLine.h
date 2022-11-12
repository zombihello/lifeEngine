/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <string>
#include <vector>
#include <unordered_map>

#include "Misc/Types.h"
#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "CoreDefines.h"

/**
 * @ingroup Core
 * @brief Holds parsed data from command line like strings, provides access to a single global GCommandLine object
 * 
 * Usage: 
 * @code
 *		-<Param>=<Value> 
 *		/<Param>=<Value> 
 *		-<Param>="<Value> <Value>"
 *		/<Param>="<Value> <Value>"
 *		-<Param> 
 *		/<Param> 
 *		-<Param> <Value> 
 *		/<Param> <Value>
 *		-<Param> "<Value> <Value>"
 *		/<Param> "<Value> <Value>"
 * @endcode
 */
class CCommandLine
{
public:
	/**
	 * @brief Typedef of array values
	 */
	typedef std::vector<std::wstring>		Values_t;

	/**
	 * @brief Init command line
	 * @param InCommandLine		ANSI command line
	 */
	FORCEINLINE void Init( const achar* InCommandLine )
	{
		params.clear();
		Parse( ANSI_TO_TCHAR( InCommandLine ) );
	}

	/**
	 * @brief Init command line
	 * @param InCommandLine		TCHAR command line
	 */
	FORCEINLINE void Init( const tchar* InCommandLine )
	{
		params.clear();
		Parse( InCommandLine );
	}

	/**
	 * @brief Shutdown command line
	 */
	FORCEINLINE void Shutdown()
	{
		params.clear();
	}

	/**
	 * @brief Is has a param
	 * 
	 * @param InParam	Param to find
	 * @return Return TRUE if command line is containing a param, otherwise will returning FALSE
	 */
	FORCEINLINE bool HasParam( const tchar* InParam ) const
	{
		return params.find( CString::ToUpper( InParam ) ) != params.end();
	}

	/**
	 * @brief Is has a param with value
	 *
	 * @param InParam	Param to find
	 * @param InValue	Value of param
	 * @return Return TRUE if command line is containing a param with value, otherwise will returning FALSE
	 */
	FORCEINLINE bool HasParam( const tchar* InParam, const tchar* InValue ) const
	{
		Params_t::const_iterator	itParam = params.find( CString::ToUpper( InParam ) );
		if ( itParam == params.end() )
		{
			return false;
		}

		bool				bResult = false;
		std::wstring		cachedValue = CString::ToUpper( InValue );
		for ( uint32 index = 0, count = itParam->second.size(); index < count; ++index )
		{
			if ( itParam->second[index] == cachedValue )
			{
				bResult = true;
				break;
			}
		}
		
		return bResult;
	}

	/**
	 * @brief Get first value from param
	 *
	 * @param InParam		Param
	 * @return Return first value in param. If not exist return empty string
	 */
	FORCEINLINE std::wstring GetFirstValue( const tchar* InParam ) const
	{
		Params_t::const_iterator	itParam = params.find( CString::ToUpper( InParam ) );
		if ( itParam == params.end() || itParam->second.empty() )
		{
			return TEXT( "" );
		}

		return itParam->second[0];
	}

	/**
	 * @brief Get values from param
	 * 
	 * @param InParam		Param
	 * @return Return array of values by param. If him is not exist in command line will return empty array
	 */
	FORCEINLINE Values_t GetValues( const tchar* InParam ) const
	{
		Params_t::const_iterator	itParam = params.find( CString::ToUpper( InParam ) );
		if ( itParam == params.end() )
		{
			return Values_t();
		}

		return itParam->second;
	}

private:
	/**
	 * @brief Typedef of map parameters
	 */
	typedef std::unordered_map<std::wstring, Values_t>		Params_t;

	/**
	 * @brief Parses a string into params (beginning with - or /) from other parameters
	 * @param InCommandLine		The string to parse
	 */
	void Parse( const tchar* InCommandLine );

	Params_t			params;		/**< Map of parameters */
};

#endif // !COMMANDLINE_H