#include <sstream>

#include "Logger/LoggerMacros.h"
#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Misc/CoreGlobals.h"
#include "System/Archive.h"
#include "System/Config.h"
#include "System/BaseFileSystem.h"

static const tchar* s_ConfigTypeNames[] =
{
	TEXT( "Engine" ),		// CT_Engine
	TEXT( "Game" ),			// CT_Game
	TEXT( "Input" ),		// CT_Input
	TEXT( "Editor" ),		// CT_Editor
};

static const tchar* s_ConfigLayerNames[] =
{
	TEXT( "Engine" ),		// CL_Engine
	TEXT( "Game" ),			// CL_Game
	TEXT( "User" )			// CL_User
};

/*
==================
CConfigManager::Init
==================
*/
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
			case CL_Engine:			pathToConfig = Sys_BaseDir() + PATH_SEPARATOR TEXT( "Engine" ) PATH_SEPARATOR TEXT( "Config" ) PATH_SEPARATOR + s_ConfigTypeNames[index] + TEXT( ".ini" );		break;
			case CL_Game:			pathToConfig = Sys_GameDir() + PATH_SEPARATOR TEXT( "Config" ) PATH_SEPARATOR + s_ConfigTypeNames[index] + TEXT( ".ini" );										break;
			default:
				Warnf( TEXT( "Config layer '0x%X' not supported\n" ), layer );
				continue;
				break;
			}

			// Load config
			if ( !config.LoadFile( pathToConfig ) )
			{
				Warnf( TEXT( "Config layer '%s' not found or failed in parse\n" ), pathToConfig.c_str() );
				continue;
			}

			Logf( TEXT( "Loaded layer '%s' for config '%s'\n" ), s_ConfigLayerNames[layer], s_ConfigTypeNames[index] );
			bSuccessed = true;
		}

		if ( !bSuccessed )
		{
			Sys_Errorf( TEXT( "Config type '%s' not loaded" ), s_ConfigTypeNames[index] );
		}

		configs[( EConfigType )index] = config;
	}
}

/*
==================
CConfig::LoadFile
==================
*/
bool CConfig::LoadFile( const std::wstring& InFile )
{
	// Open file for read
	CArchive*	archive = g_FileSystem->CreateFileReader( InFile );
	if ( !archive )
	{
		Warnf( TEXT( "Config file '%s' not found\n" ), InFile.c_str() );
		return false;
	}

	// Serialize data from it
	bool	bResult = Serialize( *archive );

	// Free allocated memory and exit
	delete archive;
	return bResult;
}

/*
==================
CConfig::SaveFile
==================
*/
bool CConfig::SaveFile( const std::wstring& InFile )
{
	// Open file for write
	CArchive*	archive = g_FileSystem->CreateFileWriter( InFile );
	if ( !archive )
	{
		Warnf( TEXT( "Config file '%s' can't create\n" ), InFile.c_str() );
		return false;
	}

	// Serialize data from it
	bool	bResult = Serialize( *archive );

	// Free allocated memory and exit
	delete archive;
	return bResult;
}

/*
==================
CConfig::Serialize
==================
*/
bool CConfig::Serialize( CArchive& InArchive )
{
	if ( InArchive.IsLoading() )
	{
		// Create string buffer and fill '\0'
		uint32				archiveSize = InArchive.GetSize() + 1;
		byte*				buffer = new byte[ archiveSize ];
		Memory::Memzero( buffer, archiveSize );

		// Serialize data to string buffer
		InArchive.Serialize( buffer, archiveSize );

		// Parse JSON with help RapidJSON
		rapidjson::Document		jsonDocument;
		jsonDocument.Parse<rapidjson::kParseDefaultFlags | rapidjson::kParseCommentsFlag>( ( achar* )buffer );
		delete[] buffer;

		if ( jsonDocument.HasParseError() )
		{		
			Warnf( TEXT( "Failed parse config, not correct syntax of JSON. RapidJSON code error %i\n" ), jsonDocument.GetParseError() );
			return false;
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
				Warnf( TEXT( "Member %s in config is not group\n" ), groupName.c_str() );
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

	return true;
}

/*
==================
CConfigValue::ToJSON
==================
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

/*
==================
CConfigObject::ToJSON
==================
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

/*
==================
CConfigValue::Set
==================
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
		Warnf( TEXT( "Unsupported type of config value in %s\n" ), InName );
	}
}

/*
==================
CConfigObject::Set
==================
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

/*
==================
CConfigObject::SetValue
==================
*/
void CConfigObject::SetValue( const tchar* InName, const CConfigValue& InValue )
{
	values[ InName ] = InValue;
}

/*
==================
CConfigObject::GetValue
==================
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

/*
==================
CConfigValue::Copy
==================
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

/*
==================
CConfigValue::Clear
==================
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