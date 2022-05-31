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
class CEditorEngine : public CBaseEngine
{
	DECLARE_CLASS( CEditorEngine, CBaseEngine )

public:
	/**
	 * Constructor
	 */
	CEditorEngine();

	/**
	 * Destructor
	 */
	virtual ~CEditorEngine();

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
	FORCEINLINE void AddViewport( class CViewport* InViewport )
	{
		viewports.push_back( InViewport );
	}

	/**
	 * Remove viewport from render list
	 * 
	 * @param[in] InViewport Viewport
	 */
	FORCEINLINE void RemoveViewport( class CViewport* InViewport )
	{
		for ( uint32 index = 0, count = ( uint32 )viewports.size(); index < count; ++index )
		{
			class CViewport*&		viewport = viewports[ index ];
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
	FORCEINLINE CEditorConstraints& GetConstraints()
	{
		return constraints;
	}

	/**
	 * Get editor constraints
	 * @return Return editor constraints
	 */
	FORCEINLINE const CEditorConstraints& GetConstraints() const
	{
		return constraints;
	}

	/**
	 * Get main editor window
	 * @return Return main editor window
	 */
	FORCEINLINE class WeMainWindow* GetMainWindow() const
	{
		return mainWindow;
	}

private:
	std::vector< class CViewport* >			viewports;		/**< Array of viewports for render */
	CEditorConstraints						constraints;	/**< Editor constraints */
	class WeMainWindow*						mainWindow;		/**< Main editor window */
};

#endif // !EDITORENGINE_H