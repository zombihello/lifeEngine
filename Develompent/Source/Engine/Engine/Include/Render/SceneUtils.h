/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCENEUTILS_H
#define SCENEUTILS_H

#include "LEBuild.h"
#include "Math/Math.h"
#include "Math/Color.h"
#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"

// Colors that are defined for a particular mesh type
// Each event type will be displayed using the defined color

/**
 * @ingroup Engine
 * @brief Macro of color draw event for lights
 */
#define DEC_LIGHT				CColor( 255,	0,		0,		255 )

/**
 * @ingroup Engine
 * @brief Macro of color draw event for skeletal meshes
 */
#define DEC_SKEL_MESH			CColor( 255,	0,		255,	255 )

/**
 * @ingroup Engine
 * @brief Macro of color draw event for static meshes
 */
#define DEC_STATIC_MESH			CColor( 0,		128,	255,	255 )

/**
 * @ingroup Engine
 * @brief Macro of color draw event for render canvas
 */
#define DEC_CANVAS				CColor( 128,	255,	255,	255 )

/**
 * @ingroup Engine
 * @brief Macro of color draw event for terrain
 */
#define DEC_TERRAIN				CColor( 0,		128,	0,		255 )

/**
 * @ingroup Engine
 * @brief Macro of color draw event for shadows
 */
#define DEC_SHADOW				CColor( 128,	128,	128,	255 )

/**
 * @ingroup Engine
 * @brief Macro of color draw event for BSP
 */
#define DEC_BSP					CColor( 255,	128,	0,		255 )

/**
 * @ingroup Engine
 * @brief Macro of color draw event for particles
 */
#define DEC_PARTICLE			CColor( 128,	0,		128,	255 )

 /**
  * @ingroup Engine
  * @brief Macro of color draw event for sprites
  */
#define DEC_SPRITE				CColor( 128,	255,	255,	255 )

/**
 * @ingroup Engine
 * @brief Macro of color general scene draw events
 */
#define DEC_SCENE_ITEMS			CColor( 128,	128,	128,	255 )

/**
 * @ingroup Engine
 * @brief Macro of color draw event for materials
 */
#define DEC_MATERIAL			CColor( 0,		128,	255,	255 )

/**
 * @ingroup Engine
 * @brief Macro of color draw event for simple elements
 */
#define DEC_SIMPLEELEMENTS		CColor( 238,	153,	26,		255 )

/**
 * @ingroup Engine
 * @brief Macro of color draw event for dynamic elements
 */
#define DEC_DYNAMICELEMENTS		CColor( 238,	153,	26,		255 )

#if FRAME_CAPTURE_MARKERS
/**
 * @ingroup Engine
 * @brief Class for scoped draw event
 */
class CScopedDrawEvent
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InColor Color of event
	 * @param InStatId Name event
	 */
	FORCEINLINE CScopedDrawEvent( const CColor& InColor, const tchar* InStatId )
	{
		GRHI->BeginDrawEvent( GRHI->GetImmediateContext(), InColor, InStatId );
	}

	/**
	 * @brief Destructor
	 */
	FORCEINLINE ~CScopedDrawEvent()
	{
		GRHI->EndDrawEvent( GRHI->GetImmediateContext() );
	}

protected:
	/**
	 * @brief Constructor
	 */
	CScopedDrawEvent() {}
};

/**
 * @ingroup Engine
 * @brief Macro for declare scroped draw event
 * 
 * @param InEventName Event name
 * @param InColor Color of event
 * @param InStatID Stat id
 */
#define SCOPED_DRAW_EVENT( InEventName, InColor, InStatID )		CScopedDrawEvent event_##InEventName( InColor, InStatID )
#else
#define SCOPED_DRAW_EVENT( InEventName, InColor, InStatID )
#endif // FRAME_CAPTURE_MARKERS

#endif // !SCENEUTILS_H