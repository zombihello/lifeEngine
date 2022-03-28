/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef IMPORTTMXCOMMANDLET_H
#define IMPORTTMXCOMMANDLET_H

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
 * Commandlet for import TMX maps
 */
class LImportTMXCommandlet : public LBaseCommandlet
{
	DECLARE_CLASS( LImportTMXCommandlet, LBaseCommandlet )

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
	 * @brief Loading tilesets from TMX
	 * @return Return true if tileset is seccussed loaded, else returning false
	 */
	bool LoadTMXTilests();

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

#endif // !IMPORTTMXCOMMANDLET_H
