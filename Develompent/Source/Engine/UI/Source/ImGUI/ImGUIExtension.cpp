#include "LEBuild.h"
#if WITH_IMGUI

#ifndef IMGUI_DEFINE_MATH_OPERATORS
	#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "Misc/UIGlobals.h"
#include "Containers/StringConv.h"
#include "Containers/String.h"
#include "Misc/Enum.h"
#include "ImGUI/ImGUIExtension.h"
#include "ImGUI/ImGUIEngine.h"
#include "ImGUI/imgui_internal.h"
#include "ImGUI/imgui_stdlib.h"
#include "System/DragNDrop.h"

#include "Misc/WorldEdGlobals.h"
#include "System/EditorEngine.h"

/*
==================
ImGui::ImageButton
==================
*/
bool ImGui::ImageButton( ImTextureID InTextureID, bool InIsSelected, const ImVec2& InSize )
{
	if ( InIsSelected )
	{
		ImGui::PushStyleColor( ImGuiCol_Button,			g_ImGUIEngine->GetStyleColor( IGC_Selection ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered,	g_ImGUIEngine->GetStyleColor( IGC_Selection ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive,	g_ImGUIEngine->GetStyleColor( IGC_Selection ) );
	}
	
	bool	bRet = ImageButton( InTextureID, InSize );
	
	if ( InIsSelected )
	{
		ImGui::PopStyleColor( 3 );
	}
	return bRet;
}

/*
==================
ImGui::Button
==================
*/
bool ImGui::Button( const char* InLabel, bool InIsSelected, const ImVec2& InSize /*= ImVec2( 0.f, 0.f )*/ )
{
	if ( InIsSelected )
	{
		ImGui::PushStyleColor( ImGuiCol_Button,			g_ImGUIEngine->GetStyleColor( IGC_Selection ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered,	g_ImGUIEngine->GetStyleColor( IGC_Selection ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive,	g_ImGUIEngine->GetStyleColor( IGC_Selection ) );
	}
	
	bool	bRet = Button( InLabel, InSize );
	
	if ( InIsSelected )
	{
		ImGui::PopStyleColor( 3 );
	}
	return bRet;
}

/*
==================
ImGui::DragVectorFloat
==================
*/
bool ImGui::DragVectorFloat( const std::wstring& InStrId, Vector& InValue, float InResetValue /*= 0.f*/, float InSpeed /*= 1.f*/, float InMin /*= 0.f*/, float InMax /*= 0.f*/, const char* InFormat /*= "%.2f"*/, ImGuiSliderFlags InFlags /*= 0*/ )
{
	bool	bValueChanged = false;
	ImGui::PushMultiItemsWidths( 3, ImGui::CalcItemWidth() );
	ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2{ 0.f, 3.f } );
	
	// X axis
	ImGui::PushStyleColor( ImGuiCol_Button, g_ImGUIEngine->GetStyleColor( IGC_AxisXColor ) );
	ImGui::PushStyleColor( ImGuiCol_ButtonHovered, g_ImGUIEngine->GetStyleColor( IGC_AxisXColor_Hovered ) );
	ImGui::PushStyleColor( ImGuiCol_ButtonActive, g_ImGUIEngine->GetStyleColor( IGC_AxisXColor_Active ) );
	if ( ImGui::Button( TCHAR_TO_ANSI( CString::Format( TEXT( "X##%s" ), InStrId.c_str() ).c_str() ) ) )
	{
		InValue.x = InResetValue;
		bValueChanged = true;
	}
	if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
	{
		ImGui::SetTooltip( "Reset value" );
	}
	ImGui::PopStyleColor( 3 );

	ImGui::SameLine();
	bValueChanged |= ImGui::DragFloat( TCHAR_TO_ANSI( CString::Format( TEXT( "##%s_X" ), InStrId.c_str() ).c_str() ), &InValue.x, InSpeed, InMin, InMax, InFormat, InFlags );
	ImGui::PopItemWidth();

	// Y axis
	ImGui::SameLine();
	ImGui::PushStyleColor( ImGuiCol_Button, g_ImGUIEngine->GetStyleColor( IGC_AxisYColor ) );
	ImGui::PushStyleColor( ImGuiCol_ButtonHovered, g_ImGUIEngine->GetStyleColor( IGC_AxisYColor_Hovered ) );
	ImGui::PushStyleColor( ImGuiCol_ButtonActive, g_ImGUIEngine->GetStyleColor( IGC_AxisYColor_Active ) );
	if ( ImGui::Button( TCHAR_TO_ANSI( CString::Format( TEXT( "Y##%s" ), InStrId.c_str() ).c_str() ) ) )
	{
		InValue.y = InResetValue;
		bValueChanged = true;
	}
	if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
	{
		ImGui::SetTooltip( "Reset value" );
	}
	ImGui::PopStyleColor( 3 );

	ImGui::SameLine();
	bValueChanged |= ImGui::DragFloat( TCHAR_TO_ANSI( CString::Format( TEXT( "##%s_Y" ), InStrId.c_str() ).c_str() ), &InValue.y, InSpeed, InMin, InMax, InFormat, InFlags );
	ImGui::PopItemWidth();

	// Z axis
	ImGui::SameLine();
	ImGui::PushStyleColor( ImGuiCol_Button, g_ImGUIEngine->GetStyleColor( IGC_AxisZColor ) );
	ImGui::PushStyleColor( ImGuiCol_ButtonHovered, g_ImGUIEngine->GetStyleColor( IGC_AxisZColor_Hovered ) );
	ImGui::PushStyleColor( ImGuiCol_ButtonActive, g_ImGUIEngine->GetStyleColor( IGC_AxisZColor_Active ) );
	if ( ImGui::Button( TCHAR_TO_ANSI( CString::Format( TEXT( "Z##%s" ), InStrId.c_str() ).c_str() ) ) )
	{
		InValue.z = InResetValue;
		bValueChanged = true;
	}
	if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
	{
		ImGui::SetTooltip( "Reset value" );
	}
	ImGui::PopStyleColor( 3 );

	ImGui::SameLine();
	bValueChanged |= ImGui::DragFloat( TCHAR_TO_ANSI( CString::Format( TEXT( "##%s_Z" ), InStrId.c_str() ).c_str() ), &InValue.z, InSpeed, InMin, InMax, InFormat, InFlags );
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();
	return bValueChanged;
}

#if WITH_EDITOR

/*
==================
ImGui::SelectAsset
==================
*/
bool ImGui::SelectAsset( const std::wstring& InStrId, const std::wstring& InLabel, std::wstring& InOutAssetReference, bool* OutNeedOpenAssetEditor /*= nullptr*/, Texture2DRHIParamRef_t InPreviewTexture /*= nullptr*/ )
{
	static const ImVec2		s_ImageButtonSize( 16.f, 16.f );
	static const ImVec2		s_AssetPreviewSize( 64.f, 64.f );
	bool					bSelectedAsset = false;
	std::string				assetReference = TCHAR_TO_ANSI( InOutAssetReference.c_str() );

	ImGui::BeginGroup();

	// Preview asset
	if ( InPreviewTexture )
	{
		ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
		
		if ( ImGui::ImageButton( g_ImGUIEngine->LockTexture( InPreviewTexture ), s_AssetPreviewSize ) && OutNeedOpenAssetEditor )
		{
			*OutNeedOpenAssetEditor = true;
		}
		if ( !InOutAssetReference.empty() && ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Open asset editor" );
		}
		
		ImGui::PopStyleColor( 3 );
		ImGui::SameLine();
	}

	// Control panel
	if ( ImGui::BeginTable( "##SelectWidgetControlPanel", 1 ) )
	{
		// Label
		ImGui::TableNextColumn();
		if ( !InLabel.empty() )
		{
			ImGui::Text( TCHAR_TO_ANSI( InLabel.c_str() ) );
			ImGui::TableNextColumn();
		}

		// Input asset reference
		ImGui::PushItemWidth( -10 );
		if ( ImGui::InputTextWithHint( TCHAR_TO_ANSI( CString::Format( TEXT( "##%s" ), InStrId.c_str() ).c_str() ), "None", &assetReference, ImGuiInputTextFlags_EnterReturnsTrue) )
		{
			InOutAssetReference = ANSI_TO_TCHAR( assetReference.c_str() );
			bSelectedAsset		= true;
		}
		ImGui::PopItemWidth();
		ImGui::TableNextColumn();

		// Insert button
		if ( ImGui::ImageButton( g_ImGUIEngine->LockTexture( g_EditorEngine->GetIcon( IT_Insert ).ToSharedPtr()->GetTexture2DRHI() ), s_ImageButtonSize ) )
		{
			InOutAssetReference = g_EditorEngine->GetContentBrowserWindow()->GetSelectedAssetReference();
			bSelectedAsset		= true;
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Use asset selected in content browser" );
		}

		// Browser button
		ImGui::SameLine();
		if ( ImGui::ImageButton( g_ImGUIEngine->LockTexture( g_EditorEngine->GetIcon( IT_Browse ).ToSharedPtr()->GetTexture2DRHI() ), s_ImageButtonSize ) )
		{
			g_EditorEngine->GetContentBrowserWindow()->ShowAsset( ANSI_TO_TCHAR( assetReference.c_str() ) );
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Show asset in content browser" );
		}

		// Remove button
		ImGui::SameLine();
		if ( ImGui::ImageButton( g_ImGUIEngine->LockTexture( g_EditorEngine->GetIcon( IT_Remove ).ToSharedPtr()->GetTexture2DRHI() ), s_ImageButtonSize ) )
		{
			InOutAssetReference = TEXT( "" );
			bSelectedAsset		= true;
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Remove reference to asset" );
		}

		ImGui::EndTable();
	}
	
	// Manage of drop a asset's reference to input widget
	ImGui::EndGroup();
	if ( ImGui::BeginDragDropTarget() )
	{
		const ImGuiPayload*		imguiPayload = ImGui::AcceptDragDropPayload( DND_ASSETREFERENCEETYPE );
		if ( imguiPayload )
		{
			tchar*				pData = ( tchar* )imguiPayload->Data;
			Assert( pData );

			// Apply dropped asset reference
			std::vector<std::wstring>	assetReference;
			DND_ParseAssetReferenceData( pData, assetReference, 1 );
			if ( !assetReference.empty() )
			{
				InOutAssetReference = assetReference[0];
				bSelectedAsset		= true;

			}
		}
		ImGui::EndDragDropTarget();
	}

	return bSelectedAsset;
}
#endif // WITH_EDITOR

/*
==================
ImGui::SelectEnum
==================
*/
bool ImGui::SelectEnum( const std::wstring& InStrId, class CEnum* InEnum, byte& InOutEnumIndex )
{
	Assert( InEnum );
	bool	bSelectedEnum = false;

	if ( ImGui::BeginCombo( TCHAR_TO_ANSI( InStrId.c_str() ), TCHAR_TO_ANSI( InEnum->GetEnum( InOutEnumIndex ).ToString().c_str() ) ) )
	{
		const std::vector<CName>&	enums = InEnum->GetEnums();
		for ( uint32 index = 0, count = enums.size(); index < count; ++index )
		{
			if ( ImGui::Selectable( TCHAR_TO_ANSI( CString::Format( TEXT( "%s" ), enums[index].ToString().c_str() ).c_str() ), index == InOutEnumIndex, ImGuiSelectableFlags_DontClosePopups ) )
			{
				bSelectedEnum	= true;
				InOutEnumIndex	= index;
			}
		}
		ImGui::EndCombo();
	}

	return bSelectedEnum;
}

/*
==================
ImGui::CollapsingHeader
==================
*/
bool ImGui::CollapsingHeader( const std::wstring& InLabel, bool InIgnoreDisabled, ImGuiTreeNodeFlags InFlags /*= 0*/ )
{
	// If need to ignore disable flag we call ImGui::EndDisabled and then in the end of the function have to re-enable it
	bool	bWasDisabled = InIgnoreDisabled && ( ImGui::GetCurrentContext()->CurrentItemFlags & ImGuiItemFlags_Disabled ) != 0;
	if ( bWasDisabled )
	{
		ImGui::EndDisabled();
	}
	
	bool	bResult = ImGui::CollapsingHeader( TCHAR_TO_ANSI( InLabel.c_str() ), InFlags );
	
	// If items was disabled we re-enable it
	if ( bWasDisabled )
	{
		ImGui::BeginDisabled( true );
	}
	return bResult;
}

/*
==================
ImGui::Arrow
==================
*/
void ImGui::Arrow( ImGuiDir InArrowDir, ImVec2 InSize )
{
	ImGuiContext*	imguiContext = GetCurrentContext();
	ImGuiWindow*	imguiWindow = GetCurrentWindow();
	if ( imguiWindow->SkipItems )
	{
		return;
	}
	
	if ( InSize.x == 0.f )
	{
		InSize.x = ImGui::GetFrameHeight();
	}

	const ImRect	rect( imguiWindow->DC.CursorPos, imguiWindow->DC.CursorPos + InSize );
	const float		defaultSize = GetFrameHeight();
	const ImU32		arrowColor = GetColorU32( ImGuiCol_Text );
	ItemSize( InSize, ( InSize.y >= defaultSize ) ? imguiContext->Style.FramePadding.y : -1.0f );
	RenderArrow( imguiWindow->DrawList, rect.Min + ImVec2( ImMax( 0.0f, ( InSize.x - imguiContext->FontSize ) * 0.5f ), ImMax( 0.0f, ( InSize.y - imguiContext->FontSize ) * 0.5f ) ), arrowColor, InArrowDir );
}

/*
==================
ImGui::CollapsingArrayHeader
==================
*/
ImGui::CollapsingArrayHeaderResult ImGui::CollapsingArrayHeader( const std::wstring& InStrId, const std::wstring& InLabel, uint32 InFlags /* = 0 */, float InItemWidthSpacing /* = 0.f */, const std::wstring& InMessage /* = TEXT( "" ) */, const std::wstring& InLabelToolTip /* = TEXT( "" ) */, const std::wstring& InAddButtonToolTip /* = TEXT( "" ) */, const std::wstring& InRemoveButtonToolTip /* = TEXT( "" ) */, bool InIgnoreDisabled /* = false */ )
{
	static const ImVec2			s_ImageButtonSize( 12.f, 12.f );
	ImGuiWindow*				imguiWindow		= GetCurrentWindow();
	ImGuiStorage*				storage			= imguiWindow->DC.StateStorage;
	ImGuiID						arrowButtonId	= imguiWindow->GetID( TCHAR_TO_ANSI( InStrId.c_str() ) );
	CollapsingArrayHeaderResult	result;
	
	// If need to ignore disable flag we call ImGui::EndDisabled and then in the end of the function have to re-enable it
	bool	bWasDisabled = InIgnoreDisabled && ( ImGui::GetCurrentContext()->CurrentItemFlags & ImGuiItemFlags_Disabled ) != 0;
	if ( bWasDisabled )
	{
		ImGui::EndDisabled();
	}

	ImGui::Columns( 2 );
	ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( ImGui::GetStyle().FramePadding.x, 1.f ) );
	ImGui::Dummy( ImVec2( InItemWidthSpacing, 0.f ) );
	ImGui::SameLine();
	
	// Draw selectable and arrow widget
	result.bIsOpened = storage->GetInt( arrowButtonId, 0 );
	if ( ImGui::Selectable( TCHAR_TO_ANSI( InStrId.c_str() ), false, ImGuiSelectableFlags_SpanRemainingColumns | ImGuiSelectableFlags_AllowItemOverlap ) )
	{
		result.bIsOpened = !result.bIsOpened;
		storage->SetInt( arrowButtonId, result.bIsOpened );
	}
	ImGui::SameLine();
	ImGui::Arrow( result.bIsOpened ? ImGuiDir_Down : ImGuiDir_Right );

	// Draw label
	ImGui::SameLine();
	ImGui::Text( TCHAR_TO_ANSI( InLabel.c_str() ) );
	if ( !InLabelToolTip.empty() && ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
	{
		ImGui::SetTooltip( TCHAR_TO_ANSI( InLabelToolTip.c_str() ) );
	}

	// Draw message
	ImGui::NextColumn();
	if ( !InMessage.empty() )
	{
		ImGui::Text( TCHAR_TO_ANSI( InMessage.c_str() ) );
		ImGui::SameLine();
	}
	
	// If items was disabled we re-enable it
	if ( bWasDisabled )
	{
		ImGui::BeginDisabled( true );
	}

	// Draw add button
	if ( InFlags & ImGuiCollapsingArrayHeaderFlags_AddButton )
	{
		ImGui::PushID( arrowButtonId );
		result.bPressedAdd = ImGui::ImageButton( g_ImGUIEngine->LockTexture( g_EditorEngine->GetIcon( IT_Add ).ToSharedPtr()->GetTexture2DRHI() ), s_ImageButtonSize );
		ImGui::PopID();

		if ( !InAddButtonToolTip.empty() && ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( TCHAR_TO_ANSI( InAddButtonToolTip.c_str() ) );
		}	
	}

	if ( ( InFlags & ImGuiCollapsingArrayHeaderFlags_AllButtons ) == ImGuiCollapsingArrayHeaderFlags_AllButtons )
	{
		ImGui::SameLine( 0, 2.f );
	}

	// Draw remove button
	if ( InFlags & ImGuiCollapsingArrayHeaderFlags_RemoveButton )
	{
		ImGui::PushID( arrowButtonId );
		result.bPressedRemove = ImGui::ImageButton( g_ImGUIEngine->LockTexture( g_EditorEngine->GetIcon( IT_Remove ).ToSharedPtr()->GetTexture2DRHI() ), s_ImageButtonSize );
		ImGui::PopID();

		if ( !InRemoveButtonToolTip.empty() && ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( TCHAR_TO_ANSI( InRemoveButtonToolTip.c_str() ) );
		}
	}

	ImGui::PopStyleVar();
	ImGui::Columns( 1 );
	return result;
}

#endif // WITH_IMGUI