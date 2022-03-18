#include "System/Archive.h"
#include "Logger/LoggerMacros.h"
#include "Misc/EngineGlobals.h"
#include "Render/Shaders/ShaderManager.h"
#include "Logger/LoggerMacros.h"
#include "RHI/BaseRHI.h"
#include "Render/Shaders/Shader.h"

/**
 * Initialize shader
 */
void FShader::Init( const FShaderCache::FShaderCacheItem& InShaderCacheItem )
{
	FBaseShaderRHI*			shaderRHI = nullptr;

	name = InShaderCacheItem.name;
	frequency = InShaderCacheItem.frequency;
	vertexFactoryHash = InShaderCacheItem.vertexFactoryHash;
	numInstructions = InShaderCacheItem.numInstructions;

	switch ( frequency )
	{
	case SF_Vertex:
		vertexShader = shaderRHI = GRHI->CreateVertexShader( name.c_str(), InShaderCacheItem.code.GetData(), InShaderCacheItem.code.Num() );
		check( vertexShader );
		break;

	case SF_Hull:
		hullShader = shaderRHI = GRHI->CreateHullShader( name.c_str(), InShaderCacheItem.code.GetData(), InShaderCacheItem.code.Num() );
		check( hullShader );
		break;

	case SF_Domain:
		domainShader = shaderRHI = GRHI->CreateDomainShader( name.c_str(), InShaderCacheItem.code.GetData(), InShaderCacheItem.code.Num() );
		check( domainShader );
		break;

	case SF_Pixel:
		pixelShader = shaderRHI = GRHI->CreatePixelShader( name.c_str(), InShaderCacheItem.code.GetData(), InShaderCacheItem.code.Num() );
		check( pixelShader );
		break;

	case SF_Geometry:
		geometryShader = shaderRHI = GRHI->CreateGeometryShader( name.c_str(), InShaderCacheItem.code.GetData(), InShaderCacheItem.code.Num() );
		check( geometryShader );
		break;

	default:
		appErrorf( TEXT( "Unsupported shader frequency %i" ), frequency );
		break;
	}
}

void FShader::SetConstantParameters( class FBaseDeviceContextRHI* InDeviceContextRHI, const class FVertexFactory* InVertexFactory, const class FMaterial* InMaterialResource ) const
{}

void FShader::SetMesh( class FBaseDeviceContextRHI* InDeviceContextRHI, const struct FMeshBatch& InMesh, uint32 InBatchElementIndex, const class FSceneView* InView ) const
{}

FArchive& operator<<( FArchive& InArchive, FShaderRef& InValue )
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
			InArchive << ( uint32 )INVALID_HASH;
		}
	}
	else
	{
		std::wstring			shaderName;
		uint32					vertexShaderHash;
		InArchive << shaderName;

		if ( InArchive.Ver() >= VER_VertexFactory )
		{
			InArchive << vertexShaderHash;
		}
		else
		{
			if ( !shaderName.empty() )
			{
				LE_LOG( LT_Warning, LC_Package, TEXT( "Deprecated package version (0x%X), shader %s not loaded" ), InArchive.Ver(), shaderName.c_str() );
			}

			InValue = nullptr;
			return InArchive;
		}

		if ( !shaderName.empty() )
		{
			InValue = GShaderManager->FindInstance( shaderName, vertexShaderHash );
		}
	}

	return InArchive;
}