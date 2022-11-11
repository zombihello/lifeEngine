#include <sstream>

#include "Logger/LoggerMacros.h"
#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Misc/CoreGlobals.h"
#include "System/Archive.h"
#include "System/Config.h"
#include "System/BaseFileSystem.h"

static const tchar* GConfigTypeNames[] =
{
	TEXT( "Engine" ),		// CT_Engine
	TEXT( "Game" ),			// CT_Game
	TEXT( "Input" ),		// CT_Input
	TEXT( "Editor" ),		// CT_Editor
};

static const tchar* GConfigLayerNames[] =
{
	TEXT( "Engine" ),		// CL_Engine
	TEXT( "Game" ),			// CL_Game
	TEXT( "User" )			// CL_User
};

void CConfigManager::Init()
{
	// Serialize all configs
	for ( uint32 index = 0; index < CT_Num; ++index )
	{
		bool		bSuccessed = false;
		CConfig		config;

		// Serialize configs of all layers
		for ( uint32 layer = 0; layer < CL_Num; ++layer )
		{
			// Getting path to config
			std::wstring				pathToConfig;
			switch ( layer )
			{
			case CL_Engine:			pathToConfig = appBaseDir() + PATH_SEPARATOR TEXT( "Engine" ) PATH_SEPARATOR TEXT( "Config" ) PATH_SEPARATOR + GConfigTypeNames[index] + TEXT( ".ini" );		break;
			case CL_Game:			pathToConfig = appGameDir() + PATH_SEPARATOR TEXT( "Config" ) PATH_SEPARATOR + GConfigTypeNames[index] + TEXT( ".ini" );										break;
			default:
				LE_LOG( LT_Warning, LC_General, TEXT( "Config layer '0x%X' not supported" ), layer );
				continue;
				break;
			}

			// Open file for reading
			CArchive*				archive = GFileSystem->CreateFileReader( pathToConfig );
			if ( !archive )
			{
				LE_LOG( LT_Warning, LC_General, TEXT( "Config layer '%s' not founded" ), pathToConfig.c_str() );
				continue;
			}

			// Serialize config
			config.Serialize( *archive );

			LE_LOG( LT_Log, LC_General, TEXT( "Loaded layer '%s' for config '%s'" ), GConfigLayerNames[layer], GConfigTypeNames[index] );
			bSuccessed = true;
		}

		if ( !bSuccessed )
		{
			appErrorf( TEXT( "Config type '%s' not loaded" ), GConfigTypeNames[index] );
		}

		configs[( EConfigType )index] = config;
	}
}

/**
 * Serialize
 */
void CConfig::Serialize( CArchive& InArchive )
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
			std::wstring		groupName			= ANSI_TO_TCHAR( itGroup->name.GetString() );

			// Find config object in memory
			CConfigObject*		configObject		= nullptr;
			MapGroups_t::iterator	itConfigGroup	= groups.find( groupName );
			if ( itConfigGroup != groups.end() )
			{
				configObject = &itConfigGroup->second;
			}
			else
			{
				configObject = &groups.insert( std::make_pair( groupName, CConfigObject() ) ).first->second;
			}

			// Set value to object
			if ( itGroup->value.IsObject() )
			{
				configObject->Set( itGroup->value, groupName.c_str() );
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
		for ( MapGroups_t::const_iterator itGroup = groups.begin(), itGroupEnd = groups.end(); itGroup != itGroupEnd; ++itGroup )
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
std::string CConfigValue::ToJSON( uint32 InCountTabs /* = 0 */ ) const
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
std::string CConfigObject::ToJSON( uint32 InCountTabs /* = 0 */ ) const
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
void CConfigValue::Set( const rapidjson::Value& InValue, const tchar* InName /* = TEXT( "UNKNOWN" ) */ )
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
		CConfigObject		object;
		object.Set( InValue, InName );
		SetObject( object );
	}
	else if ( InValue.IsArray() )
	{
		auto							array = InValue.GetArray();
		std::vector< CConfigValue >		configValues;

		for ( uint32 index = 0, count = array.Size(); index < count; ++index )
		{
			CConfigValue		configValue;
			configValue.Set( array[ index ], CString::Format( TEXT( "%s[%i]" ), InName, index ).c_str() );
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
void CConfigObject::Set( const rapidjson::Value& InValue, const tchar* InName /* = TEXT( "UNKNOWN" ) */ )
{
	std::wstring			objectName = InName;

	for ( auto itValue = InValue.MemberBegin(), itValueEnd = InValue.MemberEnd(); itValue != itValueEnd; ++itValue )
	{
		std::wstring		valueName = ANSI_TO_TCHAR( itValue->name.GetString() );
		CConfigValue		value;

		value.Set( itValue->value, ( objectName + TEXT( "::" ) + valueName ).c_str() );
		values[ valueName ] = value;
	}
}

/**
 * Set value
 */
void CConfigObject::SetValue( const tchar* InName, const CConfigValue& InValue )
{
	values[ InName ] = InValue;
}

/**
 * Get value
 */
CConfigValue CConfigObject::GetValue( const tchar* InName ) const
{
	auto		itValue = values.find( InName );
	if ( itValue == values.end() )
	{
		return CConfigValue();
	}

	return itValue->second;
}

/**
 * Copy value
 */
void CConfigValue::Copy( const CConfigValue& InCopy )
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
void CConfigValue::Clear()
{
	if ( !value ) return;

	switch ( type )
	{
	case T_Bool:            delete static_cast< bool* >( value );							break;
	case T_Int:				delete static_cast< int32* >( value );							break;
	case T_Float:			delete static_cast< float* >( value );							break;
	case T_String:			delete static_cast< std::wstring* >( value );					break;
	case T_Object:			delete static_cast< CConfigObject* >( value );					break;
	case T_Array:			delete static_cast< std::vector< CConfigValue >* >( value );	break;
	}

	value = nullptr;
	type = T_None;
}