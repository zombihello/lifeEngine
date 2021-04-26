/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

/**
 * @ingroup Engine
 * @brief Class for work with script engine
 */
class ScriptEngine
{
public:
	/**
	 * @brief Constructor
	 */
						ScriptEngine();

	/**
	 * @brief Destructor
	 */
						~ScriptEngine();

	/**
	 * @brief Initialize
	 */
	void				Init();

private:
	class asIScriptEngine*			asScriptEngine;
};

#endif // !SCRIPTENGINE_H
