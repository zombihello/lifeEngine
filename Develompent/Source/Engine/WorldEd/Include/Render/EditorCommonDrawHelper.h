/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EDITORCOMMONDRAWHELPER_H
#define EDITORCOMMONDRAWHELPER_H

#include "Math/Color.h"
#include "Render/Viewport.h"

/**
 * @ingroup WorldEd
 * @brief Implements some basic functionality used by all editor viewports
 */
class CEditorCommonDrawHelper
{
public:
	/**
	 * @brief Constructor
	 */
	CEditorCommonDrawHelper();

	/**
	 * @brief Draws the viewport grid
	 * 
	 * @param InSceneView			Scene view
	 * @param InViewportType		Viewport type
	 * @param InScene				Scene
	 */
	void DrawGrid( const class CSceneView* InSceneView, ELevelViewportType InViewportType, class CScene* InScene );

private:
	/**
	 * @brief Draw grid section
	 * 
	 * @param InViewportLocX		Viewport location by X
	 * @param InViewportGridY		Viewport grid by Y
	 * @param InStart				Coords of start grid
	 * @param InEnd					Coords of end grid
	 * @param InStartX				Reference to changing component in InStart
	 * @param InEndX				References to changing component in InEnd
	 * @param InAxis				Axis ID (X = 0, Y = 1, Z = 2)
	 * @param InIsAlphaCase			Is alpha case?
	 * @param InSceneView			Scene view
	 * @param InSDG					Scene depth group
	 */
	void DrawGridSection( int32 InViewportLocX, int32 InViewportGridY, Vector& InStart, Vector& InEnd, float& InStartX, float& InEndX, int32 InAxis, bool InIsAlphaCase, const class CSceneView* InSceneView, struct SSceneDepthGroup& InSDG );

	bool		bDrawColoredOrigin;			/**< Is need draw colored origins */
	bool		bDrawWorldBox;				/**< Is need draw world box */
	float		perspectiveGridSize;		/**< Perspective grid size */
	CColor		gridColorHi;				/**< Grid color height */
	CColor		gridColorLo;				/**< Grid color low */
	CColor		worldBoxColor;				/**< World box color */
};

#endif // !EDITORCOMMONDRAWHELPER_H