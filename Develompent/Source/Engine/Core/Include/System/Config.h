/**
 * @file
 * @addtogroup Core Core
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/24/2021
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <unordered_map>
#include <rapidjson/document.h>

#include "Core.h"

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
	void				Serialize( class BaseArchive& InArchive );

	/**
	 * @brief Set value bool
	 * 
	 * @param[in] InGroup Name of group in config
	 * @param[in] InName Name of value in config group
	 * @param[in] InValue Value
	 */
	void				SetBool( const tchar* InGroup, const tchar* InName, bool InValue );

	/**
	 * @brief Set value int
	 *
	 * @param[in] InGroup Name of group in config
	 * @param[in] InName Name of value in config group
	 * @param[in] InValue Value
	 */
	void				SetInt( const tchar* InGroup, const tchar* InName, int32 InValue );

	/**
	 * @brief Set value float
	 *
	 * @param[in] InGroup Name of group in config
	 * @param[in] InName Name of value in config group
	 * @param[in] InValue Value
	 */
	void				SetFloat( const tchar* InGroup, const tchar* InName, float InValue );

	/**
	 * @brief Set value string
	 *
	 * @param[in] InGroup Name of group in config
	 * @param[in] InName Name of value in config group
	 * @param[in] InValue Value
	 */
	void				SetString( const tchar* InGroup, const tchar* InName, const tchar* InValue );

	/**
	 * @brief Get value bool
	 * 
	 * @param[in] InGroup Name of group in config
	 * @param[in] InName Name of value in config group
	 * @param[in] InDefaultValue If value not found return this value
	 * @return Return value from config, if not founded or value not bool return InDefaultValue
	 */
	bool				GetBool( const tchar* InGroup, const tchar* InName, bool InDefaultValue = false ) const;

	/**
	 * @brief Get value int
	 * 
	 * @param[in] InGroup Name of group in config
	 * @param[in] InName Name of value in config group
	 * @param[in] InDefaultValue If value not found return this value
	 * @return Return value from config, if not founded or value not integer return InDefaultValue
	 */
	int32				GetInt( const tchar* InGroup, const tchar* InName, int32 InDefaultValue = 0 ) const;

	/**
	 * @brief Get value float
	 * 
	 * @param[in] InGroup Name of group in config
	 * @param[in] InName Name of value in config group
	 * @param[in] InDefaultValue If value not found return this value
	 * @return Return value from config, if not founded or value not float return InDefaultValue
	 */
	float				GetFloat( const tchar* InGroup, const tchar* InName, float InDefaultValue = 0.f ) const;

	/**
	 * @brief Get value string
	 * 
	 * @param[in] InGroup Name of group in config
	 * @param[in] InName Name of value in config group
	 * @param[in] InDefaultValue If value not found return this value
	 * @return Return value from config, if not founded or value not string return InDefaultValue
	 */
	std::wstring		GetString( const tchar* InGroup, const tchar* InName, const tchar* InDefaultValue = TEXT( "" ) ) const;

private:
	/**
	 * @brief Class value of config
	 */
	class Value
	{
	public:
		friend BaseArchive& operator<<( BaseArchive& InArchive, Value& InValue );

		/**
		 * @brief Enumeration of types value
		 */
		enum EType
		{
			T_None,				/**< No type */
			T_Bool,				/**< Bool type */
			T_Int,				/**< Integer type */
			T_Float,			/**< Float type */
			T_String			/**< String type */
		};

		/**
		 * @brief Constructor
		 */
		FORCEINLINE						Value() :
			type( T_None ),
			value( nullptr )
		{}

		/**
		 * @brief Constructor of copy
		 * @param[in] InCopy Copy of value
		 */
		FORCEINLINE						Value( const Value& InCopy ) :
			type( T_None ),
			value( nullptr )
		{
			Copy( InCopy );
		}

		/**
		 * @brief Destructor
		 */
		FORCEINLINE						~Value()
		{
			Clear();
		}

		/**
		 * @brief Clear value
		 */
		void							Clear();

		/**
		 * @brief Copy value
		 * @param[in] InCopy Copy
		 */
		void							Copy( const Value& InCopy );

		/**
		 * @brief Convert value to string
		 * @return Return converted value in string
		 */
		std::wstring					ToString() const;

		/**
		 * @brief Set value from RapidJSON value
		 * @param[in] InValue RapidJSON value
		 * @param[in] InName RapidJSON name for print name of value in log while error
		 */
		void							Set( const rapidjson::Value& InValue, const tchar* InName = TEXT( "UNKNOWN" ) );

		/**
		 * @brief Set bool
		 * @param[in] InValue Value
		 */
		FORCEINLINE void				SetBool( bool InValue )
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
		FORCEINLINE void				SetInt( int32 InValue )
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
		FORCEINLINE void				SetFloat( float InValue )
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
		FORCEINLINE void				SetString( const std::wstring& InValue )
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
		 * @brief Get type value
		 * @return Type of value
		 */
		FORCEINLINE EType				GetType() const
		{
			return type;
		}

		/**
		 * @brief Get bool
		 * @return Value with type bool, if type not correct return false
		 */
		FORCEINLINE bool				GetBool() const
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
		FORCEINLINE int32				GetInt() const
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
		FORCEINLINE float				GetFloat() const
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
		FORCEINLINE std::wstring		GetString() const
		{
			if ( type != T_String || !value )
			{
				return TEXT( "" );
			}

			return *static_cast< std::wstring* >( value );
		}

		/**
		 * @brief Operator = for copy value
		 * @param[in] InCopy Copy of value
		 */
		FORCEINLINE Value& operator=( const Value& InCopy )
		{
			Copy( InCopy );
			return *this;
		}

	private:
		EType			type;		/**< Type of value */
		void*			value;		/**< Pointer to value */
	};

	typedef std::unordered_map< std::wstring, Value >			MapValues;
	typedef std::unordered_map< std::wstring, MapValues >		MapGroups;

	/**
	 * @brief Find value
	 * 
	 * @param[in] InGroup Name of group in config
	 * @param[in] InName Name of value in config group
	 * @return Return pointer to value. If not founded return nullptr
	 */
	Value*				FindValue( const tchar* InGroup, const tchar* InName ) const;

	MapGroups		values;			/**< Config values */
};

#endif // !CONFIG_H
