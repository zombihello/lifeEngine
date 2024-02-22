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
#include <vector>
#include <unordered_map>

#include "core/debug.h"
#include "core/icommandline.h"

/**
 * @ingroup core
 * @brief Command line
 * Holds parsed data from command line like strings
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
class CCommandLine : public ICommandLine
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~CCommandLine();

	/**
	 * @brief Init command line
	 * @param pCommandLine	Command line
	 */
	virtual void Init( const achar* pCommandLine ) override;

	/**
	 * @brief Shutdown command line
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Is has a param
	 *
	 * @param pParam	Param to find
	 * @return Return TRUE if command line is containing a param, otherwise will returning FALSE
	 */
	virtual bool HasParam( const achar* pParam ) const override;

	/**
	 * @brief Is has a param with value
	 *
	 * @param pParam		Param to find
	 * @param pValue		Value of param
	 * @return Return TRUE if command line is containing a param with value, otherwise will returning FALSE
	 */
	virtual bool HasParam( const achar* pParam, const achar* pValue ) const override;

	/**
	 * @brief Get first value from param
	 *
	 * @param pParam	Param
	 * @return Return first value in param. If not exist return empty string
	 */
	virtual const achar* GetFirstValue( const achar* pParam ) const override;

	/**
	 * @brief Get values from param
	 *
	 * @param pParam	Param
	 * @param size		Output count values
	 * @return Return array of values by param. If it is not exist in command line will return NULL
	 */
	virtual const achar** GetValues( const achar* pParam, uint32& size ) const override;

private:
	/**
	 * @brief Type value array
	 */
	typedef std::vector<const achar*>	values_t;

	/**
	 * @brief Type of map parameters
	 */
	typedef std::unordered_map<std::string, values_t>	paramDict_t;

	/**
	 * @brief Parses a string into params (beginning with - or /) from other parameters
	 * @param pCommandLine	Command line
	 */
	void Parse( const achar* pCommandLine );

	paramDict_t		paramsDict;		/**< Dictionary of parameters */
};


/*
==================
CommandLine
==================
*/
static CCommandLine		s_CommandLine;
ICommandLine* CommandLine()
{
	return &s_CommandLine;
}

/*
==================
ParseToken
==================
*/
static bool ParseToken( const achar*& pStr, std::string& result, bool bUseEscape )
{
	//
	// Grab the next space-delimited string from the input stream.
	// If quoted, gets entire quoted string.
	//
	result.clear();

	// Skip preceeding spaces and tabs.
	while ( L_IsSpace( *pStr ) || *pStr == '=' )
	{
		pStr++;
	}

	if ( *pStr == '"' )
	{
		// Get quoted string
		pStr++;

		while ( *pStr && *pStr != '"' )
		{
			achar		c = *pStr++;
			if ( c == '\\' && bUseEscape )
			{
				// Get escape
				c = *pStr++;
				if ( !c )
				{
					break;
				}
			}

			result += c;
		}

		if ( *pStr == '"' )
		{
			pStr++;
		}
	}
	else
	{
		// Get unquoted string (that might contain a quoted part, which will be left intact).
		// For example, -ARG="foo bar baz", will be treated as one token, with quotes intact
		bool bInQuote = false;

		while ( true )
		{
			achar		character = *pStr;
			if ( character == 0 || ( ( L_IsSpace( character ) || character == '=' ) && !bInQuote ) )
			{
				break;
			}
			pStr++;

			// Preserve escapes if they're in a quoted string (the check for " is in the else to let \" work as expected)
			if ( character == '\\' && bUseEscape && bInQuote )
			{
				result += character;

				character = *pStr;
				if ( !character )
				{
					break;
				}
				pStr++;
			}
			else if ( character == '"' )
			{
				bInQuote = !bInQuote;
			}

			result += character;
		}
	}

	return !result.empty();
}


/*
==================
CCommandLine::~CCommandLine
==================
*/
CCommandLine::~CCommandLine()
{
	Shutdown();
}

/*
==================
CCommandLine::Init
==================
*/
void CCommandLine::Init( const achar* pCommandLine )
{
	Shutdown();
	Parse( pCommandLine );
	Msg( "CommandLine: inited with arguments '%s'", pCommandLine );
}

/*
==================
CCommandLine::Shutdown
==================
*/
void CCommandLine::Shutdown()
{
	// Free memory
	for ( paramDict_t::iterator it = paramsDict.begin(), itEnd = paramsDict.end(); it != itEnd; ++it )
	{
		const values_t&		valueArray = it->second;
		for ( uint32 index = 0, count = ( uint32 )valueArray.size(); index < count; ++index )
		{
			free( ( achar* )valueArray[index] );
		}
	}
	paramsDict.clear();
}

/*
==================
CCommandLine::Parse
==================
*/
void CCommandLine::Parse( const achar* pCommandLine )
{
	std::string				nextToken;
	paramDict_t::iterator	itCurrentParam = paramsDict.end();

	while ( ParseToken( pCommandLine, nextToken, false ) )
	{
		L_Strlwr( nextToken.data() );
		if ( nextToken[0] == '-' || nextToken[0] == '/' )
		{
			std::string		token( &nextToken[1], nextToken.size() - 1 );
			itCurrentParam = paramsDict.find( token );
			if ( itCurrentParam == paramsDict.end() )
			{
				itCurrentParam = paramsDict.insert( std::make_pair( token, values_t() ) ).first;
			}
		}
		else if ( itCurrentParam != paramsDict.end() )
		{
			uint32		size = ( uint32 )nextToken.size();
			achar*		pData = ( achar* )malloc( ( size + 1 ) * sizeof( achar ) );
			L_Strcpy( pData, nextToken.data() );
			pData[size]	= '\0';
			itCurrentParam->second.push_back( pData );
		}
	}
}

/*
==================
CCommandLine::HasParam
==================
*/
bool CCommandLine::HasParam( const achar* pParam ) const
{
	std::string		param = pParam;
	L_Strlwr( param.data() );
	return paramsDict.find( param ) != paramsDict.end();
}

/*
==================
CCommandLine::HasParam
==================
*/
bool CCommandLine::HasParam( const achar* pParam, const achar* pValue ) const
{
	std::string		param = pParam;
	L_Strlwr( param.data() );
	paramDict_t::const_iterator		itParam = paramsDict.find( param );
	if ( itParam == paramsDict.end() )
	{
		return false;
	}

	std::string			value = pValue;
	L_Strlwr( value.data() );
	bool				bResult = false;
	const values_t&		valueArray = itParam->second;
	for ( uint32 index = 0, count = ( uint32 )valueArray.size(); index < count; ++index )
	{
		if ( !L_Strcmp( valueArray[index], value.c_str() ) )
		{
			bResult = true;
			break;
		}
	}

	return bResult;
}

/*
==================
CCommandLine::GetFirstValue
==================
*/
const achar* CCommandLine::GetFirstValue( const achar* pParam ) const
{
	std::string		param = pParam;
	L_Strlwr( param.data() );
	paramDict_t::const_iterator		itParam = paramsDict.find( param );
	if ( itParam == paramsDict.end() || itParam->second.empty() )
	{
		return "";
	}

	return itParam->second[0];
}

/*
==================
CCommandLine::GetValues
==================
*/
const achar** CCommandLine::GetValues( const achar* pParam, uint32& size ) const
{
	std::string		param = pParam;
	L_Strlwr( param.data() );
	paramDict_t::const_iterator		itParam = paramsDict.find( param );
	if ( itParam == paramsDict.end() )
	{
		size = 0;
		return nullptr;
	}

	size = ( uint32 )itParam->second.size();
	return ( const achar** )itParam->second.data();
}