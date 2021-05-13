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

#define DECLARE_CLASS( TClass, TBaseClass ) \
	typedef TClass		ThisClass; \
	typedef TBaseClass	Super;

/**
 * @ingroup Engine
 * @brief Class for work with script object
 */
class ScriptObject
{
public:
	/**
	 * @brief Destructor
	 */
	virtual										~ScriptObject();

	/**
	 * @brief Get script object
	 * @return Return pointer to script object. If not created return nullptr
	 */
	FORCEINLINE class asIScriptObject*			GetHandle() const
	{
		return self;
	}

protected:
	/**
	 * @brief Enumeration for disable creating script object in constructor
	 */
	enum ENoInit		{ NoInit };

	/**
	 * @brief Initialize script object
	 * @param[in] InScriptObject Pointer to script object
	 */
	virtual void		Init( class asIScriptObject* InScriptObject );

	class asITypeInfo*				typeInfo;	/**< Type info */
	class asIScriptObject*			self;		/**< Pointer to script object */
};

#endif // !SCRIPTOBJECT_H

