/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef IMPORTMESHCOMMANDLET_H
#define IMPORTMESHCOMMANDLET_H

#include <unordered_map>
#include <vector>
#include <assimp/scene.h>

#include "Commandlets/BaseCommandlet.h"
#include "Render/StaticMesh.h"

// Forward declarations
struct aiNode;
struct aiMesh;
struct aiScene;

/**
 * @ingroup WorldEd
 * Container for Assimp mesh
 */
struct AiMesh
{
	/**
	 * Constructor
	 */
	AiMesh()
	{}

	/**
	 * Constructor
	 * 
	 * @param[in] InTransformation Transform matrix
	 * @param[in] InMesh Pointer to Assimp mesh
	 */
	AiMesh( const aiMatrix4x4& InTransformation, struct aiMesh* InMesh )
		: transformation( InTransformation )
		, mesh( InMesh )
	{}

	aiMatrix4x4			transformation;		/**< Transformation matrix */
	aiMesh*				mesh;				/**< Pointer to Assimp mesh */
};

/**
 * @ingroup WorldEd
 * Typedef container AiMeshes
 */
typedef std::unordered_map< uint32, std::vector< AiMesh > >			AiMeshesMap_t;

 /**
  * @ingroup WorldEd
  * Commandlet for import mesh to engine
  */
class LE_DEPRECATED( 0.4.0, "CImportMeshCommandlet is deprecated, use CStaticMeshImporter." ) CImportMeshCommandlet : public CBaseCommandlet
{
	DECLARE_CLASS( CImportMeshCommandlet, CBaseCommandlet, CLASS_Deprecated, 0, TEXT( "WorldEd" ) )

public:
	/**
	 * Main method of execute commandlet
	 *
	 * @param InCommandLine		Command line
	 * @return Return TRUE if commandlet executed is seccussed, otherwise will return FALSE
	 */
	virtual bool Main( const CCommandLine& InCommandLine ) override;

	/**
	 * Convert static mesh (obj, 3ds, fbx, etc) to engine static mesh
	 *
	 * @param InPath Path to mesh
	 * @param InAssetName Asset name for new mesh
	 * @return Return converted static mesh, if failed returning false
	 */
	TSharedPtr<CStaticMesh> ConvertStaticMesh( const std::wstring& InPath, const std::wstring& InAssetName );

	/**
	 * Get supported meshes extensions
	 * @retun Return array of supported extensions (e.g "obj", "3ds", etc)
	 */
	FORCEINLINE static std::vector< std::wstring > GetSupportedExtensins()
	{
		static std::vector< std::wstring >		supportedExtensions =
		{
			TEXT( "obj" ),
			TEXT( "fbx" )
		};
		return supportedExtensions;
	}

	/**
	 * Is supported mesh extension
	 *
	 * @param InExtension Mesh extensions ("obj", "fbx", etc)
	 * @return Return true if supported, else return false
	 */
	FORCEINLINE static bool IsSupportedExtension( const std::wstring& InExtension )
	{
		const std::vector< std::wstring >& supportedExtensions = GetSupportedExtensins();
		for ( uint32 index = 0, count = supportedExtensions.size(); index < count; ++index )
		{
			if ( supportedExtensions[ index ] == InExtension )
			{
				return true;
			}
		}

		return false;
	}

private:
	/**
	 * Fill array meshes from Assimp root scene
	 * 
	 * @param[in] InNode Pointer to Assimp node
	 * @param[in] InScene Pointer to Assimp scene
	 * @param[out] OutMeshes Array filled from Assimp scene
	 */
	void ProcessNode( aiNode* InNode, const aiScene* InScene, AiMeshesMap_t& OutMeshes );
};

#endif // !IMPORTMESHCOMMANDLET_H