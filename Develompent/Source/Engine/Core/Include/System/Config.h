/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <unordered_map>
#include <rapidjson/document.h>

#include "Core.h"

#undef GetObject

 /**
  * @ingroup Core
  * @brief Class object of config
  */
class ConfigObject
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE						ConfigObject()
	{}

	/**
	 * @brief Constructor of copy
	 * @param[in] InCopy Copy of object
	 */
	FORCEINLINE						ConfigObject( const ConfigObject& InCopy ) :
		values( InCopy.values )
	{}

	/**
	 * @brief Destructor
	 */
	FORCEINLINE						~ConfigObject()
	{
		Clear();
	}

	/**
	 * @brief Clear
	 */
	FORCEINLINE void				Clear()
	{
		values.clear();
	}

	/**
	 * @brief Copy
	 * @param[in] InCopy Copy of object
	 */
	FORCEINLINE void				Copy( const ConfigObject& InCopy )
	{
		values = InCopy.values;
	}

	/**
	 * @brief Convert object to JSON string
	 * 
	 * @param[in] InCountTabs Number of tabs for indentation
	 * @return Return converted object in JSON string
	 */
	std::string						ToJSON( uint32 InCountTabs = 0 ) const;

	/**
	 * @brief Set object from RapidJSON value
	 * @param[in] InValue RapidJSON value
	 * @param[in] InName RapidJSON name for print name of object in log while error
	 */
	void							Set( const rapidjson::Value& InValue, const tchar* InName = TEXT( "UNKNOWN" ) );

	/**
	 * @brief Set value
	 *
	 * @param[in] InName Name of value
	 * @param[in] InValue Value
	 */
	void							SetValue( const tchar* InName, const class ConfigValue& InValue );

	/**
	 * @brief Get value
	 *
	 * @param[in] InName Name of value
	 * @return Return value from object. If not exist value in object - return empty
	 */
	class ConfigValue				GetValue( const tchar* InName ) const;

	/**
	 * @brief Operator = for copy value
	 * @param[in] InCopy Copy of value
	 */
	FORCEINLINE ConfigObject& operator=( const ConfigObject& InCopy )
	{
		Copy( InCopy );
		return *this;
	}

private:
	std::unordered_map< std::wstring, class ConfigValue >			values;		/**< Values in object */
};

/**
 * @ingroup Core
 * @brief Class value of config
 */
class ConfigValue
{
public:
	/**
	 * @brief Enumeration of types value
	 */
	enum EType
	{
		T_None,				/**< No type */
		T_Bool,				/**< Bool type */
		T_Int,				/**< Integer type */
		T_Float,			/**< Float type */
		T_String,			/**< String type */
		T_Object			/**< Object type (ConfigObject) */
	};

	/**
	 * @brief Constructor
	 */
	FORCEINLINE								ConfigValue() :
		type( T_None ),
		value( nullptr )
	{}

	/**
	 * @brief Constructor of copy
	 * @param[in] InCopy Copy of value
	 */
	FORCEINLINE								ConfigValue( const ConfigValue& InCopy ) :
		type( T_None ),
		value( nullptr )
	{
		Copy( InCopy );
	}

	/**
	 * @brief Destructor
	 */
	FORCEINLINE								~ConfigValue()
	{
		Clear();
	}

	/**
	 * @brief Clear value
	 */
	void									Clear();

	/**
	 * @brief Copy value
	 * @param[in] InCopy Copy
	 */
	void									Copy( const ConfigValue& InCopy );

	/**
	 * @brief Convert value to JSON string
	 * 
	 * @param[in] InCountTabs Number of tabs for indentation
	 * @return Return converted value in JSON string
	 */
	std::string								ToJSON( uint32 InCountTabs = 0 ) const;

	/**
	 * @brief Set value from RapidJSON value
	 * @param[in] InValue RapidJSON value
	 * @param[in] InName RapidJSON name for print name of value in log while error
	 */
	void									Set( const rapidjson::Value& InValue, const tchar* InName = TEXT( "UNKNOWN" ) );

	/**
	 * @brief Set bool
	 * @param[in] InValue Value
	 */
	FORCEINLINE void						SetBool( bool InValue )
	{
		if ( type != T_Bool )
		{
			Clear();
		}

		if ( !value )
		{
			value = new bool;
		}

		*static_cast< bool* >( value ) = InValue;
		type = T_Bool;
	}

	/**
	 * @brief Set int
	 * @param[in] InValue Value
	 */
	FORCEINLINE void						SetInt( int32 InValue )
	{
		if ( type != T_Int )
		{
			Clear();
		}

		if ( !value )
		{
			value = new int32;
		}

		*static_cast< int32* >( value ) = InValue;
		type = T_Int;
	}

	/**
	 * @brief Set float
	 * @param[in] InValue Value
	 */
	FORCEINLINE void						SetFloat( float InValue )
	{
		if ( type != T_Float )
		{
			Clear();
		}

		if ( !value )
		{
			value = new float;
		}

		*static_cast< float* >( value ) = InValue;
		type = T_Float;
	}

	/**
	 * @brief Set string
	 * @param[in] InValue Value
	 */
	FORCEINLINE void						SetString( const std::wstring& InValue )
	{
		if ( type != T_String )
		{
			Clear();
		}

		if ( !value )
		{
			value = new std::wstring();
		}

		*static_cast< std::wstring* >( value ) = InValue;
		type = T_String;
	}

	/**
	 * @brief Set object
	 * @param[in] InValue Value
	 */
	FORCEINLINE void						SetObject( const ConfigObject& InValue )
	{
		if ( type != T_Object )
		{
			Clear();
		}

		if ( !value )
		{
			value = new ConfigObject();
		}

		*static_cast< ConfigObject* >( value ) = InValue;
		type = T_Object;
	}

	/**
	 * @brief Get type value
	 * @return Type of value
	 */
	FORCEINLINE EType						GetType() const
	{
		return type;
	}

	/**
	 * @brief Get bool
	 * @return Value with type bool, if type not correct return false
	 */
	FORCEINLINE bool						GetBool() const
	{
		if ( type != T_Bool || !value )
		{
			return false;
		}

		return *static_cast< bool* >( value );
	}

	/**
	 * @brief Get int
	 * @return Value with type integer, if type not correct return 0
	 */
	FORCEINLINE int32						GetInt() const
	{
		if ( type != T_Int || !value )
		{
			return 0;
		}

		return *static_cast< int32* >( value );
	}

	/**
	 * @brief Get float
	 * @return Value with type float, if type not correct return 0.f
	 */
	FORCEINLINE float						GetFloat() const
	{
		if ( type != T_Float || !value )
		{
			return 0.f;
		}

		return *static_cast< float* >( value );
	}

	/**
	 * @brief Get string
	 * @return Value with type string, if type not correct return TEXT( "" )
	 */
	FORCEINLINE std::wstring				GetString() const
	{
		if ( type != T_String || !value )
		{
			return TEXT( "" );
		}

		return *static_cast< std::wstring* >( value );
	}

	/**
	 * @brief Get object
	 * @return Value with type object, if type not correct return empty object
	 */
	FORCEINLINE ConfigObject				GetObject() const
	{
		if ( type != T_Object || !value )
		{
			return ConfigObject();
		}

		return *static_cast< ConfigObject* >( value );
	}

	/**
	 * @brief Operator = for copy value
	 * @param[in] InCopy Copy of value
	 */
	FORCEINLINE ConfigValue&				operator=( const ConfigValue& InCopy )
	{
		Copy( InCopy );
		return *this;
	}

private:
	EType			type;		/**< Type of value */
	void*			value;		/**< Pointer to value */
};

/**
 * @ingroup Core
 * @brief Class for work with config files
 */
class Config
{
public:
	/**
	 * @brief Serialize
	 * 
	 * @param[in] InArchive Archive for serialization
	 */
	void						Serialize( class BaseArchive& InArchive );

	/**
	 * @brief Set value
	 * 
	 * @param[in] InGroup Name of group in config
	 * @param[in] InName Name of value in config group
	 * @param[in] InValue Value
	 */
	FORCEINLINE void			SetValue( const tchar* InGroup, const tchar* InName, const ConfigValue& InValue )
	{
		groups[ InGroup ].SetValue( InName, InValue );
	}

	/**
	 * @brief Get value
	 * 
	 * @param[in] InGroup Name of group in config
	 * @param[in] InName Name of value in config group
	 * @return Return value from config, if not founded return empty value
	 */
	FORCEINLINE ConfigValue		GetValue( const tchar* InGroup, const tchar* InName ) const
	{
		MapGroups::const_iterator		itGroup = groups.find( InGroup );
		if ( itGroup == groups.end() )
		{
			return ConfigValue();
		}

		return itGroup->second.GetValue( InName );
	}

private:
	typedef std::unordered_map< std::wstring, ConfigObject >		MapGroups;

	MapGroups			groups;			/**< Config values */
};

#endif // !CONFIG_H
