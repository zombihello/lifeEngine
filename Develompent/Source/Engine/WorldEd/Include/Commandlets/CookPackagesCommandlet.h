/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef COOKPACKAGESCOMMANDLET_H
#define COOKPACKAGESCOMMANDLET_H

#include <tmxlite/Map.hpp>
#include <tmxlite/Tileset.hpp>
#include <unordered_map>

#include "Math/Math.h"
#include "Math/Rect.h"
#include "Render/Material.h"
#include "Render/Texture.h"
#include "Commandlets/BaseCommandlet.h"
#include "Render/Shaders/ShaderCache.h"
#include "Render/Shaders/ShaderCompiler.h"
#include "System/AudioBuffer.h"

/**
 * @ingroup WorldEd
 * @brief Info about TMX tileset
 */
struct FTMXTileset
{
	uint32							firstGID;		/**< First ID of tile in set */
	uint32							lastGID;		/**< Last ID of tile in set */
	FVector2D						tileSize;		/**< Size of tile */
	FVector2D						tileOffset;		/**< Offset of tile */
	FMaterialRef					material;		/**< Material of tileset */
	std::vector< FRectFloat >		textureRects;	/**< Array of rects with tiles */
};

 /**
  * @ingroup WorldEd
  * Commandlet for generate table of contents
  */
class LCookPackagesCommandlet : public LBaseCommandlet
{
	DECLARE_CLASS( LCookPackagesCommandlet, LBaseCommandlet )

public:
	/**
	 * Constructor
	 */
	LCookPackagesCommandlet();

	/**
	 * Main method of execute commandlet
	 *
	 * @param[in] InCommands Arguments of start commandlet
	 * @return Return true if commandlet executed is seccussed, else returning false
	 */
	virtual bool Main( const std::wstring& InCommand ) override;

	/**
	 * Cook audio buffer
	 *
	 * @param InAssetRef Reference to asset in format <PackageName>:<AssetName>
	 * @param OutAudioBuffer Output cooked audio buffer
	 * @return Return true if seccussed cook, else returning false
	 */
	FORCEINLINE bool CookAudioBuffer( const std::wstring& InAssetRef, FAudioBufferRef& OutAudioBuffer )
	{
		std::wstring		packageName;
		std::wstring		assetName;
		FResourceInfo		resourceInfo;

		ParseReferenceToAsset( InAssetRef, packageName, assetName );
		if ( !FindResource( audiosMap, packageName, assetName, resourceInfo ) )
		{
			appErrorf( TEXT( "Audio buffer '%s' not founded" ), InAssetRef.c_str() );
			return false;
		}

		bool	result = CookAudioBuffer( resourceInfo, OutAudioBuffer );
		if ( !result )
		{
			appErrorf( TEXT( "Failed cooking audio buffer '%s'" ), InAssetRef.c_str() );
			return false;
		}

		return true;
	}

	/**
	 * Get supported map extensions
	 * @retun Return array of supported extensions (e.g "png", "jpg", etc)
	 */
	FORCEINLINE static std::vector< std::wstring > GetSupportedMapExtensins()
	{
		static std::vector< std::wstring >		supportedExtensions =
		{
			TEXT( "tmx" )
		};
		return supportedExtensions;
	}

	/**
	 * Is supported map extension
	 *
	 * @param InExtension Map extensions ("png", "jpg", etc)
	 * @return Return true if supported, else return false
	 */
	FORCEINLINE static bool IsSupportedMapExtension( const std::wstring& InExtension )
	{
		return IsSupportedExtension( InExtension, GetSupportedMapExtensins() );
	}

	/**
	 * Get supported material extensions
	 * @retun Return array of supported extensions (e.g "png", "jpg", etc)
	 */
	FORCEINLINE static std::vector< std::wstring > GetSupportedMaterialExtensins()
	{
		static std::vector< std::wstring >		supportedExtensions =
		{
			TEXT( "lmt" )
		};
		return supportedExtensions;
	}

	/**
	 * Is supported material extension
	 *
	 * @param InExtension Material extensions ("png", "jpg", etc)
	 * @return Return true if supported, else return false
	 */
	FORCEINLINE static bool IsSupportedMaterialExtension( const std::wstring& InExtension )
	{
		return IsSupportedExtension( InExtension, GetSupportedMaterialExtensins() );
	}

	/**
	 * Get supported texture extensions
	 * @retun Return array of supported extensions (e.g "png", "jpg", etc)
	 */
	FORCEINLINE static std::vector< std::wstring > GetSupportedTextureExtensins()
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
	FORCEINLINE static bool IsSupportedTextureExtension( const std::wstring& InExtension )
	{
		return IsSupportedExtension( InExtension, GetSupportedTextureExtensins() );
	}

	/**
	 * Get supported audio extensions
	 * @retun Return array of supported extensions (e.g "png", "jpg", etc)
	 */
	FORCEINLINE static std::vector< std::wstring > GetSupportedAudioExtensins()
	{
		static std::vector< std::wstring >		supportedExtensions =
		{
			TEXT( "ogg" )
		};
		return supportedExtensions;
	}

	/**
	 * Is supported audio extension
	 *
	 * @param InExtension Audio extensions ("png", "jpg", etc)
	 * @return Return true if supported, else return false
	 */
	FORCEINLINE static bool IsSupportedAudioExtension( const std::wstring& InExtension )
	{
		return IsSupportedExtension( InExtension, GetSupportedAudioExtensins() );
	}

private:
	/**
	 * Struct info about resource
	 */
	struct FResourceInfo
	{
		std::wstring		packageName;		/**< Name of package */
		std::wstring		filename;			/**< Name of file */
		std::wstring		path;				/**< Path to resource */
		bool				bAlwaysCook;		/**< Is need always cook */
	};

	/**
	 * Struct for containing of extensions for all resource type
	 */
	struct FExtensionInfo
	{
		std::wstring		package;		/**< Package extension */
		std::wstring		map;			/**< Map extension */
	};

	/**
	 * Typedef map of resources
	 */
	typedef std::unordered_map< std::wstring, std::unordered_map< std::wstring, FResourceInfo > >		FResourceMap;

	/**
	 * Is supported extension
	 *
	 * @param InExtension Checkable extension
	 * @param InSupportedExtensions Array of supported extensions
	 * @return Return true if supported, else return false
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
	 * @brief Loading tilesets from TMX
	 * 
	 * @param InTMXMap TMX map
	 * @param OutTilesets Output loaded tilesets
	 * @return Return true if tileset is seccussed loaded, else returning false
	 */
	bool LoadTMXTilests( const tmx::Map& InTMXMap, std::vector< FTMXTileset >& OutTilesets );

	/**
	 * @brief Spawn tiles in world
	 * 
	 * @param InTMXMap TMX map
	 * @param InTilesets Array of tilesets
	 */
	void SpawnTilesInWorld( const tmx::Map& InTMXMap, const std::vector< FTMXTileset >& InTilesets );

	/**
	 * @brief Spawn actors in world
	 * 
	 * @param InTMXMap TMX map
	 * @param InTilesets Array of tilesets
	 */
	void SpawnActorsInWorld( const tmx::Map& InTMXMap, const std::vector< FTMXTileset >& InTileset );

	/**
	 * Find tileset by ID tile
	 *
	 * @param InTilesets Array of tilesets
	 * @param InIDTile ID tile
	 * @param OutTileset Output finded tileset info
	 * @param OutTextureRect Output texture rect for InIDTile
	 * @return Return true if tileset is finded, else returning false
	 */
	bool FindTileset( const std::vector< FTMXTileset >& InTilesets, uint32 InIDTile, FTMXTileset& OutTileset, FRectFloat& OutTextureRect ) const;

	/**
	 * Indexing all resources in engine and game. Recursive method
	 * 
	 * @param InRootDir Path to root directory
	 * @param InIsRootDir Is door door
	 * @param InIsAlwaysCookDir Is always cook dir
	 * @param InParentDirName Parent directory name
	 */
	void IndexingResources( const std::wstring& InRootDir, bool InIsRootDir = false, bool InIsAlwaysCookDir = false, const std::wstring& InParentDirName = TEXT( "" ) );

	/**
	 * Cook all resources
	 * 
	 * @param InIsOnlyAlwaysCook Is need cook only resources with enabled flag bAlwaysCook
	 */
	void CookAllResources( bool InIsOnlyAlwaysCook = false );

	/**
	 * Cook map
	 * 
	 * @param InMapInfo Info about resource
	 * @return Return true if seccussed cook, else returning false
	 */
	bool CookMap( const FResourceInfo& InMapInfo );

	/**
	 * Cook material
	 * 
	 * @param InMaterialInfo Info about resource
	 * @param OutMaterial Output cooked material
	 * @return Return true if seccussed cook, else returning false
	 */
	bool CookMaterial( const FResourceInfo& InMaterialInfo, FMaterialRef& OutMaterial );

	/**
	 * Cook texture 2D
	 *
	 * @param InTexture2DInfo Info about resource
	 * @param OutTexture2D Output cooked texture
	 * @return Return true if seccussed cook, else returning false
	 */
	bool CookTexture2D( const FResourceInfo& InTexture2DInfo, FTexture2DRef& OutTexture2D );

	/**
	 * Cook audio buffer
	 * 
	 * @param InAudioBufferInfo Info about audio buffer
	 * @param OutAudioBuffer Output cooked audio buffer
	 * @return Return true if seccussed cook, else returning false
	 */
	bool CookAudioBuffer( const FResourceInfo& InAudioBufferInfo, FAudioBufferRef& OutAudioBuffer );

	/**
	 * Find resource
	 * 
	 * @param InResourceMap Resource map
	 * @param InPackageName Package name
	 * @param InAssetName Asset name
	 * @param OutResourceInfo Output founded resource
	 * @return Return true if resource is founded, else returning false
	 */
	FORCEINLINE bool FindResource( const FResourceMap& InResourceMap, const std::wstring& InPackageName, const std::wstring& InAssetName, FResourceInfo& OutResourceInfo ) const
	{
		auto		itPackage = InResourceMap.find( InPackageName );
		if ( itPackage == InResourceMap.end() )
		{
			return false;
		}

		auto		itAsset = itPackage->second.find( InAssetName );
		if ( itAsset == itPackage->second.end() )
		{
			return false;
		}

		OutResourceInfo = itAsset->second;
		return true;
	}

	/**
	 * Save to package
	 * 
	 * @param InResourceInfo Resource info
	 * @param InAsset Asset for save
	 * @return Return true if package saved seccussed, else return false
	 */
	bool SaveToPackage( const FResourceInfo& InResourceInfo, FAssetRef InAsset );

	FExtensionInfo											extensionInfo;			/**< Info about extensions of output formats */
	FResourceMap											texturesMap;			/**< All textures */
	FResourceMap											materialsMap;			/**< All materials */
	FResourceMap											audiosMap;				/**< All audios */
	std::unordered_map< std::wstring, FResourceInfo >		mapsMap;				/**< All maps */
	FShaderCache											shaderCache;			/**< Cooked shader cache */
	EShaderPlatform											cookedShaderPlatform;	/**< Cooked shader platform */
	EPlatformType											cookedPlatform;			/**< Cooked platform */
};

#endif // !COOKPACKAGESCOMMANDLET_H