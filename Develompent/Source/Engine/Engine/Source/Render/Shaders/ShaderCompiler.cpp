#include "LEBuild.h"
#if WITH_EDITOR

#include "Containers/String.h"
#include "Logger/LoggerMacros.h"
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "System/BaseFileSystem.h"
#include "System/SplashScreen.h"
#include "RHI/BaseRHI.h"
#include "Render/Shaders/ShaderCompiler.h"
#include "Render/Shaders/ShaderManager.h"
#include "Render/VertexFactory/VertexFactory.h"

/**
 * Compile all shaders
 */
bool FShaderCompiler::CompileAll( const tchar* InOutputCache )
{
	const std::unordered_map< std::wstring, FShaderMetaType* >&								shaderTypes = FShaderManager::FContainerShaderTypes::Get()->shaderMetaTypes;
	const FVertexFactoryMetaType::FContainerVertexFactoryMetaType::FVertexFactoryMap&		vertexFactoryTypes = FVertexFactoryMetaType::FContainerVertexFactoryMetaType::Get()->GetRegisteredTypes();
	FShaderCache																			shaderCache;
	checkMsg( !vertexFactoryTypes.empty(), TEXT( "In engine not a single vertex factory registered" ) );
	
	for ( auto itShader = shaderTypes.begin(), itShaderEnd = shaderTypes.end(); itShader != itShaderEnd; ++itShader )
	{
		FShaderMetaType*					metaType = itShader->second;
		EShaderFrequency					frequency = metaType->GetFrequency();
		std::wstring						shaderName = metaType->GetName();
		std::wstring						shaderSourceFileName = metaType->GetFileName();
		std::wstring						functionName = metaType->GetFunctionName();
		bool								result = false;

		// If the current shader is a vertex shader, then we compile it for each vertex factory	
		if ( frequency == SF_Vertex )
		{
			for ( auto itVFType = vertexFactoryTypes.begin(), itVFTypeEnd = vertexFactoryTypes.end(); itVFType != itVFTypeEnd; ++itVFType )
			{
				FVertexFactoryMetaType*			vertexFactoryType = itVFType->second;
				
				appSetSplashText( STT_StartupProgress, FString::Format( TEXT( "Compiling shader %s for %s..." ), shaderName.c_str(), vertexFactoryType->GetName() ).c_str() );			
				result = CompileShader( shaderName, shaderSourceFileName, functionName, frequency, shaderCache, vertexFactoryType );
				check( result );

				LE_LOG( LT_Log, LC_Shader, TEXT( "Shader %s for %s compiled" ), shaderName.c_str(), vertexFactoryType->GetName().c_str() );
			}
		}
		// Otherwise, compile the shader once
		else
		{
			appSetSplashText( STT_StartupProgress, FString::Format( TEXT( "Compiling shader %s..." ), shaderName.c_str() ).c_str() );
			result = CompileShader( shaderName, shaderSourceFileName, functionName, frequency, shaderCache );
			check( result );

			LE_LOG( LT_Log, LC_Shader, TEXT( "Shader %s compiled" ), shaderName.c_str() );
		}
	}

	// Save shader cache
	FArchive*			archive = GFileSystem->CreateFileWriter( InOutputCache, AW_NoFail );
	if ( archive )
	{
		archive->SetType( AT_ShaderCache );

		archive->SerializeHeader();
		shaderCache.Serialize( *archive );
		delete archive;
	}
	
	return true;
}

bool FShaderCompiler::CompileShader( const std::wstring& InShaderName, const std::wstring& InShaderSourceFileName, const std::wstring& InFunctionName, EShaderFrequency InShaderFrequency, class FShaderCache& InOutShaderCache, class FVertexFactoryMetaType* InVertexFactoryType /* = nullptr */ )
{
	FShaderCompilerEnvironment		environment( InShaderFrequency );
	FShaderCompilerOutput			output;
	uint32							vertexFactoryHash = ( uint32 )INVALID_HASH;

	if ( InVertexFactoryType )
	{
		environment.vertexFactoryFileName = InVertexFactoryType->GetFileName();
		vertexFactoryHash = InVertexFactoryType->GetHash();
	}

	bool		result = GRHI->CompileShader( InShaderSourceFileName.c_str(), InFunctionName.c_str(), InShaderFrequency, environment, output );
	if ( result )
	{
		FShaderCache::FShaderCacheItem			shaderCacheItem;
		shaderCacheItem.name = InShaderName;
		shaderCacheItem.frequency = InShaderFrequency;
		shaderCacheItem.vertexFactoryHash = vertexFactoryHash;
		shaderCacheItem.code = output.code;
		shaderCacheItem.numInstructions = output.numInstructions;
		InOutShaderCache.Add( shaderCacheItem );
	}

	return result;
}
#endif // WITH_EDITOR