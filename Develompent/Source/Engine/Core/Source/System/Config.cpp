#include <sstream>

#include "Logger/LoggerMacros.h"
#include "Misc/StringConv.h"
#include "Misc/CoreGlobals.h"
#include "Misc/Misc.h"
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
CConfig::Init
==================
*/
void CConfig::Init()
{
	// Serialize all configs
	for ( uint32 index = 0; index < CT_Num; ++index )
	{
		bool			bSuccessed = false;
		CJsonDocument	jsonDocument;

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
			if ( !jsonDocument.LoadFromFile( pathToConfig.c_str() ) )
			{
				Warnf( TEXT( "Config layer '%s' not found or failed to parse\n" ), pathToConfig.c_str() );
				continue;
			}

			Logf( TEXT( "Loaded layer '%s' for config '%s'\n" ), s_ConfigLayerNames[layer], s_ConfigTypeNames[index] );
			bSuccessed = true;
		}

		if ( !bSuccessed )
		{
			Sys_Error( TEXT( "Config type '%s' not loaded" ), s_ConfigTypeNames[index] );
		}
		configs[( EConfigType )index] = jsonDocument;
	}
}

/*
==================
CConfig::SetValue
==================
*/
void CConfig::SetValue( EConfigType InType, const tchar* InGroup, const tchar* InName, const CJsonValue& InValue )
{
	CJsonValue*		jsonValue = GetConfig( InType ).GetValue( InGroup );
	if ( jsonValue && jsonValue->IsValid() && jsonValue->IsA( JVT_Object ) )
	{
		CJsonObject*	jsonObject = const_cast<CJsonObject*>( jsonValue->GetObject() );
		jsonObject->SetValue( InName, InValue );
	}
	else
	{
		Warnf( TEXT( "Not found or invalid '%s:%s\n" ), InGroup, InName );
	}
}

/*
==================
CConfig::GetValue
==================
*/
const CJsonValue* CConfig::GetValue( EConfigType InType, const tchar* InGroup, const tchar* InName ) const
{
	const CJsonValue*		jsonValue = GetConfig( InType ).GetValue( InGroup );
	if ( jsonValue && jsonValue->IsValid() && jsonValue->IsA( JVT_Object ) )
	{
		CJsonObject*	jsonObject = const_cast<CJsonObject*>( jsonValue->GetObject() );
		return jsonObject->GetValue( InName );
	}

	Warnf( TEXT( "Not found or invalid '%s:%s\n" ), InGroup, InName );
	return nullptr;
}