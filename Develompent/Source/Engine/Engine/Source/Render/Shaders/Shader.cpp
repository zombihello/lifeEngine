#include "System/Archive.h"
#include "Logger/LoggerMacros.h"
#include "Misc/EngineGlobals.h"
#include "Render/Shaders/ShaderManager.h"
#include "Logger/LoggerMacros.h"
#include "RHI/BaseRHI.h"
#include "Render/Shaders/Shader.h"

/*
==================
CShader::Init
==================
*/
void CShader::Init( const CShaderCache::SShaderCacheItem& InShaderCacheItem )
{
	name = InShaderCacheItem.name;
	frequency = InShaderCacheItem.frequency;
	vertexFactoryHash = InShaderCacheItem.vertexFactoryHash;
	numInstructions = InShaderCacheItem.numInstructions;

	switch ( frequency )
	{
	case SF_Vertex:
		vertexShader = g_RHI->CreateVertexShader( name.c_str(), InShaderCacheItem.code.GetData(), InShaderCacheItem.code.Num() );
		Assert( vertexShader );
		break;

	case SF_Hull:
		hullShader = g_RHI->CreateHullShader( name.c_str(), InShaderCacheItem.code.GetData(), InShaderCacheItem.code.Num() );
		Assert( hullShader );
		break;

	case SF_Domain:
		domainShader = g_RHI->CreateDomainShader( name.c_str(), InShaderCacheItem.code.GetData(), InShaderCacheItem.code.Num() );
		Assert( domainShader );
		break;

	case SF_Pixel:
		pixelShader = g_RHI->CreatePixelShader( name.c_str(), InShaderCacheItem.code.GetData(), InShaderCacheItem.code.Num() );
		Assert( pixelShader );
		break;

	case SF_Geometry:
		geometryShader = g_RHI->CreateGeometryShader( name.c_str(), InShaderCacheItem.code.GetData(), InShaderCacheItem.code.Num() );
		Assert( geometryShader );
		break;

	default:
		Sys_Errorf( TEXT( "Unsupported shader frequency %i" ), frequency );
		break;
	}
}

/*
==================
CShader::SetConstantParameters
==================
*/
void CShader::SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const
{}

/*
==================
CShader::SetMesh
==================
*/
void CShader::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{}

#if WITH_EDITOR
/*
==================
CShader::ShouldCache
==================
*/
bool CShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	return true;
}

/*
==================
CShader::ModifyCompilationEnvironment
==================
*/
void CShader::ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, SShaderCompilerEnvironment& InEnvironment )
{}
#endif // WITH_EDITOR

/*
==================
operator<<
==================
*/
CArchive& operator<<( CArchive& InArchive, CShader*& InValue )
{
	if ( InArchive.IsSaving() )
	{
		if ( InValue )
		{
			InArchive << InValue->GetName();
			InArchive << InValue->GetVertexFactoryHash();
		}
		else
		{
			InArchive << std::wstring();
			InArchive << ( uint64 )INVALID_HASH;
		}
	}
	else
	{
		std::wstring			shaderName;
		uint64					vertexFactoryHash;
		InArchive << shaderName;

		if ( InArchive.Ver() >= VER_VertexFactory )
		{
			if ( InArchive.Ver() < VER_HashUInt64 )
			{
				uint32		tmpVertexFactoryHash = 0;
				InArchive << tmpVertexFactoryHash;
				vertexFactoryHash = tmpVertexFactoryHash;
			}
			else
			{
				InArchive << vertexFactoryHash;
			}
		}
		else
		{
			if ( !shaderName.empty() )
			{
				Warnf( TEXT( "Deprecated package version (0x%X), shader %s not loaded\n" ), InArchive.Ver(), shaderName.c_str() );
			}

			InValue = nullptr;
			return InArchive;
		}

		if ( !shaderName.empty() )
		{
			InValue = g_ShaderManager->FindInstance( shaderName, vertexFactoryHash );
		}
	}

	return InArchive;
}