/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef IMGUIENGINE_H
#define IMGUIENGINE_H

#include "Core.h"
#include "System/ThreadingBase.h"
#include "ImGUI/imgui.h"

/**
 * @ingroup WorldEd
 * Count draw data buffers for render ImGUI
 */
#define IMGUI_DRAWBUFFERS_COUNT		3

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
 * @brief Class for draw data of ImGUI
 */
class FImGUIDrawData
{
public:
	/**
	 * Constructor
	 */
	FImGUIDrawData();

	/**
	 * Destructor
	 */
	~FImGUIDrawData();

	/**
	 * Mark buffer is free
	 */
	FORCEINLINE void MarkFree()
	{
		isFree = true;
	}

	/**
	 * Clear buffer
	 */
	void Clear();

	/**
	 * Set draw data
	 * @param[in] InDrawData Pointer to draw data of ImGUI
	 */
	void SetDrawData( ImDrawData* InDrawData );

	 /**
	  * Get ImGUI draw data. Const version
	  *
	  * @return Return pointer to ImGUI draw data
	  */
	FORCEINLINE const ImDrawData* GetDrawData() const
	{
		return &drawData;
	}

	/**
	 * Get ImGUI draw data
	 *
	 * @return Return pointer to ImGUI draw data
	 */
	FORCEINLINE ImDrawData* GetDrawData()
	{
		return &drawData;
	}

	/**
	 * Is free buffer
	 * @return Return true if buffer is free, else return false
	 */
	FORCEINLINE bool IsFree() const
	{
		return isFree;
	}

private:
	bool					isFree;				/**< Is free buffer */
	ImDrawData				drawData;			/**< ImGUI draw data */
};

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
	 */
	void					Init();

	/**
	 * @brief Shutdown ImGUI on platform
	 */
	void					Shutdown();

	/**
	 * @brief Process event for ImGUI
	 * 
	 * @param[in] InWindowEvent Window event
	 */
	void					ProcessEvent( struct SWindowEvent& InWindowEvent );

	/**
	 * @brief Begin draw commands for render ImGUI
	 */
	void					BeginDraw();

	/**
	 * @brief End draw commands for render ImGUI
	 */
	void					EndDraw();

private:
	struct ImGuiContext*		imguiContext;									/**< Pointer to ImGUI context */
	FCriticalSection			criticalSection;								/**< Critical section */
	FImGUIDrawData				drawDataBuffers[ IMGUI_DRAWBUFFERS_COUNT ];		/**< Buffers of ImDrawData for draw. In one buffer write, from one read */
	uint32						indexCurrentBuffer;								/**< Index of current buffer */
};

#endif // !IMGUIENGINE_H
