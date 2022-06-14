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
#include <unordered_map>
#include <qstring.h>
#include <assimp/scene.h>

#include "Render/Texture.h"
#include "System/AudioBank.h"
#include "Render/StaticMesh.h"
#include "Dialogs/ImportSettingsMeshDialog.h"
#include "Core.h"

// Forward declarations
struct aiNode;
struct aiMesh;
struct aiScene;

/**
 * @ingroup WorldEd
 * Generic helper for importer assets
 */
class CHelperAssetImporter
{
public:
	/**
	 * Enumeration of flags for making filter supported extensions (see CHelperAssetImporter::MakeFilterOfSupportedExtensions)
	 */
	enum EExtensionType
	{
		ET_Texture2D	= 1 << 0,										/**< Add in filter extensions for texture 2D */
		ET_AudioBank	= 1 << 1,										/**< Add in filter extensions for audio bank */
		ET_StaticMesh	= 1 << 2,										/**< Add in filter extensions for static mesh */
		ET_All			= ET_Texture2D | ET_AudioBank | ET_StaticMesh	/**< Add in filter all assets type */
	};

	/**
	 * Enumeration of import result
	 */
	enum EImportResult
	{
		IR_Seccussed,		/**< Asset is imported seccussed */
		IR_Error,			/**< Asset not imported due to error */
		IR_AlreadyExist,	/**< Asset not imported because in package already exist asset with name */
		IR_Cancel			/**< Import asset canceled by user */
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
	 * @param InFlags		Flags for making filter (see CHelperAssetImporter::EExtensionType)
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
	 * @return Return import result
	 */
	static EImportResult Import( const QString& InPath, CPackage* InPackage, TAssetHandle<CAsset>& OutAsset, std::wstring& OutError, bool InIsForceImport = false );

	/**
	 * Reimport asset
	 *
	 * @param InAsset		Asset for reimport
	 * @param OutError		Output error message
	 * @return Return TRUE if seccussed reimported, else FALSE
	 */
	static bool Reimport( const TAssetHandle<CAsset>& InAsset, std::wstring& OutError );

	/**
	 * Clear cached settings of import/reimport
	 */
	static void ClearCachedSettings();

private:
	/**
	 * Struct for containing cache of import settings
	 */
	template<typename TImportSettings>
	struct SCacheImportSettings
	{
		/**
		 * Constructor
		 */
		SCacheImportSettings()
			: bIsCached( false )
		{}

		/**
		 * Reset cached import settings
		 */
		FORCEINLINE void Reset()
		{
			bIsCached = false;
		}

		/**
		 * Set import settings
		 * 
		 * @param InImportSettings	Import settings
		 */
		FORCEINLINE void Set( const TImportSettings& InImportSettings )
		{
			importSettings = InImportSettings;
		}

		/**
		 * Get import settings
		 * @return Return import settings
		 */
		FORCEINLINE const TImportSettings& Get() const
		{
			return importSettings;
		}

		/**
		 * Is cached
		 * @return Return TRUE if import settings is cached, else return FALSE
		 */
		FORCEINLINE bool IsCached() const
		{
			return bIsCached;
		}

	private:
		TImportSettings		importSettings;		/**< Import settings */
		bool				bIsCached;			/**< Is cached */
	};

	static SCacheImportSettings<WeImportSettingsMeshDialog::SImportSettings>		meshImportSettings;		/**< Mesh import settings */
};

/**
 * @ingroup WorldEd
 * Class for importer textures 2D
 */
class CTexture2DImporter
{
public:
	/**
	 * Import texture 2D
	 *
	 * @param InPath		Path to source file
	 * @param OutError		Output error message
	 * @return Return imported texture 2D, if failed return NULL
	 */
	static TSharedPtr<CTexture2D> Import( const std::wstring& InPath, std::wstring& OutError );

	/**
	 * Reimport texture 2D
	 *
	 * @param InTexture2D	Texture 2D
	 * @param OutError		Output error message
	 * @return Return TRUE if seccussed reimported, else return FALSE
	 */
	static bool Reimport( const TSharedPtr<CTexture2D>& InTexture2D, std::wstring& OutError );

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
class CAudioBankImporter
{
public:
	/**
	 * Import audio bank
	 *
	 * @param InPath		Path to source file
	 * @param OutError		Output error message
	 * @return Return imported audio bank, if failed return NULL
	 */
	static TSharedPtr<CAudioBank> Import( const std::wstring& InPath, std::wstring& OutError );

	/**
	 * Reimport audio bank
	 *
	 * @param InAudioBank	Audio bank
	 * @param OutError		Output error message
	 * @return Return TRUE if seccussed reimported, else return FALSE
	 */
	static bool Reimport( const TSharedPtr<CAudioBank>& InAudioBank, std::wstring& OutError );

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

/**
 * @ingroup WorldEd
 * Class for importer static mesh
 */
class CStaticMeshImporter
{
public:
	/**
	 * Import static mesh
	 *
	 * @param InPath			Path to source file
	 * @param InImportSettings	Import settings
	 * @param OutError			Output error message
	 * @return Return imported static mesh, if failed return NULL
	 */
	static TSharedPtr<CStaticMesh> Import( const std::wstring& InPath, const WeImportSettingsMeshDialog::SImportSettings& InImportSettings, std::wstring& OutError );

	/**
	 * Reimport static mesh
	 *
	 * @param InStaticMesh		Static mesh
	 * @param InImportSettings	Import settings
	 * @param OutError			Output error message
	 * @return Return TRUE if seccussed reimported, else return FALSE
	 */
	static bool Reimport( const TSharedPtr<CStaticMesh>& InStaticMesh, const WeImportSettingsMeshDialog::SImportSettings& InImportSettings, std::wstring& OutError );

	/**
	 * Get supported static mesh extensions
	 * @return Return list of supported static mesh extensions
	 */
	static const std::vector< std::wstring >& GetSupportedExtensions();

private:
	/**
	 * Container for Assimp mesh
	 */
	struct SAiMesh
	{
		/**
		 * Constructor
		 */
		FORCEINLINE SAiMesh()
		{}

		/**
		 * Constructor
		 *
		 * @param InTransformation	Transform matrix
		 * @param InMesh			Pointer to Assimp mesh
		 */
		FORCEINLINE SAiMesh( const aiMatrix4x4& InTransformation, struct aiMesh* InMesh )
			: transformation( InTransformation )
			, mesh( InMesh )
		{}

		aiMatrix4x4		transformation;		/**< Transformation matrix */
		aiMesh*			mesh;				/**< Pointer to Assimp mesh */
	};

	/**
	 * Typedef container AiMeshes
	 */
	typedef std::unordered_map< uint32, std::vector<SAiMesh> >			AiMeshesMap_t;

	/**
	 * Fill array meshes from Assimp root scene
	 *
	 * @param[in] InNode Pointer to Assimp node
	 * @param[in] InScene Pointer to Assimp scene
	 * @param[out] OutMeshes Array filled from Assimp scene
	 */
	static void ProcessNode( aiNode* InNode, const aiScene* InScene, AiMeshesMap_t& OutMeshes );

	/**
	 * Change axis up in vector
	 * 
	 * @param InOutVector	Input and output vector
	 * @param InAxisUp		Axis up
	 */
	FORCEINLINE static void ChangeAxisUpInVector( aiVector3D& InOutVector, WeImportSettingsMeshDialog::EAxis InAxisUp )
	{
		switch ( InAxisUp )
		{
		case WeImportSettingsMeshDialog::A_MinusX:
			InOutVector.y	= -InOutVector.y;

		case WeImportSettingsMeshDialog::A_PlusX:
			Swap( InOutVector.y, InOutVector.x );
			break;

		case WeImportSettingsMeshDialog::A_MinusY:
			InOutVector.y	= -InOutVector.y;

		case WeImportSettingsMeshDialog::A_PlusY:
			break;

		case WeImportSettingsMeshDialog::A_MinusZ:
			InOutVector.y	= -InOutVector.y;

		case WeImportSettingsMeshDialog::A_PlusZ:
			Swap( InOutVector.y, InOutVector.z );
			break;
		}
	}
};

#endif // !ASSETSIMPORT_H
