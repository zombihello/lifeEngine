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
#include "System/AudioBank.h"
#include "System/PhysicsMaterial.h"

/**
 * @ingroup WorldEd
 * @brief Info about TMX tileset
 */
struct STMXTileset
{
	uint32							firstGID;		/**< First ID of tile in set */
	uint32							lastGID;		/**< Last ID of tile in set */
	Vector2D						tileSize;		/**< Size of tile */
	Vector2D						tileOffset;		/**< Offset of tile */
	TAssetHandle<CMaterial>			material;		/**< Material of tileset */
	std::vector< RectFloat_t >		textureRects;	/**< Array of rects with tiles */
};

 /**
  * @ingroup WorldEd
  * Commandlet for generate table of contents
  */
class CCookPackagesCommandlet : public ÑBaseCommandlet
{
	DECLARE_CLASS( CCookPackagesCommandlet, ÑBaseCommandlet )

public:
	/**
	 * Constructor
	 */
	CCookPackagesCommandlet();

	/**
	 * Main method of execute commandlet
	 *
	 * @param[in] InCommands Arguments of start commandlet
	 * @return Return true if commandlet executed is seccussed, else returning false
	 */
	virtual bool Main( const std::wstring& InCommand ) override;

	/**
	 * Convert texture
	 * 
	 * @param InPath		Path to source texture
	 * @param InName		Name of texture. If is empty string, name getting from path
	 * @return Return pointer to converted texture
	 */
	TSharedPtr<CTexture2D> ConvertTexture2D( const std::wstring& InPath, const std::wstring& InName = TEXT( "" ) );

	/**
	 * Convert audio bank
	 *
	 * @param InPath		Path to source audio bank
	 * @param InName		Name of audio bank. If is empty string, name getting from path
	 * @return Return pointer to converted audio bank
	 */
	TSharedPtr<CAudioBank> ConvertAudioBank( const std::wstring& InPath, const std::wstring& InName = TEXT( "" ) );

	/**
	 * Cook audio bank
	 *
	 * @param InAssetRef Reference to asset in format <PackageName>:<AssetName>
	 * @param OutAudioBank Output cooked audio bank
	 * @return Return true if seccussed cook, else returning false
	 */
	FORCEINLINE bool CookAudioBank( const std::wstring& InAssetRef, TAssetHandle<CAudioBank>& OutAudioBank )
	{
		std::wstring		packageName;
		std::wstring		assetName;
		EAssetType			assetType;
		SResourceInfo		resourceInfo;

		ParseReferenceToAsset( InAssetRef, packageName, assetName, assetType );
		if ( assetType != AT_AudioBank )
		{
			appErrorf( TEXT( "Asset '%s' is not audio bank" ), InAssetRef.c_str() );
			return false;
		}

		if ( !FindResource( audiosMap, packageName, assetName, resourceInfo ) )
		{
			appErrorf( TEXT( "Audio bank '%s' not founded" ), InAssetRef.c_str() );
			return false;
		}

		bool	result = CookAudioBank( resourceInfo, OutAudioBank );
		if ( !result )
		{
			appErrorf( TEXT( "Failed cooking audio bank '%s'" ), InAssetRef.c_str() );
			return false;
		}

		return true;
	}

	/**
	 * Cook physics material
	 *
	 * @param InAssetRef Reference to asset in format <PackageName>:<AssetName>
	 * @param OutPhysMaterial Output cooked physics material
	 * @return Return true if seccussed cook, else returning false
	 */
	FORCEINLINE bool CookPhysMaterial( const std::wstring& InAssetRef, TAssetHandle<CPhysicsMaterial>& OutPhysMaterial )
	{
		std::wstring		packageName;
		std::wstring		assetName;
		EAssetType			assetType;
		SResourceInfo		resourceInfo;

		ParseReferenceToAsset( InAssetRef, packageName, assetName, assetType );
		if ( assetType != AT_PhysicsMaterial )
		{
			appErrorf( TEXT( "Asset '%s' is not physics material" ), InAssetRef.c_str() );
			return false;
		}

		if ( !FindResource( physMaterialsMap, packageName, assetName, resourceInfo ) )
		{
			appErrorf( TEXT( "Physics material '%s' not founded" ), InAssetRef.c_str() );
			return false;
		}

		bool	result = CookPhysMaterial( resourceInfo, OutPhysMaterial );
		if ( !result )
		{
			appErrorf( TEXT( "Failed cooking physics material '%s'" ), InAssetRef.c_str() );
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

	/**
	 * Get supported physics material extensions
	 * @retun Return array of supported extensions (e.g "png", "jpg", etc)
	 */
	FORCEINLINE static std::vector< std::wstring > GetSupportedPhysMaterialExtensins()
	{
		static std::vector< std::wstring >		supportedExtensions =
		{
			TEXT( "pmt" )
		};
		return supportedExtensions;
	}

	/**
	 * Is supported physics material extension
	 *
	 * @param InExtension Physics material extensions ("png", "jpg", etc)
	 * @return Return true if supported, else return false
	 */
	FORCEINLINE static bool IsSupportedPhysMaterialExtension( const std::wstring& InExtension )
	{
		return IsSupportedExtension( InExtension, GetSupportedPhysMaterialExtensins() );
	}

private:
	/**
	 * Struct info about resource
	 */
	struct SResourceInfo
	{
		std::wstring		packageName;		/**< Name of package */
		std::wstring		filename;			/**< Name of file */
		std::wstring		path;				/**< Path to resource */
		bool				bAlwaysCook;		/**< Is need always cook */
	};

	/**
	 * Struct for containing of extensions for all resource type
	 */
	struct SExtensionInfo
	{
		std::wstring		package;		/**< Package extension */
		std::wstring		map;			/**< Map extension */
	};

	/**
	 * Typedef map of resources
	 */
	typedef std::unordered_map< std::wstring, std::unordered_map< std::wstring, SResourceInfo > >		ResourceMap_t;

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
	bool LoadTMXTilests( const tmx::Map& InTMXMap, std::vector< STMXTileset >& OutTilesets );

	/**
	 * @brief Spawn tiles in world
	 * 
	 * @param InTMXMap TMX map
	 * @param InTilesets Array of tilesets
	 */
	void SpawnTilesInWorld( const tmx::Map& InTMXMap, const std::vector< STMXTileset >& InTilesets );

	/**
	 * @brief Spawn actors in world
	 * 
	 * @param InTMXMap TMX map
	 * @param InTilesets Array of tilesets
	 */
	void SpawnActorsInWorld( const tmx::Map& InTMXMap, const std::vector< STMXTileset >& InTileset );

	/**
	 * Find tileset by ID tile
	 *
	 * @param InTilesets Array of tilesets
	 * @param InIDTile ID tile
	 * @param OutTileset Output finded tileset info
	 * @param OutTextureRect Output texture rect for InIDTile
	 * @return Return true if tileset is finded, else returning false
	 */
	bool FindTileset( const std::vector< STMXTileset >& InTilesets, uint32 InIDTile, STMXTileset& OutTileset, RectFloat_t& OutTextureRect ) const;

	/**
	 * Indexing all resources in engine and game. Recursive method
	 * 
	 * @param InRootDir Path to root directory
	 * @param InIsRootDir Is door door
	 * @param InIsAlwaysCookDir Is always cook dir
	 * @param InParentDirName Parent directory name
	 */
	void IndexingResources( const std::wstring& InRootDir, bool InIsRootDir = false, bool InIsAlwaysCookDir = false, const std::wstring& InPackageSufix = GGameName );

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
	bool CookMap( const SResourceInfo& InMapInfo );

	/**
	 * Cook material
	 * 
	 * @param InMaterialInfo Info about resource
	 * @param OutMaterial Output cooked material
	 * @return Return true if seccussed cook, else returning false
	 */
	bool CookMaterial( const SResourceInfo& InMaterialInfo, TAssetHandle<CMaterial>& OutMaterial );

	/**
	 * Cook texture 2D
	 *
	 * @param InTexture2DInfo Info about resource
	 * @param OutTexture2D Output cooked texture
	 * @return Return true if seccussed cook, else returning false
	 */
	bool CookTexture2D( const SResourceInfo& InTexture2DInfo, TAssetHandle<CTexture2D>& OutTexture2D );

	/**
	 * Cook audio bank
	 * 
	 * @param InAudioBankInfo Info about audio bank
	 * @param OutAudioBank Output cooked audio bank
	 * @return Return true if seccussed cook, else returning false
	 */
	bool CookAudioBank( const SResourceInfo& InAudioBankInfo, TAssetHandle<CAudioBank>& OutAudioBank );

	/**
	 * Cook physics material
	 *
	 * @param InPhysMaterialInfo Info about physics material
	 * @param OutPhysMaterial Output cooked physics material
	 * @return Return true if seccussed cook, else returning false
	 */
	bool CookPhysMaterial( const SResourceInfo& InPhysMaterialInfo, TAssetHandle<CPhysicsMaterial>& OutPhysMaterial );

	/**
	 * Insert resource to list
	 * 
	 * @param InOutResourceMap Resource map
	 * @param InPackageName Package name
	 * @param InFilename Filename
	 * @param InResourceInfo Resource info
	 */
	void InsertResourceToList( ResourceMap_t& InOutResourceMap, const std::wstring& InPackageName, const std::wstring& InFilename, const SResourceInfo& InResourceInfo );

	/**
	 * Find resource
	 * 
	 * @param InResourceMap Resource map
	 * @param InPackageName Package name
	 * @param InAssetName Asset name
	 * @param OutResourceInfo Output founded resource
	 * @return Return true if resource is founded, else returning false
	 */
	FORCEINLINE bool FindResource( const ResourceMap_t& InResourceMap, const std::wstring& InPackageName, const std::wstring& InAssetName, SResourceInfo& OutResourceInfo ) const
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
	bool SaveToPackage( const SResourceInfo& InResourceInfo, const TAssetHandle<CAsset>& InAsset );

	SExtensionInfo											extensionInfo;			/**< Info about extensions of output formats */
	ResourceMap_t											texturesMap;			/**< All textures */
	ResourceMap_t											materialsMap;			/**< All materials */
	ResourceMap_t											audiosMap;				/**< All audios */
	ResourceMap_t											physMaterialsMap;		/**< All physics materials */
	std::unordered_map< std::wstring, SResourceInfo >		mapsMap;				/**< All maps */
	CShaderCache											shaderCache;			/**< Cooked shader cache */
	EShaderPlatform											cookedShaderPlatform;	/**< Cooked shader platform */
	EPlatformType											cookedPlatform;			/**< Cooked platform */
};

#endif // !COOKPACKAGESCOMMANDLET_H