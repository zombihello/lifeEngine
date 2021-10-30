/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EDITORENGINE_H
#define EDITORENGINE_H

#include <vector>

#include "System/BaseEngine.h"

/**
 * @ingroup WorldEd
 * Class of editor engine
 */
class LEditorEngine : public LBaseEngine
{
	DECLARE_CLASS( LEditorEngine, LBaseEngine )

public:
	/**
	 * Constructor
	 */
	LEditorEngine();

	/**
	 * Destructor
	 */
	virtual ~LEditorEngine();

	/**
	 * Initialize engine
	 */
	virtual void Init();

	/**
	 * Update logic of engine
	 *
	 * @param[in] InDeltaTime Delta time
	 */
	virtual void Tick( float InDeltaSeconds );

	/**
	 * Shutdown engine
	 */
	virtual void Shutdown();

	/**
	 * Add viewport to render list
	 * 
	 * @param[in] InViewport Viewport
	 */
	FORCEINLINE void AddViewport( class FViewport* InViewport )
	{
		viewports.push_back( InViewport );
	}

	/**
	 * Remove viewport from render list
	 * 
	 * @param[in] InViewport Viewport
	 */
	FORCEINLINE void RemoveViewport( class FViewport* InViewport )
	{
		for ( uint32 index = 0, count = ( uint32 )viewports.size(); index < count; ++index )
		{
			class FViewport*&		viewport = viewports[ index ];
			if ( viewport == InViewport )
			{
				viewports.erase( viewports.begin() + index );
				return;
			}
		}
	}

	/**
	 * Get editor name
	 * 
	 * @return Return editor name
	 */
	virtual std::wstring GetEditorName() const;

private:
	std::vector< class FViewport* >			viewports;		/**< Array of viewports for render */
	class WeMainWindow*						mainWindow;		/**< Main editor window */
};

#endif // !EDITORENGINE_H