/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Core/LEBuild.h"

#if WITH_EDITOR
#include "Core/Containers/String.h"
#include "Core/Logger/LoggerMacros.h"
#include "Core/Misc/CoreGlobals.h"
#include "Core/System/BaseFileSystem.h"
#include "RHI/BaseRHI.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/System/SplashScreen.h"
#include "Engine/Render/Shaders/ShaderCompiler.h"
#include "Engine/Render/Shaders/ShaderManager.h"
#include "Engine/Render/VertexFactory/VertexFactory.h"

/*
==================
CShaderCompiler::CompileAll
==================
*/
bool CShaderCompiler::CompileAll( const tchar* InOutputCache, EShaderPlatform InShaderPlatform, bool InOnlyGlobals /* = false */ )
{
	CShaderCache	shaderCache;
	bool			result = CompileAll( shaderCache, InShaderPlatform, InOnlyGlobals );
	if ( !result )
	{
		return false;
	}

	// Save shader cache
	CArchive*			archive = g_FileSystem->CreateFileWriter( InOutputCache, AW_NoFail );
	if ( archive )
	{
		archive->SetType( AT_ShaderCache );

		archive->SerializeHeader();
		shaderCache.Serialize( *archive );
		delete archive;
	}
	
	return true;
}

/*
==================
CShaderCompiler::CompileAll
==================
*/
bool CShaderCompiler::CompileAll( CShaderCache& InOutShaderCache, EShaderPlatform InShaderPlatform, bool InOnlyGlobals /* = false */ )
{
	std::wstring																			errorMsg;
	const std::unordered_map< std::wstring, CShaderMetaType* >&								shaderTypes = CShaderManager::ContainerShaderTypes::Get()->shaderMetaTypes;
	const CVertexFactoryMetaType::ContainerVertexFactoryMetaType::VertexFactoryMap_t&		vertexFactoryTypes = CVertexFactoryMetaType::ContainerVertexFactoryMetaType::Get()->GetRegisteredTypes();
	AssertMsg( !vertexFactoryTypes.empty(), TEXT( "In engine not a single vertex factory registered" ) );
	
	for ( auto itShader = shaderTypes.begin(), itShaderEnd = shaderTypes.end(); itShader != itShaderEnd; ++itShader )
	{
		CShaderMetaType*					metaType = itShader->second;
		if ( !metaType->ShouldCache( InShaderPlatform ) || ( !metaType->IsGlobal() && InOnlyGlobals ) )
		{
			continue;
		}
		const std::wstring&					shaderName = metaType->GetName();

		// Compile shader for each vertex factory
		for ( auto itVFType = vertexFactoryTypes.begin(), itVFTypeEnd = vertexFactoryTypes.end(); itVFType != itVFTypeEnd; ++itVFType )
		{
			CVertexFactoryMetaType*			vertexFactoryType = itVFType->second;
			if ( !vertexFactoryType->ShouldCache( InShaderPlatform ) || !metaType->ShouldCache( InShaderPlatform, vertexFactoryType ) )
			{
				continue;
			}

			Sys_SetSplashText( STT_StartupProgress, CString::Format( TEXT( "Compiling shader %s for %s..." ), shaderName.c_str(), vertexFactoryType->GetName().c_str() ).c_str() );
			bool		result = CompileShader( metaType, InShaderPlatform, InOutShaderCache, errorMsg, vertexFactoryType );
			AssertMsg( result, errorMsg.c_str() );
		}
	}

	return true;
}

/*
==================
CShaderCompiler::CompileShader
==================
*/
bool CShaderCompiler::CompileShader( class CShaderMetaType* InShaderMetaType, EShaderPlatform InShaderPlatform, class CShaderCache& InOutShaderCache, std::wstring& OutErrorMsg, class CVertexFactoryMetaType* InVertexFactoryType /* = nullptr */ )
{
	ShaderCompilerOutput			output;
	uint64							vertexFactoryHash = ( uint64 )INVALID_HASH;
	EShaderFrequency				shaderFrequency = InShaderMetaType->GetFrequency();
	
	ShaderCompilerEnvironment		environment( shaderFrequency );
	InShaderMetaType->ModifyCompilationEnvironment( InShaderPlatform, environment );

	if ( InVertexFactoryType )
	{
		environment.vertexFactoryFileName = InVertexFactoryType->GetFileName();
		vertexFactoryHash = InVertexFactoryType->GetHash();
		InVertexFactoryType->ModifyCompilationEnvironment( InShaderPlatform, environment );
	}

	bool		result = g_RHI->CompileShader( InShaderMetaType->GetFileName().c_str(), InShaderMetaType->GetFunctionName().c_str(), shaderFrequency, environment, output, g_AllowDebugShaderDump );
	if ( result )
	{
		CShaderCache::ShaderCacheItem			shaderCacheItem;
		shaderCacheItem.name = InShaderMetaType->GetName();
		shaderCacheItem.frequency = shaderFrequency;
		shaderCacheItem.vertexFactoryHash = vertexFactoryHash;
		shaderCacheItem.code = output.code;
		shaderCacheItem.numInstructions = output.numInstructions;
		shaderCacheItem.parameterMap = output.parameterMap;
		OutErrorMsg = TEXT( "" );

		// Validate compiled shader
		CShader*		shader = InShaderMetaType->CreateCompiledInstance();
		shader->Init( shaderCacheItem );
		delete shader;

		// Add shader to cache
		InOutShaderCache.Add( shaderCacheItem );
		Logf( TEXT( "Shader %s for %s compiled\n" ), InShaderMetaType->GetName().c_str(), InVertexFactoryType->GetName().c_str() );
	}
	else
	{
		OutErrorMsg = output.errorMsg;
		Errorf( TEXT( "Failed compiling shader %s for %s\n" ), InShaderMetaType->GetName().c_str(), InVertexFactoryType->GetName().c_str() );
	}

	return result;
}
#endif // WITH_EDITOR