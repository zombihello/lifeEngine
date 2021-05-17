/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPT_H
#define SCRIPT_H

/**
 * @ingroup Engine
 * @brief Class for work with scripts
 */
class FScript
{
public:
	/**
	 * @brief Constructor
	 */
					FScript();

	/**
	 * @brief Destructor
	 */
					~FScript();

	/**
	 * @brief Serialize script
	 */
	void			Serialize( class FBaseArchive& InArchive );

private:
	struct lua_State*		luaVM;		/**< Pointer to lua virtual machine */
};

#endif // !SCRIPT_H
