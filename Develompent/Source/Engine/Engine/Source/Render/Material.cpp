#include "Logger/LoggerMacros.h"
#include "Render/Material.h"
#include "Render/VertexFactory/StaticMeshVertexFactory.h"
#include "Render/VertexFactory/SpriteVertexFactory.h"

FMaterial::FMaterial() :
	FAsset( AT_Material ),
	isNeedUpdateShaderMap( false ),
	isTwoSided( false ),
	isWireframe( false ),
	usage( MU_AllMeshes )
{
	appMemzero( &shadersType, sizeof( shadersType ) );
}

FMaterial::~FMaterial()
{}

void FMaterial::Serialize( class FArchive& InArchive )
{
	if ( InArchive.Ver() < VER_ShaderMap )
	{
		return;
	}

	if ( InArchive.IsSaving() && isNeedUpdateShaderMap )
	{
		CacheShaderMap();
	}
	else if ( InArchive.IsLoading() )
	{
		isNeedUpdateShaderMap = true;
	}

	FAsset::Serialize( InArchive );
	InArchive << isTwoSided;
	InArchive << isWireframe;
	InArchive << usage;

	for ( uint32 index = 0; index < SF_NumDrawFrequencies; ++index )
	{
		InArchive << shadersType[ index ];
	}

	InArchive << scalarParameters;
	InArchive << textureParameters;
}

FShaderRef FMaterial::GetShader( uint64 InVertexFactoryHash, EShaderFrequency InShaderFrequency )
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

	isNeedUpdateShaderMap = false;
}

std::vector< FShaderRef > FMaterial::GetMeshShaders( uint64 InVertexFactoryHash ) const
{
	std::vector< FShaderRef >		result;
	result.resize( SF_NumDrawFrequencies );

	for ( uint32 index = 0; index < SF_NumDrawFrequencies; ++index )
	{
		const FShaderMetaType*		shaderType = shadersType[ index ];
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
		return false;
	}

	OutValue = itFind->second;
	return true;
}
