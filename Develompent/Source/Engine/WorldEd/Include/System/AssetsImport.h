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
	 * Enumeration of import result
	 */
	enum EImportResult
	{
		IR_Seccussed,		/**< Asset is imported seccussed */
		IR_Error,			/**< Asset not imported due to error */
		IR_AlreadyExist		/**< Asset not imported because in package already exist asset with name */
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

	/**
	 * Import asset to package
	 *
	 * @param InPath				Path to source file
	 * @param InPackage				Package of place for import
	 * @param OutAsset				Output imported asset, in case of fail return NULL
	 * @param OutError				Output error message
	 * @param InIsForceImport		Is need replace already exist asset with name
	 * @return Return imported asset, in case of fail return NULL
	 */
	static EImportResult Import( const QString& InPath, FPackage* InPackage, FAssetRef& OutAsset, std::wstring& OutError, bool InIsForceImport = false );

	/**
	 * Reimport asset
	 *
	 * @param InAsset		Asset for reimport
	 * @param OutError		Output error message
	 * @return Return TRUE if seccussed reimported, else FALSE
	 */
	static bool Reimport( FAsset* InAsset, std::wstring& OutError );
};

/**
 * @ingroup WorldEd
 * Class for importer textures 2D
 */
class FTexture2DImporter
{
public:
	/**
	 * Import texture 2D
	 *
	 * @param InPath		Path to source file
	 * @param OutError		Output error message
	 * @return Return imported texture 2D, if failed return NULL
	 */
	static FTexture2DRef Import( const std::wstring& InPath, std::wstring& OutError );

	/**
	 * Reimport texture 2D
	 *
	 * @param InTexture2D	Texture 2D
	 * @param OutError		Output error message
	 * @return Return TRUE if seccussed reimported, else return FALSE
	 */
	static bool Reimport( FTexture2D* InTexture2D, std::wstring& OutError );

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
};

/**
 * @ingroup WorldEd
 * Class for importer audio banks
 */
class FAudioBankImporter
{
public:
	/**
	 * Import audio bank
	 *
	 * @param InPath		Path to source file
	 * @param OutError		Output error message
	 * @return Return imported audio bank, if failed return NULL
	 */
	static FAudioBankRef Import( const std::wstring& InPath, std::wstring& OutError );

	/**
	 * Reimport audio bank
	 *
	 * @param InAudioBank	Audio bank
	 * @param OutError		Output error message
	 * @return Return TRUE if seccussed reimported, else return FALSE
	 */
	static bool Reimport( FAudioBank* InAudioBank, std::wstring& OutError );

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
};

#endif // !ASSETSIMPORT_H
