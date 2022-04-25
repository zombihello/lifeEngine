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

void FShader::SetMesh( class FBaseDeviceContextRHI* InDeviceContextRHI, const struct FMeshBatch& InMesh, const class FVertexFactory* InVertexFactory, const class FSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{}

#if WITH_EDITOR
bool FShader::ShouldCache( EShaderPlatform InShaderPlatform, class FVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	return true;
}

void FShader::ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, FShaderCompilerEnvironment& InEnvironment )
{}
#endif // WITH_EDITOR

FArchive& operator<<( FArchive& InArchive, FShader*& InValue )
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
				LE_LOG( LT_Warning, LC_Package, TEXT( "Deprecated package version (0x%X), shader %s not loaded" ), InArchive.Ver(), shaderName.c_str() );
			}

			InValue = nullptr;
			return InArchive;
		}

		if ( !shaderName.empty() )
		{
			InValue = GShaderManager->FindInstance( shaderName, vertexFactoryHash );
		}
	}

	return InArchive;
}