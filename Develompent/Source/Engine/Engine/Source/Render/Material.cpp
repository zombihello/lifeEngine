#include "Logger/LoggerMacros.h"
#include "Render/Material.h"

FMaterial::FMaterial() :
	FAsset( AT_Material ),
	isTwoSided( false ),
	isWireframe( false )
{}

FMaterial::~FMaterial()
{}

void FMaterial::Serialize( class FArchive& InArchive )
{
	if ( InArchive.Ver() < VER_Assets )
	{
		return;
	}

	FAsset::Serialize( InArchive );

	// Serialize base parameters
	InArchive << isTwoSided;
	InArchive << isWireframe;

	// Serialize shaders
	for ( uint32 index = 0; index < SF_NumFrequencies; ++index )
	{
		InArchive << shaders[ index ];
	}

	// Serialize shader parameters
	if ( InArchive.IsSaving() )
	{
		InArchive << ( uint32 )scalarParameters.size();
		for ( auto itParam = scalarParameters.begin(), itParamEnd = scalarParameters.end(); itParam != itParamEnd; ++itParam )
		{
			InArchive << itParam->first;
			InArchive << itParam->second;
		}

		InArchive << ( uint32 )textureParameters.size();
		for ( auto itParam = textureParameters.begin(), itParamEnd = textureParameters.end(); itParam != itParamEnd; ++itParam )
		{
			if ( !itParam->second )
			{
				continue;
			}

			InArchive << itParam->first;
			InArchive << itParam->second;
		}
	}
	else
	{
		uint32			numScalarParameters = 0;
		InArchive << numScalarParameters;
		for ( uint32 index = 0; index < numScalarParameters; ++index )
		{
			std::wstring		paramName;
			float				paramValue = 0.f;

			InArchive << paramName;
			InArchive << paramValue;
			scalarParameters[ paramName ] = paramValue;
		}

		uint32			numTextureParameters = 0;
		InArchive << numTextureParameters;
		for ( uint32 index = 0; index < numTextureParameters; ++index )
		{
			std::wstring		paramName;
			FTexture2DRef		paramValue;

			InArchive << paramName;
			InArchive << paramValue;

			if ( paramValue )
			{
				textureParameters[ paramName ] = paramValue;
			}
		}
	}
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