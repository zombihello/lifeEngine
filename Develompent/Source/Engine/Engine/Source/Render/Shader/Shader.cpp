#include "Misc/EngineGlobals.h"
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
	numInstructions = InShaderCacheItem.numInstructions;

	switch ( frequency )
	{
	case SF_Vertex:
		vertexShader = shaderRHI = GRHI->CreateVertexShader( name.c_str(), InShaderCacheItem.code.data(), ( uint32 )InShaderCacheItem.code.size() );
		check( vertexShader );
		break;

	case SF_Hull:
		hullShader = shaderRHI = GRHI->CreateHullShader( name.c_str(), InShaderCacheItem.code.data(), ( uint32 )InShaderCacheItem.code.size() );
		check( hullShader );
		break;

	case SF_Domain:
		domainShader = shaderRHI = GRHI->CreateDomainShader( name.c_str(), InShaderCacheItem.code.data(), ( uint32 )InShaderCacheItem.code.size() );
		check( domainShader );
		break;

	case SF_Pixel:
		pixelShader = shaderRHI = GRHI->CreatePixelShader( name.c_str(), InShaderCacheItem.code.data(), ( uint32 )InShaderCacheItem.code.size() );
		check( pixelShader );
		break;

	case SF_Geometry:
		geometryShader = shaderRHI = GRHI->CreateGeometryShader( name.c_str(), InShaderCacheItem.code.data(), ( uint32 )InShaderCacheItem.code.size() );
		check( geometryShader );
		break;

	default:
		appErrorf( TEXT( "Unsupported shader frequency %i" ), frequency );
		break;
	}
}