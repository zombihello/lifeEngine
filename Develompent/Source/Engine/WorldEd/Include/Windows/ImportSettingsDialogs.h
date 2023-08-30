/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef IMPORTSETTINGSDIALOGS_H
#define IMPORTSETTINGSDIALOGS_H

#include "ImGUI/ImGUIEngine.h"
#include "System/Delegate.h"
#include "System/Package.h"

/**
 * @ingroup WorldEd
 * @brief Dialog of import settings a static mesh
 */
class CStaticMeshImportSettingsDialog : public CImGUIPopup
{
public:
	/**
	 * @brief Enumeration of exis up
	 */
	enum EAxisUp
	{
		AU_PlusX,					/**< +X */
		AU_MinusX,					/**< -X */
		AU_PlusY,					/**< +Y */
		AU_MinusY,					/**< -Y */
		AU_PlusZ,					/**< +Z */
		AU_MinusZ,					/**< -Z */

		AU_Num,						/**< Number of axis ups */
		AU_Default = AU_PlusY,		/**< Default axis up */
	};

	/**
	 * @brief Struct with import settings
	 */
	struct ImportSettings
	{
		/**
		 * @brief Constructor
		 */
		ImportSettings()
			: bCombineMeshes( false )
			, axisUp( AU_PlusY )
		{}

		bool		bCombineMeshes;		/**< Is need combine all meshes to one */
		EAxisUp		axisUp;				/**< Axis up */
	};

	/**
	 * @brief Delegate of resume, called when pressed button 'Import', 'Import All or 'Cancel'
	 */
	DECLARE_MULTICAST_DELEGATE( COnResume, CAssetFactory::EResultShowImportSettings /*InResult*/, const ImportSettings& /*InImportSettings*/ );

	/**
	 * @brief Constructor
	 */
	CStaticMeshImportSettingsDialog();

	/**
	 * @brief Get delegate of resume
	 * @return Return delegate of resume
	 */
	FORCEINLINE COnResume& OnResume() const
	{
		return onResume;
	}

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

private:
	ImportSettings			importSettings;	/**< Import settings */
	mutable COnResume		onResume;		/**< Delegates of resume */
};

#endif // !IMPORTSETTINGSDIALOGS_H