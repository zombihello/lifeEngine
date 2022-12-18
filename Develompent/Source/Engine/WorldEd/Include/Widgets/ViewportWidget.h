/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef VIEWPORTWIDGET_H
#define VIEWPORTWIDGET_H

#include "Misc/RefCountPtr.h"
#include "Render/Viewport.h"
#include "Render/RenderTarget.h"

 /**
  * @ingroup WorldEd
  * @brief Viewport widget
  */
class CViewportWidget
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InIsEnabled				Is enabled the viewport. If is FALSE him will not draw
	 * @param InViewportClient			Viewport client
	 * @param InDeleteViewportClient	Is need delete viewport client when destroying this widget
	 */
	CViewportWidget( bool InIsEnabled = true, CViewportClient* InViewportClient = nullptr, bool InDeleteViewportClient = false );

	/**
	 * @brief Destructor
	 */
	~CViewportWidget();

	/**
	 * @brief Init
	 */
	void Init();

	/**
	 * @brief Tick
	 */
	void Tick();

	/**
	 * @brief Process event
	 *
	 * @param InWindowEvent		Window event
	 */
	void ProcessEvent( struct SWindowEvent& InWindowEvent );

	/**
	 * @brief Set enabled
	 * @param InIsEnabled		Is enabled the viewport
	 */
	void SetEnabled( bool InIsEnabled );

	 /**
	  * @brief Set viewport client
	  *
	  * @param InViewportClient			Viewport client
	  * @param InDeleteViewportClient	Is need delete viewport client when destroying this widget
	  */
	FORCEINLINE void SetViewportClient( CViewportClient* InViewportClient, bool InDeleteViewportClient )
	{
		viewportClient = InViewportClient;
		bDeleteViewportClient = InDeleteViewportClient;
		viewport->SetViewportClient( viewportClient );
	}

	/**
	 * @brief Set size
	 * @param InNewSize		New widget size
	 */
	FORCEINLINE void SetSize( const Vector2DInt& InNewSize )
	{
		// If size less or equal then 0 we do nothing
		if ( InNewSize.x <= 0 || InNewSize.y <= 0 )
		{
			return;
		}

		size = InNewSize;
		renderTarget->Update( false, InNewSize.x, InNewSize.y, PF_A8R8G8B8, 0, TEXT( "ViewportRT" ) );
		viewport->Update( false, InNewSize.x, InNewSize.y, renderTarget );
	}

	/**
	 * @brief Get render viewport
	 * @return Return render viewport
	 */
	FORCEINLINE const CViewport& GetViewport() const
	{
		return viewport;
	}

	/**
	 * @brief Is layer inited
	 * @return Return TRUE if layer is inited
	 */
	FORCEINLINE bool IsInit() const
	{
		return bInit;
	}

	/**
	 * @brief Is enabled viewport
	 * @return Return TRUE if viewport is enabled, otherwise will return FALSE
	 */
	FORCEINLINE bool IsEnabled() const
	{
		return bEnabled;
	}

	/**
	 * @brief Get widget size
	 * @return Return widget size
	 */
	FORCEINLINE const Vector2DInt& GetSize() const
	{
		return size;
	}

	/**
	 * @brief Is hovered viewport
	 * @return Return TRUE if viewport is hovered, otherwise will return FALSE
	 */
	FORCEINLINE bool IsHovered() const
	{
		return bHovered;
	}

private:
	bool							bInit;						/**< Is inited widget */
	bool							bEnabled;					/**< Is enabled viewport */
	bool							bDeleteViewportClient;		/**< Is need delete viewport client in destroy time */
	bool							bViewportOnDrawing;			/**< Is viewport drawing */
	bool							bHovered;					/**< Is hovered */
	TRefCountPtr<CViewport>			viewport;					/**< Viewport of render */
	TRefCountPtr<CRenderTarget>		renderTarget;				/**< Render target */
	CViewportClient*				viewportClient;				/**< Viewport client */
	Vector2DInt						size;						/**< Size widget */
};

#endif // !VIEWPORTWIDGET_H