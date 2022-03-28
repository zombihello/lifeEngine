/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef IMPORTMATERIALCOMMANDLET_H
#define IMPORTMATERIALCOMMANDLET_H

#include "Commandlets/BaseCommandlet.h"
#include "Render/Material.h"

/**
 * @ingroup WorldEd
 * Commandlet for import material from LMT (JSON) to engine class and save to package
 */
class LImportMaterialCommandlet : public LBaseCommandlet
{
	DECLARE_CLASS( LImportMaterialCommandlet, LBaseCommandlet )

public:
	/**
	 * Main method of execute commandlet
	 *
	 * @param[in] InCommands Arguments of start commandlet
	 * @return Return true if commandlet executed is seccussed, else returning false
	 */
	virtual bool Main( const std::wstring& InCommand ) override;

	/**
	 * Get supported material extensions
	 * @retun Return array of supported extensions (e.g "png", "jpg", etc)
	 */
	FORCEINLINE static std::vector< std::wstring > GetSupportedExtensins()
	{
		static std::vector< std::wstring >		supportedExtensions =
		{
			TEXT( "lmt" )
		};
		return supportedExtensions;
	}

	/**
	 * Is supported material extension
	 *
	 * @param InExtension Material extensions ("png", "jpg", etc)
	 * @return Return true if supported, else return false
	 */
	FORCEINLINE static bool IsSupportedExtension( const std::wstring& InExtension )
	{
		const std::vector< std::wstring >&		supportedExtensions = GetSupportedExtensins();
		for ( uint32 index = 0, count = supportedExtensions.size(); index < count; ++index )
		{
			if ( supportedExtensions[ index ] == InExtension )
			{
				return true;
			}
		}

		return false;
	}

private:
	/**
	 * Import material
	 * 
	 * @param InPath Path to material in JSON format
	 * @param InName Name material
	 * @return Return imported material. If failed return nullptr
	 */
	FMaterialRef ImportMaterial( const std::wstring& InPath, const std::wstring& InName );
};

#endif // !IMPORTMATERIALCOMMANDLET_H
