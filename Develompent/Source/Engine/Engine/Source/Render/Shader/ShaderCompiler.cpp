#include "Logger/LoggerMacros.h"
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "System/BaseFileSystem.h"
#include "RHI/BaseRHI.h"
#include "Render/Shaders/ShaderCompiler.h"
#include "Render/Shaders/ShaderManager.h"

/**
 * Compile all shaders
 */
bool FShaderCompiler::CompileAll( const tchar* InOutputCache )
{
	const std::unordered_map< std::wstring, FShaderMetaType* >&			shaderTypes = FShaderManager::FContainerShaderTypes::Get()->shaderMetaTypes;
	FShaderCache														shaderCache;

	for ( auto itShader = shaderTypes.begin(), itShaderEnd = shaderTypes.end(); itShader != itShaderEnd; ++itShader )
	{
		FShaderMetaType*					metaType = itShader->second;
		FShaderCompilerEnvironment			environment( metaType->GetFrequency() );
		FShaderCompilerOutput				output;

		bool		result = GRHI->CompileShader( metaType->GetFileName().c_str(), metaType->GetFunctionName().c_str(), metaType->GetFrequency(), environment, output );
		if ( !result )
		{
			check( false );
			return false;
		}

		// TODO BG yehor.pohuliaka - Serialization of shader need move to FShader
		FShaderCache::FShaderCacheItem			shaderCacheItem;
		shaderCacheItem.name = metaType->GetName();
		shaderCacheItem.frequency = metaType->GetFrequency();
		shaderCacheItem.code = output.code;
		shaderCacheItem.numInstructions = output.numInstructions;
		
		LE_LOG( LT_Log, LC_Shader, TEXT( "Shader %s compiled" ), shaderCacheItem.name.c_str() );
		shaderCache.Add( shaderCacheItem );
	}

	// Save shader cache
	FBaseArchive*			archive = GFileSystem->CreateFileWriter( InOutputCache, AW_NoFail );
	if ( archive )
	{
		shaderCache.Serialize( *archive );
		delete archive;
	}
	
	return true;
}