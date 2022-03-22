/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef COMPILETMXCOMMANDLET_H
#define COMPILETMXCOMMANDLET_H

#include <tmxlite/Map.hpp>
#include <tmxlite/Tileset.hpp>
#include <string>
#include <vector>

#include "Commandlets/BaseCommandlet.h"
#include "Math/Math.h"
#include "Math/Rect.h"
#include "Render/Texture.h"
#include "Render/Material.h"

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
 * Commandlet for compile TMX maps
 */
class LCompileTMXCommandlet : public LBaseCommandlet
{
	DECLARE_CLASS( LCompileTMXCommandlet, LBaseCommandlet )

public:
	/**
	 * Main method of execute commandlet
	 *
	 * @param[in] InCommands Arguments of start commandlet
	 * @return Return true if commandlet executed is seccussed, else returning false
	 */
	virtual bool Main( const std::wstring& InCommand ) override;

private:
	/**
	 * @brief Convert tileset to engine format
	 *
	 * @param InTileset TMX tileset
	 * @param InPackageDir Path to directory for new packages
	 * @param OutTilesetTexture Output converted tileset texture
	 * @param OutTilesetMaterial Output converted tileset material
	 * @return Return true if tileset is seccussed converted, else returning false
	 */
	bool ConvertTileset( const tmx::Tileset& InTileset, const std::wstring& InPackageDir, FTexture2DRef& OutTilesetTexture, FMaterialRef& OutTilesetMaterial );

	/**
	 * @brief Loading tilesets from TMX
	 *
	 * @param InPackageDir Path to directory for new packages
	 * @return Return true if tileset is seccussed loaded, else returning false
	 */
	bool LoadTMXTilests( const std::wstring& InPackageDir );

	/**
	 * @brief Spawn tiles in world
	 */
	void SpawnTilesInWorld();

	/**
	 * Find tileset by ID tile
	 * 
	 * @param InIDTile ID tile
	 * @param OutTileset Output finded tileset info
	 * @param OutTextureRect Output texture rect for InIDTile
	 * @return Return true if tileset is finded, else returning false
	 */
	bool FindTileset( uint32 InIDTile, FTMXTileset& OutTileset, FRectFloat& OutTextureRect ) const;

	tmx::Map*						tmxMap;			/**< TMX map parser */
	std::vector< FTMXTileset >		tilesets;		/**< Array of tilesets */
};

#endif // !COMPILETMXCOMMANDLET_H
