// STL
#include <regex>

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

		// If enum in other module - skip
		if ( strcmp( typeInfo->GetModule()->GetName(), InScriptModule->GetName() ) )
		{
			continue;
		}

		tableTypesASToCPP[ typeInfo->GetTypeId() ] = SASType{ typeInfo->GetName(), typeInfo };
	}

	// Register typedefs
	uint32		countTypedef = InScriptModule->GetTypedefCount();
	for ( uint32 indexTypedef = 0; indexTypedef < countTypedef; ++indexTypedef )
	{
		asITypeInfo*		typeInfo = InScriptModule->GetTypedefByIndex( indexTypedef );
		check( typeInfo );

		tableTypesASToCPP[ typeInfo->GetTypeId() ] = SASType{ typeInfo->GetName(), typeInfo };
	}

	// Register script classes
	uint32		countClasses = InScriptModule->GetObjectTypeCount();
	for ( uint32 indexClass = 0; indexClass < countClasses; ++indexClass )
	{
		asITypeInfo*		typeInfo = InScriptModule->GetObjectTypeByIndex( indexClass );
		check( typeInfo );
		
		// If class in other module - skip
		if ( strcmp( typeInfo->GetModule()->GetName(), InScriptModule->GetName() ) )
		{
			continue;
		}

		tableTypesASToCPP[ typeInfo->GetTypeId() ] = SASType{ std::string( "O" ) + typeInfo->GetName(), typeInfo };
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

	for ( uint32 index = 0, count = ( uint32 )sources.size(); index < count; ++index )
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
	ModuleInitDesc			moduleInitDesc;

	*arCPPHeader << "/*=========================================\n";
	*arCPPHeader << " C++ class definitions exported from AngelScript.\n";
	*arCPPHeader << " This is automatically generated by the tools.\n";
	*arCPPHeader << " DO NOT modify this manually! Edit the corresponding .as files instead!\n";
	*arCPPHeader << " BSOD-Games, All Rights Reserved.\n";
	*arCPPHeader << "==========================================*/\n\n";

	*arCPPHeader << "#pragma once\n\n";

	*arCPPHeader << "#include <angelscript.h>\n";
	*arCPPHeader << "#include \"Scripts/ScriptVar.h\"\n";
	*arCPPHeader << "#include \"Scripts/ScriptObject.h\"\n\n";

	// Write typedefs
	*arCPPHeader << "// ----------------------------------\n";
	*arCPPHeader << "// TYPEDEFS\n";
	*arCPPHeader << "// ----------------------------------\n\n";

	uint32			countTypedefs = InScriptModule->GetTypedefCount();
	if ( countTypedefs > 0 )
	{
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
	*arCPPHeader << "// ----------------------------------\n";
	*arCPPHeader << "// ENUMS\n";
	*arCPPHeader << "// ----------------------------------\n\n";

	uint32			countEnums = InScriptModule->GetEnumCount();
	if ( countEnums > 0 )
	{
		uint32		countEnumsInHeader = 0;
		for ( uint32 indexEnum = 0; indexEnum < countEnums; ++indexEnum )
		{
			asITypeInfo*		typeInfo = InScriptModule->GetEnumByIndex( indexEnum );
			check( typeInfo );

			// If enum in other module - skip
			if ( strcmp( typeInfo->GetModule()->GetName(), InScriptModule->GetName() ) )
			{
				continue;
			}

			*arCPPHeader << ( achar* )GenerateCPPEnum( typeInfo, indexEnum ).c_str() << "\n";
			++countEnumsInHeader;

			if ( indexEnum + 1 < countEnums )
			{
				*arCPPHeader << "\n";
			}
		}

		if ( countEnumsInHeader > 0 )
		{
			*arCPPHeader << "\n";
		}
	}

	// Write functions
	*arCPPHeader << "// ----------------------------------\n";
	*arCPPHeader << "// FUNCTIONS\n";
	*arCPPHeader << "// ----------------------------------\n\n";

	uint32			countFunctions = InScriptModule->GetFunctionCount();
	if ( countFunctions > 0 )
	{
		uint32			countFunctionsInHeader = 0;
		for ( uint32 indexFunction = 0; indexFunction < countFunctions; ++indexFunction )
		{
			asIScriptFunction*		function = InScriptModule->GetFunctionByIndex( indexFunction );
			check( function );

			// If function in other module - skip
			if ( strcmp( function->GetModule()->GetName(), InScriptModule->GetName() ) )
			{
				continue;
			}

			*arCPPHeader << ( achar* )GenerateCPPFunction( function, indexFunction ).c_str() << "\n";
			++countFunctionsInHeader;

			if ( indexFunction + 1 < countFunctions )
			{
				*arCPPHeader << "\n";
			}
		}

		if ( countFunctionsInHeader > 0 )
		{
			*arCPPHeader << "\n";
		}
	}

	// Write classes
	*arCPPHeader << "// ----------------------------------\n";
	*arCPPHeader << "// CLASSES\n";
	*arCPPHeader << "// ----------------------------------\n\n";

	uint32		countClasses = InScriptModule->GetObjectTypeCount();
	if ( countClasses > 0 )
	{
		uint32			countClassesInHeader = 0;
		for ( uint32 indexClass = 0; indexClass < countClasses; ++indexClass )
		{
			asITypeInfo*		typeInfo = InScriptModule->GetObjectTypeByIndex( indexClass );
			check( typeInfo );

			// If class in other module - skip
			if ( strcmp( typeInfo->GetModule()->GetName(), InScriptModule->GetName() ) )
			{
				continue;
			}

			*arCPPHeader << ( achar* )GenerateCPPClass( typeInfo, indexClass ).c_str() << "\n";
			++countClassesInHeader;

			if ( indexClass + 1 < countClasses )
			{
				*arCPPHeader << "\n";
			}
		}

		if ( countClassesInHeader > 0 )
		{
			*arCPPHeader << "\n";
		}
	}

	// Write global values
	*arCPPHeader << "// ----------------------------------\n";
	*arCPPHeader << "// GLOBAL VALUES\n";
	*arCPPHeader << "// ----------------------------------\n\n";

	uint32			countGlobalValues = InScriptModule->GetGlobalVarCount();
	if ( countGlobalValues > 0 )
	{
		for ( uint32 indexGlobalValue = 0; indexGlobalValue < countGlobalValues; ++indexGlobalValue )
		{
			*arCPPHeader << "extern " << ( achar* )GenerateCPPGlobalValue( InScriptModule, indexGlobalValue, &moduleInitDesc ).c_str() << "\n";
		}

		*arCPPHeader << "\n";
	}

	// Generate initialization macros and functions
	*arCPPHeader << "// ----------------------------------\n";
	*arCPPHeader << "// INITIALIZATION MACROS\n";
	*arCPPHeader << "// ----------------------------------\n\n";

	*arCPPHeader << "#define DECLARATE_GLOBALVALUES_SCRIPTMODULE_" << ( achar* )InScriptModule->GetName(); 
	
	if ( !moduleInitDesc.externDefine.empty() )
	{
		*arCPPHeader << " \\\n";
		for ( uint32 indexExtern = 0, countExterns = ( uint32 )moduleInitDesc.externDefine.size(); indexExtern < countExterns; ++indexExtern )
		{
			*arCPPHeader << "\t" << ( achar* )moduleInitDesc.externDefine[ indexExtern ].c_str() << ";";

			if ( indexExtern + 1 < countExterns )
			{
				*arCPPHeader << " \\\n";
			}
		}
	}
	*arCPPHeader << "\n";

	*arCPPHeader << "\n";
	*arCPPHeader << "// ----------------------------------\n";
	*arCPPHeader << "// INITIALIZATION FUNCTION\n";
	*arCPPHeader << "// ----------------------------------\n\n";
	*arCPPHeader << "void InitScriptModule_"<< ( achar* )InScriptModule->GetName() << "()\n";
	*arCPPHeader << "{\n";

	if ( !moduleInitDesc.globalVarInit.empty() )
	{
		for ( uint32 indexGlobalVar = 0, countGlobalVars = ( uint32 ) moduleInitDesc.globalVarInit.size(); indexGlobalVar < countGlobalVars; ++indexGlobalVar )
		{
			*arCPPHeader << "\t" << ( achar* ) moduleInitDesc.globalVarInit[ indexGlobalVar ].c_str() << ";\n";
		}
	}

	*arCPPHeader << "}";

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
	strStream << "typedef " << TypeIDToString( typeInfo->GetTypedefTypeId() ) << " " << typeInfo->GetName() << ";";
	return strStream.str();
}

/**
 * Generate C++ code of enum
 */
std::string ScriptEngine::GenerateCPPEnum( class asITypeInfo* InEnumType, uint32 InIndexEnum )
{
	std::stringstream	strStream;
	strStream << "enum " << InEnumType->GetName() << "\n";
	strStream << "{\n";
	
	uint32			valueCount = InEnumType->GetEnumValueCount();
	for ( uint32 indexValue = 0; indexValue < valueCount; ++indexValue )
	{
		int32			value = 0;
		const achar*	name = InEnumType->GetEnumValueByIndex( indexValue, &value );
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
 * Generate C++ code of global value
 */
std::string ScriptEngine::GenerateCPPGlobalValue( class asIScriptModule* InScriptModule, uint32 InIndexGlobalValue, ModuleInitDesc* InOutModuleInitDesc )
{
	check( InScriptModule && InIndexGlobalValue >= 0 && InIndexGlobalValue < InScriptModule->GetGlobalVarCount() );
	
	const achar*		name = nullptr;
	const achar*		nameSpace = nullptr;
	int32				typeId = 0;
	bool				isConst = false;
	int32				result = InScriptModule->GetGlobalVar( InIndexGlobalValue, &name, &nameSpace, &typeId, &isConst );
	check( result >= 0 );

	std::stringstream		strStream;
	strStream << "ScriptVar< " << TypeIDToString( typeId, isConst ? asTM_CONST : 0 ) << " > " << name;
	
	// If InOutModuleInitDesc not nullptr - we write info for generate macros and functions for initialize module
	if ( InOutModuleInitDesc )
	{
		InOutModuleInitDesc->externDefine.push_back( strStream.str() );
		InOutModuleInitDesc->globalVarInit.push_back( std::string( name ) + ".Init( " + std::to_string( InIndexGlobalValue ) + ", TEXT( \"" + InScriptModule->GetName() + "\" ) )" );
	}
	
	strStream << ";";
	return strStream.str();
}

/**
 * Get C++ code for set arg of function/constructor/method
 */
std::string ScriptEngine::GetCallMethodOfScriptContext( const SCPPParam& InParam )
{
	std::string				callMethod;

	// If parameter is referenced - need set arg by address
	if ( InParam.flags & asTM_INREF || InParam.flags & asTM_OUTREF || InParam.flags & asTM_INOUTREF )
	{
		callMethod = "SetArgAddress( " + std::to_string( InParam.id ) + ", ( void* ) &" + InParam.name + " )";
	}
	else
	{
		switch ( InParam.typeID )
		{
			// Int8/UInt8 and Bool
		case asTYPEID_INT8:
		case asTYPEID_UINT8:
		case asTYPEID_BOOL:
			callMethod = "SetArgByte( " + std::to_string( InParam.id ) + ", " + InParam.name + " )";
			break;

			// Int16 and UInt16
		case asTYPEID_INT16:
		case asTYPEID_UINT16:
			callMethod = "SetArgWord( " + std::to_string( InParam.id ) + ", " + InParam.name + " )";
			break;

			// Int32 and UInt32
		case asTYPEID_INT32:
		case asTYPEID_UINT32:
			callMethod = "SetArgDWord( " + std::to_string( InParam.id ) + ", " + InParam.name + " )";
			break;

			// Int64 and UInt64
		case asTYPEID_INT64:
		case asTYPEID_UINT64:
			callMethod = "SetArgQWord( " + std::to_string( InParam.id ) + ", " + InParam.name + " )";
			break;

			// Float
		case asTYPEID_FLOAT:
			callMethod = "SetArgFloat( " + std::to_string( InParam.id ) + ", " + InParam.name + " )";
			break;

			// Double
		case asTYPEID_DOUBLE:
			callMethod = "SetArgDouble( " + std::to_string( InParam.id ) + ", " + InParam.name + " )";
			break;

			// C++ and AngelScript user types
		default:
			auto		itType = tableTypesASToCPP.find( InParam.typeID );
			if ( itType == tableTypesASToCPP.end() )
			{
				appErrorf( TEXT( "Unknown AngelScript type 0x%X in param %s" ), InParam.typeID, ANSI_TO_TCHAR( InParam.name.c_str() ) );
				break;
			}

			asITypeInfo* typeInfo = itType->second.asTypeInfo;
			check( typeInfo );
			int32				flags = typeInfo->GetFlags();

			// Objects
			if ( flags & asOBJ_VALUE )
			{
				callMethod = "SetArgObject( " + std::to_string( InParam.id ) + ", &" + InParam.name + " )";
			}

			// Enums
			else if ( flags & asOBJ_ENUM )
			{
				callMethod = "SetArgDWord( " + std::to_string( InParam.id ) + ", " + InParam.name + " )";
			}

			// Else unsupported type
			else
			{
				appErrorf( TEXT( "AngelScript type 0x%X in param %s not supported" ), InParam.typeID, ANSI_TO_TCHAR( InParam.name.c_str() ) );
			}

			break;
		}
	}

	return callMethod;
}

/**
 * Generate C++ code of function
 */
std::string ScriptEngine::GenerateCPPFunction( class asIScriptFunction* InScriptFunction, uint32 InIndexFunction, bool InIsMethod /* = false */ )
{
	check( InScriptFunction );

	// Get return type of function
	std::stringstream		strStream;	
	asDWORD					retTypeFlags = 0;
	int32					retTypeId = InScriptFunction->GetReturnTypeId( &retTypeFlags );

	// Get all params in function
	std::vector< SCPPParam >		params;
	std::string						strParamsDeclaration;

	for ( uint32 indexParam = 0, countParams = InScriptFunction->GetParamCount(); indexParam < countParams; ++indexParam )
	{
		SCPPParam		cppParam = GetCPPParamFromFunction( InScriptFunction, indexParam );
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
	strStream << TypeIDToString( retTypeId ) << " exec" << InScriptFunction->GetName() << ( params.empty() ? "()" : ( "( " + strParamsDeclaration + " )" ) ) << "\n";
	strStream << "{\n";

	// Generate prepare to execute script function
	strStream << "\tasIScriptEngine*		scriptEngine = GScriptEngine->GetASScriptEngine();\n";
	strStream << "\tasIScriptContext*		scriptContext = scriptEngine->CreateContext();\n";
	strStream << "\tcheck( scriptContext );\n\n";

	strStream << "\tasIScriptFunction*		function = ";

	// If this function
	if ( !InIsMethod )
	{
		strStream << "scriptEngine->GetModule( \"" << InScriptFunction->GetModuleName() << "\" )->GetFunctionByIndex( " << InIndexFunction << " );\n";
	}
	// Else this method of class
	else
	{
		strStream << "typeInfo->GetMethodByIndex( " << InIndexFunction << " );\n";
	}

	strStream << "\tcheck( function );\n\n";

	strStream << "\tint32	result = scriptContext->Prepare( function );\n";
	strStream << "\tcheck( result >= 0 );\n\n";

	// If this method of class need set object
	if ( InIsMethod )
	{
		strStream << "\tresult = scriptContext->SetObject( self );\n";
		strStream << "\tcheck( result >= 0 );\n\n";
	}

	// Generate code for set arguments for start function
	for ( uint32 indexParam = 0, countParams = ( uint32 )params.size(); indexParam < countParams; ++indexParam )
	{
		strStream << "\tscriptContext->" << GetCallMethodOfScriptContext( params[ indexParam ] ) << ";\n";
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
			appErrorf( TEXT( "Unknown returning AngelScript type 0x%X in function %s::%s from module %s" ), retTypeId, ANSI_TO_TCHAR( InScriptFunction->GetObjectName() ), ANSI_TO_TCHAR( InScriptFunction->GetName() ), ANSI_TO_TCHAR( InScriptFunction->GetModuleName() ) );
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
			appErrorf( TEXT( "Not supported AngelScript type 0x%X in function %s::%s from module %s" ), retTypeId, ANSI_TO_TCHAR( InScriptFunction->GetObjectName() ), ANSI_TO_TCHAR( InScriptFunction->GetName() ), ANSI_TO_TCHAR( InScriptFunction->GetModuleName() ) );
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
 * Is script class property inherited from base type
 */
bool IsScriptClassPropertyInherited( asITypeInfo* InClassType, const achar* InPropertyName )
{
	if ( !InClassType )
	{
		return false;
	}

	for ( uint32 indexProperty = 0, countProperties = InClassType->GetPropertyCount(); indexProperty < countProperties; ++indexProperty )
	{
		const achar*		name = nullptr;
		int32				result = InClassType->GetProperty( indexProperty, &name );
		check( result >= 0 );

		// If property not equal InPropertyName - this other value
		if ( strcmp( InPropertyName, name ) )
		{
			continue;
		}

		return true;
	}

	return IsScriptClassPropertyInherited( InClassType->GetBaseType(), InPropertyName );
}

/**
 * Generate C++ code of class
 */
std::string ScriptEngine::GenerateCPPClass( class asITypeInfo* InClassType, uint32 InIndexClass )
{
	struct CPPProperty
	{
		uint32			id;
		std::string		name;
		std::string		declaration;
	};

	check( InClassType );

	std::stringstream				strStream;
	std::vector< CPPProperty >		publicProperties;
	std::vector< CPPProperty >		protectedProperties;
	std::vector< CPPProperty >		privateProperties;
	asITypeInfo*					baseType = InClassType->GetBaseType();

	// If the base type is nullptr means that the class did not inherit from any class - in this case, 
	// the base class will be ScriptObest, otherwise we inherit from the base type in scripts 
	// (the "O" prefix + the name of the base type from the script) 
	std::string						cppClassName = std::string( "O" ) + InClassType->GetName();
	std::string						cppBaseClassName = !baseType ? "ScriptObject" : ( std::string( "O" ) + baseType->GetName() );
	std::string						cppBaseClassCtrArgs = !baseType ? "" : "ScriptObject::NoInit";

	// Getting all properties from script class
	{
		uint32		countProperties = InClassType->GetPropertyCount();
		for ( uint32 indexProperty = 0; indexProperty < countProperties; ++indexProperty )
		{
			const achar*		name = nullptr;
			int32				typeId = 0;
			bool				isPrivate = false;
			bool				isProtected = false;
			int32				result = InClassType->GetProperty( indexProperty, &name, &typeId, &isPrivate, &isProtected );
			check( result >= 0 );

			// Is property inherited from base type - skip
			if ( IsScriptClassPropertyInherited( baseType, name ) )
			{
				continue;
			}

			CPPProperty			cppProperty;
			cppProperty.id = indexProperty;
			cppProperty.name = name;
			cppProperty.declaration = "ScriptVar< " + TypeIDToString( typeId ) + " > " + name;

			if ( !isPrivate && !isProtected )
			{
				publicProperties.push_back( cppProperty );
			}
			else if ( isProtected )
			{
				protectedProperties.push_back( cppProperty );
			}
			else if ( isPrivate )
			{
				privateProperties.push_back( cppProperty );
			}
		}
	}

	strStream << "class " << cppClassName << " : public " << cppBaseClassName << "\n";	
	strStream << "{\n";

	// Insert macros DECLARE_CLASS
	strStream << "\tDECLARE_CLASS( " << cppClassName << ", " << cppBaseClassName << " )\n\n";

	// Generate section of props
	strStream << "\t//## BEGIN PROPS " << cppClassName << "\n";
	{
		// Public properties
		if ( !publicProperties.empty() )
		{
			strStream << "public:\n";
			for ( uint32 indexProperty = 0, countProperty = ( uint32 )publicProperties.size(); indexProperty < countProperty; ++indexProperty )
			{
				strStream << "\t" << publicProperties[ indexProperty ].declaration << ";\n";
			}
		}

		// Protected properties
		if ( !protectedProperties.empty() )
		{
			strStream << "protected:\n";
			for ( uint32 indexProperty = 0, countProperty = ( uint32 )protectedProperties.size(); indexProperty < countProperty; ++indexProperty )
			{
				strStream << "\t" << protectedProperties[ indexProperty ].declaration << ";\n";
			}
		}

		// Private properties
		if ( !privateProperties.empty() )
		{
			strStream << "private:\n";
			for ( uint32 indexProperty = 0, countProperty = ( uint32 )privateProperties.size(); indexProperty < countProperty; ++indexProperty )
			{
				strStream << "\t" << privateProperties[ indexProperty ].declaration << ";\n";
			}
		}
	}
	strStream << "\t//## END PROPS " << cppClassName << "\n\n";

	// Generate constructor
	strStream << "public:\n";
	
	uint32		countConstructors = InClassType->GetFactoryCount();
	for ( uint32 indexConstructor = 0; indexConstructor < countConstructors; ++indexConstructor )
	{
		asIScriptFunction*		function = InClassType->GetFactoryByIndex( indexConstructor );

		// Get all params in constructor
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

		// Generate header of constructor
		// Example: Class( float InA ) : Super( ScriptObject::NoInit )
		strStream << "\t" << cppClassName << "(" << ( strParamsDeclaration.empty() ? "" : " " + strParamsDeclaration + " " ) << ")" << ( !baseType ? "" : " : " + cppBaseClassName + "( " + cppBaseClassCtrArgs + " )" ) << "\n";
		strStream << "\t{\n";

		// Getting script engine, script module and creating script context for executable
		strStream << "\t\tasIScriptEngine*		scriptEngine = GScriptEngine->GetASScriptEngine();\n";
		strStream << "\t\tasIScriptModule*		scriptModule = scriptEngine->GetModule( \"" << InClassType->GetModule()->GetName() << "\" );\n";
		strStream << "\t\tasIScriptContext*		scriptContext = scriptEngine->CreateContext();\n";
		strStream << "\t\tcheck( scriptContext && scriptModule );\n\n";

		// Getting object type
		strStream << "\t\tasITypeInfo*			objectType = scriptModule->GetObjectTypeByIndex( " << InIndexClass << " );\n";
		strStream << "\t\tcheck( objectType );\n\n";

		// Getting factory function
		strStream << "\t\tasIScriptFunction*		factory = objectType->GetFactoryByIndex( " << indexConstructor << " );\n";
		strStream << "\t\tcheck( factory );\n\n";

		strStream << "\t\tint32					result = scriptContext->Prepare( factory );\n";
		strStream << "\t\tcheck( result >= 0 );\n\n";

		// Generate code for set arguments for start constructor
		for ( uint32 indexParam = 0, countParams = ( uint32 ) params.size(); indexParam < countParams; ++indexParam )
		{
			strStream << "\t\tscriptContext->" << GetCallMethodOfScriptContext( params[ indexParam ] ) << ";\n";
			if ( indexParam + 1 == countParams )
			{
				strStream << "\n";
			}
		}

		// Execute constructor
		strStream << "\t\tresult = scriptContext->Execute();\n";
		strStream << "\t\tcheck( result >= 0 );\n\n";

		// Init object
		strStream << "\t\tInit( *( asIScriptObject** )scriptContext->GetAddressOfReturnValue() );\n";
		strStream << "\t}\n\n";
	}

	// Constructor with already created script object
	strStream << "\t" << cppClassName << "( class asIScriptObject* InScriptObject )" << ( !baseType ? "" : " : " + cppBaseClassName + "( " + cppBaseClassCtrArgs + " )" ) << " { Init( InScriptObject ); }\n";

	// Constructor with no init option
	strStream << "\t" << cppClassName << "( ENoInit )" << ( !baseType ? "" : " : " + cppBaseClassName + "( " + cppBaseClassCtrArgs + " )" ) << " {}\n\n";

	// Generate methods
	uint32		countMethods = InClassType->GetMethodCount();
	for ( uint32 indexMethod = 0; indexMethod < countMethods; ++indexMethod )
	{
		// Getting method and generating cpp function with indent 1 tab
		asIScriptFunction*		function = InClassType->GetMethodByIndex( indexMethod );

		// If function in other class - skip
		if ( strcmp( function->GetObjectName(), cppClassName.c_str() + 1 ) )		// Move the pointer by one to skip the "O" prefix
		{
			continue;
		}

		strStream << "\t" << std::regex_replace( GenerateCPPFunction( function, indexMethod, true ), std::regex( "\n" ), "\n\t" ) << "\n\n";
	}

	// Insert in private section method for init class
	strStream << "protected:\n";
	strStream << "\tvoid Init( asIScriptObject* InScriptObject ) override\n";
	strStream << "\t{\n";
	strStream << "\t\tSuper::Init( InScriptObject );\n";

	// Generate code for init properties
	// Public properties
	for ( uint32 indexProperty = 0, countProperties = ( uint32 ) publicProperties.size(); indexProperty < countProperties; ++indexProperty )
	{
		const CPPProperty&			cppProperty = publicProperties[ indexProperty ];
		strStream << "\t\t" << cppProperty.name << ".Init( " << cppProperty.id << ", self );\n";
	}

	// Protected properties
	for ( uint32 indexProperty = 0, countProperties = ( uint32 ) protectedProperties.size(); indexProperty < countProperties; ++indexProperty )
	{
		const CPPProperty&			cppProperty = protectedProperties[ indexProperty ];
		strStream << "\t\t" << cppProperty.name << ".Init( " << cppProperty.id << ", self );\n";
	}

	// Private properties
	for ( uint32 indexProperty = 0, countProperties = ( uint32 ) privateProperties.size(); indexProperty < countProperties; ++indexProperty )
	{
		const CPPProperty&			cppProperty = privateProperties[ indexProperty ];
		strStream << "\t\t" << cppProperty.name << ".Init( " << cppProperty.id << ", self );\n";
	}

	strStream << "\t}\n";
	strStream << "};";
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

	return SCPPParam{ InIndexParam, name ? name : std::string( "Param" ) + std::to_string( InIndexParam ), typeId, flags };
}

/**
 * Convert AngelScript type ID to C++ name
 */
std::string ScriptEngine::TypeIDToString( int32 InTypeID, int64 InFlags )
{
	bool				isConst = InFlags & asTM_CONST;
	bool				isReference = InFlags & asTM_INREF || InFlags & asTM_OUTREF || InFlags & asTM_INOUTREF;
	std::string			nameType;

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

		nameType = nameCPPType[ InTypeID ];
	}

	// C++ user types
	else if ( InTypeID & asTYPEID_APPOBJECT )
	{
		nameType = GScriptEngine->tableTypesASToCPP[ InTypeID ].cppName;
	}

	// AngelScript user types
	else
	{
		auto		itType = GScriptEngine->tableTypesASToCPP.find( InTypeID );
		if ( itType != GScriptEngine->tableTypesASToCPP.end() )
		{
			nameType = itType->second.cppName;
		}
	}

	if ( nameType.empty() )
	{
		appErrorf( TEXT( "Unknown type 0x%X of AngelScript" ), InTypeID );
		return "Unknown";
	}

	return TCHAR_TO_ANSI( String::Format( TEXT( "%s%s%s" ),					// Format: <const><typeName><&>
						  isConst ? TEXT( "const " ) : TEXT( "" ),			// Const modifier						  
						  ANSI_TO_TCHAR( nameType.c_str() ),				// Type name
						  isReference ? TEXT( "&" ) : TEXT( "" ) ) );		// Reference modifier		
}
#endif // WITH_EDITOR