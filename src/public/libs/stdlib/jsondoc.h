/**
 * @file
 * @addtogroup stdlib stdlib
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

#ifndef JSONDOC_H
#define JSONDOC_H

#include <string>
#include <unordered_map>
#include <vector>
#include <rapidjson/document.h>

#include "core/types.h"
#include "core/platform.h"

/**
 * @ingroup stdlib
 * @brief Enumeration of JSON value types
 */
enum EJsonValueType
{
	JSONVALUE_TYPE_UNKNOWN,		/**< Unknown type */
	JSONVALUE_TYPE_BOOL,		/**< Bool type */
	JSONVALUE_TYPE_INT,			/**< Integer type */
	JSONVALUE_TYPE_FLOAT,		/**< Float type */
	JSONVALUE_TYPE_STRING,		/**< String type */
	JSONVALUE_TYPE_OBJECT,		/**< Object type (CJsonObject) */
	JSONVALUE_TYPE_ARRAY		/**< Array type */
};

/**
 * @ingroup stdlib
 * @brief Json object
 */
class CJsonObject
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE CJsonObject()
	{}

	/**
	 * @brief Copy constructor
	 * @param copy	Object to copy
	 */
	FORCEINLINE CJsonObject( const CJsonObject& copy )
		: valuesDict( copy.valuesDict )
	{}

	/**
	 * @brief Destructor
	 */
	FORCEINLINE ~CJsonObject()
	{
		Clear();
	}

	/**
	 * @brief Clear
	 */
	FORCEINLINE void Clear()
	{
		valuesDict.clear();
	}

	/**
	 * @brief Copy
	 * @param copy	Copy to object
	 */
	FORCEINLINE void Copy( const CJsonObject& copy )
	{
		valuesDict = copy.valuesDict;
	}

	/**
	 * @brief Convert object to JSON string
	 *
	 * @param countTabs		Number of tabs for indentation
	 * @return Return converted object in JSON string
	 */
	std::string ToJson( uint32 countTabs = 0 ) const;

	/**
	 * @brief Set object from RapidJson value
	 * @param value		RapidJson value
	 */
	void Set( const rapidjson::Value& value );

	/**
	 * @brief Set value
	 *
	 * @param pName		Name of value
	 * @param value		Value
	 */
	void SetValue( const achar* pName, const class CJsonValue& value );

	/**
	 * @brief Get value
	 *
	 * @param pName		Name of value
	 * @return Return value from object. If isn't exist returns empty
	 */
	class CJsonValue GetValue( const achar* pName ) const;

	/**
	 * @brief Operator = for copy value
	 * @param copy	Copy to value
	 */
	FORCEINLINE CJsonObject& operator=( const CJsonObject& copy )
	{
		Copy( copy );
		return *this;
	}

private:
	std::unordered_map<std::string, class CJsonValue>		valuesDict;		/**< Dictionary with values in object */
};

/**
 * @ingroup stdlib
 * @brief Json value
 */
class CJsonValue
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE CJsonValue()
		: type( JSONVALUE_TYPE_UNKNOWN )
		, pValue( nullptr )
	{}

	/**
	 * @brief Copy constructor
	 * @param copy	Copy to value
	 */
	FORCEINLINE CJsonValue( const CJsonValue& copy )
		: type( JSONVALUE_TYPE_UNKNOWN )
		, pValue( nullptr )
	{
		Copy( copy );
	}

	/**
	 * @brief Destructor
	 */
	FORCEINLINE ~CJsonValue()
	{
		Clear();
	}

	/**
	 * @brief Clear value
	 */
	void Clear();

	/**
	 * @brief Copy value
	 * @param copy	Copy
	 */
	void Copy( const CJsonValue& copy );

	/**
	 * @brief Convert value to JSON string
	 *
	 * @param countTabs		Number of tabs for indentation
	 * @return Return converted value in JSON string
	 */
	std::string	ToJson( uint32 countTabs = 0 ) const;

	/**
	 * @brief Is valid JSON value
	 * @return Return TRUE if value is valid, otherwise returns FALSE
	 */
	FORCEINLINE bool IsValid() const
	{
		return type != JSONVALUE_TYPE_UNKNOWN;
	}

	/**
	 * @brief Is JSON value is a type
	 *
	 * @param type	Checked type
	 * @return Return if type value is 'type' return TRUE, otherwise returns FALSE
	 */
	FORCEINLINE bool IsA( EJsonValueType type ) const
	{
		return CJsonValue::type == type;
	}

	/**
	 * @brief Set value from RapidJson value
	 * @param value		RapidJson value
	 */
	void Set( const rapidjson::Value& value );

	/**
	 * @brief Set bool
	 * @param value		Value
	 */
	FORCEINLINE void SetBool( bool value )
	{
		if ( type != JSONVALUE_TYPE_BOOL )
		{
			Clear();
		}

		if ( !CJsonValue::pValue )
		{
			CJsonValue::pValue = new bool;
		}

		( *( bool* )CJsonValue::pValue ) = value;
		type = JSONVALUE_TYPE_BOOL;
	}

	/**
	 * @brief Set int
	 * @param value		Value
	 */
	FORCEINLINE void SetInt( int32 value )
	{
		if ( type != JSONVALUE_TYPE_INT )
		{
			Clear();
		}

		if ( !CJsonValue::pValue )
		{
			CJsonValue::pValue = new int32;
		}

		( *( int32* )CJsonValue::pValue ) = value;
		type = JSONVALUE_TYPE_INT;
	}

	/**
	 * @brief Set float
	 * @param value		Value
	 */
	FORCEINLINE void SetFloat( float value )
	{
		if ( type != JSONVALUE_TYPE_FLOAT )
		{
			Clear();
		}

		if ( !CJsonValue::pValue )
		{
			CJsonValue::pValue = new float;
		}

		( *( float* )CJsonValue::pValue ) = value;
		type = JSONVALUE_TYPE_FLOAT;
	}

	/**
	 * @brief Set string
	 * @param value		Value
	 */
	FORCEINLINE void SetString( const std::string& value )
	{
		if ( type != JSONVALUE_TYPE_STRING )
		{
			Clear();
		}

		if ( !CJsonValue::pValue )
		{
			CJsonValue::pValue = new std::string();
		}

		( *( std::string* )CJsonValue::pValue ) = value;
		type = JSONVALUE_TYPE_STRING;
	}

	/**
	 * @brief Set object
	 * @param value		Value
	 */
	FORCEINLINE void SetObject( const CJsonObject& value )
	{
		if ( type != JSONVALUE_TYPE_OBJECT )
		{
			Clear();
		}

		if ( !CJsonValue::pValue )
		{
			CJsonValue::pValue = new CJsonObject();
		}

		( *( CJsonObject* )CJsonValue::pValue ) = value;
		type = JSONVALUE_TYPE_OBJECT;
	}

	/**
	 * @brief Set array
	 * @param value		Value
	 */
	FORCEINLINE void SetArray( const std::vector<CJsonValue>& value )
	{
		if ( type != JSONVALUE_TYPE_ARRAY )
		{
			Clear();
		}

		if ( !CJsonValue::pValue )
		{
			CJsonValue::pValue = new std::vector<CJsonValue>();
		}

		( *( std::vector<CJsonValue>* )CJsonValue::pValue ) = value;
		type = JSONVALUE_TYPE_ARRAY;
	}

	/**
	 * @brief Get type value
	 * @return Return type of value
	 */
	FORCEINLINE EJsonValueType GetType() const
	{
		return type;
	}

	/**
	 * @brief Get bool
	 * 
	 * @param defaultValue	Default value
	 * @return Return value with type bool, if type isn't correct returns defaultValue
	 */
	FORCEINLINE bool GetBool( bool defaultValue = false ) const
	{
		if ( type != JSONVALUE_TYPE_BOOL || !pValue )
		{
			return defaultValue;
		}

		return *( bool* )pValue;
	}

	/**
	 * @brief Get number
	 * 
	 * @param defaultValue	Default value
	 * @return Return int/float when value is JSONVALUE_TYPE_INT or JSONVALUE_TYPE_FLOAT, otherwise returns defaultValue
	 */
	FORCEINLINE float GetNumber( float defaultValue = 0.f ) const
	{
		if ( type != JSONVALUE_TYPE_INT && type != JSONVALUE_TYPE_FLOAT || !pValue )
		{
			return defaultValue;
		}

		if ( type == JSONVALUE_TYPE_INT )
		{
			return ( float )GetInt();
		}
		else
		{
			return GetFloat();
		}
	}

	/**
	 * @brief Get int
	 * 
	 * @param defaultValue		Default value
	 * @return Return value with type integer, if type isn't correct returns defaultValue
	 */
	FORCEINLINE int32 GetInt( int32 defaultValue = 0 ) const
	{
		if ( type != JSONVALUE_TYPE_INT || !pValue )
		{
			return defaultValue;
		}

		return *( int32* )pValue;
	}

	/**
	 * @brief Get float
	 * 
	 * @param defaultValue	Default value
	 * @return Return value with type float, if type isn't correct returns defaultValue
	 */
	FORCEINLINE float GetFloat( float defaultValue = 0.f ) const
	{
		if ( type != JSONVALUE_TYPE_FLOAT || !pValue )
		{
			return defaultValue;
		}

		return *( float* )pValue;
	}

	/**
	 * @brief Get string
	 * 
	 * @param defaultValue	Default value
	 * @return Return value with type string, if type isn't correct returns defaultValue
	 */
	FORCEINLINE std::string GetString( const std::string& defaultValue = "" ) const
	{
		if ( type != JSONVALUE_TYPE_STRING || !pValue )
		{
			return defaultValue;
		}

		return *( std::string* )pValue;
	}

	/**
	 * @brief Get object
	 * 
	 * @param defaultValue	Default value
	 * @return Return value with type object, if type isn't correct returns defaultValue
	 */
	FORCEINLINE CJsonObject GetObject( const CJsonObject& defaultValue = CJsonObject() ) const
	{
		if ( type != JSONVALUE_TYPE_OBJECT || !pValue )
		{
			return defaultValue;
		}

		return *( CJsonObject* )pValue;
	}

	/**
	 * @brief Get array
	 * 
	 * @param defaultValue	Default value
	 * @return Return value with type array, if type isn't correct returns defaultValue
	 */
	FORCEINLINE std::vector<CJsonValue>	GetArray( const std::vector<CJsonValue>& defaultValue = std::vector<CJsonValue>() ) const
	{
		if ( type != JSONVALUE_TYPE_ARRAY || !pValue )
		{
			return defaultValue;
		}

		return *( std::vector<CJsonValue>* )pValue;
	}

	/**
	 * @brief Operator = for copy value
	 * @param copy	Copy to value
	 */
	FORCEINLINE CJsonValue& operator=( const CJsonValue& copy )
	{
		Copy( copy );
		return *this;
	}

private:
	EJsonValueType	type;		/**< Type of value */
	void*			pValue;		/**< Pointer to value */
};

/**
 * @ingroup stdlib
 * @brief Json document
 */
class CJsonDoc
{
public:
	/**
	 * @brief Load from file
	 * @warning For use StdLib must be connected by ConnectStdLib
	 * 
	 * @param pPath		Path to file
	 * @return Return TRUE if file successfully loaded, otherwise returns FALSE
	 */
	bool LoadFromFile( const achar* pPath );

	/**
	 * @brief Load from buffer
	 * 
	 * @param pBuffer	Buffer. The buffer must be null terminated
	 * @return Return TRUE if file successfully loaded, otherwise returns FALSE
	 */
	bool LoadFromBuffer( const achar* pBuffer );

	/**
	 * @brief Save to file
	 * @warning For use StdLib must be connected by ConnectStdLib
	 * 
	 * @param pPath		Path to file
	 * @return Return TRUE if successfully saved, otherwise returns FALSE
	 */
	bool SaveToFile( const achar* pPath );

	/**
	 * @brief Save to buffer
	 * 
	 * @param buffer	Output buffer
	 * @return Return TRUE if successfully saved, otherwise returns FALSE
	 */
	bool SaveToBuffer( std::string& buffer );

	/**
	 * @brief Set value
	 *
	 * @param pName		Name of value
	 * @param value		Value
	 */
	FORCEINLINE void SetValue( const achar* pName, const CJsonValue& value )
	{
		valuesDict[pName] = value;
	}

	/**
	 * @brief Get value
	 *
	 * @param pName		Name of value in Json
	 * @return Return value from JSON, if isn't found returns empty value
	 */
	FORCEINLINE CJsonValue GetValue( const achar* pName ) const
	{
		valuesDict_t::const_iterator	itValues = valuesDict.find( pName );
		if ( itValues == valuesDict.end() )
		{
			return CJsonValue();
		}

		return itValues->second;
	}

private:
	/**
	 * @brief Type dictionary with JSON values
	 */
	typedef std::unordered_map<std::string, CJsonValue>		valuesDict_t;

	valuesDict_t	valuesDict;			/**< Json values */
};

#endif // !JSONDOC_H