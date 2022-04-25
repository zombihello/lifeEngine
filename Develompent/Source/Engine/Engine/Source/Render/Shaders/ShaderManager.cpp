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
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/Shaders/ShaderCompiler.h"

FShaderParameter::FShaderParameter()
	: bufferIndex( 0 )
	, baseIndex( 0 )
	, numBytes( 0 )
{}

void FShaderParameter::Bind( const FShaderParameterMap& InParameterMap, const tchar* InParameterName, bool InIsOptional /*= false*/ )
{
	uint32 			unusedSamplerIndex = 0;
	if ( !InParameterMap.FindParameterAllocation( InParameterName, bufferIndex, baseIndex, numBytes, unusedSamplerIndex ) && !InIsOptional )
	{
		checkMsg( false, TEXT( "Failure to bind non-optional shader parameter %s! The parameter is either not present in the shader, or the shader compiler optimized it out" ), InParameterName );
	}
}

FShaderResourceParameter::FShaderResourceParameter()
	: baseIndex( 0 )
	, numResources( 0 )
	, samplerIndex( 0 )
{}

void FShaderResourceParameter::Bind( const FShaderParameterMap& InParameterMap, const tchar* InParameterName, bool InIsOptional /* = false */ )
{
	uint32		unusedBufferIndex = 0;
	if ( !InParameterMap.FindParameterAllocation( InParameterName, unusedBufferIndex, baseIndex, numResources, samplerIndex ) && !InIsOptional )
	{
		checkMsg( false, TEXT( "Failure to bind non-optional shader resource parameter %s! The parameter is either not present in the shader, or the shader compiler optimized it out" ), InParameterName );
	}
}

/**
 * Constructor of FShaderMetaType
 */
FShaderMetaType::FShaderMetaType( const std::wstring& InName, const std::wstring& InFileName, const std::wstring& InFunctionName, EShaderFrequency InFrequency, bool InIsGlobal, FConstructSerializedInstance InConstructSerializedInstance, FConstructCompiledInstance InConstructCompiledInstance
#if WITH_EDITOR
								  , FShouldCacheFunc InShouldCacheFunc, FModifyCompilationEnvironmentFunc InModifyCompilationEnvironmentFunc
#endif // WITH_EDITOR
)
	: bGlobal( InIsGlobal )
	, name( InName )
	, fileName( appShaderDir() + InFileName.c_str() )
	, functionName( InFunctionName )
	, frequency( InFrequency )
	, ConstructSerializedInstance( InConstructSerializedInstance )
	, ConstructCompiledInstance( InConstructCompiledInstance )
#if WITH_EDITOR
	, ShouldCacheFunc( InShouldCacheFunc )
	, ModifyCompilationEnvironmentFunc( InModifyCompilationEnvironmentFunc )
#endif // WITH_EDITOR
{
	FShaderManager::RegisterShaderType( this );
}

/**
 * Constructor of copy FShaderMetaType
 */
FShaderMetaType::FShaderMetaType( const FShaderMetaType& InCopy )
{
	bGlobal = InCopy.bGlobal;
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
FShader* FShaderManager::FContainerShaderTypes::CreateShaderInstance( const tchar* InShaderName )
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
		FShader*									shader = FContainerShaderTypes::CreateShaderInstance( item.name.c_str() );
		if ( !shader )
		{
			LE_LOG( LT_Warning, LC_Shader, TEXT( "Shader %s not loaded, because not found meta type" ), item.name.c_str() );
			continue;
		}	
		shader->Init( item );

		FVertexFactoryMetaType*			vertexFactoryType = FVertexFactoryMetaType::FContainerVertexFactoryMetaType::Get()->FindRegisteredType( item.vertexFactoryHash );
		if ( vertexFactoryType )
		{
			LE_LOG( LT_Log, LC_Shader, TEXT( "Shader %s for %s loaded" ), item.name.c_str(), vertexFactoryType->GetName().c_str() );
			shaders[ item.vertexFactoryHash ][ item.name ] = shader;
		}
		else
		{
			LE_LOG( LT_Warning, LC_Shader, TEXT( "Shader %s for vertex factory with hash 0x%X not loaded, because factory not found" ), item.name.c_str(), item.vertexFactoryHash );
		}	
	}

	return true;
}

FShader* FShaderManager::FindInstance( const std::wstring& InShaderName, uint64 InVertexFactoryHash )
{
	FMeshShaderMap::const_iterator		itMeshShaderMap = shaders.find( InVertexFactoryHash );
	if ( itMeshShaderMap == shaders.end() )
	{
		LE_LOG( LT_Warning, LC_Shader, TEXT( "For vertex factory hash 0x%X does not exist in the shaders cache" ), InVertexFactoryHash );
		return nullptr;
	}

	FShaderMap::const_iterator			itShaderMap = itMeshShaderMap->second.find( InShaderName );
	if ( itShaderMap == itMeshShaderMap->second.end() )
	{
		LE_LOG( LT_Warning, LC_Shader, TEXT( "Shader %s with vertex factory hash 0x%X not found in cache" ), InShaderName.c_str(), InVertexFactoryHash );
		return nullptr;
	}

	return itShaderMap->second;
}

std::wstring FShaderManager::GetShaderCacheFilename( EShaderPlatform InShaderPlatform )
{
	return FString::Format( TEXT( "GlobalShaderCache-%s.bin" ), ShaderPlatformToText( InShaderPlatform ) );
}

/**
 * Initialize shader manager
 */
void FShaderManager::Init()
{
	std::wstring		pathShaderCache;
#if WITH_EDITOR
	if ( GIsEditor || GIsCooker || GIsCommandlet )
	{
		pathShaderCache = appGameDir() + PATH_SEPARATOR + TEXT( "Content" ) + PATH_SEPARATOR + GetShaderCacheFilename( GRHI->GetShaderPlatform() );
	}
#endif // WITH_EDITOR
	else
	{
		pathShaderCache = GCookedDir + PATH_SEPARATOR + GetShaderCacheFilename( GRHI->GetShaderPlatform() );
	}
		
	if ( !LoadShaders( pathShaderCache.c_str() ) )
	{
#if WITH_EDITOR
		// Compile shaders only in cooker or commandlets
		if ( GIsCooker || GIsCommandlet || GIsEditor )
		{
			FShaderCompiler			shaderCompiler;
			bool					result = shaderCompiler.CompileAll( pathShaderCache.c_str(), GRHI->GetShaderPlatform() );
			check( result );

			result = LoadShaders( pathShaderCache.c_str() );
			if ( !result )
			{
				appErrorf( TEXT( "Failed loading shader cache [%s]" ), pathShaderCache.c_str() );
				return;
			}
		}
		else
#endif // WITH_EDITOR
		{
			appErrorf( TEXT( "Shader cache [%s] not found" ), pathShaderCache.c_str() );
			return;
		}
	}
}

void FShaderManager::Shutdown()
{
	shaders.clear();
	LE_LOG( LT_Log, LC_Shader, TEXT( "All shaders unloaded" ) );
}