/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef IMPORTTEXTURECOMMANDLET_H
#define IMPORTTEXTURECOMMANDLET_H

#include <string>
#include <vector>

#include "Commandlets/BaseCommandlet.h"
#include "Render/Texture.h"

/**
 * @ingroup WorldEd
 * Commandlet for import texture to engine
 */
class LImportTextureCommandlet : public LBaseCommandlet
{
	DECLARE_CLASS( LImportTextureCommandlet, LBaseCommandlet )

public:
	/**
	 * Main method of execute commandlet
	 *
	 * @param[in] InCommands Arguments of start commandlet
	 * @return Return true if commandlet executed is seccussed, else returning false
	 */
	virtual bool Main( const std::wstring& InCommand ) override;

	/**
	 * Convert image (png, jpg, etc) to texture 2D
	 * 
	 * @param InPath Path to image
	 * @param InAssetName Asset name for new texture
	 * @return Return converted texture 2D, if failed returning false
	 */
	FTexture2DRef ConvertTexture2D( const std::wstring& InPath, const std::wstring& InAssetName );

	/**
	 * Get supported texture extensions
	 * @retun Return array of supported extensions (e.g "png", "jpg", etc)
	 */
	FORCEINLINE static std::vector< std::wstring > GetSupportedExtensins()
	{
		static std::vector< std::wstring >		supportedExtensions =
		{
			TEXT( "png" ),
			TEXT( "jpg" )
		};
		return supportedExtensions;
	}

	/**
	 * Is supported texture extension
	 * 
	 * @param InExtension Texture extensions ("png", "jpg", etc)
	 * @return Return true if supported, else return false
	 */
	FORCEINLINE static bool IsSupportedExtension( const std::wstring& InExtension )
	{
		const std::vector< std::wstring >&		supportedExtensions = GetSupportedExtensins();
		for ( uint32 index = 0, count = supportedExtensions.size(); index < count; ++index )
		{
			if ( supportedExtensions[ index ] == InExtension )
			{
				return true;
			}
		}

		return false;
	}
};

#endif // !IMPORTTEXTURECOMMANDLET_H