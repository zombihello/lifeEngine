#include "Render/Material.h"

FMaterial::FMaterial() :
	isTwoSided( false ),
	isWireframe( false )
{}

FMaterial::~FMaterial()
{}

void FMaterial::Serialize( class FBaseArchive& InArchive )
{
	checkMsg( false, TEXT( "Not implemented" ) );
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