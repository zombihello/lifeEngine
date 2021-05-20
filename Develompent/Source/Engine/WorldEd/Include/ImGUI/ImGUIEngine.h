/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef IMGUIENGINE_H
#define IMGUIENGINE_H

/**
 * @ingroup WorldEd
 * @brief Initialize ImGUI on platform
 */
extern bool				appImGUIInit();

/**
 * @ingroup WorldEd
 * @brief Shutdown ImGUI on platform
 */
extern void				appImGUIShutdown();

/**
 * @ingroup WorldEd
 * @brief Begin drawing ImGUI
 */
extern void				appImGUIBeginDrawing();

/**
 * @ingroup WorldEd
 * @brief End drawing ImGUI
 */
extern void				appImGUIEndDrawing();

/**
 * @ingroup WorldEd
 * @brief Process event for ImGUI
 * 
 * @param[in] InWindowEvent Window event
 */
extern void				appImGUIProcessEvent( struct SWindowEvent& InWindowEvent );

/**
 * @ingroup WorldEd
 * @brief Class for work with ImGUI and initialize her on platforms
 */
class FImGUIEngine
{
public:
	/**
	 * @brief Constructor
	 */
							FImGUIEngine();

	/**
	 * @brief Destructor
	 */
							~FImGUIEngine();

	/**
	 * @brief Initialize ImGUI
	 * 
	 * @param[in] InDeviceContext Device context
	 * @return Return true if initialization was successful, else returning false
	 */
	bool					Init( class FBaseDeviceContextRHI* InDeviceContext );

	/**
	 * @brief Shutdown ImGUI on platform
	 * 
	 * @param[in] InDeviceContext Device context
	 */
	void					Shutdown( class FBaseDeviceContextRHI* InDeviceContext );

	/**
	 * @brief Process event for ImGUI
	 * 
	 * @param[in] InWindowEvent Window event
	 */
	void					ProcessEvent( struct SWindowEvent& InWindowEvent );

	/**
	 * @brief Begin drawing ImGUI
	 * 
	 * @param[in] InDeviceContext Device context
	 */
	void					BeginDrawing( class FBaseDeviceContextRHI* InDeviceContext );

	/**
	 * @brief End drawing ImGUI
	 * 
	 * @param[in] InDeviceContext Device context
	 */
	void					EndDrawing( class FBaseDeviceContextRHI* InDeviceContext );

private:
	struct ImGuiContext*		imguiContext;		/**< Pointer to ImGUI context */
};

#endif // !IMGUIENGINE_H
