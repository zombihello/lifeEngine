#include "Misc/CoreGlobals.h"
#include "Logger/LoggerMacros.h"
#include "System/Package.h"
#include "Render/Material.h"
#include "Render/VertexFactory/StaticMeshVertexFactory.h"
#include "Render/VertexFactory/SpriteVertexFactory.h"

#if !SHIPPING_BUILD
#include "Render/VertexFactory/DynamicMeshVertexFactory.h"
#endif // !SHIPPING_BUILD

CMaterial::CMaterial() :
	CAsset( AT_Material ),
	isNeedUpdateShaderMap( true ),
	isTwoSided( false ),
	isWireframe( false ),
	usage( MU_AllMeshes )
{}

CMaterial::~CMaterial()
{}

void CMaterial::Serialize( class CArchive& InArchive )
{
	if ( InArchive.Ver() < VER_ShaderMap )
	{
		return;
	}

	if ( InArchive.IsLoading() )
	{
		isNeedUpdateShaderMap = true;
	}

	CAsset::Serialize( InArchive );
	InArchive << isTwoSided;
	InArchive << isWireframe;
	InArchive << usage;

	if ( InArchive.Ver() < VER_RemovedShadersTypeFromMaterial )
	{
		class CShaderMetaType*		shadersType[ SF_NumDrawFrequencies ];
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

CShader* CMaterial::GetShader( uint64 InVertexFactoryHash, EShaderFrequency InShaderFrequency )
{
	check( InShaderFrequency < SF_NumDrawFrequencies );
	if ( isNeedUpdateShaderMap )
	{
		CacheShaderMap();
	}

	MeshShaderMap_t::const_iterator		itVT = shaderMap.find( InVertexFactoryHash );
	if ( itVT == shaderMap.end() )
	{
		return nullptr;
	}
	return itVT->second[ InShaderFrequency ];
}

void CMaterial::CacheShaderMap()
{
	if ( !isNeedUpdateShaderMap )
	{
		return;
	}

	// If material usage for render static mesh
	{
		const uint64			vertexFactoryHash = CStaticMeshVertexFactory::staticType.GetHash();
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
		const uint64			vertexFactoryHash = CSpriteVertexFactory::staticType.GetHash();
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
		const uint64			vertexFactoryHash = CDynamicMeshVertexFactory::staticType.GetHash();
		shaderMap[ vertexFactoryHash ] = GetMeshShaders( vertexFactoryHash );
	}
#endif // !SHIPPING_BUILD

	isNeedUpdateShaderMap = false;
}

std::vector< CShader* > CMaterial::GetMeshShaders( uint64 InVertexFactoryHash ) const
{
	std::vector< CShader* >		result;
	result.resize( SF_NumDrawFrequencies );

	for ( uint32 index = 0; index < SF_NumDrawFrequencies; ++index )
	{
		const CShaderMetaType*		shaderType = GetShaderType( ( EShaderFrequency )index );
		if ( !shaderType || InVertexFactoryHash == ( uint32 )INVALID_HASH )
		{
			continue;
		}

		result[ index ] = GShaderManager->FindInstance( shaderType->GetName(), InVertexFactoryHash );
	}

	return result;
}

bool CMaterial::GetScalarParameterValue( const std::wstring& InParameterName, float& OutValue ) const
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

bool CMaterial::GetTextureParameterValue( const std::wstring& InParameterName, TAssetHandle<CTexture2D>& OutValue ) const
{
	auto		itFind = textureParameters.find( InParameterName );
	if ( itFind == textureParameters.end() )
	{
		OutValue = GPackageManager->FindDefaultAsset( AT_Texture2D );
		return false;
	}

	OutValue = itFind->second.IsAssetValid() ? itFind->second : GPackageManager->FindDefaultAsset( AT_Texture2D );
	return itFind->second.IsAssetValid();
}

bool CMaterial::GetVectorParameterValue( const std::wstring& InParameterName, Vector4D& OutValue ) const
{
	auto		itFind = vectorParameters.find( InParameterName );
	if ( itFind == vectorParameters.end() )
	{
		OutValue = Vector4D( 0.f, 0.f, 0.f, 0.f );
		return false;
	}

	OutValue = itFind->second;
	return true;
}

void CMaterial::GetDependentAssets( SetDependentAssets_t& OutDependentAssets, EAssetType InFilter /* = AT_Unknown */ ) const
{
	// Fill set of dependent assets
	if ( InFilter == AT_Unknown || InFilter == AT_Texture2D )
	{
		for ( auto itTexture = textureParameters.begin(), itTextureEnd = textureParameters.end(); itTexture != itTextureEnd; ++itTexture )
		{
			if ( !itTexture->second.IsAssetValid() )
			{
				continue;
			}

			OutDependentAssets.insert( itTexture->second );
		}
	}
}

void CMaterial::ReloadDependentAssets( bool InForce /* = false */ )
{
	for ( auto itTexture = textureParameters.begin(), itTextureEnd = textureParameters.end(); itTexture != itTextureEnd; ++itTexture )
	{
		TAssetHandle<CAsset>		assetHandle = itTexture->second;
		if ( !assetHandle.IsValid() || assetHandle.IsAssetValid() )
		{
			continue;
		}

		TSharedPtr<SAssetReference>	assetReference = itTexture->second.GetReference();
		itTexture->second			= GPackageManager->FindAsset( assetReference->guidPackage, assetReference->guidAsset, assetReference->type );
	}
}