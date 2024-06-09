/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef OBJECTIMPORTER_H
#define OBJECTIMPORTER_H

#include <string>
#include <unordered_map>

#include "Misc/Platform.h"
#include "Misc/JsonDocument.h"
#include "Reflection/Object.h"

/**
 * @ingroup WorldEd
 * @brief Object importer
 */
class CObjectImporter
{
public:
	/**
	 * @brief Constructor
	 */
	CObjectImporter();

	/**
	 * @brief Import objects from JSON
	 *
	 * @param InString			The JSON string
	 * @param OutObject			After execute the function here will be pointer to imported a new objects
	 * @param InImportRootScope	The root object of import
	 * @param InPortFlags		Export flags (see EPropertyPortFlags)
	 */
	void Import( const std::wstring& InString, std::vector<CObject*>& OutObjects, CObject* InImportRootScope, uint32 InPortFlags = PPF_None );

private:
	 /**
	  * @brief Import an object from JSON
	  *
	  * @param InJsonObject			JSON object
	  * @param OutObject			After execute the function here will be pointer to imported a new object
	  * @param InOuter				The outer object for import
	  * @param InIsSubobject		Is this import object subobject
	  */
	void ImportObject( const CJsonObject* InJsonObject, CObject*& OutObject, CObject* InOuter, bool InIsSubobject );

	uint32														portFlags;			/**< Export/import flags (see EPropertyPortFlags) */
	CObject*													importRootScope;	/**< The root object of import */
	std::unordered_map<CName, CObject*, CName::HashFunction>	objectDict;			/**< Dictionary of already imported objects */
};

#endif // !OBJECTIMPORTER_H