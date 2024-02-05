/**
 * @file
 * @addtogroup WorldEd World editor
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

#ifndef IMPORTSETTINGSDIALOGS_H
#define IMPORTSETTINGSDIALOGS_H

#include "Core/System/Delegate.h"
#include "Core/System/Package.h"
#include "UI/ImGUI/ImGUIEngine.h"

/**
 * @ingroup Editor
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