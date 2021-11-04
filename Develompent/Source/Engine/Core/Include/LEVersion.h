/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LEVERSION_H
#define LEVERSION_H

#define ENGINE_VERSION			20
#define MAJOR_VERSION			0
#define MINOR_VERSION			2
#define PATH_VERSION			0

#define ENGINE_NAME				"lifeEngine"
#define ENGINE_VERSION_STRING	"0.2.0"

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
	VER_PackageBase						= 1,					/**< Min version for serialize content*/

	//
	// New versions can be added here
	//

	VER_MinVersion						= VER_PackageBase,		/**< This need for check min supported version */
	VER_AutomaticVersionPlusOne									/**< This needs for automatic update last version of package */
};

/**
 * @ingroup Core
 * Latest version of package
 */
#define VER_PACKAGE_LATEST					( PREPROCESSOR_ENUM_PROTECT( VER_AutomaticVersionPlusOne ) - 1 )

/**
 * @ingroup Core
 * Package tag in file
 */
#define PACKAGE_FILE_TAG					0x5241454C

#endif // !LEVERSION_H
