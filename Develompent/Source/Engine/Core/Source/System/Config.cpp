#include <sstream>

#include "Logger/LoggerMacros.h"
#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "System/Archive.h"
#include "System/Config.h"

/**
 * Serialize
 */
void FConfig::Serialize( FArchive& InArchive )
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
		jsonDocument.Parse<rapidjson::kParseDefaultFlags | rapidjson::kParseCommentsFlag>( ( achar* )buffer );
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
				FConfigObject		object;
				object.Set( itGroup->value, groupName.c_str() );
				groups[ groupName ] = object;
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
		for ( MapGroups::const_iterator itGroup = groups.begin(), itGroupEnd = groups.end(); itGroup != itGroupEnd; ++itGroup )
		{
			InArchive << "\t\"" << TCHAR_TO_ANSI( itGroup->first.c_str() ) << "\": " << ( achar* )itGroup->second.ToJSON( 1 ).c_str();
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
 * Convert value to JSON string
 */
std::string FConfigValue::ToJSON( uint32 InCountTabs /* = 0 */ ) const
{
	std::stringstream	strStream;
	
	switch ( type )
	{
	case T_Bool:	strStream << this->GetBool() ? "true" : "false";							break;
	case T_Float:	strStream << this->GetFloat();												break;
	case T_Int:		strStream << this->GetInt();												break;
	case T_String:	strStream << "\"" << TCHAR_TO_ANSI( this->GetString().c_str() ) << "\"";	break;
	case T_Object:	strStream << this->GetObject().ToJSON( InCountTabs + 1 );					break;
	default:		strStream << "null";														break;
	}

	return strStream.str();
}

/**
 * Convert object to JSON string
 */
std::string FConfigObject::ToJSON( uint32 InCountTabs /* = 0 */ ) const
{
	std::stringstream	strStream;
	std::string			tabs;
	for ( uint32 index = 0; index < InCountTabs; ++index )
	{
		tabs += "\t";
	}

	strStream << "{\n";

	for ( auto itValue = values.begin(), itValueEnd = values.end(); itValue != itValueEnd; ++itValue )
	{
		strStream << tabs << "\t\"" << TCHAR_TO_ANSI( itValue->first.c_str() ) << "\": " << itValue->second.ToJSON( InCountTabs );
		if ( std::next( itValue ) != itValueEnd )
		{
			strStream << ",\n";
		}
		else
		{
			strStream << "\n";
		}
	}

	strStream << tabs << "}";
	return strStream.str();
}

/**
 * Set value from RapidJSON value
 */
void FConfigValue::Set( const rapidjson::Value& InValue, const tchar* InName /* = TEXT( "UNKNOWN" ) */ )
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
	else if ( InValue.IsObject() )
	{
		FConfigObject		object;
		object.Set( InValue, InName );
		SetObject( object );
	}
	else if ( InValue.IsArray() )
	{
		auto							array = InValue.GetArray();
		std::vector< FConfigValue >		configValues;

		for ( uint32 index = 0, count = array.Size(); index < count; ++index )
		{
			FConfigValue		configValue;
			configValue.Set( array[ index ], FString::Format( TEXT( "%s[%i]" ), InName, index ).c_str() );
			configValues.push_back( configValue );
		}

		SetArray( configValues );
	}
	else
	{
		LE_LOG( LT_Warning, LC_General, TEXT( "Unsupported type of config value in %s" ), InName );
	}
}

/**
 * Set object from RapidJSON value
 */
void FConfigObject::Set( const rapidjson::Value& InValue, const tchar* InName /* = TEXT( "UNKNOWN" ) */ )
{
	std::wstring			objectName = InName;

	for ( auto itValue = InValue.MemberBegin(), itValueEnd = InValue.MemberEnd(); itValue != itValueEnd; ++itValue )
	{
		std::wstring		valueName = ANSI_TO_TCHAR( itValue->name.GetString() );
		FConfigValue		value;

		value.Set( itValue->value, ( objectName + TEXT( "::" ) + valueName ).c_str() );
		values[ valueName ] = value;
	}
}

/**
 * Set value
 */
void FConfigObject::SetValue( const tchar* InName, const FConfigValue& InValue )
{
	values[ InName ] = InValue;
}

/**
 * Get value
 */
FConfigValue FConfigObject::GetValue( const tchar* InName ) const
{
	auto		itValue = values.find( InName );
	if ( itValue == values.end() )
	{
		return FConfigValue();
	}

	return itValue->second;
}

/**
 * Copy value
 */
void FConfigValue::Copy( const FConfigValue& InCopy )
{
	type = InCopy.type;

	switch ( type )
	{
	case T_Bool:		SetBool( InCopy.GetBool() );		break;
	case T_Int:			SetInt( InCopy.GetInt() );			break;
	case T_Float:		SetFloat( InCopy.GetFloat() );		break;
	case T_String:		SetString( InCopy.GetString() );	break;
	case T_Object:		SetObject( InCopy.GetObject() );	break;
	case T_Array:		SetArray( InCopy.GetArray() );		break;
	default:			value = nullptr; type = T_None;		break;
	}
}

/**
 * Clear value
 */
void FConfigValue::Clear()
{
	if ( !value ) return;

	switch ( type )
	{
	case T_Bool:            delete static_cast< bool* >( value );							break;
	case T_Int:				delete static_cast< int32* >( value );							break;
	case T_Float:			delete static_cast< float* >( value );							break;
	case T_String:			delete static_cast< std::wstring* >( value );					break;
	case T_Object:			delete static_cast< FConfigObject* >( value );					break;
	case T_Array:			delete static_cast< std::vector< FConfigValue >* >( value );	break;
	}

	value = nullptr;
	type = T_None;
}