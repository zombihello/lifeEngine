/**
 * @file
 * @addtogroup UI User interface
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef IMGUIENGINE_H
#define IMGUIENGINE_H

#include "LEBuild.h"
#if WITH_IMGUI

#include <vector>

#include "Core.h"
#include "Misc/RefCounted.h"
#include "Misc/RefCountPtr.h"
#include "System/ThreadingBase.h"
#include "ImGUI/imgui.h"
#include "RHI/TypesRHI.h"

/**
 * @ingroup UI
 * Count draw data buffers for render ImGUI
 */
#define IMGUI_DRAWBUFFERS_COUNT		3

/**
 * @ingroup UI
 * @brief Initialize ImGUI on platform
 */
extern bool				appImGUIInit();

/**
 * @ingroup UI
 * @brief Shutdown ImGUI on platform
 */
extern void				appImGUIShutdown();

/**
 * @ingroup UI
 * @brief Begin drawing ImGUI
 */
extern void				appImGUIBeginDrawing();

/**
 * @ingroup UI
 * @brief End drawing ImGUI
 */
extern void				appImGUIEndDrawing();

/**
 * @ingroup UI
 * @brief Process event for ImGUI
 * 
 * @param[in] InWindowEvent Window event
 */
extern void				appImGUIProcessEvent( struct SWindowEvent& InWindowEvent );

/**
 * @ingroup UI
 * @brief Class for draw data of ImGUI
 */
class CImGUIDrawData : public CRefCounted
{
public:
	/**
	 * Constructor
	 */
	CImGUIDrawData();

	/**
	 * Destructor
	 */
	~CImGUIDrawData();

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
 * @ingroup UI
 * Class of update window ImGUI
 */
class ÑImGUIWindow
{
public:
	/**
	 * Constructor
	 * @param[in] InViewport Pointer to viewport of ImGUI
	 */
	ÑImGUIWindow( ImGuiViewport* InViewport );

	/**
	 * Update ImGUI windows
	 */
	void Tick();

	/**
	 * Get ImGUI viewport
	 * @return Return Pointer to ImGUI viewport
	 */
	FORCEINLINE ImGuiViewport* GetViewport() const
	{
		return imguiViewport;
	}

private:
	ImGuiViewport*						imguiViewport;									/**< Pointer to ImGUI viewport */
	TRefCountPtr< CImGUIDrawData >		drawDataBuffers[ IMGUI_DRAWBUFFERS_COUNT ];		/**< Buffers of ImDrawData for draw. In one buffer write, from one read */
	uint32								indexCurrentBuffer;								/**< Index of current buffer */
};

/**
 * @ingroup UI
 * @brief Class for work with ImGUI and initialize her on platforms
 */
class ÑImGUIEngine
{
public:
	/**
	 * @brief Constructor
	 */
							ÑImGUIEngine();

	/**
	 * @brief Destructor
	 */
							~ÑImGUIEngine();

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

	/**
	 * @brief Open new ImGUI window
	 * @param[in] InViewport Pointer to viewport of ImGUI
	 */
	void					OpenWindow( ImGuiViewport* InViewport );

	/**
	 * @brief Close ImGUI window
	 * @param[in] InViewport Pointer to viewport of ImGUI
	 */
	void					CloseWindow( ImGuiViewport* InViewport );

private:
	struct ImGuiContext*				imguiContext;	/**< Pointer to ImGUI context */
	std::vector< ÑImGUIWindow* >		windows;		/**< Array of windows ImGUI */
};

#endif // WITH_IMGUI
#endif // !IMGUIENGINE_H
