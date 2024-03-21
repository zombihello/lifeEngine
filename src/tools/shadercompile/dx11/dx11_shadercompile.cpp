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

#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "interfaces/interfaces.h"
#include "stdlib/stdlib.h"
#include "stdlib/strtools.h"
#include "stdlib/filetools.h"
#include "filesystem/ifilesystem.h"
#include "studiorender/studioapi/dx11/dx11_studiobuffer.h"
#include "tools/shadercompile/ishadercompile_backend.h"

/**
 * @ingroup shadercompile_dx11
 * @brief An implementation of the D3DX include interface to access a IShaderCompileEnvironment
 */
class CIncludeEnvironmentDx11 : public ID3DInclude
{
public:
	/**
	 * @brief Constructor
	 * @param pEnvironment		Shader compile environment
	 */
	CIncludeEnvironmentDx11( IShaderCompileEnvironment* pEnvironment ) :
		pEnvironment( pEnvironment )
	{}

	/**
	 * @brief Open file
	 * 
	 * @param type			Include type
	 * @param name			File name
	 * @param parentData	???
	 * @param data			Read data from file
	 * @param bytes			Count read bytes from file
	 */
	STDMETHOD( Open )( D3D_INCLUDE_TYPE type, LPCSTR pName, LPCVOID pParentData, LPCVOID* pData, UINT* pBytes )
	{
		// For including 'vertexfactory.hlsl' we take path from the environment
		const achar*	pFileName = pName;
		if ( !L_Stricmp( pName, "vertexfactory.hlsl" ) )
		{
			pFileName = pEnvironment->GetVertexFactoryFileName();
		}

		// Try find file in the file system
		TRefPtr<IFileReader>	pFile;

		// For absolute path we simple try to create a reader by the path
		if ( L_IsAbsolutePath( pFileName ) )
		{
			pFile = g_pFileSystem->IsFileExists( pFileName ) ? g_pFileSystem->CreateFileReader( pFileName ) : nullptr;
		}
		// Otherwise try to create a reader with using include directories
		else 
		{
			for ( uint32 index = 0, count = pEnvironment->GetNumIncludeDirs(); index < count; ++index )
			{
				// Compute the full file path
				std::string		fullPath = L_Sprintf( "%s/%s", pEnvironment->GetIncludeDir( index ), pFileName );
				
				// We are found file!
				if ( g_pFileSystem->IsFileExists( fullPath.c_str() ) )
				{
					pFile = g_pFileSystem->CreateFileReader( fullPath.c_str() );
					break;
				}
			}
		}

		// We not found this file :(
		if ( !pFile )
		{
			Error( "CShaderCompileBackendDx11: Not found included shader file '%s'", pFileName );
			return E_FAIL;
		}

		// Read data into memory
		*pBytes = ( UINT )pFile->GetSize();
		byte*	pLocalData = new byte[*pBytes + 1];
		Mem_Memzero( pLocalData, *pBytes + 1 );
		pFile->Read( pLocalData, *pBytes );
		*pData	= ( LPCVOID )pLocalData;

		// We are done
		return S_OK;
	}

	/**
	 * @brief Close file
	 * @param pData		Read data from file
	 */
	STDMETHOD( Close )( LPCVOID pData )
	{
		delete[] pData;
		return S_OK;
	}

private:
	IShaderCompileEnvironment*	pEnvironment;		/**< Shader compile environment */
};

/**
 * @ingroup shadercompile_dx11
 * @brief DirectX 11 Shader compile
 */
class CShaderCompileBackendDx11 : public IShaderCompileBackend
{
public:
	/**
	 * @brief Connect shader compile backend
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes the backend is connected, otherwise return FALSE
	 */
	virtual bool Connect( CreateInterfaceFn_t pFactory ) override;

	/**
	 * @brief Disconnect shader compile backend
	 */
	virtual void Disconnect() override;

	/**
	 * @brief Compile shader
	 *
	 * @param pSrcFileName		Path to source file of shader
	 * @param pFunctionName		Main function in the shader
	 * @param type				Shader type (vertex, pixel, etc)
	 * @param pEnvironment		Environment of shader
	 * @param pOutput			Output data after compiling
	 * @param bDebugDump		Is need create debug dump of shader?
	 * @param pShaderSubDir		SubDir for debug dump
	 * @return Return TRUE if compilation is succeed, otherwise returns FALSE
	 */
	virtual bool CompileShader( const achar* pSrcFileName, const achar* pFunctionName, EStudioShaderType type, IShaderCompileEnvironment* pEnvironment, IShaderCompileOutput* pOutput, bool bDebugDump = false, const achar* pShaderSubDir = "" ) override;

	/**
	 * @brief Get shader platform name
	 * @return Return shader platform name (dx11, vulkan, etc)
	 */
	virtual const achar* GetShaderPlatform() const override;
};

EXPOSE_SINGLE_INTERFACE( CShaderCompileBackendDx11, IShaderCompileBackend, SHADERCOMPILEBACKEND_INTERFACE_VERSION );


/*
==================
TranslateCompileFlagsDx11
==================
*/
static DWORD TranslateCompileFlagsDx11( uint32 compileFlags )
{
	DWORD	result = 0x0;
	if ( compileFlags & SHADERCOMPILE_PREFER_FLOW_CONTROL )
	{
		result |= D3D10_SHADER_PREFER_FLOW_CONTROL;
	}

	if ( compileFlags & SHADERCOMPILE_DEBUG )
	{
		result |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
	}
	else
	{
		result |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
	}

	if ( compileFlags & SHADERCOMPILE_AVOID_FLOW_CONTROL )
	{
		result |= D3D10_SHADER_AVOID_FLOW_CONTROL;
	}

	if ( compileFlags & SHADERCOMPILE_SKIP_VALIDATION )
	{
		result |= D3D10_SHADER_SKIP_VALIDATION;
	}
	return result;
}


/*
==================
CShaderCompileBackendDx11::Connect
==================
*/
bool CShaderCompileBackendDx11::Connect( CreateInterfaceFn_t factory )
{
	return ConnectStdLib( factory );
}

/*
==================
CShaderCompileBackendDx11::Disconnect
==================
*/
void CShaderCompileBackendDx11::Disconnect()
{
	DisconnectStdLib();
}

/*
==================
CShaderCompileBackendDx11::CompileShader
==================
*/
bool CShaderCompileBackendDx11::CompileShader( const achar* pSrcFileName, const achar* pFunctionName, EStudioShaderType type, IShaderCompileEnvironment* pEnvironment, IShaderCompileOutput* pOutput, bool bDebugDump /* = false */, const achar* pShaderSubDir /* = "" */ )
{
	// Load source shader file
	TRefPtr<IFileReader>	pShaderFile = g_pFileSystem->CreateFileReader( pSrcFileName );
	if ( !pShaderFile )
	{
		pOutput->SetErrorMsg( L_Sprintf( "Failed to open shader file '%s'", pSrcFileName ).c_str() );
		return false;
	}

	// Read file into memory
	uint64		fileSize = pShaderFile->GetSize();
	byte*		pBuffer = new byte[fileSize + 1];
	Mem_Memzero( pBuffer, fileSize + 1 );
	pShaderFile->Read( pBuffer, fileSize );

	// Get shader profile
	const achar*	pShaderProfile = nullptr;
	switch ( type )
	{
	case STUDIO_SHADER_VERTEX:		pShaderProfile = "vs_5_0";	break;
	case STUDIO_SHADER_HULL:		pShaderProfile = "hs_5_0";	break;
	case STUDIO_SHADER_DOMAIN:		pShaderProfile = "ds_5_0";	break;
	case STUDIO_SHADER_GEOMETRY:	pShaderProfile = "gs_5_0";	break;
	case STUDIO_SHADER_PIXEL:		pShaderProfile = "ps_5_0";	break;
	case STUDIO_SHADER_COMPUTE:		pShaderProfile = "cs_5_0";	break;
	default:
		pOutput->SetErrorMsg( L_Sprintf( "Unknown shader type 0x%X", type ).c_str() );

		// Free allocated memory
		delete[] pBuffer;
		return false;
	}

	// Convert the environment's defines to D3D_SHADER_MACROs
	std::vector<D3D_SHADER_MACRO>		dxDefines;
	for ( uint32 index = 0, count = pEnvironment->GetNumDefines(); index < count; ++index )
	{
		const achar*	pName = nullptr;
		const achar*	pValue = nullptr;
		pEnvironment->GetDefine( index, pName, pValue );
		dxDefines.emplace_back( D3D_SHADER_MACRO{ pName, pValue } );
	}

	// Add platform specific defines
	dxDefines.emplace_back( D3D_SHADER_MACRO{ "PLATFORM_DX11", "1" } );

	// Terminate the dxDefines list
	dxDefines.push_back( D3D_SHADER_MACRO{ nullptr, nullptr } );

	// Get compile flags
	DWORD	compileFlags = TranslateCompileFlagsDx11( ( bDebugDump ? SHADERCOMPILE_DEBUG : 0 ) | pEnvironment->GetCompileFlags() );

	// Compile shader
	CIncludeEnvironmentDx11		includeEnvironment( pEnvironment );
	ID3DBlob*					pShaderBlob = nullptr;
	ID3DBlob*					pErrorsBlob = nullptr;
	HRESULT						result = D3DCompile( pBuffer, fileSize, pSrcFileName, dxDefines.data(), &includeEnvironment, pFunctionName, pShaderProfile, compileFlags, 0, &pShaderBlob, &pErrorsBlob);
	if ( FAILED( result ) )
	{
		// Copy the error text to the output
		achar*	pErrorBuffer = pErrorsBlob ? ( achar* )pErrorsBlob->GetBufferPointer() : nullptr;
		if ( pErrorBuffer )
		{
			// Remove '\n' if it exists at the end in pErrorBuffer, because the one no need
			uint32		errorBufferLength = L_Strlen( pErrorBuffer );
			if ( pErrorBuffer[errorBufferLength - 1] == '\n' )
			{
				pErrorBuffer[errorBufferLength - 1] = '\0';
			}

			pOutput->SetErrorMsg( pErrorBuffer );
			pErrorsBlob->Release();
		}
		else
		{
			pOutput->SetErrorMsg( "Compile failed without warnings!" );
		}

		// Free allocated memory
		delete[] pBuffer;
		return false;
	}

	// Save code of the shader and get shader reflector
	pOutput->SetBytecode( ( const byte* )pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize() );

	ID3D11ShaderReflection*		pShaderReflector = nullptr;
	result = D3DReflect( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, ( void** )&pShaderReflector );
	if ( FAILED( result ) )
	{
		pOutput->SetErrorMsg( "Failed to get reflection from the shader" );

		// Free allocated memory
		pShaderBlob->Release();
		delete[] pBuffer;
		return false;
	}

	// Read the constant table description
	D3D11_SHADER_DESC			shaderDesc;
	pShaderReflector->GetDesc( &shaderDesc );

	// Add parameters for shader resources (constant buffers, textures, samplers, etc)
	for ( uint32 resourceIndex = 0; resourceIndex < shaderDesc.BoundResources; ++resourceIndex )
	{
		D3D11_SHADER_INPUT_BIND_DESC 		bindDesc;
		pShaderReflector->GetResourceBindingDesc( resourceIndex, &bindDesc );

		if ( bindDesc.Type == D3D10_SIT_CBUFFER || bindDesc.Type == D3D10_SIT_TBUFFER )
		{
			const uint32 								cbIndex = bindDesc.BindPoint;
			ID3D11ShaderReflectionConstantBuffer* 		pConstantBuffer = pShaderReflector->GetConstantBufferByName( bindDesc.Name );
			D3D11_SHADER_BUFFER_DESC 					cbDesc;
			pConstantBuffer->GetDesc( &cbDesc );

			// We support of buffer slots in range [CONSTBUFFERSLOT_FIRST_SLOT; CONSTBUFFERSLOT_LAST_SLOT]
			if ( cbIndex < CONSTBUFFERSLOT_FIRST_SLOT || cbIndex > CONSTBUFFERSLOT_LAST_SLOT )
			{
				pOutput->SetErrorMsg( L_Sprintf( "Constant buffer '%s' has invalid slot (%i), support of slots in range [%i; %i]", bindDesc.Name, cbIndex, CONSTBUFFERSLOT_FIRST_SLOT, CONSTBUFFERSLOT_LAST_SLOT ).c_str() );

				// Free allocated memory
				pShaderReflector->Release();
				pShaderBlob->Release();
				delete[] pBuffer;
				return false;
			}

			if ( cbDesc.Size > DX11_GetMaxConstantBufferSize( ( EConstantBufferSlotDx11 )cbIndex ) )
			{
				pOutput->SetErrorMsg( L_Sprintf( "Constant buffer '%s' has size (%d) bigger then maximum size (%d) for this buffer index (%i)", bindDesc.Name, cbDesc.Size, DX11_GetMaxConstantBufferSize( ( EConstantBufferSlotDx11 )cbIndex ), cbIndex).c_str());

				// Free allocated memory
				pShaderReflector->Release();
				pShaderBlob->Release();
				delete[] pBuffer;
				return false;
			}

			// Track all of the variables in this constant buffer
			for ( uint32 constantIndex = 0; constantIndex < cbDesc.Variables; ++constantIndex )
			{
				ID3D11ShaderReflectionVariable* 		pVariable = pConstantBuffer->GetVariableByIndex( constantIndex );
				D3D11_SHADER_VARIABLE_DESC 				variableDesc;
				pVariable->GetDesc( &variableDesc );

				if ( variableDesc.uFlags & D3D10_SVF_USED )
				{
					pOutput->AddParameterAllocation(
						variableDesc.Name,
						cbIndex,
						variableDesc.StartOffset,
						variableDesc.Size,
						0
						);
				}
				else
				{
					Warning( "CShaderCompileBackendDx11: Found unused shader parameter '%s' (offset %i size %i)", variableDesc.Name, variableDesc.StartOffset, variableDesc.Size );
				}
			}
		}
		else if ( bindDesc.Type == D3D10_SIT_TEXTURE )
		{
			std::string			officialName = bindDesc.Name;
			uint32				bindCount = 1;

			// Assign the name and optionally strip any "[#]" suffixes
			std::size_t			bracketLocation = officialName.find( '[' );
			if ( bracketLocation != std::wstring::npos )
			{
				officialName.erase( bracketLocation, officialName.size() );
				const uint32	numCharactersBeforeArray = ( uint32 )officialName.size();

				// In SM5, for some reason, array suffixes are included in Name, i.e. "Textures[0]", rather than "Textures"
				// Additionally elements in an array are listed as SEPERATE bound resources.
				// However, they are always contiguous in resource index, so iterate over the samplers and textures of the initial association
				// and count them, identifying the bindpoint and bindcounts
				while ( resourceIndex + 1 < shaderDesc.BoundResources )
				{
					D3D11_SHADER_INPUT_BIND_DESC		bindDesc2;
					pShaderReflector->GetResourceBindingDesc( resourceIndex + 1, &bindDesc2 );
					if ( bindDesc2.Type == D3D10_SIT_TEXTURE && L_Strncmp( bindDesc2.Name, bindDesc.Name, numCharactersBeforeArray ) == 0 )
					{
						// Skip over this resource since it is part of an array
						bindCount++;						
						resourceIndex++;
					}
					else
					{
						break;
					}
				}
			}

			// Add a parameter for the texture only, the sampler index will be invalid
			pOutput->AddParameterAllocation(
				officialName.c_str(),
				0,
				bindDesc.BindPoint,
				bindCount,
				-1
			);
		}
		else if ( bindDesc.Type == D3D11_SIT_UAV_RWTYPED || bindDesc.Type >= D3D11_SIT_STRUCTURED )
		{
			// TODO yehor.pohuliaka: Arrays are not yet supported
			const uint32	bindCount = 1;
			pOutput->AddParameterAllocation(
				bindDesc.Name,
				0,
				bindDesc.BindPoint,
				bindCount,
				-1
			);
		}
		else if ( bindDesc.Type == D3D10_SIT_SAMPLER )
		{ 
			std::string			officialName = bindDesc.Name;
			uint32				bindCount = 1;

			// Assign the name and optionally strip any "[#]" suffixes
			std::size_t			bracketLocation = officialName.find( '[' );
			if ( bracketLocation != std::wstring::npos )
			{
				officialName.erase( bracketLocation, officialName.size() );
				const uint32	numCharactersBeforeArray = ( uint32 )officialName.size();

				// In SM5, for some reason, array suffixes are included in Name, i.e. "Textures[0]", rather than "Textures"
				// Additionally elements in an array are listed as SEPERATE bound resources.
				// However, they are always contiguous in resource index, so iterate over the samplers and textures of the initial association
				// and count them, identifying the bindpoint and bindcounts
				while ( resourceIndex + 1 < shaderDesc.BoundResources )
				{
					D3D11_SHADER_INPUT_BIND_DESC	bindDesc2;
					pShaderReflector->GetResourceBindingDesc( resourceIndex + 1, &bindDesc2 );
					if ( bindDesc2.Type == D3D10_SIT_SAMPLER && L_Strncmp( bindDesc2.Name, bindDesc.Name, numCharactersBeforeArray ) == 0 )
					{
						// Skip over this resource since it is part of an array
						bindCount++;
						resourceIndex++;
					}
					else
					{
						break;
					}
				}
			}

			// Add a parameter for the sampler only, the texture index will be invalid
			pOutput->AddParameterAllocation(
				officialName.c_str(),
				0,
				-1,
				bindCount,
				bindDesc.BindPoint
			);
		}
	}

	// Set the number of instructions
	pOutput->SetNumInstructions( shaderDesc.InstructionCount );

	// Reflector is a com interface, so it needs to be released
	pShaderReflector->Release();

	// We don't need the reflection data anymore, and it just takes up space
	if ( !bDebugDump )
	{
		ID3DBlob*	pStrippedShader = nullptr;
		result = D3DStripShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), D3DCOMPILER_STRIP_REFLECTION_DATA | D3DCOMPILER_STRIP_DEBUG_INFO | D3DCOMPILER_STRIP_TEST_BLOBS, &pStrippedShader );
		if ( result == S_OK )
		{
			pOutput->SetBytecode( ( const byte* )pStrippedShader->GetBufferPointer(), pStrippedShader->GetBufferSize() );
			pStrippedShader->Release();
		}
		else
		{
			Warning( "CShaderCompileBackendDx11: Failed to strip the reflection data in the shader" );
		}
	}

	// Free allocated memory
	pShaderBlob->Release();
	delete[] pBuffer;
	return true;
}

/*
==================
CShaderCompileBackendDx11::GetShaderPlatform
==================
*/
const achar* CShaderCompileBackendDx11::GetShaderPlatform() const
{
	return "pc-sm5";
}