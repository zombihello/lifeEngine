/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASEENGINE_H
#define BASEENGINE_H

#include <string>

#include "Misc/Object.h"
#include "Render/Texture.h"
#include "Render/Material.h"
#include "Core.h"

/**
 * @ingroup Engine
 * Class of base engine
 */
class LBaseEngine : public LObject
{
	DECLARE_CLASS( LBaseEngine, LObject )

public:
	/**
	 * Destructor
	 */
	virtual ~LBaseEngine() {}

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
	virtual void ProcessEvent( struct SWindowEvent& InWindowEvent );

	/**
	 * Get max tick rate in second
	 * @return Return max tick rate
	 */
	virtual float GetMaxTickRate() const;

	/**
	 * Get default texture
	 * @return Return default texture
	 */
	FORCEINLINE FTexture2DPtr GetDefaultTexture() const
	{
		return defaultTexture;
	}

	/**
	 * Get default material
	 * @return Return default material
	 */
	FORCEINLINE FMaterialPtr GetDefaultMaterial() const
	{
		return defaultMaterial;
	}

#if WITH_EDITOR
	/**
	 * Get default wireframe material
	 * @return Return default wireframe material
	 */
	FORCEINLINE FMaterialPtr GetDefaultWireframeMaterial() const
	{
		return defaultWireframeMaterial;
	}
#endif // WITH_EDITOR

protected:
	FTexture2DPtr			defaultTexture;					/**< Default texture */
	FMaterialPtr			defaultMaterial;				/**< Default material */

#if WITH_EDITOR
	FMaterialPtr			defaultWireframeMaterial;		/**< Default wireframe material */
#endif // WITH_EDITOR
};

#endif // !BASEENGINE_H