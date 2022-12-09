/**
 * @file
 * @addtogroup UI User interface
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef UIENGINE_H
#define UIENGINE_H

/**
 * @ingroup UI
 * @brief Main class for work UI
 */
class CUIEngine
{
public:
	/**
	 * Initialize UI
	 */
	void Init();

	/**
	 * @brief Update logic
	 *
	 * @param InDeltaTime	Delta time
	 */
	void Tick( float InDeltaSeconds );

	/**
	 * Process event
	 *
	 * @param[in] InWindowEvent Window event
	 */
	void ProcessEvent( struct SWindowEvent& InWindowEvent );

	/**
	 * @brief Begin draw commands for render UI
	 */
	void BeginDraw();

	/**
	 * @brief End draw commands for render UI
	 */
	void EndDraw();

	/**
	 * Shutdown UI
	 */
	void Shutdown();
};

#endif // !UIENGINE_H
