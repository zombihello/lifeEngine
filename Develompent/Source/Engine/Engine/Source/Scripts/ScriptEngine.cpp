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

	// Register native type for AngelScript
	RegisterNativeTypes();
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

/**
 * Register native types for AngelScript
 */
void ScriptEngine::RegisterNativeTypes()
{
	// Register std strings in scripts
	RegisterStdString( asScriptEngine );

	// Register native types for generate CPP headers
#if WITH_EDITOR
	// std::string
	asITypeInfo*		typeInfo = asScriptEngine->GetTypeInfoByDecl( "string" );
	check( typeInfo );

	tableTypesASToCPP[ typeInfo->GetTypeId() ] = SASType{ "std::string", typeInfo };
#endif // WITH_EDITOR
}

#if WITH_EDITOR
/**
 * Register name of types from AngelScript to C++
 */
void ScriptEngine::RegisterTypesASToCPP( class asIScriptModule* InScriptModule )
{
	check( InScriptModule );

	// Register enums
	uint32		countEnums = InScriptModule->GetEnumCount();
	for ( uint32 indexEnum = 0; indexEnum < countEnums; ++indexEnum )
	{
		asITypeInfo*		typeInfo = InScriptModule->GetEnumByIndex( indexEnum );
		check( typeInfo );

		tableTypesASToCPP[ typeInfo->GetTypeId() ] = SASType{ typeInfo->GetName(), typeInfo };
	}

	// Register typedefs
	uint32		countTypedef = InScriptModule->GetTypedefCount();
	for ( uint32 indexTypedef = 0; indexTypedef < countTypedef; ++indexTypedef )
	{
		asITypeInfo*		typeInfo =InScriptModule->GetTypedefByIndex( indexTypedef );
		check( typeInfo );

		tableTypesASToCPP[ typeInfo->GetTypeId() ] = SASType{ typeInfo->GetName(), typeInfo };
	}
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
		
		asIScriptModule*			scriptModule = asScriptEngine->GetModule( TCHAR_TO_ANSI( name.c_str() ) );
		check( scriptModule );

		RegisterTypesASToCPP( scriptModule );
		GenerateHeadersForModule( scriptModule, outputCPPHeaders.c_str() );	
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
	
	std::vector< std::wstring >			sources = GFileSystem->FindFiles( InPathToModuleDir, true, false );
	check( !sources.empty() );

	for ( uint32 index = 0, count = ( uint32 ) sources.size(); index < count; ++index )
	{
		const std::wstring&		file = sources[ index ];

		BaseArchive*			arArchive = GFileSystem->CreateFileReader( String::Format( TEXT( "%s/%s" ), InPathToModuleDir, file.c_str() ), AR_NoFail );
		uint32					arSize = arArchive->GetSize() + 1;
		byte*					buffer = new byte[ arSize + 1 ];
		memset( buffer, '\0', arSize );

		arArchive->Serialize( buffer, arSize );

		int32		result = InScriptBuilder->AddSectionFromMemory( TCHAR_TO_ANSI( file.c_str() ), ( achar* )buffer, arSize-1 );
		check( result >= 0 );

		delete[] buffer;
		delete arArchive;
	}

	// Compile module
	int32		result = InScriptBuilder->BuildModule();
	check( result >= 0 );

	// Save compiled module to cache
	asIScriptModule*	module = asScriptEngine->GetModule( TCHAR_TO_ANSI( InNameModule ) );
	check( module );
	
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

	// Write typedefs
	uint32			countTypedefs = InScriptModule->GetTypedefCount();
	if ( countTypedefs > 0 )
	{
		*arCPPHeader << "// ----------------------------------\n";
		*arCPPHeader << "// TYPEDEFS\n";
		*arCPPHeader << "// ----------------------------------\n\n";

		for ( uint32 indexTypedef = 0; indexTypedef < countTypedefs; ++indexTypedef )
		{
			*arCPPHeader << ( achar* )GenerateCPPTypedef( InScriptModule, indexTypedef ).c_str() << "\n";

			if ( indexTypedef + 1 < countTypedefs )
			{
				*arCPPHeader << "\n";
			}
		}

		*arCPPHeader << "\n";
	}

	// Write enums
	uint32			countEnums = InScriptModule->GetEnumCount();
	if ( countEnums > 0 )
	{
		*arCPPHeader << "// ----------------------------------\n";
		*arCPPHeader << "// ENUMS\n";
		*arCPPHeader << "// ----------------------------------\n\n";

		for ( uint32 indexEnum = 0; indexEnum < countEnums; ++indexEnum )
		{
			*arCPPHeader << ( achar* )GenerateCPPEnum( InScriptModule, indexEnum ).c_str() << "\n";

			if ( indexEnum + 1 < countEnums )
			{
				*arCPPHeader << "\n";
			}
		}

		*arCPPHeader << "\n";
	}

	// Write functions
	uint32			coutFunctions = InScriptModule->GetFunctionCount();
	if ( coutFunctions > 0 )
	{
		*arCPPHeader << "// ----------------------------------\n";
		*arCPPHeader << "// FUNCTIONS\n";
		*arCPPHeader << "// ----------------------------------\n\n";

		for ( uint32 indexFunction = 0; indexFunction < coutFunctions; ++indexFunction )
		{
			*arCPPHeader << ( achar* ) GenerateCPPFunction( InScriptModule, indexFunction ).c_str() << "\n";

			if ( indexFunction + 1 < coutFunctions )
			{
				*arCPPHeader << "\n";
			}
		}
	}

	delete arCPPHeader;
}

/**
 * Generate C++ code of typedefs
 */
std::string ScriptEngine::GenerateCPPTypedef( class asIScriptModule* InScriptModule, uint32 InIndexTypedef )
{
	check( InScriptModule && InIndexTypedef >= 0 && InIndexTypedef < InScriptModule->GetTypedefCount() );

	asITypeInfo*		typeInfo = InScriptModule->GetTypedefByIndex( InIndexTypedef );
	check( typeInfo );

	std::stringstream		strStream;
	strStream << "typedef " << TypeIDToString( typeInfo->GetTypedefTypeId() ) << "\t\t" << typeInfo->GetName() << ";";
	return strStream.str();
}

/**
 * Generate C++ code of enum
 */
std::string ScriptEngine::GenerateCPPEnum( class asIScriptModule* InScriptModule, uint32 InIndexEnum )
{
	check( InScriptModule && InIndexEnum >= 0 && InIndexEnum < InScriptModule->GetEnumCount() );

	std::stringstream	strStream;
	asITypeInfo*		typeInfo = InScriptModule->GetEnumByIndex( InIndexEnum );
	check( typeInfo );

	strStream << "enum " << typeInfo->GetName() << "\n";
	strStream << "{\n";
	
	uint32			valueCount = typeInfo->GetEnumValueCount();
	for ( uint32 indexValue = 0; indexValue < valueCount; ++indexValue )
	{
		int32			value = 0;
		const achar*	name = typeInfo->GetEnumValueByIndex( indexValue, &value );
		strStream << "\t" << name << "\t\t\t=" << value;

		if ( indexValue + 1 < valueCount )
		{
			strStream << ",\n";
		}
		else
		{
			strStream << "\n";
		}
	}
	
	strStream << "};";
	return strStream.str();
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

			// C++ and AngelScript user types
		default:
			auto		itType = tableTypesASToCPP.find( cppParam.typeID );
			if ( itType == tableTypesASToCPP.end() )
			{
				appErrorf( TEXT( "Unknown AngelScript type 0x%X in param %s" ), cppParam.typeID, ANSI_TO_TCHAR( cppParam.name.c_str() ) );
				break;
			}

			asITypeInfo*		typeInfo = itType->second.asTypeInfo;
			check( typeInfo );
			int32				flags = typeInfo->GetFlags();

			// Objects
			if ( flags & asOBJ_VALUE )
			{
				callMethod = "SetArgObject( " + std::to_string( indexParam ) + ", &" + cppParam.name + " )";
			}

			// Enums
			else if ( flags & asOBJ_ENUM )
			{
				callMethod = "SetArgDWord( " + std::to_string( indexParam ) + ", " + cppParam.name + " )";
			}

			// Else unsupported type
			else
			{
				appErrorf( TEXT( "AngelScript type 0x%X in param %s not supported" ), cppParam.typeID, ANSI_TO_TCHAR( cppParam.name.c_str() ) );
			}

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

		// C++ and AngelScript user types
	default:
		auto		itType = tableTypesASToCPP.find( retTypeId );
		if ( itType == tableTypesASToCPP.end() )
		{
			appErrorf( TEXT( "Unknown returning AngelScript type 0x%X in function %s::%s" ), retTypeId, ANSI_TO_TCHAR( function->GetModuleName() ), ANSI_TO_TCHAR( function->GetName() ) );
			break;
		}

		asITypeInfo* typeInfo = itType->second.asTypeInfo;
		check( typeInfo );
		int32				flags = typeInfo->GetFlags();

		// Objects
		if ( flags & asOBJ_VALUE )
		{
			strStream << "*( ( " << TypeIDToString( retTypeId ) << "* )scriptContext->GetReturnObject() );\n";
		}

		// Enums
		else if ( flags & asOBJ_ENUM )
		{
			strStream << "( " << TypeIDToString( retTypeId ) << " )scriptContext->GetReturnDWord();\n";
		}

		// Else unsupported type
		else
		{
			appErrorf( TEXT( "Not supported AngelScript type 0x%X in function %s::%s" ), retTypeId, ANSI_TO_TCHAR( function->GetModuleName() ), ANSI_TO_TCHAR( function->GetName() ) );
		}

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
		return GScriptEngine->tableTypesASToCPP[ InTypeID ].cppName;
	}

	// AngelScript user types
	else
	{
		auto		itType = GScriptEngine->tableTypesASToCPP.find( InTypeID );
		if ( itType != GScriptEngine->tableTypesASToCPP.end() )
		{
			return itType->second.cppName;
		}
	}

	appErrorf( TEXT( "Unknown type 0x%X of AngelScript" ), InTypeID );
	return "Unknown";
}
#endif // WITH_EDITOR