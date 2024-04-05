#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Logger/BaseLogger.h"
#include "Logger/LoggerMacros.h"
#include "System/Archive.h"
#include "System/BaseFileSystem.h"
#include "RHI/BaseRHI.h"
#include "Render/Shaders/Shader.h"
#include "Render/Shaders/ShaderManager.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/Shaders/ShaderCompiler.h"

#if WITH_EDITOR
#include "System/SplashScreen.h"
#endif // WITH_EDITOR

/*
==================
CShaderParameter::CShaderParameter
==================
*/
CShaderParameter::CShaderParameter()
	: bufferIndex( 0 )
	, baseIndex( 0 )
	, numBytes( 0 )
{}

/*
==================
CShaderParameter::Bind
==================
*/
void CShaderParameter::Bind( const CShaderParameterMap& InParameterMap, const tchar* InParameterName, bool InIsOptional /*= false*/ )
{
	uint32 			unusedSamplerIndex = 0;
	if ( !InParameterMap.FindParameterAllocation( InParameterName, bufferIndex, baseIndex, numBytes, unusedSamplerIndex ) && !InIsOptional )
	{
		AssertMsg( false, TEXT( "Failure to bind non-optional shader parameter %s! The parameter is either not present in the shader, or the shader compiler optimized it out" ), InParameterName );
	}
}

/*
==================
CShaderResourceParameter::CShaderResourceParameter
==================
*/
CShaderResourceParameter::CShaderResourceParameter()
	: baseIndex( 0 )
	, numResources( 0 )
	, samplerIndex( 0 )
{}

/*
==================
CShaderResourceParameter::Bind
==================
*/
void CShaderResourceParameter::Bind( const CShaderParameterMap& InParameterMap, const tchar* InParameterName, bool InIsOptional /* = false */ )
{
	uint32		unusedBufferIndex = 0;
	if ( !InParameterMap.FindParameterAllocation( InParameterName, unusedBufferIndex, baseIndex, numResources, samplerIndex ) && !InIsOptional )
	{
		AssertMsg( false, TEXT( "Failure to bind non-optional shader resource parameter %s! The parameter is either not present in the shader, or the shader compiler optimized it out" ), InParameterName );
	}
}


/*
==================
CShaderMetaType::CShaderMetaType
==================
*/
CShaderMetaType::CShaderMetaType( const std::wstring& InName, const std::wstring& InFileName, const std::wstring& InFunctionName, EShaderFrequency InFrequency, bool InIsGlobal, ConstructSerializedInstanceFn_t InConstructSerializedInstance, ConstructCompiledInstanceFn_t InConstructCompiledInstance
#if WITH_EDITOR
								  , ShouldCacheFn_t InShouldCacheFunc, ModifyCompilationEnvironmentFn_t InModifyCompilationEnvironmentFunc
#endif // WITH_EDITOR
)
	: bGlobal( InIsGlobal )
	, name( InName )
	, fileName( Sys_ShaderDir() + InFileName.c_str() )
	, functionName( InFunctionName )
	, frequency( InFrequency )
	, ConstructSerializedInstance( InConstructSerializedInstance )
	, ConstructCompiledInstance( InConstructCompiledInstance )
#if WITH_EDITOR
	, ShouldCacheFunc( InShouldCacheFunc )
	, ModifyCompilationEnvironmentFunc( InModifyCompilationEnvironmentFunc )
#endif // WITH_EDITOR
{
	CShaderManager::RegisterShaderType( this );
}

/*
==================
CShaderMetaType::CShaderMetaType
==================
*/
CShaderMetaType::CShaderMetaType( const CShaderMetaType& InCopy )
{
	bGlobal = InCopy.bGlobal;
	name = InCopy.name;
	fileName = InCopy.fileName;
	functionName = InCopy.functionName;
	frequency = InCopy.frequency;
	ConstructSerializedInstance = InCopy.ConstructSerializedInstance;
	ConstructCompiledInstance = InCopy.ConstructCompiledInstance;
}

/*
==================
CShaderManager::ContainerShaderTypes::CreateShaderInstance
==================
*/
CShader* CShaderManager::ContainerShaderTypes::CreateShaderInstance( const tchar* InShaderName )
{
	ContainerShaderTypes*		container = ContainerShaderTypes::Get();
	auto		itShaderMetaType = container->shaderMetaTypes.find( InShaderName );
	if ( itShaderMetaType == container->shaderMetaTypes.end() )
	{
		return nullptr;
	}

	return itShaderMetaType->second->CreateSerializedInstace();
}

/*
==================
CShaderManager::LoadShaders
==================
*/
bool CShaderManager::LoadShaders( const tchar* InPathShaderCache )
{
	CArchive*		archive = g_FileSystem->CreateFileReader( InPathShaderCache );
	if ( !archive )
	{
		return false;
	}

	CShaderCache		shaderCache;
	archive->SerializeHeader();
	shaderCache.Serialize( *archive );
	delete archive;

	uint32														numLoadedShaders = 0;
	uint32														numLegacyShaders = 0;
	const std::vector< CShaderCache::ShaderCacheItem >			shaderCacheItems = shaderCache.GetItems();
	for ( uint32 indexItem = 0, countItems = ( uint32 )shaderCacheItems.size(); indexItem < countItems; ++indexItem )
	{
		const CShaderCache::ShaderCacheItem&		item = shaderCacheItems[ indexItem ];
		CShader*									shader = ContainerShaderTypes::CreateShaderInstance( item.name.c_str() );
		if ( !shader )
		{
			Warnf( TEXT( "Shader %s not loaded, because not found meta type\n" ), item.name.c_str() );
			++numLegacyShaders;
			continue;
		}

		CVertexFactoryMetaType*			vertexFactoryType = CVertexFactoryMetaType::ContainerVertexFactoryMetaType::Get()->FindRegisteredType( item.vertexFactoryHash );

#if WITH_EDITOR
		if ( g_IsEditor )
		{
			Sys_SetSplashText( STT_StartupProgress, L_Sprintf( TEXT( "Loading shader %s for %s..." ), item.name.c_str(), vertexFactoryType->GetName().c_str() ).c_str() );
			Sys_Sleep( 0.01f );
		}
#endif // WITH_EDITOR

		shader->Init( item );
		++numLoadedShaders;
	
		if ( vertexFactoryType )
		{
			Logf( TEXT( "Shader %s for %s loaded\n" ), item.name.c_str(), vertexFactoryType->GetName().c_str() );
			shaders[ item.vertexFactoryHash ][ item.name ] = shader;
		}
		else
		{
			Warnf( TEXT( "Shader %s for vertex factory with hash 0x%X not loaded, because factory not found\n" ), item.name.c_str(), item.vertexFactoryHash );
		}	
	}

	Logf( TEXT( "Loaded %i shaders, %i legacy\n" ), numLoadedShaders, numLegacyShaders );
	return true;
}

/*
==================
CShaderManager::FindInstance
==================
*/
CShader* CShaderManager::FindInstance( const std::wstring& InShaderName, uint64 InVertexFactoryHash )
{
	MeshShaderMap_t::const_iterator		itMeshShaderMap = shaders.find( InVertexFactoryHash );
	if ( itMeshShaderMap == shaders.end() )
	{
		Warnf( TEXT( "For vertex factory hash 0x%X does not exist in the shaders cache\n" ), InVertexFactoryHash );
		return nullptr;
	}

	ShaderMap_t::const_iterator			itShaderMap = itMeshShaderMap->second.find( InShaderName );
	if ( itShaderMap == itMeshShaderMap->second.end() )
	{
		Warnf( TEXT( "%s with vertex factory hash 0x%X not found in cache\n" ), InShaderName.c_str(), InVertexFactoryHash );
		return nullptr;
	}

	return itShaderMap->second;
}

/*
==================
CShaderManager::GetShaderCacheFilename
==================
*/
std::wstring CShaderManager::GetShaderCacheFilename( EShaderPlatform InShaderPlatform )
{
	return L_Sprintf( TEXT( "GlobalShaderCache-%s.bin" ), ShaderPlatformToText( InShaderPlatform ) );
}


/*
==================
CShaderManager::Init
==================
*/
void CShaderManager::Init()
{
	std::wstring		pathShaderCache;
#if WITH_EDITOR
	if ( g_IsEditor || g_IsCooker || g_IsCommandlet )
	{
		pathShaderCache = Sys_GameDir() + PATH_SEPARATOR + TEXT( "Content" ) + PATH_SEPARATOR + GetShaderCacheFilename( g_RHI->GetShaderPlatform() );
	}
	else
#endif // WITH_EDITOR
	{
		pathShaderCache = g_CookedDir + PATH_SEPARATOR + GetShaderCacheFilename( g_RHI->GetShaderPlatform() );
	}
		
	if ( !LoadShaders( pathShaderCache.c_str() ) )
	{
#if WITH_EDITOR
		// Compile shaders only in cooker or commandlets
		if ( g_IsCooker || g_IsCommandlet || g_IsEditor )
		{
			CShaderCompiler			shaderCompiler;
			bool					result = shaderCompiler.CompileAll( pathShaderCache.c_str(), g_RHI->GetShaderPlatform() );
			Assert( result );

			result = LoadShaders( pathShaderCache.c_str() );
			if ( !result )
			{
				Sys_Error( TEXT( "Failed loading shader cache [%s]" ), pathShaderCache.c_str() );
				return;
			}
		}
		else
#endif // WITH_EDITOR
		{
			Sys_Error( TEXT( "Shader cache [%s] not found" ), pathShaderCache.c_str() );
			return;
		}
	}
}

/*
==================
CShaderManager::Shutdown
==================
*/
void CShaderManager::Shutdown()
{
	shaders.clear();
	Logf( TEXT( "All shaders unloaded\n" ) );
}