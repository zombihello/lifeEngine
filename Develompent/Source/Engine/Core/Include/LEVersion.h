/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LEVERSION_H
#define LEVERSION_H

#define ENGINE_VERSION					0x040
#define ENGINE_MAJOR_VERSION			0
#define ENGINE_MINOR_VERSION			4
#define ENGINE_PATH_VERSION				0

#define ENGINE_NAME						"lifeEngine"
#define ENGINE_VERSION_STRING			"0.4.0"

/**
 * @ingroup Core
 * Cast to ensure that the construct cannot be used in a #if without compiler error.
 * This is useful as enum vales cannot be seen by the preprocessor.
 */
#define PREPROCESSOR_ENUM_PROTECT( InValue )		( ( unsigned int )( InValue ) )

/**
 * @ingroup Core
 * Package file version log
 */
enum ELifeEnginePackageVersion
{
	VER_PackageBase							= 1,					/**< Min version for serialize content*/
	VER_Assets								= 2,					/**< Added in archive assets (CTexture2D, CMaterial, etc) */
	VER_LeftOnlyPixelShaderInMaterial		= 3,					/**< Removed all shaders except pixel shader from CMaterial */
	VER_VertexFactory						= 4,					/**< Implemented vertex factory */
	VER_StaticMesh							= 5,					/**< Implemented static mesh */
	VER_ShaderMap							= 6,					/**< Added in material shader map */
	VER_AssetName							= 7,					/**< Added asset name in package */
	VER_RemovedTFC							= 8,					/**< Removed texture file cache */
	VER_CompressedZlib						= 9,					/**< Added in archive compression ZLIB */
	VER_AssetName_V2						= 10,					/**< Moved asset name from him to table of info about this asset */
	VER_GUIDPackages						= 11,					/**< Added GUID in package */
	VER_NamePackage							= 12,					/**< Added name package */
	VER_GUIDAssets							= 13,					/**< Added GUID to assets */
	VER_ShaderParameterMap 					= 14,					/**< Added shader parameter map in shader cache */
	VER_HashUInt64							= 15,					/**< Changed type of hashes from uint32 to uint64 */
	VER_AssetSourceFiles					= 16,					/**< Added in assets path to source file for reimport */
	VER_RemovedShadersTypeFromMaterial		= 17,					/**< Removed shaders type from material asset */
	VER_AssetName_V3						= 18,					/**< Moved asset name to CAsset */
	VER_AssetOnlyEditor						= 19,					/**< Added field 'bOnlyEditor' to asset */
	VER_CName								= 20,					/**< Added CName for IDs in string view */
	VER_BBoxInStaticMesh					= 21,					/**< Added CBox to a static mesh */
	VER_Mipmaps								= 22,					/**< Added mipmaps to textures */
	VER_FixedSerializeComponents			= 23,					/**< Fixed serialization components in actors */

	//
	// New versions can be added here
	//

	VER_AutomaticVersionPlusOne,									/**< This needs for automatic update last version of package */
	VER_MinVersion							= VER_PackageBase,		/**< This need for check min supported version */
};

/**
 * @ingroup Core
 * Latest version of package
 */
#define VER_PACKAGE_LATEST					( PREPROCESSOR_ENUM_PROTECT( VER_AutomaticVersionPlusOne )-1 )

/**
 * @ingroup Core
 * Archive tag in file
 */
#define ARCHIVE_FILE_TAG					0xAFEB3A00

/**
 * @ingroup Core
 * Package tag in file
 */
#define PACKAGE_FILE_TAG					0x4B50454C

#endif // !LEVERSION_H
