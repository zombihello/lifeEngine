#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/Object.hpp>
#include <string>
#include <vector>

#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Containers/StringConv.h"
#include "Containers/String.h"
#include "Commandlets/CompileTMXCommandlet.h"
#include "System/BaseFileSystem.h"
#include "System/Archive.h"
#include "System/Package.h"
#include "System/World.h"
#include "Render/Texture.h"
#include "Render/Material.h"
#include "Render/Shaders/BasePassShader.h"
#include "Actors/Sprite.h"
#include "Actors/PlayerStart.h"

IMPLEMENT_CLASS( LCompileTMXCommandlet )

bool LCompileTMXCommandlet::Main( const std::wstring& InCommand )
{
	std::wstring			srcFilename;
	std::wstring			dstFilename;
	std::wstring			dstNewPackages;

	// Parse arguments
	{
		std::vector< std::wstring >		tokens;
		std::vector< std::wstring >		switches;
		appParseCommandLine( InCommand.c_str(), tokens, switches );

		for ( uint32 index = 0, count = ( uint32 )switches.size(); index < count; ++index )
		{
			const std::wstring&			param = switches[ index ];
			const std::wstring&			token = tokens[ index + 2 ];

			// Source filename
			if ( param == TEXT( "src" ) || param == TEXT( "source" ) )
			{
				srcFilename = token;
			}

			// Destination filename
			else if ( param == TEXT( "dst" ) || param == TEXT( "destination" ) )
			{
				dstFilename = token;
			}

			// Destination new packages
			else if ( param == TEXT( "dstNewPackages" ) )
			{
				dstNewPackages = token;
			}
		}
	}

	// Parse TMX map
	try
	{
		// Load TMX map
		tmxMap = new tmx::Map();
		if ( !tmxMap->load( TCHAR_TO_ANSI( srcFilename.c_str() ) ) )
		{
			throw;
		}

		// Loading tilesets from TMX
		if ( !LoadTMXTilests( dstNewPackages ) )
		{
			throw;
		}

		// Clear world for spawn new actors
		GWorld->CleanupWorld();
		GWorld->SpawnActor< APlayerStart >( FVector( 0.f, 0.f, -10.f ) );

		// Spawn tiles
		SpawnTilesInWorld();

		// Serialize world to HDD
		FArchive*		archive = GFileSystem->CreateFileWriter( dstFilename );
		if ( !archive )
		{
			throw;
		}

		archive->SetType( AT_World );
		archive->SerializeHeader();
		GWorld->Serialize( *archive );
		delete archive;
	}
	catch ( ... )
	{
		if ( tmxMap )
		{
			delete tmxMap;
		}
		return false;
	}

	delete tmxMap;
	return true;
}

void LCompileTMXCommandlet::SpawnTilesInWorld()
{
	check( tmxMap );
	const std::vector< tmx::Layer::Ptr >&		tmxLayers = tmxMap->getLayers();
	const tmx::Vector2u&						mapSize = tmxMap->getTileCount();
	const tmx::Vector2u&						mapTileSize = tmxMap->getTileSize();

	for ( uint32 indexLayer = 0, countLayers = tmxLayers.size(); indexLayer < countLayers; ++indexLayer )
	{
		if ( tmxLayers[ indexLayer ]->getType() == tmx::Layer::Type::Tile )
		{
			tmx::TileLayer*									tmxLayer = ( tmx::TileLayer* )tmxLayers[ indexLayer ].get();
			const std::vector< tmx::TileLayer::Tile >&		tmxTiles = tmxLayer->getTiles();

			float		x = 0.f;
			float		y = 0.f;
			float		maxX = mapSize.x;
			for ( int32 indexTile = tmxTiles.size()-1; indexTile >= 0; --indexTile )
			{
				const tmx::TileLayer::Tile&			tile = tmxTiles[ indexTile ];
				if ( tile.ID != 0 )
				{
					// Find material for tile
					FTMXTileset		tileset;
					FRectFloat		textureRect;
					bool			result = FindTileset( tile.ID, tileset, textureRect );
					check( result );

					ASprite*				sprite = GWorld->SpawnActor< ASprite >( FVector( x * mapTileSize.x / tileset.tileOffset.x, y * mapTileSize.y / tileset.tileOffset.y, indexLayer ), FRotator( 0.f, 180.f, 0.f ) );
					LSpriteComponent*		spriteComponent = sprite->GetSpriteComponent();
					spriteComponent->SetType( ST_Static );
					spriteComponent->SetMaterial( tileset.material );
					spriteComponent->SetSpriteSize( FVector2D( mapTileSize.x, mapTileSize.y ) );
					spriteComponent->SetTextureRect( textureRect );
				}

				--x;
				if ( -x >= maxX )
				{
					x = 0.f;
					++y;
				}
			}
		}
	}
}

bool LCompileTMXCommandlet::FindTileset( uint32 InIDTile, FTMXTileset& OutTileset, FRectFloat& OutTextureRect ) const
{
	for ( uint32 indexTileset = 0, countTilesets = tilesets.size(); indexTileset < countTilesets; ++indexTileset )
	{
		const FTMXTileset&		tileset = tilesets[ indexTileset ];
		if ( InIDTile < tileset.firstGID || InIDTile > tileset.lastGID )
		{
			continue;
		}

		OutTileset = tileset;
		OutTextureRect = tileset.textureRects[ InIDTile - tileset.firstGID ];
		return true;
	}

	return false;
}

bool LCompileTMXCommandlet::LoadTMXTilests( const std::wstring& InPackageDir )
{
	check( tmxMap );
	const std::vector< tmx::Tileset >&		tmxTilesets = tmxMap->getTilesets();
	const tmx::Vector2u&					mapTileSize = tmxMap->getTileSize();

	for ( uint32 index = 0, count = tmxTilesets.size(); index < count; ++index )
	{
		const tmx::Tileset&		tmxTileset			= tmxTilesets[ index ];
		tmx::Vector2u			tmxTilesetSize		= tmxTileset.getImageSize();
		tmx::Vector2u			tmxTileSize			= tmxTileset.getTileSize();
		int32					countCulumns		= tmxTilesetSize.x / tmxTileSize.x;
		int32					countRows			= tmxTilesetSize.y / tmxTileSize.y;
		FTexture2DRef			tilesetTexture;
		FMaterialRef			tilesetMaterial;

		// Convert tileset to engine format
		if ( !ConvertTileset( tmxTileset, InPackageDir, tilesetTexture, tilesetMaterial ) )
		{
			return false;
		}

		// Save info about tileset
		FTMXTileset			tileset;
		tileset.firstGID	= tmxTileset.getFirstGID();
		tileset.lastGID		= tmxTileset.getLastGID();
		tileset.material	= tilesetMaterial;
		tileset.tileSize	= FVector2D( tmxTileSize.x, tmxTileSize.y );
		tileset.tileOffset	= FVector2D( ( float )tmxTileSize.x / mapTileSize.x, ( float )tmxTileSize.y / mapTileSize.y );

		// Precalculates rectangles in tileset
		for ( uint32 y = 0; y < countRows; ++y )
		{
			for ( uint32 x = 0; x < countCulumns; ++x )
			{
				FRectFloat		rect;
				rect.width		= tmxTileSize.x;
				rect.height		= tmxTileSize.y;
				rect.top		= y * tmxTileSize.y;
				rect.left		= x * tmxTileSize.x;

				// Convert texture rect in range from 0 to 1
				rect.width		/= tmxTilesetSize.x;
				rect.height		/= tmxTilesetSize.y;
				rect.left		/= tmxTilesetSize.x;
				rect.top		/= tmxTilesetSize.y;

				tileset.textureRects.push_back( rect );
			}
		}

		tilesets.push_back( tileset );
	}

	return true;
}

bool LCompileTMXCommandlet::ConvertTileset( const tmx::Tileset &InTileset, const std::wstring &InPackageDir, FTexture2DRef& OutTilesetTexture, FMaterialRef& OutTilesetMaterial )
{
	// Parse from tilesetName package and asset name
	const std::string&		tilesetName = InTileset.getName();
	std::wstring			packageName;
	std::wstring			assetName;
	if ( !ParseReferenceToAsset( ANSI_TO_TCHAR( tilesetName.c_str() ), packageName, assetName ) )
	{
		return false;
	}

	// If asset is exist - we exit from method
	OutTilesetTexture = ( FTexture2DRef )GPackageManager->FindAsset( ANSI_TO_TCHAR( tilesetName.c_str() ), AT_Unknown );
	OutTilesetMaterial = ( FMaterialRef )GPackageManager->FindAsset( FString::Format( TEXT( "%s:%s_Mat" ), packageName.c_str(), assetName.c_str() ), AT_Unknown );
	if ( OutTilesetTexture && OutTilesetMaterial )
	{
		return true;
	}

	// Convert texture
	bool				result = false;
	std::wstring		packagePath = FString::Format( TEXT( "%s/%s.lpak" ), InPackageDir.c_str(), packageName.c_str() );
	if ( !OutTilesetTexture )
	{
		const std::string&		tilesetPath = InTileset.getImagePath();
		LBaseCommandlet::ExecCommandlet( FString::Format( TEXT( "ImportTexture -src \"%s\" -dst \"%s\" -name \"%s\"" ), ANSI_TO_TCHAR( tilesetPath.c_str() ), packagePath.c_str(), assetName.c_str() ), 0, &result );
	}

	// If texture is seccussed created - we make material for use on sprites
	if ( result && !OutTilesetMaterial )
	{
		OutTilesetTexture = ( FTexture2DRef )GPackageManager->FindAsset( packagePath, assetName, AT_Unknown );
		check( OutTilesetTexture );

		OutTilesetMaterial = new FMaterial();
		OutTilesetMaterial->SetShader( FBasePassVertexShader::staticType );
		OutTilesetMaterial->SetShader( FBasePassPixelShader::staticType );
		OutTilesetMaterial->SetUsageFlags( MU_Sprite );
		OutTilesetMaterial->SetAssetName( FString::Format( TEXT( "%s_Mat" ), assetName.c_str() ) );
		OutTilesetMaterial->SetTextureParameterValue( TEXT( "diffuse" ), OutTilesetTexture );

		FPackageRef		package = GPackageManager->LoadPackage( packagePath, true );
		package->Add( OutTilesetMaterial );
		if ( !package->Save( packagePath ) )
		{
			result = false;
		}
	}

	// Update table of content if we added new packages
	if ( result )
	{
		LBaseCommandlet::ExecCommandlet( TEXT( "CookerSync" ) );
	}
	return result;
}
