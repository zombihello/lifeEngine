/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ASSETSIMPORT_H
#define ASSETSIMPORT_H

#include <vector>
#include <string>
#include <qstring.h>

#include "Render/Texture.h"
#include "System/AudioBank.h"
#include "Core.h"

/**
 * @ingroup WorldEd
 * Generic helper for importer assets
 */
class FHelperAssetImporter
{
public:
	/**
	 * Enumeration of flags for making filter supported extensions (see FHelperAssetImporter::MakeFilterOfSupportedExtensions)
	 */
	enum EExtensionType
	{
		ET_Texture2D	= 1 << 0,							/**< Add in filter extensions for texture 2D */
		ET_AudioBank	= 1 << 1,							/**< Add in filter extensions for audio bank */
		ET_All			= ET_Texture2D | ET_AudioBank		/**< Add in filter all assets type */
	};

	/**
	 * Is supported extension
	 *
	 * @param InExtension				Check extensions ("png", "jpg", etc)
	 * @param InSupportedExtensions		List of supported extensions
	 * @return Return TRUE if supported, else return FALSE
	 */
	FORCEINLINE static bool IsSupportedExtension( const std::wstring& InExtension, const std::vector< std::wstring >& InSupportedExtensions )
	{
		for ( uint32 index = 0, count = InSupportedExtensions.size(); index < count; ++index )
		{
			if ( InSupportedExtensions[ index ] == InExtension )
			{
				return true;
			}
		}

		return false;
	}

	/**
	 * Make filter for file dialog with supported extensions
	 * 
	 * @param InFlags		Flags for making filter (see FHelperAssetImporter::EExtensionType)
	 * @return Return generated string for using in file dialogs
	 */
	static QString MakeFilterOfSupportedExtensions( uint32 InFlags = ET_All );
};

/**
 * @ingroup WorldEd
 * Class for importer textures 2D
 */
class FTexture2DImporter
{
public:
	friend class FAssetManager;

	/**
	 * Get supported texture extensions
	 * @return Return list of supported texture extensions
	 */
	FORCEINLINE static const std::vector< std::wstring >& GetSupportedExtensions()
	{
		static std::vector< std::wstring >		supportedExtensions =
		{
			TEXT( "png" ),
			TEXT( "jpg" ),
			TEXT( "tga" ),
			TEXT( "bmp" ),
			TEXT( "psd" ),
			TEXT( "gif" ),
			TEXT( "hdr" ),
			TEXT( "pic" ),
			TEXT( "pnm" )
		};
		return supportedExtensions;
	}

private:
	/**
	 * Import texture 2D
	 *
	 * @param InPath		Path to source file
	 * @return Return imported texture 2D, if failed return NULL
	 */
	static FTexture2DRef Import( const std::wstring& InPath );

	/**
	 * Reimport texture 2D
	 *
	 * @param InPath		Path to source file
	 * @param InTexture2D	Texture 2D
	 * @return Return TRUE if seccussed reimported, else return FALSE
	 */
	static bool Reimport( FTexture2D* InTexture2D );
};

/**
 * @ingroup WorldEd
 * Class for importer audio banks
 */
class FAudioBankImporter
{
public:
	friend class FAssetManager;

	/**
	 * Get supported audio extensions
	 * @return Return list of supported audio extensions
	 */
	FORCEINLINE static const std::vector< std::wstring >& GetSupportedExtensions()
	{
		static std::vector< std::wstring >		supportedExtensions =
		{
			TEXT( "ogg" )
		};
		return supportedExtensions;
	}

private:
	/**
	 * Import audio bank
	 *
	 * @param InPath		Path to source file
	 * @return Return imported audio bank, if failed return NULL
	 */
	static FAudioBankRef Import( const std::wstring& InPath );

	/**
	 * Reimport audio bank
	 *
	 * @param InPath		Path to source file
	 * @param InAudioBank	Audio bank
	 * @return Return TRUE if seccussed reimported, else return FALSE
	 */
	static bool Reimport( FAudioBank* InAudioBank );
};

#endif // !ASSETSIMPORT_H
