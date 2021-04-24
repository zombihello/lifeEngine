#include "Logger/LoggerMacros.h"
#include "Containers/StringConv.h"
#include "System/BaseArchive.h"
#include "System/Config.h"

#undef GetObject

/**
 * Serialize
 */
void Config::Serialize( BaseArchive& InArchive )
{
	if ( InArchive.IsLoading() )
	{
		// Create string buffer and fill '\0'
		uint32				archiveSize = InArchive.GetSize() + 1;
		byte*				buffer = new byte[ archiveSize ];
		memset( buffer, '\0', archiveSize );

		// Serialize data to string buffer
		InArchive.Serialize( buffer, archiveSize );

		// Parse JSON with help RapidJSON
		rapidjson::Document		jsonDocument;
		jsonDocument.Parse( ( achar* )buffer );
		delete[] buffer;

		if ( jsonDocument.HasParseError() )
		{		
			appErrorf( TEXT( "Failed parse config, not correct syntax of JSON. RapidJSON code error %i" ), jsonDocument.GetParseError() );
		}

		// Reading all values
		for ( auto itGroup = jsonDocument.MemberBegin(), itGroupEnd = jsonDocument.MemberEnd(); itGroup != itGroupEnd; ++itGroup )
		{
			std::wstring		groupName = ANSI_TO_TCHAR( itGroup->name.GetString() );

			if ( itGroup->value.IsObject() )
			{
				for ( auto itValue = itGroup->value.GetObject().MemberBegin(), itValueEnd = itGroup->value.GetObject().MemberEnd(); itValue != itValueEnd; ++itValue )
				{					
					std::wstring		valueName = ANSI_TO_TCHAR( itValue->name.GetString() );
					if ( itValue->value.IsObject() )
					{
						LE_LOG( LT_Warning, LC_General, TEXT( "Groups in group not supported. Group %s" ), ( groupName + TEXT( "::" ) + valueName ).c_str() );
						continue;
					}

					Value				value;
					value.Set( itValue->value, ( groupName + TEXT( "::" ) + valueName ).c_str() );
					values[ groupName ][ valueName ] = value;
				}
			}
			else
			{
				LE_LOG( LT_Warning, LC_General, TEXT( "Member %s in config is not group" ), groupName.c_str() );
			}
		}
	}
	else
	{
		InArchive << "{\n";
		for ( MapGroups::const_iterator itGroup = values.begin(), itGroupEnd = values.end(); itGroup != itGroupEnd; ++itGroup )
		{
			InArchive << "\t\"" << TCHAR_TO_ANSI( itGroup->first.c_str() ) << "\": {\n";	
			for ( MapValues::const_iterator itValue = itGroup->second.begin(), itValueEnd = itGroup->second.end(); itValue != itValueEnd; ++itValue )
			{
				const Value&		value = itValue->second;
				InArchive << "\t\t\"" << TCHAR_TO_ANSI( itValue->first.c_str() ) << "\": ";

				switch ( value.GetType() )
				{
				case Value::T_Bool:
				case Value::T_Float:
				case Value::T_Int:
					InArchive << TCHAR_TO_ANSI( value.ToString().c_str() );
					break;

				case Value::T_String:	
					InArchive << "\"" << TCHAR_TO_ANSI( value.ToString().c_str() ) << "\"";
					break;

				default:				
					InArchive << "null";
					break;
				}

				if ( std::next( itValue ) != itValueEnd )
				{
					InArchive << ",\n";
				}
				else
				{
					InArchive << "\n";
				}
			}
			InArchive << "\t}";

			if ( std::next( itGroup ) != itGroupEnd )
			{
				InArchive << ",\n\n";
			}
			else
			{
				InArchive << "\n";
			}
		}
		InArchive << "}";
	}
}

/**
 * Set value bool
 */
void Config::SetBool( const tchar* InGroup, const tchar* InName, bool InValue )
{
	Value		value;
	value.SetBool( InValue );

	values[ InGroup ][ InName ] = value;
}

/**
 * Set value int
 */
void Config::SetInt( const tchar* InGroup, const tchar* InName, int32 InValue )
{
	Value		value;
	value.SetInt( InValue );

	values[ InGroup ][ InName ] = value;
}

/**
 * Set value float
 */
void Config::SetFloat( const tchar* InGroup, const tchar* InName, float InValue )
{
	Value		value;
	value.SetFloat( InValue );

	values[ InGroup ][ InName ] = value;
}

/**
 * Set value string
 */
void Config::SetString( const tchar* InGroup, const tchar* InName, const tchar* InValue )
{
	Value		value;
	value.SetString( InValue );

	values[ InGroup ][ InName ] = value;
}

/**
 * Find value
 */
Config::Value* Config::FindValue( const tchar* InGroup, const tchar* InName ) const
{
	MapGroups::const_iterator		itGroup = values.find( InGroup );
	if ( itGroup == values.end() )
	{
		return nullptr;
	}

	MapValues::const_iterator		itValue = itGroup->second.find( InName );
	if ( itValue == itGroup->second.end() )
	{
		return nullptr;
	}

	return ( Config::Value* ) &itValue->second;
}

/**
 * Get value bool
 */
bool Config::GetBool( const tchar* InGroup, const tchar* InName, bool InDefaultValue /* = false */ ) const
{
	Value*		value = FindValue( InGroup, InName );
	if ( !value || value->GetType() != Value::T_Bool )
	{
		return InDefaultValue;
	}

	return value->GetBool();
}

/**
 * Get value int
 */
int32 Config::GetInt( const tchar* InGroup, const tchar* InName, int32 InDefaultValue /* = 0 */ ) const
{
	Value*		value = FindValue( InGroup, InName );
	if ( !value || value->GetType() != Value::T_Int )
	{
		return InDefaultValue;
	}

	return value->GetInt();
}

/**
 * Get value float
 */
float Config::GetFloat( const tchar* InGroup, const tchar* InName, float InDefaultValue /* = 0.f */ ) const
{
	Value* value = FindValue( InGroup, InName );
	if ( !value || value->GetType() != Value::T_Float )
	{
		return InDefaultValue;
	}

	return value->GetFloat();
}

/**
 * Get value string
 */
std::wstring Config::GetString( const tchar* InGroup, const tchar* InName, const tchar* InDefaultValue /* = TEXT( "" ) */ ) const
{
	Value* value = FindValue( InGroup, InName );
	if ( !value || value->GetType() != Value::T_String )
	{
		return InDefaultValue;
	}

	return value->GetString();
}

/**
 * Convert value to string
 */
std::wstring Config::Value::ToString() const
{
	switch ( type )
	{
	case T_Bool:	return this->GetBool() ? TEXT( "true" ) : TEXT( "false" );
	case T_Float:	return std::to_wstring( this->GetFloat() );
	case T_Int:		return std::to_wstring( this->GetInt() );
	case T_String:	return this->GetString();
	default:		return TEXT( "null" );
	}
}

/**
 * Set value from RapidJSON value
 */
void Config::Value::Set( const rapidjson::Value& InValue, const tchar* InName )
{
	if ( InValue.IsNull() )
	{
		return;
	}
	else if ( InValue.IsBool() )
	{
		SetBool( InValue.GetBool() );
	}
	else if ( InValue.IsInt() )
	{
		SetInt( InValue.GetInt() );
	}
	else if ( InValue.IsFloat() )
	{
		SetFloat( InValue.GetFloat() );
	}
	else if ( InValue.IsString() )
	{
		SetString( ANSI_TO_TCHAR( InValue.GetString() ) );
	}
	else
	{
		LE_LOG( LT_Warning, LC_General, TEXT( "Unsupported type of config value in %s" ), InName );
	}
}

/**
 * Copy value
 */
void Config::Value::Copy( const Value& InCopy )
{
	type = InCopy.type;

	switch ( type )
	{
	case T_Bool:		SetBool( InCopy.GetBool() );		break;
	case T_Int:			SetInt( InCopy.GetInt() );			break;
	case T_Float:		SetFloat( InCopy.GetFloat() );		break;
	case T_String:		SetString( InCopy.GetString() );	break;
	default:			value = nullptr; type = T_None;		break;
	}
}

/**
 * Clear value
 */
void Config::Value::Clear()
{
	if ( !value ) return;

	switch ( type )
	{
	case T_Bool:            delete static_cast< bool* >( value );			break;
	case T_Int:				delete static_cast< int32* >( value );			break;
	case T_Float:			delete static_cast< float* >( value );			break;
	case T_String:			delete static_cast< std::wstring* >( value );	break;
	}

	value = nullptr;
	type = T_None;
}