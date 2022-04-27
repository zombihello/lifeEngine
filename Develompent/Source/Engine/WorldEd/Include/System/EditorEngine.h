/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EDITORENGINE_H
#define EDITORENGINE_H

#include <vector>

#include "Logger/LoggerMacros.h"
#include "System/BaseEngine.h"
#include "System/EditorConstraints.h"

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
	 * Print log to log widget
	 * 
	 * @param InLogType Log type
	 * @param InMessage Message of log
	 */
	virtual void PrintLogToWidget( ELogType InLogType, const tchar* InMessage );

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
	 * Load map
	 *
	 * @param[in] InMap Path to map
	 * @param[out] OutError Error of loading map
	 * @return Return true if map loaded successed, else return false
	 */
	virtual bool LoadMap( const std::wstring& InMap, std::wstring& OutError ) override;

	/**
	 * Get editor name
	 * 
	 * @return Return editor name
	 */
	virtual std::wstring GetEditorName() const;

	/**
	 * Get editor constraints
	 * @return Return editor constraints
	 */
	FORCEINLINE FEditorConstraints& GetConstraints()
	{
		return constraints;
	}

	/**
	 * Get editor constraints
	 * @return Return editor constraints
	 */
	FORCEINLINE const FEditorConstraints& GetConstraints() const
	{
		return constraints;
	}

private:
	std::vector< class FViewport* >			viewports;				/**< Array of viewports for render */
	FEditorConstraints						constraints;			/**< Editor constraints */
};

#endif // !EDITORENGINE_H