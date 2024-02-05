/**
 * @file
 * @addtogroup Editor Editor
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

#ifndef EDITORENGINE_H
#define EDITORENGINE_H

#include "Core/Logger/BaseLogger.h"
#include "Engine/System/BaseEngine.h"
#include "Editor/Editor.h"
#include "Editor/System/EditorConstraints.h"

/**
 * @ingroup Editor
 * @brief Enumeration icon types
 */
enum EIconType
{
	IT_Color_R,										/**< Color R channel */
	IT_Color_G,										/**< Color G channel */
	IT_Color_B,										/**< Color B channel */
	IT_Color_A,										/**< Color A channel */
	IT_Import,										/**< Import */
	IT_Insert,										/**< Insert */
	IT_Browse,										/**< Browse */
	IT_Remove,										/**< Remove */
	IT_Add,											/**< Add */
	IT_ToolSelect,									/**< Tool select */
	IT_ToolTranslate,								/**< Tool translate */
	IT_ToolRotate,									/**< Tool rotate */
	IT_ToolScale,									/**< Tool scale */
	IT_PlayStandaloneGame,							/**< Play standalone game */
	IT_Thumbnail_Asset_Unknown,						/**< Unknown asset */
	IT_Thumbnail_Asset_Texture2D,					/**< Texture2D asset */
	IT_Thumbnail_Asset_Material,					/**< Material asset */
	IT_Thumbnail_Asset_Script,						/**< Script asset */
	IT_Thumbnail_Asset_StaticMesh,					/**< StaticMesh asset */
	IT_Thumbnail_Asset_AudioBank,					/**< AudioBank asset */
	IT_Thumbnail_Asset_PhysicsMaterial,				/**< PhysicsMaterial asset */
	IT_Num											/**< Number of icon types */
};

/**
 * @ingroup WorldEd
 * @brief Base editor engine
 */
class CEditorEngine : public CBaseEngine
{
	DECLARE_CLASS( CEditorEngine, CBaseEngine, 0, 0 )

public:
	/**
	 * @brief Constructor
	 */
	CEditorEngine();

	/**
	 * @brief Destructor
	 */
	virtual ~CEditorEngine();

	/**
	 * @brief Initialize engine
	 */
	virtual void Init() override;

	/**
	 * @brief Print log to log widget
	 * 
	 * @param InLogType		Log type
	 * @param InMessage		Message of log
	 */
	virtual void PrintLogToWidget( ELogType InLogType, const tchar* InMessage );

	/**
	 * @brief Get selected asset
	 * @param Return reference of selected asset. If isn't returns empty string
	 */
	virtual std::wstring GetSelectedAsset() const;

	/**
	 * @brief Set selected asset
	 * @param InAssetReference	New asset reference to select
	 */
	virtual void SetSelectedAsset( const std::wstring& InAssetReference );

	/**
	 * @brief Create new map
	 */
	virtual void NewMap();

	/**
	 * @brief Load map
	 *
	 * @param InMap		Path to map
	 * @param OutError	Error of loading map
	 * @return Return TRUE if map was loaded successes, otherwise FALSE
	 */
	virtual bool LoadMap( const std::wstring& InMap, std::wstring& OutError ) override;

	/**
	 * @brief Save map
	 *
	 * @param InMap		Path to map
	 * @param OutError	Error of saving map
	 * @return Return true if map was saved successes, otherwise FALSE
	 */
	virtual bool SaveMap( const std::wstring& InMap, std::wstring& OutError );

	/**
	 * @brief Serialize TOC
	 *
	 * @param InIsSave	Is save TOC file
	 * @return Return TRUE if TOC file serialized successes, otherwise FALSE
	 */
	bool SerializeTOC( bool InIsSave = false );

	/**
	 * @brief Get editor name
	 * @return Return editor name
	 */
	virtual std::wstring GetEditorName() const;

	/**
	 * @brief Get game content directory
	 * @return Return game content directory
	 */
	FORCEINLINE std::wstring GetGameContentDir() const
	{
		return CString::Format( TEXT( "%s" ) PATH_SEPARATOR TEXT( "Content" ) PATH_SEPARATOR, Sys_GameDir().c_str() );
	}

	/**
	 * @brief Get engine content directory
	 * @return Return engine content directory
	 */
	FORCEINLINE std::wstring GetEngineContentDir() const
	{
		return CString::Format( TEXT( "%s" ) PATH_SEPARATOR TEXT( "Engine" ) PATH_SEPARATOR TEXT( "Content" ) PATH_SEPARATOR, Sys_BaseDir().c_str() );
	}

	/**
	 * @brief Get selection color
	 * @return Return selection color
	 */
	FORCEINLINE CColor GetSelectionColor() const
	{
		return selectionColor;
	}

	/**
	 * @brief Set current editor mode
	 * @param InNewEditorMode	New editor mode
	 */
	FORCEINLINE void SetEditorMode( EEditorMode InNewEditorMode )
	{
		currentEditorMode = InNewEditorMode;
		EditorDelegates::onEditorModeChanged.Broadcast( InNewEditorMode );
	}

	/**
	 * @brief Get current editor mode
	 * @return Return current editor mode
	 */
	FORCEINLINE EEditorMode GetEditorMode() const
	{
		return currentEditorMode;
	}

	/**
	 * @brief Get editor constraints
	 * @return Return editor constraints
	 */
	FORCEINLINE CEditorConstraints& GetConstraints()
	{
		return constraints;
	}

	/**
	 * @brief Get editor constraints
	 * @return Return editor constraints
	 */
	FORCEINLINE const CEditorConstraints& GetConstraints() const
	{
		return constraints;
	}

	/**
	 * @brief Get icon
	 *
	 * @param InIcon	Icon
	 * @return Return asset handle of icon
	 */
	FORCEINLINE TAssetHandle<CTexture2D> GetIcon( EIconType InIcon ) const
	{
		Assert( InIcon != IT_Num );
		return icons[InIcon];
	}

protected:
	CColor						selectionColor;		/**< Selection color */
	EEditorMode					currentEditorMode;	/**< Current editor mode */
	CEditorConstraints			constraints;		/**< Editor constraints */
	TAssetHandle<CTexture2D>	icons[IT_Num];		/**< Icons */

private:
	/**
	 * @brief Add TOC entries
	 * @param InRootDir		Root directory
	 */
	void AddTOCEntries( const std::wstring& InRootDir );
};

#endif // !EDITORENGINE_H