/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <qstring>

#include "System/Package.h"

/**
 * @ingroup WorldEd
 * Asset manager for import/reimport and controlling all assets
 */
class FAssetManager
{
public:
	/**
	 * Enumeration of import result
	 */
	enum EImportResult
	{
		IR_Seccussed,		/**< Asset is imported seccussed */
		IR_Error,			/**< Asset not imported due to error */
		IR_AlreadyExist		/**< Asset not imported because in package already exist asset with name */
	};

	/**
	 * Import asset to package
	 * 
	 * @param InPath				Path to source file
	 * @param InPackage				Package of place for import
	 * @param OutAsset				Output imported asset, in case of fail return NULL
	 * @param InIsForceImport		Is need replace already exist asset with name
	 * @return Return imported asset, in case of fail return NULL
	 */
	EImportResult Import( const QString& InPath, FPackage* InPackage, FAssetRef& OutAsset, bool InIsForceImport = false );

	/**
	 * Reimport asset
	 * 
	 * @param InAsset		Asset for reimport
	 * @return Return TRUE if seccussed reimported, else FALSE
	 */
	bool Reimport( FAsset* InAsset );
};

#endif // !ASSETMANAGER_H