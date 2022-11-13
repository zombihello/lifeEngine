/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ASSETSIMPORT_H
#define ASSETSIMPORT_H

#include <string>

#include "Misc/SharedPointer.h"
#include "Render/Texture.h"

/**
 * @ingroup WorldEd
 * @brief Importer of a 2D texture
 */
class CTexture2DImporter
{
public:
	/**
	 * @brief Import a texture 2D
	 * 
	 * @param InPath		Path to a source file
	 * @param OutError		Output error message
	 * @return Return imported the texture 2D, otherwise will return NULL
	 */
	static TSharedPtr<CTexture2D> Import( const std::wstring& InPath, std::wstring& OutError );

	/**
	 * @brief Reimport a texture 2D
	 * 
	 * @param InTexture2D	Texture 2D
	 * @param OutError		Output error message
	 * @return Return TRUE if seccussed reimported, otherwise will return FALSE
	 */
	static bool Reimport( const TSharedPtr<CTexture2D>& InTexture2D, std::wstring& OutError );

	/**
	 * @brief Get supported texture extensions
	 * @return Return list of supported texture extensions
	 */
	FORCEINLINE static const std::vector< std::wstring >& GetSupportedExtensions()
	{
		static std::vector<std::wstring>		supportedExtensions =
		{
			TEXT( "PNG" ),
			TEXT( "JPG" ),
			TEXT( "JPEG" ),
			TEXT( "TGA" ),
			TEXT( "BMp" ),
			TEXT( "PSD" ),
			TEXT( "GIF" ),
			TEXT( "HDR" ),
			TEXT( "PIC" ),
			TEXT( "PNM" )
		};
		return supportedExtensions;
	}
};

#endif // !ASSETSIMPORT_H