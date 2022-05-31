/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EDITORCONSTRAINTS_H
#define EDITORCONSTRAINTS_H

#include "Core.h"

/**
 * @ingroup WorldEd
 * @brief Editor constraints
 */
class CEditorConstraints
{
public:
	enum
	{
		MaxGridSizes = 11		/**< Max count grid sizes in WorldEd */
	};

	/**
	 * @brief Constructor
	 */
	CEditorConstraints();

	/**
	 * @brief Get current grid size
	 * @return Return current grid size
	 */
	FORCEINLINE float GetGridSize() const
	{
		return gridSizes[ currentGridSize ];
	}

private:
	uint32		currentGridSize;				/**< Current ID of grid size  */
	float		gridSizes[ MaxGridSizes ];		/**< Array of grid sizes */
};

#endif // !EDITORCONSTRAINTS_H