#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Logger/BaseLogger.h"
#include "Logger/LoggerMacros.h"
#include "Containers/String.h"
#include "System/Archive.h"
#include "System/BaseFileSystem.h"
#include "RHI/BaseRHI.h"
#include "Render/Shaders/Shader.h"
#include "Render/Shaders/ShaderManager.h"

#if WITH_EDITOR
	#include "Render/Shaders/ShaderCompiler.h"
#endif // WITH_EDITOR

/**
 * Constructor of FShaderMetaType
 */
FShaderMetaType::FShaderMetaType( const std::wstring& InName, const std::wstring& InFileName, const std::wstring& InFunctionName, EShaderFrequency InFrequency, FConstructSerializedInstance InConstructSerializedInstance, FConstructCompiledInstance InConstructCompiledInstance ) :
	name( InName ),
	fileName( FString::Format( TEXT( "%s/%s" ), FShaderManager::GetShaderDir(), InFileName.c_str() ) ),
	functionName( InFunctionName ),
	frequency( InFrequency ),
	ConstructSerializedInstance( InConstructSerializedInstance ),
	ConstructCompiledInstance( InConstructCompiledInstance )
{
	FShaderManager::RegisterShaderType( this );
}

/**
 * Constructor of copy FShaderMetaType
 */
FShaderMetaType::FShaderMetaType( const FShaderMetaType& InCopy )
{
	name = InCopy.name;
	fileName = InCopy.fileName;
	functionName = InCopy.functionName;
	frequency = InCopy.frequency;
	ConstructSerializedInstance = InCopy.ConstructSerializedInstance;
	ConstructCompiledInstance = InCopy.ConstructCompiledInstance;
}

/**
 * Create instance of shader
 */
FShaderRef FShaderManager::FContainerShaderTypes::CreateShaderInstance( const tchar* InShaderName )
{
	FContainerShaderTypes*		container = FContainerShaderTypes::Get();
	auto		itShaderMetaType = container->shaderMetaTypes.find( InShaderName );
	if ( itShaderMetaType == container->shaderMetaTypes.end() )
	{
		return nullptr;
	}

	return itShaderMetaType->second->CreateSerializedInstace();
}

/**
 * Loading shader from cache
 */
bool FShaderManager::LoadShaders( const tchar* InPathShaderCache )
{
	FArchive*		archive = GFileSystem->CreateFileReader( InPathShaderCache );
	if ( !archive )
	{
		return false;
	}

	FShaderCache		shaderCache;
	archive->SerializeHeader();
	shaderCache.Serialize( *archive );
	delete archive;

	const std::vector< FShaderCache::FShaderCacheItem >			shaderCacheItems = shaderCache.GetItems();
	for ( uint32 indexItem = 0, countItems = ( uint32 )shaderCacheItems.size(); indexItem < countItems; ++indexItem )
	{
		const FShaderCache::FShaderCacheItem&		item = shaderCacheItems[ indexItem ];
		FShaderRef									shader = FContainerShaderTypes::CreateShaderInstance( item.name.c_str() );
		if ( !shader )
		{
			LE_LOG( LT_Warning, LC_Shader, TEXT( "Shader %s not loaded, because not found meta type" ), item.name.c_str() );
			continue;
		}

		shader->Init( item );

		LE_LOG( LT_Log, LC_Shader, TEXT( "Shader %s loaded" ), item.name.c_str() );
		shaders.insert( std::make_pair( item.name, shader ) );
	}

	return true;
}

/**
 * Initialize shader manager
 */
void FShaderManager::Init()
{
	std::wstring		pathShaderCache = FString::Format( TEXT( "../../Content/GlobalShaderCache-%s.bin" ), GRHI->GetRHIName() );
	if ( !LoadShaders( pathShaderCache.c_str() ) )
	{
#if WITH_EDITOR
		FShaderCompiler			shaderCompiler;
		bool					result = shaderCompiler.CompileAll( pathShaderCache.c_str() );
		check( result );

		result = LoadShaders( pathShaderCache.c_str() );
		if ( !result )
		{
			appErrorf( TEXT( "Failed loading shader cache [%s]" ), pathShaderCache.c_str() );
			return;
		}
#else
		appErrorf( TEXT( "Shader cache [%s] not found" ), pathShaderCache.c_str() );
		return;
#endif // WITH_EDITOR
	}
}

void FShaderManager::Shutdown()
{
	shaders.clear();
	LE_LOG( LT_Log, LC_Shader, TEXT( "All shaders unloaded" ) );
}