#include <sstream>

#include "Logger/LoggerMacros.h"
#include "Misc/StringConv.h"
#include "Misc/CoreGlobals.h"
#include "Misc/Misc.h"
#include "System/Archive.h"
#include "System/BaseFileSystem.h"
#include "Misc/JsonDocument.h"

/*
==================
CJsonDocument::LoadFromFile
==================
*/
bool CJsonDocument::LoadFromFile( const tchar* InPath )
{
	// Try open file
	CArchive*	file = g_FileSystem->CreateFileReader( InPath );
	if ( !file )
	{
		return false;
	}

	// Allocate memory for buffer
	uint32	fileSize = file->GetSize() + 1;
	byte*	buffer = ( byte* )Memory::MallocZeroed( fileSize );

	// Serialize data to string buffer
	file->Serialize( buffer, fileSize );

	// Load data from buffer
	bool	bResult = LoadFromBuffer( ( const achar* )buffer );

	// Free allocated memory
	Memory::Free( buffer );
	delete file;
	return bResult;
}

/*
==================
CJsonDocument::LoadFromBuffer
==================
*/
bool CJsonDocument::LoadFromBuffer( const achar* InBuffer )
{
	// Do nothing if buffer isn't valid
	if ( !InBuffer || !InBuffer[0] )
	{
		Assert( false );
		return false;
	}

	// Parse JSON by RapidJSON
	rapidjson::Document		jsonDocument;
	jsonDocument.Parse<rapidjson::kParseDefaultFlags | rapidjson::kParseCommentsFlag>( InBuffer );
	if ( jsonDocument.HasParseError() )
	{
		Warnf( TEXT( "Failed to parse JSON document, not correct JSON syntax. RapidJson code error 0x%X\n" ), jsonDocument.GetParseError() );
		return false;
	}

	// Read all JSON values
	for ( auto itValue = jsonDocument.MemberBegin(), itValueEnd = jsonDocument.MemberEnd(); itValue != itValueEnd; ++itValue )
	{
		std::wstring			valueName = ANSI_TO_TCHAR( itValue->name.GetString() );
		
		// Find JSON value in memory
		CJsonValue*				jsonValue = nullptr;
		ValuesMap_t::iterator	itJsonValue = valuesMap.find( valueName );
		if ( itJsonValue != valuesMap.end() )
		{
			jsonValue = &itJsonValue->second;
		}
		else
		{
			jsonValue = &valuesMap.insert( std::make_pair( valueName, CJsonValue() ) ).first->second;
		}

		// Set value
		if ( jsonValue && itValue->value.IsObject() && jsonValue->IsA( JVT_Object )  )
		{
			CJsonObject*	jsonObject = const_cast<CJsonObject*>( jsonValue->GetObject() );
			Assert( jsonObject );
			jsonObject->Set( itValue->value, valueName.c_str() );
		}
		else
		{
			jsonValue->Set( itValue->value, valueName.c_str() );
		}
	}

	return true;
}

/*
==================
CJsonDocument::SaveToFile
==================
*/
bool CJsonDocument::SaveToFile( const tchar* InPath ) const
{
	// Save data to buffer
	std::string		buffer;
	if ( !SaveToBuffer( buffer ) )
	{
		Warnf( TEXT( "Failed to save JSON document into buffer\n" ) );
		return false;
	}

	// Try open file for save
	CArchive*	file = g_FileSystem->CreateFileWriter( InPath );
	if ( !file )
	{
		Warnf( TEXT( "Failed to create a file\n" ) );
		return false;
	}

	// Serialize buffer to the file
	file->Serialize( buffer.data(), buffer.size() * sizeof( achar ) );
	delete file;
	return true;
}

/*
==================
CJsonDocument::SaveToBuffer
==================
*/
bool CJsonDocument::SaveToBuffer( std::string& OutBuffer ) const
{
	OutBuffer = "{\n";
	for ( ValuesMap_t::const_iterator itValue = valuesMap.begin(), itValueEnd = valuesMap.end(); itValue != itValueEnd; ++itValue )
	{
		OutBuffer += L_Sprintf( "\t\"%s\": %s", itValue->first.c_str(), itValue->second.ToJson( 1 ).c_str() );
		if ( std::next( itValue ) != itValueEnd )
		{
			OutBuffer += ",\n";
		}
		else
		{
			OutBuffer += "\n";
		}
	}

	OutBuffer += "}";
	return true;
}

/*
==================
CJsonValue::ToJson
==================
*/
std::string CJsonValue::ToJson( uint32 InCountTabs /* = 0 */ ) const
{
	std::stringstream	strStream;
	static CJsonObject	s_EmptyJsonObject;
	switch ( type )
	{
	case JVT_Bool:		strStream << GetBool() ? "true" : "false";																			break;
	case JVT_Float:		strStream << GetFloat();																							break;
	case JVT_Int:		strStream << GetInt();																								break;
	case JVT_String:	strStream << "\"" << TCHAR_TO_ANSI( GetString().c_str() ) << "\"";													break;
	case JVT_Object:	strStream << GetObject() ? GetObject()->ToJson( InCountTabs + 1 ) : s_EmptyJsonObject.ToJson( InCountTabs + 1 );	break;
	default:			strStream << "null";																								break;
	}

	return strStream.str();
}

/*
==================
CJsonObject::ToJson
==================
*/
std::string CJsonObject::ToJson( uint32 InCountTabs /* = 0 */ ) const
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
		strStream << tabs << "\t\"" << TCHAR_TO_ANSI( itValue->first.c_str() ) << "\": " << itValue->second.ToJson( InCountTabs );
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
CJsonValue::Set
==================
*/
void CJsonValue::Set( const rapidjson::Value& InValue, const tchar* InDebugName /* = TEXT( "UNKNOWN" ) */ )
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
		CJsonObject		object;
		object.Set( InValue, InDebugName );
		SetObject( object );
	}
	else if ( InValue.IsArray() )
	{
		auto						array = InValue.GetArray();
		std::vector<CJsonValue>		values;
		for ( uint32 index = 0, count = array.Size(); index < count; ++index )
		{		
			values.emplace_back().Set( array[index], L_Sprintf( TEXT( "%s[%i]" ), InDebugName, index ).c_str() );
		}

		SetArray( values );
	}
	else
	{
		Warnf( TEXT( "Unsupported JSON type in %s\n" ), InDebugName );
	}
}

/*
==================
CJsonObject::Set
==================
*/
void CJsonObject::Set( const rapidjson::Value& InValue, const tchar* InDebugName /* = TEXT( "UNKNOWN" ) */ )
{
	for ( auto itValue = InValue.MemberBegin(), itValueEnd = InValue.MemberEnd(); itValue != itValueEnd; ++itValue )
	{
		std::wstring	valueName = ANSI_TO_TCHAR( itValue->name.GetString() );
		CJsonValue		value;

		value.Set( itValue->value, L_Sprintf( TEXT( "%s::%s" ), InDebugName, valueName.c_str() ).c_str() );
		values[valueName] = value;
	}
}

/*
==================
CJsonObject::SetValue
==================
*/
void CJsonObject::SetValue( const tchar* InName, const class CJsonValue& InValue )
{
	values[InName] = InValue;
}

/*
==================
CJsonObject::GetValue
==================
*/
CJsonValue* CJsonObject::GetValue( const tchar* InName )
{
	auto		itValue = values.find( InName );
	if ( itValue == values.end() )
	{
		return nullptr;
	}
	return &itValue->second;
}

/*
==================
CJsonObject::GetValue
==================
*/
const CJsonValue* CJsonObject::GetValue( const tchar* InName ) const
{
	auto		itValue = values.find( InName );
	if ( itValue == values.end() )
	{
		return nullptr;
	}
	return &itValue->second;
}

/*
==================
CJsonValue::Copy
==================
*/
void CJsonValue::Copy( const CJsonValue& InCopy )
{
	switch ( InCopy.type )
	{
	case JVT_Bool:		SetBool( InCopy.GetBool() );		break;
	case JVT_Int:		SetInt( InCopy.GetInt() );			break;
	case JVT_Float:		SetFloat( InCopy.GetFloat() );		break;
	case JVT_String:	SetString( InCopy.GetString() );	break;
	case JVT_Object:	SetObject( *InCopy.GetObject() );	break;
	case JVT_Array:		SetArray( *InCopy.GetArray() );		break;
	default:			value = nullptr; type = JVT_None;	break;
	}
}

/*
==================
CJsonValue::Clear
==================
*/
void CJsonValue::Clear()
{
	if ( !value )
	{
		return;
	}

	switch ( type )
	{
	case JVT_Bool:          delete static_cast<bool*>( value );							break;
	case JVT_Int:			delete static_cast<int32*>( value );						break;
	case JVT_Float:			delete static_cast<float*>( value );						break;
	case JVT_String:		delete static_cast<std::wstring*>( value );					break;
	case JVT_Object:		delete static_cast<CJsonObject*>( value );					break;
	case JVT_Array:			delete static_cast<std::vector<CJsonValue>*>( value );		break;
	}

	value = nullptr;
	type = JVT_None;
}