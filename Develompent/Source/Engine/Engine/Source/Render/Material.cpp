#include "Misc/CoreGlobals.h"
#include "Logger/LoggerMacros.h"
#include "System/Package.h"
#include "Render/Material.h"
#include "Render/VertexFactory/StaticMeshVertexFactory.h"
#include "Render/VertexFactory/SpriteVertexFactory.h"

#if !SHIPPING_BUILD
#include "Render/VertexFactory/DynamicMeshVertexFactory.h"
#endif // !SHIPPING_BUILD

FMaterial::FMaterial() :
	FAsset( AT_Material ),
	isNeedUpdateShaderMap( true ),
	isTwoSided( false ),
	isWireframe( false ),
	usage( MU_AllMeshes )
{}

FMaterial::~FMaterial()
{}

void FMaterial::Serialize( class FArchive& InArchive )
{
	if ( InArchive.Ver() < VER_ShaderMap )
	{
		return;
	}

	if ( InArchive.IsLoading() )
	{
		isNeedUpdateShaderMap = true;
	}

	FAsset::Serialize( InArchive );
	InArchive << isTwoSided;
	InArchive << isWireframe;
	InArchive << usage;

	if ( InArchive.Ver() < VER_RemovedShadersTypeFromMaterial )
	{
		class FShaderMetaType*		shadersType[ SF_NumDrawFrequencies ];
		appMemzero( shadersType, sizeof( shadersType ) );

		for ( uint32 index = 0; index < SF_NumDrawFrequencies; ++index )
		{
			InArchive << shadersType[ index ];
		}
	}

	InArchive << scalarParameters;
	InArchive << vectorParameters;
	InArchive << textureParameters;
}

FShader* FMaterial::GetShader( uint64 InVertexFactoryHash, EShaderFrequency InShaderFrequency )
{
	check( InShaderFrequency < SF_NumDrawFrequencies );
	if ( isNeedUpdateShaderMap )
	{
		CacheShaderMap();
	}

	FMeshShaderMap::const_iterator		itVT = shaderMap.find( InVertexFactoryHash );
	if ( itVT == shaderMap.end() )
	{
		return nullptr;
	}
	return itVT->second[ InShaderFrequency ];
}

void FMaterial::CacheShaderMap()
{
	if ( !isNeedUpdateShaderMap )
	{
		return;
	}

	// If material usage for render static mesh
	{
		const uint64			vertexFactoryHash = FStaticMeshVertexFactory::staticType.GetHash();
		if ( usage & MU_StaticMesh )
		{	
			shaderMap[ vertexFactoryHash ] = GetMeshShaders( vertexFactoryHash );
		}
		else
		{
			shaderMap.erase( vertexFactoryHash );
		}
	}

	// If material usage for render sprite mesh
	{
		const uint64			vertexFactoryHash = FSpriteVertexFactory::staticType.GetHash();
		if ( usage & MU_Sprite )
		{
			shaderMap[ vertexFactoryHash ] = GetMeshShaders( vertexFactoryHash );
		}
		else
		{
			shaderMap.erase( vertexFactoryHash );
		}
	}

	// Only for debug and WorldEd
#if !SHIPPING_BUILD
	// Dynamic mesh
	{
		const uint64			vertexFactoryHash = FDynamicMeshVertexFactory::staticType.GetHash();
		shaderMap[ vertexFactoryHash ] = GetMeshShaders( vertexFactoryHash );
	}
#endif // !SHIPPING_BUILD

	isNeedUpdateShaderMap = false;
}

std::vector< FShader* > FMaterial::GetMeshShaders( uint64 InVertexFactoryHash ) const
{
	std::vector< FShader* >		result;
	result.resize( SF_NumDrawFrequencies );

	for ( uint32 index = 0; index < SF_NumDrawFrequencies; ++index )
	{
		const FShaderMetaType*		shaderType = GetShaderType( ( EShaderFrequency )index );
		if ( !shaderType || InVertexFactoryHash == ( uint32 )INVALID_HASH )
		{
			continue;
		}

		result[ index ] = GShaderManager->FindInstance( shaderType->GetName(), InVertexFactoryHash );
	}

	return result;
}

bool FMaterial::GetScalarParameterValue( const std::wstring& InParameterName, float& OutValue ) const
{
	auto		itFind = scalarParameters.find( InParameterName );
	if ( itFind == scalarParameters.end() )
	{
		OutValue = 0.f;
		return false;
	}

	OutValue = itFind->second;
	return true;
}

bool FMaterial::GetTextureParameterValue( const std::wstring& InParameterName, FTexture2DRef& OutValue ) const
{
	auto		itFind = textureParameters.find( InParameterName );
	if ( itFind == textureParameters.end() )
	{
		OutValue = GPackageManager->FindDefaultAsset( AT_Texture2D );
		return false;
	}

	OutValue = itFind->second ? itFind->second : GPackageManager->FindDefaultAsset( AT_Texture2D );
	return itFind->second.IsValid();
}

bool FMaterial::GetVectorParameterValue( const std::wstring& InParameterName, FVector4D& OutValue ) const
{
	auto		itFind = vectorParameters.find( InParameterName );
	if ( itFind == vectorParameters.end() )
	{
		OutValue = FVector4D( 0.f, 0.f, 0.f, 0.f );
		return false;
	}

	OutValue = itFind->second;
	return true;
}
