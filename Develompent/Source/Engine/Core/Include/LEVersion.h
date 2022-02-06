/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LEVERSION_H
#define LEVERSION_H

#define ENGINE_VERSION					30
#define ENGINE_MAJOR_VERSION			0
#define ENGINE_MINOR_VERSION			3
#define ENGINE_PATH_VERSION				0

#define ENGINE_NAME						"lifeEngine"
#define ENGINE_VERSION_STRING			"0.3.0"

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
	VER_Assets								= 2,					/**< Added in archive assets (FTexture2D, FMaterial, etc) */
	VER_LeftOnlyPixelShaderInMaterial		= 3,					/**< Removed all shaders except pixel shader from FMaterial */
	VER_VertexFactory						= 4,					/**< Implemented vertex factory */
	VER_StaticMesh							= 5,					/**< Implemented static mesh */
	VER_ShaderMap							= 6,					/**< Added in material shader map */
	VER_AssetName							= 7,					/**< Added asset name in package */
	VER_RemovedTFC							= 8,					/**< Removed texture file cache */
	VER_CompressedZlib						= 9,					/**< Added in archive compression ZLIB */
	VER_AssetName_V2						= 10,					/**< Moved asset name from him to table of info about this asset */
	VER_GUIDPackages						= 11,					/**< Added GUID in package */
	VER_NamePackage							= 12,					/**< Added name package */

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
