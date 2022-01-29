/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCENERENDERTARGETS_H
#define SCENERENDERTARGETS_H

#include "RenderResource.h"
#include "RenderUtils.h"
#include "RHI/BaseSurfaceRHI.h"
#include "RHI/TypesRHI.h"

/**
 * @ingroup Engine
 * Enumeration of scene render target types
 */
enum ESceneRenderTargetTypes
{
	SRTT_SceneColor,						/**< Render target for scene colors */
	SRTT_SceneDepthZ,						/**< Render target for scene depths */
	SRTT_MaxSceneRenderTargets				/**< Max scene RTs available */
};

/**
 * @ingroup Engine
 * Render targets for scene rendering
 */
class FSceneRenderTargets : public FRenderResource
{
public:
	/**
	 * @brief Constructor
	 */
	FSceneRenderTargets();

	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;

	/**
	 * @brief Releases the RHI resources used by this resource.
	 * Called when the resource is released.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseRHI() override;

	/**
	 * @brief Allocate render targets for new size
	 * 
	 * @param InNewSizeX New size by X
	 * @param InNewSizeY New size by Y
	 */
	void Allocate( uint32 InNewSizeX, uint32 InNewSizeY );

	/**
	 * @brief Get texture of scene color
	 * @return Return texture of scene color
	 */
	FORCEINLINE FTexture2DRHIRef GetSceneColorTexture() const
	{
		return renderTargets[ SRTT_SceneColor ].texture;
	}

	/**
	 * @brief Get surface of scene color
	 * @return Return surface of scene color
	 */
	FORCEINLINE FSurfaceRHIRef GetSceneColorSurface() const
	{
		return renderTargets[ SRTT_SceneColor ].surface;
	}

	/**
	 * @brief Get texture of scene depth Z
	 * @return Return texture of scene depth Z
	 */
	FORCEINLINE FTexture2DRHIRef GetSceneDepthZTexture() const
	{
		return renderTargets[ SRTT_SceneDepthZ ].texture;
	}

	/**
	 * @brief Get surface of scene depth Z
	 * @return Return surface of scene depth Z
	 */
	FORCEINLINE FSurfaceRHIRef GetSceneDepthZSurface() const
	{
		return renderTargets[ SRTT_SceneDepthZ ].surface;
	}

private:
	/**
	 * Element of scene render target
	 */
	struct FSceneRenderTargetItem
	{
		FTexture2DRHIRef		texture;											/**< Texture for resolving to */
		FSurfaceRHIRef			surface;											/**< Surface to render to */
	};

	uint32						bufferSizeX;										/**< Size of buffer by X */
	uint32						bufferSizeY;										/**< Size of buffer by Y */
	FSceneRenderTargetItem		renderTargets[ SRTT_MaxSceneRenderTargets ];		/**< Static array of all the scene render targets */
};

extern TGlobalResource< FSceneRenderTargets >		GSceneRenderTargets;			/**< The global render targets used for scene rendering */

#endif // !SCENERENDERTARGETS_H