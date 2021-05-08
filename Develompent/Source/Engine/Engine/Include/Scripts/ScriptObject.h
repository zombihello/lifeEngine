/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTOBJECT_H
#define SCRIPTOBJECT_H

#include "Misc/Types.h"

/**
 * @ingroup Engine
 * @brief Class for work with script object
 */
class ScriptObject
{
public:
	/**
	 * @brief Constructor
	 * @param[in] InObjectTypeIndex Index of script object type in module
	 * @param[in] InModuleName Script module name
	 */
					ScriptObject( uint32 InObjectTypeIndex, const tchar* InModuleName );

	/**
	 * @brief Destructor
	 */
	virtual			~ScriptObject();

protected:
	class asITypeInfo*				typeInfo;	/**< Type info */
	class asIScriptObject*			self;		/**< Pointer to script object */
};

#endif // !SCRIPTOBJECT_H

