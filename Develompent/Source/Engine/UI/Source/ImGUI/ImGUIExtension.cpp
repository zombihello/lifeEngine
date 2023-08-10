#include "LEBuild.h"
#if WITH_IMGUI

#include "Misc/UIGlobals.h"
#include "Containers/StringConv.h"
#include "Containers/String.h"
#include "ImGUI/ImGUIExtension.h"
#include "ImGUI/ImGUIEngine.h"
#include "ImGUI/imgui_internal.h"

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
	ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2{ 0.f, 0.f } );
	
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
#endif // WITH_IMGUI