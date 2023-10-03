/**
 * @file
 * @addtogroup UI User interface
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef IMGUIEXTENSION_H
#define IMGUIEXTENSION_H

#include "LEBuild.h"
#if WITH_IMGUI

#include <string>

#include "Math/Math.h"
#include "ImGUI/imgui.h"
#include "Render/Texture.h"

/**
 * @ingroup UI
 * @brief Enumeration flags for ImGui::CollapsingArrayHeader
 */
enum ImGuiCollapsingArrayHeaderFlags
{
	ImGuiCollapsingArrayHeaderFlags_AddButton		= 1 << 1,																						/**< Draw an add button */
	ImGuiCollapsingArrayHeaderFlags_RemoveButton	= 1 << 2,																						/**< Draw an remove button */
	ImGuiCollapsingArrayHeaderFlags_AllButtons		= ImGuiCollapsingArrayHeaderFlags_AddButton | ImGuiCollapsingArrayHeaderFlags_RemoveButton		/**< Draw all buttons */
};

namespace ImGui
{
	/**
	 * @ingroup UI
	 * @brief Structure for return results from ImGui::CollapsingArrayHeader
	 */
	struct CollapsingArrayHeaderResult
	{
		/**
		 * @brief Constructor
		 */
		CollapsingArrayHeaderResult()
			: bIsOpened( false )
			, bPressedAdd( false )
			, bPressedRemove( false )
		{}

		bool	bIsOpened;		/**< Is opened header */
		bool	bPressedAdd;	/**< Button 'Add' was pressed */
		bool	bPressedRemove;	/**< Button 'Remove' was pressed */
	};

	/**
	 * @ingroup UI
	 * @brief ImGui help function for draw ImageButton with selection opportunity
	 * 
	 * @param InTextureID	Texture ID
	 * @param InIsSelected	Is selected button
	 * @param InSize		Imgae button size
	 * @return Return TRUE if image button is pressed (same behavior as original ImGui::ImageButton)
	 */
	bool ImageButton( ImTextureID InTextureID, bool InIsSelected, const ImVec2& InSize );

	/**
	 * @ingroup UI
	 * @brief ImGui help function for draw Button with selection opportunity
	 *
	 * @param InLabel		Label
	 * @param InIsSelected	Is selected button
	 * @param InSize		Imgae button size
	 * @return Return TRUE if button is pressed (same behavior as original ImGui::Button)
	 */
	bool Button( const char* InLabel, bool InIsSelected, const ImVec2& InSize = ImVec2( 0.f, 0.f ) );

	/**
	 * @ingroup UI
	 * @brief Draw a widget for dragging Vector value
	 * 
	 * @param InStrId		String that used as an ID
	 * @param InValue		Editable value
	 * @param InResetValue	When pressed button 'X'/'Y'/'Z' then corresponding component of InValue will be reset to that value
	 * @param InSpeed		Speed are per-pixel of mouse movement (InSpeed=0.2f: mouse needs to move by 5 pixels to increase value by 1)
	 * @param InMin			Min value (if equal to InMax will be ignore)
	 * @param InMax			Max value (if equal to InMin will be ignore)
	 * @param InFormat		Format of printing value on interface
	 * @param InFlags		ImGui flags (see enum ImGuiSliderFlags_)
	 * @return Return TRUE if some component of InValue was changed, otherwise FALSE
	 */
	bool DragVectorFloat( const std::wstring& InStrId, Vector& InValue, float InResetValue = 0.f, float InSpeed = 1.f, float InMin = 0.f, float InMax = 0.f, const char* InFormat = "%.2f", ImGuiSliderFlags InFlags = 0 );

#if WITH_EDITOR
	/**
	 * @ingroup UI
	 * @brief Draw a widget for select asset
	 * 
	 * @param InStrId					String that used as an ID
	 * @param InLabel					Label
	 * @param InOutAssetReference		String of asset reference to display. If it changed here will be new value
	 * @param OutNeedOpenAssetEditor	TRUE when user pressed on button of preview image
	 * @param InPreviewTexture			Preview texture. If NULL she will not drawed
	 * @return Return TRUE when asset reference is changed, otherwise FALSE
	 */
	bool SelectAsset( const std::wstring& InStrId, const std::wstring& InLabel, std::wstring& InOutAssetReference, bool* OutNeedOpenAssetEditor = nullptr, Texture2DRHIParamRef_t InPreviewTexture = nullptr );
#endif // WITH_EDITOR

	/**
	 * @ingroup UI
	 * @brief Draw a widget for select enum
	 * 
	 * @param InStrId			String that used as an ID
	 * @param InEnum			Enum
	 * @param InOutEnumIndex	Current index of enum. In here will be return new enum index 
	 * @return Return TRUE when enum is changed, otherwise FALSE
	 */
	bool SelectEnum( const std::wstring& InStrId, class CEnum* InEnum, byte& InOutEnumIndex );

	/**
	 * @ingroup UI
	 * @brief Draw a collapsing header
	 * 
	 * @param InLabel			Label
	 * @param InIgnoreDisabled	Is need ignore disable flag
	 * @param InFlags			Flags
	 * @return Return TRUE when header is opened, otherwise FALSE
	 */
	bool CollapsingHeader( const std::wstring& InLabel, bool InIgnoreDisabled, ImGuiTreeNodeFlags InFlags = 0 );

	/**
	 * @ingroup UI
	 * @brief Draw an arrow
	 * 
	 * @param InArrowDir	Arrow direction
	 * @param InSize		Arrow Size. If X component is zero it will be replaced on ImGui::GetFrameHeight()
	 */
	void Arrow( ImGuiDir InArrowDir, ImVec2 InSize = ImVec2( 0.f, 0.f ) );

	/**
	 * @ingroup UI
	 * @brief Draw a collapsing header for array
	 * 
	 * @param InStrId				String that used as an ID
	 * @param InLabel				Label
	 * @param InFlags				Flags (see ImGuiCollapsingArrayHeaderFlags)
	 * @param InItemWidthSpacing	Item spacing by width
	 * @param InMessage				Message in second column, may be empty
	 * @param InLabelToolTip		Tooltip message for label. If is empty tooltip won't show
	 * @param InAddButtonToolTip	Tooltip message for add button. If is empty tooltip won't show
	 * @param InRemoveButtonToolTip	Tooltip message for remove button. If is empty tooltip won't show
	 * @param InIgnoreDisabled		Is need ignore disable flag
	 * @return Return CollapsingArrayHeaderResult whom describe any statuses, i.g is opened header or was pressed add button
	 */
	CollapsingArrayHeaderResult CollapsingArrayHeader( const std::wstring& InStrId, const std::wstring& InLabel, uint32 InFlags = 0, float InItemWidthSpacing = 0.f, const std::wstring& InMessage = TEXT( "" ), const std::wstring& InLabelToolTip = TEXT( "" ), const std::wstring& InAddButtonToolTip = TEXT( "" ), const std::wstring& InRemoveButtonToolTip = TEXT( "" ), bool InIgnoreDisabled = false );
}

#endif // WITH_IMGUI
#endif // !IMGUIEXTENSION_H