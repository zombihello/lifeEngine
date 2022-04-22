/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef VIEWPORTS_H
#define VIEWPORTS_H

#include <string>

/**
 * @ingroup WorldEd
 * @brief Enumeration of viewport config
 */
enum EViewportConfig
{
	VC_None			= -1,		/**< None */
	VC_2_2_Split,				/**< 2x2 split */
	VC_1_2_Split,				/**< 1x2 split */
	VC_1_1_SplitH,				/**< 1x1 horizontal split */
	VC_1_1_SplitV,				/**< 1x1 vertical split */
	VC_Max						/**< Max configs */
};

/**
 * @ingroup WorldEd
 * @brief Enumeration of viewport type
 */
enum ELevelViewportType
{
	LVT_None		= -1,	/**< None */
	LVT_OrthoXY,			/**< Ortho XY */
	LVT_OrthoXZ,			/**< Ortho XZ */
	LVT_OrthoYZ,			/**< Ortho YZ */
	LVT_Perspective			/**< Perspective */
};

/**
 * @ingroup WorldEd
 * @brief The default values for a viewport within an FViewportConfig_Template
 */
struct FViewportConfig_Viewport
{
public:
	/**
	 * @brief Constructor
	 */
	FViewportConfig_Viewport();

	/**
	 * @brief Destructor
	 */
	virtual ~FViewportConfig_Viewport();

	bool					bEnabled;				/**< Is enabled viewport */
	bool					bSetListenerPosition;	/**< Is need sets the listener position */
	ELevelViewportType		viewportType;			/**< Viewport type */
};

/**
 * @ingroup WorldEd
 * @brief A template for a viewport configuration. Holds the baseline layouts and flags for a config
 */
struct FViewportConfig_Template
{
public:
	/**
	 * @brief Constructor
	 */
	FViewportConfig_Template();

	/**
	 * @brief Destructor
	 */
	virtual ~FViewportConfig_Template();

	/**
	 * @brief Set template
	 * @param InViewportConfig		Type viewport config
	 */
	void Set( EViewportConfig InViewportConfig );

	std::wstring				description;				/**< The description for this configuration */
	EViewportConfig				viewportConfig;				/**< The viewport config this template represents */
	FViewportConfig_Viewport	viewportTemplates[ 4 ];		/**< The viewport templates within this config template */
};

#endif // !VIEWPORTS_H