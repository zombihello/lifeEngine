/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef IMPORTMESHCOMMANDLET_H
#define IMPORTMESHCOMMANDLET_H

#include <unordered_map>
#include <vector>

#include "Commandlets/BaseCommandlet.h"

// Forward declarations
struct aiNode;
struct aiMesh;
struct aiScene;

/**
 * @ingroup WorldEd
 * Container for Assimp mesh
 */
struct FAiMesh
{
	/**
	 * Constructor
	 */
	FAiMesh()
	{}

	/**
	 * Constructor
	 * 
	 * @param[in] InTransformation Transofmration matrix
	 * @param[in] InMesh Pointer to Assimp mesh
	 */
	FAiMesh( const aiMatrix4x4& InTransformation, struct aiMesh* InMesh )
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
typedef std::unordered_map< uint32, std::vector< FAiMesh > >			FAiMeshesMap;

 /**
  * @ingroup WorldEd
  * Commandlet for import mesh to engine
  */
class LImportMeshCommandlet : public LBaseCommandlet
{
	DECLARE_CLASS( LImportMeshCommandlet, LBaseCommandlet )

public:
	/**
	 * Main method of execute commandlet
	 *
	 * @param[in] InCommands Arguments of start commandlet
	 */
	virtual void Main( const std::wstring& InCommand );

private:
	/**
	 * Fill array meshes from Assimp root scene
	 * 
	 * @param[in] InNode Pointer to Assimp node
	 * @param[in] InScene Pointer to Assimp scene
	 * @param[out] OutMeshes Array filled from Assimp scene
	 */
	void ProcessNode( aiNode* InNode, const aiScene* InScene, FAiMeshesMap& OutMeshes );
};

#endif // !IMPORTMESHCOMMANDLET_H