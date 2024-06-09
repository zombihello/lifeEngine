/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef JSONDOCUMENT_H
#define JSONDOCUMENT_H

// Override malloc/realloc/free for RapidJSON
#define RAPIDJSON_MALLOC( Size )			Memory::Malloc( Size )
#define RAPIDJSON_REALLOC( Ptr, NewSize )	Memory::Realloc( Ptr, NewSize )
#define RAPIDJSON_FREE( Ptr )				Memory::Free( Ptr );

#include <string>
#include <unordered_map>

#include "Core.h"
#include <rapidjson/document.h>

/**
 * @ingroup Core
 * @brief Json type values
 */
enum EJsonValueType
{
	JVT_None,			/**< No type */
	JVT_Bool,			/**< Bool type */
	JVT_Int,			/**< Integer type */
	JVT_Float,			/**< Float type */
	JVT_String,			/**< String type */
	JVT_Object,			/**< Object type (CJsonObject) */
	JVT_Array			/**< Array type */
};

/**
 * @ingroup Core
 * @brief Json Object
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
	 * @brief Constructor of copy
	 * @param InCopy	Copy of object
	 */
	FORCEINLINE CJsonObject( const CJsonObject& InCopy ) 
		: values( InCopy.values )
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
		values.clear();
	}

	/**
	 * @brief Copy
	 * @param InCopy	Copy of object
	 */
	FORCEINLINE void Copy( const CJsonObject& InCopy )
	{
		values = InCopy.values;
	}

	/**
	 * @brief Convert object to JSON string
	 * 
	 * @param InCountTabs	Number of tabs for indentation
	 * @return Return converted object in JSON string
	 */
	std::string ToJson( uint32 InCountTabs = 0 ) const;

	/**
	 * @brief Set object from RapidJSON value
	 * 
	 * @param InValue		RapidJSON value
	 * @param InDebugName	Debug name
	 */
	void Set( const rapidjson::Value& InValue, const tchar* InDebugName = TEXT( "UNKNOWN" ) );

	/**
	 * @brief Set value
	 *
	 * @param InName	Name of value
	 * @param InValue	Value
	 */
	void SetValue( const tchar* InName, const class CJsonValue& InValue );

	/**
	 * @brief Get value
	 *
	 * @param InName	Name of value
	 * @return Return value from object. If not exist value in object returns NULL
	 */
	class CJsonValue* GetValue( const tchar* InName );

	/**
	 * @brief Get value
	 *
	 * @param InName	Name of value
	 * @return Return value from object. If not exist value in object returns NULL
	 */
	const class CJsonValue* GetValue( const tchar* InName ) const;

	/**
	 * @brief Operator =
	 * @param InCopy	Copy of value
	 */
	FORCEINLINE CJsonObject& operator=( const CJsonObject& InCopy )
	{
		Copy( InCopy );
		return *this;
	}

private:
	std::unordered_map<std::wstring, class CJsonValue>			values;		/**< Values in object */
};

/**
 * @ingroup Core
 * @brief Json value
 */
class CJsonValue
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE CJsonValue() 
		: type( JVT_None )
		, value( nullptr )
	{}

	/**
	 * @brief Constructor of copy
	 * @param InCopy	Copy of value
	 */
	FORCEINLINE CJsonValue( const CJsonValue& InCopy )
		: type( JVT_None )
		, value( nullptr )
	{
		Copy( InCopy );
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
	 * @param InCopy	Copy
	 */
	void Copy( const CJsonValue& InCopy );

	/**
	 * @brief Convert value to JSON string
	 * 
	 * @param InCountTabs	Number of tabs for indentation
	 * @return Return converted value in JSON string
	 */
	std::string ToJson( uint32 InCountTabs = 0 ) const;

	/**
	 * @brief Is valid config value
	 * @return Return TRUE if value is valid, otherwise returns FALSE
	 */
	FORCEINLINE bool IsValid() const
	{
		return type != JVT_None;
	}

	/**
	 * @brief Is config value is a type
	 * 
	 * @param InType	Checked type
	 * @return Return TRUE if this value has same type, otherwise returns FALSE 
	 */
	FORCEINLINE bool IsA( EJsonValueType InType ) const
	{
		return type == InType;
	}

	/**
	 * @brief Set value from RapidJSON value
	 * 
	 * @param InValue		RapidJSON value
	 * @param InDebugName	Debug name
	 */
	void Set( const rapidjson::Value& InValue, const tchar* InDebugName = TEXT( "UNKNOWN" ) );

	/**
	 * @brief Set bool
	 * @param InValue	Value
	 */
	FORCEINLINE void SetBool( bool InValue )
	{
		if ( type != JVT_Bool )
		{
			Clear();
		}

		if ( !value )
		{
			value = new bool;
		}

		*static_cast<bool*>( value ) = InValue;
		type = JVT_Bool;
	}

	/**
	 * @brief Set int
	 * @param InValue	Value
	 */
	FORCEINLINE void SetInt( int32 InValue )
	{
		if ( type != JVT_Int )
		{
			Clear();
		}

		if ( !value )
		{
			value = new int32;
		}

		*static_cast<int32*>( value ) = InValue;
		type = JVT_Int;
	}

	/**
	 * @brief Set float
	 * @param InValue	Value
	 */
	FORCEINLINE void SetFloat( float InValue )
	{
		if ( type != JVT_Float )
		{
			Clear();
		}

		if ( !value )
		{
			value = new float;
		}

		*static_cast<float*>( value ) = InValue;
		type = JVT_Float;
	}

	/**
	 * @brief Set string
	 * @param InValue	Value
	 */
	FORCEINLINE void SetString( const std::wstring& InValue )
	{
		if ( type != JVT_String )
		{
			Clear();
		}

		if ( !value )
		{
			value = new std::wstring();
		}

		*static_cast<std::wstring*>( value ) = InValue;
		type = JVT_String;
	}

	/**
	 * @brief Set object
	 * @param InValue	Value
	 */
	FORCEINLINE void SetObject( const CJsonObject& InValue )
	{
		if ( type != JVT_Object )
		{
			Clear();
		}

		if ( !value )
		{
			value = new CJsonObject();
		}

		*static_cast<CJsonObject*>( value ) = InValue;
		type = JVT_Object;
	}

	/**
	 * @brief Set array
	 * @param InValue	Value
	 */
	FORCEINLINE void SetArray( const std::vector<CJsonValue>& InValue )
	{
		if ( type != JVT_Array )
		{
			Clear();
		}

		if ( !value )
		{
			value = new std::vector<CJsonValue>();
		}

		*static_cast<std::vector<CJsonValue>*>( value ) = InValue;
		type = JVT_Array;
	}

	/**
	 * @brief Get type value
	 * @return Type of value
	 */
	FORCEINLINE EJsonValueType GetType() const
	{
		return type;
	}

	/**
	 * @brief Get bool
	 * 
	 * @param InDefaultValue	Default value
	 * @return Return value. If type isn't T_Bool returns InDefaultValue
	 */
	FORCEINLINE bool GetBool( bool InDefaultValue = false ) const
	{
		if ( type != JVT_Bool || !value )
		{
			return InDefaultValue;
		}
		return *static_cast<bool*>( value );
	}

	/**
	 * @brief Get number
	 * 
	 * @param InDefaultValue	Default value
	 * @return Return value. If type isn't JVT_Bool nor JVT_Int nor JVT_Float returns InDefaultValue
	 */
	FORCEINLINE float GetNumber( float InDefaultValue = 0.f ) const
	{
		if ( type != JVT_Bool && type != JVT_Int && type != JVT_Float || !value )
		{
			return InDefaultValue;
		}

		if ( type == JVT_Int )
		{
			return ( float )GetInt();
		}
		else if ( type == JVT_Float )
		{
			return GetFloat();
		}
		else
		{
			return ( bool )GetBool();
		}
	}

	/**
	 * @brief Get int
	 * 
	 * @param InDefaultValue	Default value
	 * @return Return value. If type isn't T_Int returns InDefaultValue
	 */
	FORCEINLINE int32 GetInt( int32 InDefaultValue = 0 ) const
	{
		if ( type != JVT_Int || !value )
		{
			return InDefaultValue;
		}
		return *static_cast<int32*>( value );
	}

	/**
	 * @brief Get float
	 * 
	 * @param InDefaultValue	Default value
	 * @return Return value. If type isn't T_Float returns InDefaultValue
	 */
	FORCEINLINE float GetFloat( float InDefaultValue = 0.f ) const
	{
		if ( type != JVT_Float || !value )
		{
			return InDefaultValue;
		}
		return *static_cast<float*>( value );
	}

	/**
	 * @brief Get string
	 * 
	 * @param InDefaultValue	Default value
	 * @return Return value. If type isn't T_String returns InDefaultValue
	 */
	FORCEINLINE const std::wstring& GetString( const std::wstring& InDefaultValue = TEXT( "" ) ) const
	{
		if ( type != JVT_String || !value )
		{
			return InDefaultValue;
		}
		return *static_cast<std::wstring*>( value );
	}

	/**
	 * @brief Get object
	 * @return Return value. If type isn't T_Object returns NULL
	 */
	FORCEINLINE const CJsonObject* GetObject() const
	{
		if ( type != JVT_Object || !value )
		{
			return nullptr;
		}
		return static_cast<CJsonObject*>( value );
	}

	/**
	 * @brief Get array
	 * @return Return value. If type isn't T_Array returns NULL
	 */
	FORCEINLINE const std::vector<CJsonValue>* GetArray() const
	{
		if ( type != JVT_Array || !value )
		{
			return nullptr;
		}
		return static_cast<std::vector<CJsonValue>*>( value );
	}

	/**
	 * @brief Operator =
	 * 
	 * @param InCopy	Copy of value
	 * @return Return reference to self
	 */
	FORCEINLINE CJsonValue& operator=( const CJsonValue& InCopy )
	{
		Copy( InCopy );
		return *this;
	}

private:
	EJsonValueType	type;		/**< Type of value */
	void*			value;		/**< Pointer to value */
};

/**
 * @ingroup Core
 * @brief Json document
 */
class CJsonDocument
{
public:
	/**
	 * @brief Load from file
	 * 
	 * @param InPath		Path to file
	 * @return Return TRUE if file successfully loaded, otherwise returns FALSE
	 */
	bool LoadFromFile( const tchar* InPath );

	/**
	 * @brief Load from buffer
	 * 
	 * @param InBuffer	Data buffer. The buffer must be null terminated
	 * @return Return TRUE if buffer successfully loaded, otherwise returns FALSE
	 */
	bool LoadFromBuffer( const achar* InBuffer );

	/**
	 * @brief Save to file
	 * 
	 * @param InPath	Path to file
	 * @return Return TRUE if successfully saved, otherwise returns FALSE
	 */
	bool SaveToFile( const tchar* InPath ) const;

	/**
	 * @brief Save to buffer
	 * 
	 * @param OutBuffer		Output buffer
	 * @return Return TRUE if successfully saved, otherwise returns FALSE
	 */
	bool SaveToBuffer( std::string& OutBuffer ) const;

	/**
	 * @brief Set value
	 *
	 * @param InName	Name of value
	 * @param InValue	Value
	 */
	FORCEINLINE void SetValue( const tchar* InName, const CJsonValue& InValue )
	{
		valuesMap[InName] = InValue;
	}

	/**
	 * @brief Get value
	 *
	 * @param InName	Name of value in Json
	 * @return Return value from JSON, if isn't found returns NULL
	 */
	FORCEINLINE CJsonValue* GetValue( const tchar* InName )
	{
		ValuesMap_t::iterator		itValues = valuesMap.find( InName );
		if ( itValues == valuesMap.end() )
		{
			return nullptr;
		}
		return &itValues->second;
	}

	/**
	 * @brief Get value
	 *
	 * @param InName	Name of value in Json
	 * @return Return value from JSON, if isn't found returns NULL
	 */
	FORCEINLINE const CJsonValue* GetValue( const tchar* InName ) const
	{
		ValuesMap_t::const_iterator		itValues = valuesMap.find( InName );
		if ( itValues == valuesMap.end() )
		{
			return nullptr;
		}
		return &itValues->second;
	}

private:
	/**
	 * @brief Type dictionary with JSON values
	 */
	typedef std::unordered_map<std::wstring, CJsonValue>		ValuesMap_t;
	ValuesMap_t		valuesMap;			/**< Json values */
};

#endif // !JSONDOCUMENT_H