// AngleScript
#include <angelscript.h>

// AngleScript addons
#include "Scripts/AngelScriptAddons/scriptstdstring/scriptstdstring.h"
#include "Scripts/AngelScriptAddons/scriptbuilder/scriptbuilder.h"

#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "System/Config.h"
#include "Scripts/ScriptEngine.h"

#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "System/BaseArchive.h"
#include "System/BaseFileSystem.h"

// STL
#if WITH_EDITOR
	#include <sstream>
#endif // WITH_EDITOR

class ScriptByteCode : public asIBinaryStream
{
public:
	/**
	 * Constructor
	 */
				ScriptByteCode() : archive( nullptr )
	{}

	/**
	 * Set archive for serialize
	 */
	void		SetArchive( BaseArchive* InArchive )
	{
		archive = InArchive;
	}

	/**
	 * Write to archive of script byte code
	 */
	int			Write( const void* InBuffer, asUINT InSize )
	{
		check( archive && archive->IsSaving() );

		archive->Serialize( ( void* )InBuffer, InSize );
		return 0;
	}

	/**
	 * Read from archive of script byte code
	 */
	int			Read( void* InBuffer, asUINT InSize ) 
	{ 
		check( archive && archive->IsLoading() );

		archive->Serialize( InBuffer, InSize );
		return 0;
	}

private:
	BaseArchive*			archive;
};

/**
 * Message callback from AngleScript engine
 */
void CDECL ASMessageCallback( const asSMessageInfo* InMessage, void* InParam )
{
	check( InMessage );

	ELogType		logType = LT_Log;
	switch ( InMessage->type )
	{
	case asMSGTYPE_WARNING:
		logType = LT_Warning;
		break;

	case asMSGTYPE_ERROR:
		logType = LT_Error;
		break;
	}

	LE_LOG( logType, LC_Script, TEXT( "%s (%d, %d) : %s" ), ANSI_TO_TCHAR( InMessage->section ), InMessage->row, InMessage->col, ANSI_TO_TCHAR( InMessage->message ) );
}

/**
 * Constructor
 */
ScriptEngine::ScriptEngine() :
	asScriptEngine( nullptr )
{}

/**
 * Destructor
 */
ScriptEngine::~ScriptEngine()
{
	if ( !asScriptEngine )		return;
	asScriptEngine->ShutDownAndRelease();
}

/**
 * Initialize
 */
void ScriptEngine::Init()
{
	if ( asScriptEngine )		return;

	// Create script engine
	asScriptEngine = asCreateScriptEngine( ANGELSCRIPT_VERSION );
	check( asScriptEngine );

	// The script compiler will send any compiler messages to the callback
	asScriptEngine->SetMessageCallback( asFUNCTION( ASMessageCallback ), 0, asCALL_CDECL );

	// Register std strings in scripts
	RegisterStdString( asScriptEngine );

#if WITH_EDITOR
	RegisterTypeASToCPP( "string", "std::string" );
#endif // WITH_EDITOR
}

/**
 * Load script modules
 */
void ScriptEngine::LoadModules()
{
	std::wstring					scriptPath = GEngineConfig.GetValue( TEXT( "ScriptEngine.ScriptEngine" ), TEXT( "DirBinary" ) ).GetString();
	std::vector< ConfigValue >		modules = GEngineConfig.GetValue( TEXT( "ScriptEngine.ScriptEngine" ), TEXT( "LoadingModules" ) ).GetArray();
	ScriptByteCode					scriptByteCode;

	for ( uint32 index = 0, count = ( uint32 ) modules.size(); index < count; ++index )
	{
		std::wstring			name = modules[ index ].GetString();
		LE_LOG( LT_Log, LC_Init, TEXT( "Loading script module %s" ), name.c_str() );

		BaseArchive*			arByteCode = GFileSystem->CreateFileReader( String::Format( TEXT( "%s/%s.bin" ), scriptPath.c_str(), name.c_str() ), AR_NoFail );
		scriptByteCode.SetArchive( arByteCode );

		asIScriptModule*		module = asScriptEngine->GetModule( TCHAR_TO_ANSI( name.c_str() ), asGM_ALWAYS_CREATE );
		check( module );

		int32	result = module->LoadByteCode( &scriptByteCode );
		check( result >= 0 );
		delete arByteCode;
	}
}

#if WITH_EDITOR
/**
 * Register name of type from AngelScript to C++
 */
void ScriptEngine::RegisterTypeASToCPP( const std::string& InASType, const std::string& InCPPType )
{
	asITypeInfo*		typeInfo = asScriptEngine->GetTypeInfoByDecl( InASType.c_str() );
	check( typeInfo );

	tableTypesASToCPP[ typeInfo->GetTypeId() ] = InCPPType;
}

/**
 * Compile scripts and generate headers for C++
 */
void ScriptEngine::Make( const tchar* InCmdLine )
{
	CScriptBuilder					asScriptBuilder;
	std::wstring					outputPath = GEngineConfig.GetValue( TEXT( "ScriptEngine.ScriptEngine" ), TEXT( "DirBinary" ) ).GetString();
	std::vector< ConfigValue >		modules = GEditorConfig.GetValue( TEXT( "ScriptEngine.Make" ), TEXT( "Modules" ) ).GetArray();

	LE_LOG( LT_Log, LC_Script, TEXT( "" ) );
	for ( uint32 index = 0, count = ( uint32 )modules.size(); index < count; ++index )
	{
		const ConfigObject&			module = modules[ index ].GetObject();
		std::wstring				name = module.GetValue( TEXT( "Name" ) ).GetString();
		std::wstring				sources = module.GetValue( TEXT( "Sources" ) ).GetString();
		std::wstring				outputCPPHeaders = module.GetValue( TEXT( "OutputCPPHeader" ) ).GetString();

		LE_LOG( LT_Log, LC_Script, TEXT( "--------------- %s ---------------" ), name.c_str() );
		
		CompileModule( &asScriptBuilder, name.c_str(), sources.c_str(), outputPath.c_str() );
		GenerateHeadersForModule( asScriptEngine->GetModule( TCHAR_TO_ANSI( name.c_str() ) ), outputCPPHeaders.c_str() );
		
		LE_LOG( LT_Log, LC_Script, TEXT( "" ) );
	}
}

/**
 * Compile module script
 */
void ScriptEngine::CompileModule( class CScriptBuilder* InScriptBuilder, const tchar* InNameModule, const tchar* InPathToModuleDir, const tchar* InOutputPath )
{
	check( InScriptBuilder && InNameModule && InPathToModuleDir && InOutputPath );
	LE_LOG( LT_Log, LC_Script, TEXT( "Compiling script module %s" ), InNameModule );
	
	InScriptBuilder->StartNewModule( asScriptEngine, TCHAR_TO_ANSI( InNameModule ) );
	
	BaseArchive*		arTestScript = GFileSystem->CreateFileReader( String::Format( TEXT( "%s/Core.as" ), InPathToModuleDir ), AR_NoFail );
	uint32				arSize = arTestScript->GetSize() + 1;
	byte*				buffer = new byte[ arSize + 1 ];
	memset( buffer, '\0', arSize );

	arTestScript->Serialize( buffer, arSize );

	int32		result = InScriptBuilder->AddSectionFromMemory( "Core", ( achar* )buffer, arSize-1 );
	check( result >= 0 );

	result = InScriptBuilder->BuildModule();
	check( result >= 0 );

	delete[] buffer;
	delete arTestScript;

	asIScriptModule*	module = asScriptEngine->GetModule( TCHAR_TO_ANSI( InNameModule ) );
	check( module );

	// Save compiled module to cache
	BaseArchive*		arByteCode = GFileSystem->CreateFileWriter( String::Format( TEXT( "%s/%s.bin" ), InOutputPath, InNameModule ), AW_NoFail );
	ScriptByteCode		scriptByteCode;
	scriptByteCode.SetArchive( arByteCode );
	
	result = module->SaveByteCode( &scriptByteCode );
	check( result >= 0 );

	delete arByteCode;
}

/**
 * Generate C++ headers for module
 */
void ScriptEngine::GenerateHeadersForModule( class asIScriptModule* InScriptModule, const tchar* InOutputPath )
{
	check( InScriptModule && InOutputPath );
	LE_LOG( LT_Log, LC_Script, TEXT( "Generate CPP headers for script module %s" ), ANSI_TO_TCHAR( InScriptModule->GetName() ) );

	BaseArchive*			arCPPHeader = GFileSystem->CreateFileWriter( String::Format( TEXT( "%s/%sClasses.h" ), InOutputPath, ANSI_TO_TCHAR( InScriptModule->GetName() ) ), AW_NoFail );

	*arCPPHeader << "/*=========================================\n";
	*arCPPHeader << " C++ class definitions exported from AngelScript.\n";
	*arCPPHeader << " This is automatically generated by the tools.\n";
	*arCPPHeader << " DO NOT modify this manually! Edit the corresponding .as files instead!\n";
	*arCPPHeader << " BSOD-Games, All Rights Reserved.\n";
	*arCPPHeader << "==========================================*/\n\n";

	*arCPPHeader << "#include <angelscript.h>\n\n";

	uint32			coutFunctions = InScriptModule->GetFunctionCount();
	for ( uint32 indexFunction = 0; indexFunction < coutFunctions; ++indexFunction )
	{
		*arCPPHeader << ( achar* )GenerateCPPFunction( InScriptModule, indexFunction ).c_str() << "\n";

		if ( indexFunction + 1 < coutFunctions )
		{
			*arCPPHeader << "\n";
		}
	}

	delete arCPPHeader;
}

/**
 * Generate C++ code of function
 */
std::string ScriptEngine::GenerateCPPFunction( class asIScriptModule* InScriptModule, uint32 InIndexFunction )
{
	check( InScriptModule && InIndexFunction >= 0 && InIndexFunction < InScriptModule->GetFunctionCount() );

	asIScriptFunction*		function = InScriptModule->GetFunctionByIndex( InIndexFunction );
	check( function );

	// Get return type of function
	std::stringstream		strStream;	
	asDWORD					retTypeFlags = 0;
	int32					retTypeId = function->GetReturnTypeId( &retTypeFlags );

	// Get all params in function
	std::vector< SCPPParam >		params;
	std::string						strParamsDeclaration;

	for ( uint32 indexParam = 0, countParams = function->GetParamCount(); indexParam < countParams; ++indexParam )
	{
		SCPPParam		cppParam = GetCPPParamFromFunction( function, indexParam );
		params.push_back( cppParam );

		// Generate declaration of params for header function
		strParamsDeclaration += cppParam.ToString();	
		if ( indexParam + 1 < countParams )
		{
			strParamsDeclaration += ", ";
		}	
	}

	// Generate header of function
	// Example: float execTest( float InA )
	strStream << TypeIDToString( retTypeId ) << " exec" << function->GetName() << ( params.empty() ? "()" : ( "( " + strParamsDeclaration + " )" ) ) << "\n";
	strStream << "{\n";

	// Generate prepare to execute script function
	strStream << "\tasIScriptContext*		scriptContext = GScriptEngine->GetASScriptEngine()->CreateContext();\n";
	strStream << "\tcheck( scriptContext );\n\n";

	strStream << "\tasIScriptFunction*		function = GScriptEngine->GetASScriptEngine()->GetModule( \"" << function->GetModuleName() << "\" )->GetFunctionByIndex( " << InIndexFunction << " );\n";
	strStream << "\tcheck( function );\n\n";

	strStream << "\tint32	result = scriptContext->Prepare( function );\n";
	strStream << "\tcheck( result >= 0 );\n\n";

	// Generate code for set arguments for start function
	for ( uint32 indexParam = 0, countParams = ( uint32 )params.size(); indexParam < countParams; ++indexParam )
	{
		const SCPPParam&		cppParam = params[ indexParam ];
		std::string				callMethod;

		switch ( cppParam.typeID )
		{
			// Int8/UInt8 and Bool
		case asTYPEID_INT8:
		case asTYPEID_UINT8:
		case asTYPEID_BOOL:
			callMethod = "SetArgByte( " + std::to_string( indexParam ) + ", " + cppParam.name + " )";
			break;

			// Int16 and UInt16
		case asTYPEID_INT16:
		case asTYPEID_UINT16:
			callMethod = "SetArgWord( " + std::to_string( indexParam ) + ", " + cppParam.name + " )";
			break;

			// Int32 and UInt32
		case asTYPEID_INT32:
		case asTYPEID_UINT32:
			callMethod = "SetArgDWord( " + std::to_string( indexParam ) + ", " + cppParam.name + " )";
			break;

			// Int64 and UInt64
		case asTYPEID_INT64:
		case asTYPEID_UINT64:
			callMethod = "SetArgQWord( " + std::to_string( indexParam ) + ", " + cppParam.name + " )";
			break;

			// Float
		case asTYPEID_FLOAT:
			callMethod = "SetArgFloat( " + std::to_string( indexParam ) + ", " + cppParam.name + " )";
			break;

			// Double
		case asTYPEID_DOUBLE:
			callMethod = "SetArgDouble( " + std::to_string( indexParam ) + ", " + cppParam.name + " )";
			break;

		default:
			// If is C++ type
			if ( cppParam.typeID & asTYPEID_APPOBJECT )
			{
				callMethod = "SetArgObject( " + std::to_string( indexParam ) + ", &" + cppParam.name + " )";
				break;
			}

			// Else unsupported type
			appErrorf( TEXT( "AngelScript type 0x%X in param %s not supported" ), cppParam.typeID, ANSI_TO_TCHAR( cppParam.name.c_str() ) );
			break;
		}

		strStream << "\tscriptContext->" << callMethod << ";\n";

		if ( indexParam + 1 == countParams )
		{
			strStream << "\n";
		}
	}

	// Execute script function
	strStream << "\tresult = scriptContext->Execute();\n";
	strStream << "\tcheck( result >= 0 );\n";

	// Return value from function after execute
	if ( retTypeId != asTYPEID_VOID )
	{
		strStream << "\n";
		strStream << "\t" << TypeIDToString( retTypeId ) << "\t\treturnValue = ";
	}

	switch ( retTypeId )
	{
		// Nothing return
	case asTYPEID_VOID:
		break;

		// Return bool or int8 or uint8
	case asTYPEID_BOOL:
	case asTYPEID_INT8:
	case asTYPEID_UINT8:
		strStream << "scriptContext->GetReturnByte();\n";
		break;

		// Return int16 or uint16
	case asTYPEID_INT16:
	case asTYPEID_UINT16:
		strStream << "scriptContext->GetReturnWord();\n";
		break;

		// Return int32 or uint32
	case asTYPEID_INT32:
	case asTYPEID_UINT32:
		strStream << "scriptContext->GetReturnDWord();\n";
		break;

		// Return int64 or uint64
	case asTYPEID_INT64:
	case asTYPEID_UINT64:
		strStream << "scriptContext->GetReturnQWord();\n";
		break;

		// Return float
	case asTYPEID_FLOAT:
		strStream << "scriptContext->GetReturnFloat();\n";
		break;

		// Return double
	case asTYPEID_DOUBLE:
		strStream << "scriptContext->GetReturnDouble();\n";
		break;

	default:
		// If is C++ type
		if ( retTypeId & asTYPEID_APPOBJECT )
		{
			strStream << "*( ( " << TypeIDToString( retTypeId ) << "* )scriptContext->GetReturnObject() );\n";
			break;
		}

		// Else unsupported type
		appErrorf( TEXT( "Not supported AngelScript type 0x%X in function %s::%s" ), retTypeId, ANSI_TO_TCHAR( function->GetModuleName() ), ANSI_TO_TCHAR( function->GetName() ) );
		break;
	}

	// End of function
	strStream << "\tscriptContext->Release();\n";
	
	if ( retTypeId != asTYPEID_VOID )
	{
		strStream << "\treturn returnValue;\n";
	}

	strStream << "}";

	return strStream.str();
}

/**
 * Get C++ param from function
 */
ScriptEngine::SCPPParam ScriptEngine::GetCPPParamFromFunction( class asIScriptFunction* InScriptFunction, uint32 InIndexParam ) const
{
	check( InScriptFunction && InIndexParam >= 0 && InIndexParam < InScriptFunction->GetParamCount() );

	int32			typeId = 0;
	asDWORD			flags = 0;
	const char*		name = "";
	const char*		defaultArg = "";

	int32			result = InScriptFunction->GetParam( InIndexParam, &typeId, &flags, &name, &defaultArg );
	check( result >= 0 );

	asITypeInfo* a = asScriptEngine->GetTypeInfoById( typeId );
	if ( a )
	{
		const char* q = a->GetName();
		q++;
	}

	return SCPPParam{ name, typeId };
}

/**
 * Convert AngelScript type ID to C++ name
 */
std::string ScriptEngine::TypeIDToString( int32 InTypeID )
{
	// Standart types
	if ( InTypeID >= 0 && InTypeID <= 11 )
	{
		static const char*			nameCPPType[] =
		{
			"void",			// CPPT_Void
			"bool",			// CPPT_Bool
			"int8",			// CPPT_Int8
			"int16",		// CPPT_Int16
			"int32",		// CPPT_Int32
			"int64",		// CPPT_Int64
			"uint8",		// CPPT_UInt8
			"uint16",		// CPPT_UInt16
			"uint32",		// CPPT_UInt32
			"uint64",		// CPPT_UInt64
			"float",		// CPPT_Float
			"double"		// CPPT_Double
		};

		return nameCPPType[ InTypeID ];
	}

	// C++ user types
	else if ( InTypeID & asTYPEID_APPOBJECT )
	{
		return GScriptEngine->tableTypesASToCPP[ InTypeID ];
	}

	appErrorf( TEXT( "Unknown type 0x%X of AngelScript" ), InTypeID );
	return "Unknown";
}
#endif // WITH_EDITOR