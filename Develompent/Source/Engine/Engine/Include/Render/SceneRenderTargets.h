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
#include "LEBuild.h"

/**
 * @ingroup Engine
 * Enumeration of scene render target types
 */
enum ESceneRenderTargetTypes
{
	SRTT_SceneColor,						/**< Render target for scene colors */
	SRTT_SceneDepthZ,						/**< Render target for scene depths */

#if ENABLE_HITPROXY
	SRRT_HitProxies,						/**< Render target for hitProxiesy */
#endif // ENABLE_HITPROXY

	SRTT_MaxSceneRenderTargets				/**< Max scene RTs available */
};

/**
 * @ingroup Engine
 * Render targets for scene rendering
 */
class CSceneRenderTargets : public CRenderResource
{
public:
	/**
	 * @brief Constructor
	 */
	CSceneRenderTargets();

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
	FORCEINLINE Texture2DRHIRef_t GetSceneColorTexture() const
	{
		return renderTargets[ SRTT_SceneColor ].texture;
	}

	/**
	 * @brief Get surface of scene color
	 * @return Return surface of scene color
	 */
	FORCEINLINE SurfaceRHIRef_t GetSceneColorSurface() const
	{
		return renderTargets[ SRTT_SceneColor ].surface;
	}

	/**
	 * @brief Get texture of scene depth Z
	 * @return Return texture of scene depth Z
	 */
	FORCEINLINE Texture2DRHIRef_t GetSceneDepthZTexture() const
	{
		return renderTargets[ SRTT_SceneDepthZ ].texture;
	}

	/**
	 * @brief Get surface of scene depth Z
	 * @return Return surface of scene depth Z
	 */
	FORCEINLINE SurfaceRHIRef_t GetSceneDepthZSurface() const
	{
		return renderTargets[ SRTT_SceneDepthZ ].surface;
	}

#if ENABLE_HITPROXY
	/**
	 * @brief Get texture of hit proxy
	 * @return Return texture of hit proxy
	 */
	FORCEINLINE Texture2DRHIRef_t GetHitProxyTexture() const
	{
		return renderTargets[ SRRT_HitProxies ].texture;
	}

	/**
	 * @brief Get surface of hit proxy
	 * @return Return surface of hit proxy
	 */
	FORCEINLINE SurfaceRHIRef_t GetHitProxySurface() const
	{
		return renderTargets[ SRRT_HitProxies ].surface;
	}
#endif // ENABLE_HITPROXY

protected:
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

private:
	/**
	 * Element of scene render target
	 */
	struct SSceneRenderTargetItem
	{
		Texture2DRHIRef_t		texture;											/**< Texture for resolving to */
		SurfaceRHIRef_t			surface;											/**< Surface to render to */
	};

	uint32						bufferSizeX;										/**< Size of buffer by X */
	uint32						bufferSizeY;										/**< Size of buffer by Y */
	SSceneRenderTargetItem		renderTargets[ SRTT_MaxSceneRenderTargets ];		/**< Static array of all the scene render targets */
};

extern TGlobalResource< CSceneRenderTargets >		GSceneRenderTargets;			/**< The global render targets used for scene rendering */

#endif // !SCENERENDERTARGETS_H