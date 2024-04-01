/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ASSETSIMPORT_H
#define ASSETSIMPORT_H

#include <string>
#include <assimp/scene.h>

#include "Misc/SharedPointer.h"
#include "Render/Texture.h"
#include "System/AudioBank.h"
#include "Render/StaticMesh.h"
#include "Windows/ImportSettingsDialogs.h"

// Forward declarations
struct aiNode;
struct aiMesh;
struct aiScene;

/**
 * @ingroup WorldEd
 * @brief Importer of a 2D texture
 */
class CTexture2DImporter
{
public:
	/**
	 * @brief Import a texture 2D
	 * 
	 * @param InPath		Path to a source file
	 * @param OutResult		Output array with imported assets
	 * @param OutError		Output error message
	 * @return Return TRUE if asset seccussed imported, otherwise will return FALSE
	 */
	static bool Import( const std::wstring& InPath, std::vector<TSharedPtr<CAsset>>& OutResult, std::wstring& OutError );

	/**
	 * @brief Reimport a texture 2D
	 * 
	 * @param InTexture2D	Texture 2D
	 * @param OutError		Output error message
	 * @return Return TRUE if seccussed reimported, otherwise will return FALSE
	 */
	static bool Reimport( const TSharedPtr<CAsset>& InTexture2D, std::wstring& OutError );

	/**
	 * @brief Get supported texture extensions
	 * @return Return list of supported texture extensions
	 */
	FORCEINLINE static const std::vector< std::wstring >& GetSupportedExtensions()
	{
		static std::vector<std::wstring>		supportedExtensions =
		{
			TEXT( "png" ),
			TEXT( "jpg" ),
			TEXT( "jpeg" ),
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
 * @brief Importer of a audio banks
 */
class CAudioBankImporter
{
public:
	/**
	 * @brief Import audio bank
	 *
	 * @param InPath		Path to source file
	 * @param OutResult		Output array with imported assets
	 * @param OutError		Output error message
	 * @return Return TRUE if asset seccussed imported, otherwise will return FALSE
	 */
	static bool Import( const std::wstring& InPath, std::vector<TSharedPtr<CAsset>>& OutResult, std::wstring& OutError );

	/**
	 * @brief Reimport audio bank
	 *
	 * @param InAudioBank	Audio bank
	 * @param OutError		Output error message
	 * @return Return TRUE if successes reimported, otherwise will return FALSE
	 */
	static bool Reimport( const TSharedPtr<CAsset>& InAudioBank, std::wstring& OutError );

	/**
	 * @brief Get supported audio extensions
	 * @return Return list of supported audio extensions
	 */
	FORCEINLINE static const std::vector<std::wstring>& GetSupportedExtensions()
	{
		static std::vector<std::wstring>		supportedExtensions =
		{
			TEXT( "ogg" )
		};
		return supportedExtensions;
	}
};

/**
 * @ingroup WorldEd
 * @brief Importer of a static meshes
 */
class CStaticMeshImporter
{
public:
	/**
	 * @brief Import static mesh
	 *
	 * @param InPath			Path to source file
	 * @param OutResult			Output array with imported assets
	 * @param OutError			Output error message
	 * @return Return TRUE if asset seccussed imported, otherwise will return FALSE
	 */
	static bool Import( const std::wstring& InPath, std::vector<TSharedPtr<CAsset>>& OutResult, std::wstring& OutError );

	/**
	 * @brief Reimport static mesh
	 *
	 * @param InStaticMesh		Static mesh
	 * @param OutError			Output error message
	 * @return Return TRUE if successes reimported, otherwise will return FALSE
	 */
	static bool Reimport( const TSharedPtr<CAsset>& InStaticMesh, std::wstring& OutError );

	/**
	 * @brief Show import settings
	 *
	 * @param InOwner			Owner ImGUI layer
	 * @param InEvent			Synchronize event, need trigger it when dialog is closed
	 * @param OutResult			Result
	 * @param OutResult			Him is TRUE when user pressed okay
	 */
	static void ShowImportSettings( class CImGUILayer* InOwner, CEvent& InEvent, CAssetFactory::EResultShowImportSettings& OutResult );

	/**
	 * @brief Get supported static mesh extensions
	 * @return Return list of supported static mesh extensions
	 */
	static const std::vector<std::wstring>& GetSupportedExtensions();

	/**
	 * @brief Set import settings
	 * @param InImportSettings		New import settings	
	 */
	FORCEINLINE static void SetImportSettings( const CStaticMeshImportSettingsDialog::ImportSettings& InImportSettings )
	{
		importSettings = InImportSettings;
	}

private:
	/**
	 * @brief Container for Assimp mesh
	 */
	struct AiMesh
	{
		/**
		 * @brief Constructor
		 */
		FORCEINLINE AiMesh()
		{}

		/**
		 * @brief Constructor
		 *
		 * @param InTransformation	Transform matrix
		 * @param InMesh			Pointer to Assimp mesh
		 */
		FORCEINLINE AiMesh( const aiMatrix4x4& InTransformation, struct aiMesh* InMesh )
			: transformation( InTransformation )
			, mesh( InMesh )
		{}

		aiMatrix4x4		transformation;		/**< Transformation matrix */
		aiMesh*			mesh;				/**< Pointer to Assimp mesh */
	};
	
	/**
	 * @brief Mesh data
	 */
	struct MeshData
	{
		std::wstring							name;			/**< Mesh name */
		std::vector<StaticMeshVertexType>		verteces;		/**< Array of mesh verteces */
		std::vector<uint32>						indeces;		/**< Array of indeces */
		StaticMeshSurface						surface;		/**< Mesh surface */
		TAssetHandle<CMaterial>					material;		/**< Material */
		uint32									materialId;		/**< Assimp material Id */
	};

	/**
	 * @brief Typedef container AiMeshes
	 */
	typedef std::unordered_map<uint32, std::vector<AiMesh>>			AiMeshesMap_t;

	/**
	 * @brief Fill array meshes from Assimp root scene
	 *
	 * @param InNode	Pointer to Assimp node
	 * @param InScene	Pointer to Assimp scene
	 * @param OutMeshes Array filled from Assimp scene
	 */
	static void ProcessNode( aiNode* InNode, const aiScene* InScene, AiMeshesMap_t& OutMeshes );

	/**
	 * @brief Parse meshes from file
	 * Function parse meshes from file with import settings in variable 'importSettings'
	 * 
	 * @param InPath		Path to source file in format '<PathToFile>' or '<PathToFile>?<MeshNameInFile>', last variant for parse concrete mesh
	 * @param OutResult		Output array with meshes data
	 * @param OutError		Output error message
	 * @return Return TRUE if parsed succussed, otherwise will return FALSE
	 */
	static bool ParseMeshes( const std::wstring& InPath, std::vector<MeshData>& OutResult, std::wstring& OutError );

	/**
	 * @brief Change axis up in vector
	 *
	 * @param InOutVector	Input and output vector
	 * @param InAxisUp		Axis up
	 */
	FORCEINLINE static void ChangeAxisUpInVector( aiVector3D& InOutVector, CStaticMeshImportSettingsDialog::EAxisUp InAxisUp )
	{
		switch ( InAxisUp )
		{
		case CStaticMeshImportSettingsDialog::AU_MinusX:
			InOutVector.y = -InOutVector.y;

		case CStaticMeshImportSettingsDialog::AU_PlusX:
			Swap( InOutVector.y, InOutVector.x );
			break;

		case CStaticMeshImportSettingsDialog::AU_MinusY:
			InOutVector.y = -InOutVector.y;

		case CStaticMeshImportSettingsDialog::AU_PlusY:
			break;

		case CStaticMeshImportSettingsDialog::AU_MinusZ:
			InOutVector.y = -InOutVector.y;

		case CStaticMeshImportSettingsDialog::AU_PlusZ:
			Swap( InOutVector.y, InOutVector.z );
			break;
		}
	}

	static CStaticMeshImportSettingsDialog::ImportSettings		importSettings;		/**< Import settings */
};

#endif // !ASSETSIMPORT_H