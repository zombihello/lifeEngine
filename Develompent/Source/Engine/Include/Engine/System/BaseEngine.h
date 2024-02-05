/**
 * @file
 * @addtogroup Engine Engine
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef BASEENGINE_H
#define BASEENGINE_H

#include <string>

#include "Core/Misc/Object.h"
#include "Core/Core.h"
#include "Engine/Render/Texture.h"
#include "Engine/Render/Material.h"

/**
 * @ingroup Engine
 * Class of base engine
 */
class CBaseEngine : public CObject
{
	DECLARE_CLASS( CBaseEngine, CObject, 0, 0 )

public:
	/**
	 * Constructor
	 */
	CBaseEngine();

	/**
	 * Destructor
	 */
	virtual ~CBaseEngine() {}

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
	 * Load map
	 * 
	 * @param[in] InMap Path to map
	 * @param[out] OutError Error of loading map
	 * @return Return true if map loaded successed, else return false
	 */
	virtual bool LoadMap( const std::wstring& InMap, std::wstring& OutError );

	/**
	 * @brief Process event
	 *
	 * @param[in] InWindowEvent Window event
	 */
	virtual void ProcessEvent( struct WindowEvent& InWindowEvent );

	/**
	 * Get max tick rate in second
	 * @return Return max tick rate
	 */
	virtual float GetMaxTickRate() const;

	/**
	 * Get default texture
	 * @return Return default texture
	 */
	FORCEINLINE TAssetHandle<CTexture2D> GetDefaultTexture() const
	{
		return defaultTexture;
	}

	/**
	 * Get default material
	 * @return Return default material
	 */
	FORCEINLINE TAssetHandle<CMaterial> GetDefaultMaterial() const
	{
		return defaultMaterial;
	}

#if WITH_EDITOR
	/**
	 * Get default wireframe material
	 * @return Return default wireframe material
	 */
	FORCEINLINE TAssetHandle<CMaterial> GetDefaultWireframeMaterial() const
	{
		return defaultWireframeMaterial;
	}
#endif // WITH_EDITOR

	/**
	 * Is enabled pre-pass render
	 * @return Return TRUE if pre-pass render is enabled
	 */
	FORCEINLINE bool IsPrePass() const
	{
		return bPrePass;
	}

	/**
	 * Is enabled auto exposure
	 * @return Return TRUE if auto exposure is enabled
	 */
	FORCEINLINE bool IsAutoExposure() const
	{
		return bAutoExposure;
	}

	/**
	 * Get exposure
	 * @return Return exposure
	 */
	FORCEINLINE float GetExposure() const
	{
		return exposure;
	}

	/**
	 * Get exposure min
	 * @return Return exposure min
	 */
	FORCEINLINE float GetExposureMin() const
	{
		return exposureMin;
	}

	/**
	 * Get exposure max
	 * @return Return exposure max
	 */
	FORCEINLINE float GetExposureMax() const
	{
		return exposureMax;
	}

	/**
	 * Get gamma correction
	 * @return Return gamma correction
	 */
	FORCEINLINE float GetGamma() const
	{
		return gamma;
	}

protected:
	TAssetHandle<CTexture2D>		defaultTexture;					/**< Default texture */
	TAssetHandle<CMaterial>			defaultMaterial;				/**< Default material */

#if WITH_EDITOR
	TAssetHandle<CMaterial>			defaultWireframeMaterial;		/**< Default wireframe material */
#endif // WITH_EDITOR

	bool							bPrePass;						/**< Is enabled pre-pass render */
	bool							bAutoExposure;					/**< Is enabled auto exposure */
	float							exposure;						/**< Exposure */
	float							exposureMin;					/**< Exposure min */
	float							exposureMax;					/**< Exposure max */
	float							gamma;							/**< Gamma correction */
};

#endif // !BASEENGINE_H