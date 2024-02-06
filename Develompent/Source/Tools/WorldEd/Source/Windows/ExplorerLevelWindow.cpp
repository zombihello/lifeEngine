/**
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

#include "Core/Containers/StringConv.h"
#include "Core/Containers/String.h"
#include "Core/System/MemoryArchive.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/System/World.h"
#include "Engine/System/InputSystem.h"
#include "UI/Misc/UIGlobals.h"
#include "UI/ImGUI/ImGUIEngine.h"
#include "Windows/ExplorerLevelWindow.h"
#include "Windows/InputTextDialog.h"

/**
  * @ingroup WorldEd
  * @brief Runnable object for rename actor on level
  */
class CRenameActorRunnable : public CRunnable
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InOwner	Owner
	 * @param InActor	Actor to rename
	 */
	CRenameActorRunnable( CExplorerLevelWindow* InOwner, ActorRef_t InActor )
		: owner( InOwner )
		, actor( InActor )
	{}

	/**
	 * @brief Initialize
	 *
	 * Allows per runnable object initialization. NOTE: This is called in the
	 * context of the thread object that aggregates this, not the thread that
	 * passes this runnable to a new thread.
	 *
	 * @return True if initialization was successful, false otherwise
	 */
	virtual bool Init() override
	{
		Assert( owner && actor );
		return true;
	}

	/**
	 * @brief Run
	 *
	 * This is where all per object thread work is done. This is only called
	 * if the initialization was successful.
	 *
	 * @return The exit code of the runnable object
	 */
	virtual uint32 Run() override
	{
		// Get new actor name. If we not press 'ok' nothing apply and exit from method
		bool			bIsOk = false;
		std::wstring	newActorName;
		{
			TSharedPtr<CInputTextDialog>	popup = owner->OpenPopup<CInputTextDialog>( TEXT( "Enter" ), TEXT( "New Actor Name" ), actor->GetName() );
			popup->OnTextEntered().Add( [&]( const std::string& InText )
										{
											bIsOk = true;
											newActorName = ANSI_TO_TCHAR( InText.c_str() );
											eventResponse.Trigger();
										} );

			popup->OnCenceled().Add( [&]()
									 {
										 bIsOk = false;
										 eventResponse.Trigger();
									 } );
			eventResponse.Wait();
			if ( !bIsOk )
			{
				return 0;
			}
		}

		// Add change actor name
		actor->SetName( newActorName.c_str() );

		// Mark world as dirty
		g_World->MarkDirty();
		return 0;
	}

	/**
	 * @brief Stop
	 *
	 * This is called if a thread is requested to terminate early
	 */
	virtual void Stop() override
	{}

	/**
	 * @brief Exit
	 *
	 * Called in the context of the aggregating thread to perform any cleanup.
	 */
	virtual void Exit() override
	{}

private:
	CExplorerLevelWindow*	owner;			/**< Owner */
	CThreadEvent			eventResponse;	/**< Event used when opened popup */
	ActorRef_t				actor;			/**< Actor to rename */
};

/*
==================
CExplorerLevelWindow::CExplorerLevelWindow
==================
*/
CExplorerLevelWindow::CExplorerLevelWindow( const std::wstring& InName )
	: CImGUILayer( InName )
{}

/*
==================
CExplorerLevelWindow::OnTick
==================
*/
void CExplorerLevelWindow::OnTick()
{
	// Draw table of actors
	if ( ImGui::BeginTable( "##ExploerLevelTable", 3, ImGuiTableFlags_Resizable ) )
	{
		// Init header table
		ImGui::TableSetupColumn( "Visibility" );
		ImGui::TableSetupColumn( "Label" );
		ImGui::TableSetupColumn( "Class" );
		ImGui::TableHeadersRow();

		// Init style
		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding,	ImVec2( ImGui::GetStyle().FramePadding.x, 2.f ) );
		ImGui::PushStyleColor( ImGuiCol_Header,				g_ImGUIEngine->GetStyleColor( IGC_Selection ) );

		// Print info about each actor on level
		for ( uint32 index = 0; index < g_World->GetNumActors(); ++index )
		{
			ActorRef_t		actor = g_World->GetActor( index );
			Assert( actor );
			bool			bNewVisibility = actor->IsVisibility();

			ImGui::TableNextRow();
			ImGui::TableSetBgColor( ImGuiTableBgTarget_RowBg0, !( index % 2 ) ? ImGui::ColorConvertFloat4ToU32( g_ImGUIEngine->GetStyleColor( IGC_TableBgColor0 ) ) : ImGui::ColorConvertFloat4ToU32( g_ImGUIEngine->GetStyleColor( IGC_TableBgColor1 ) ) );
			
			// Visibility flag
			ImGui::TableNextColumn();
			if ( ImGui::Checkbox( TCHAR_TO_ANSI( CString::Format( TEXT( "##Visibility_ID_%i" ), index ).c_str() ), &bNewVisibility ) )
			{
				actor->SetVisibility( bNewVisibility );
				g_World->MarkDirty();
			}
			
			// Label
			ImGui::TableNextColumn();
			ImGui::Text( TCHAR_TO_ANSI( actor->GetName().c_str() ) );		

			// Class name
			ImGui::TableNextColumn();
			ImGui::Text( TCHAR_TO_ANSI( actor->GetClass()->GetName().c_str() ) );

			// Process events
			ProcessItemEvents( index, actor );
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::EndTable();
	}

	// Draw popup menu
	DrawPopupMenu();

	// Reset selection if clicked not on item
	if ( ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) || ImGui::IsMouseDown( ImGuiMouseButton_Right ) ) )
	{
		g_World->UnselectAllActors();
	}
}

/*
==================
CExplorerLevelWindow::ProcessItemEvents
==================
*/
void CExplorerLevelWindow::ProcessItemEvents( uint32 InIndex, ActorRef_t InActor )
{
	ImGui::SameLine();
	{
		const bool	bCtrlDown = g_InputSystem->IsKeyDown( BC_KeyLControl ) || g_InputSystem->IsKeyDown( BC_KeyRControl );
		const bool	bRMBDown = ImGui::IsMouseDown( ImGuiMouseButton_Right );
		bool		bNeedPopStyleColor = false;
		bool		bSelected = InActor->IsSelected();

		if ( bSelected )
		{
			ImGui::PushStyleColor( ImGuiCol_HeaderHovered, g_ImGUIEngine->GetStyleColor( IGC_Selection ) );
			bNeedPopStyleColor = true;
		}
		if ( ImGui::Selectable( TCHAR_TO_ANSI( CString::Format( TEXT( "##Selectable_ID_%i" ), InIndex ).c_str() ), &bSelected, ImGuiSelectableFlags_SpanAllColumns ) ||
			 ImGui::IsItemHovered() && bRMBDown )
		{
			bool	bUnselectedAllActors = false;
			if ( ( !bCtrlDown && !bRMBDown ) || ( bRMBDown && !bSelected ) )
			{
				g_World->UnselectAllActors();
				bUnselectedAllActors = true;
			}

			if ( bSelected || ( bUnselectedAllActors && !bCtrlDown ) || bRMBDown )
			{
				g_World->SelectActor( InActor );
			}
			else
			{
				g_World->UnselectActor( InActor );
			}
		}
		if ( bNeedPopStyleColor )
		{
			ImGui::PopStyleColor();
		}
	}
}

/*
==================
CExplorerLevelWindow::DrawPopupMenu
==================
*/
void CExplorerLevelWindow::DrawPopupMenu()
{
	if ( ImGui::BeginPopupContextWindow( "", ImGuiMouseButton_Right ) )
	{	
		bool	bSelectedActors			= g_World->GetNumSelectedActors() > 0;
		bool	bSelectedOnlyOneActor	= g_World->GetNumSelectedActors() == 1;

		// Rename
		if ( ImGui::MenuItem( "Rename", "", nullptr, bSelectedOnlyOneActor ) )
		{
			const std::vector<ActorRef_t>&		selectedActors = g_World->GetSelectedActors();
			CThread*							renameActorThread = new CThread();
			renameActorThread->Create( new CRenameActorRunnable( this, selectedActors[0] ), CString::Format( TEXT( "RenameActor_%s" ), selectedActors[0]->GetName() ).c_str(), true, true );
		}

		// Duplicate
		if ( ImGui::MenuItem( "Duplicate", "", nullptr, bSelectedActors ) )
		{
			std::vector<ActorRef_t>		selectedActors = g_World->GetSelectedActors();
			std::vector<byte>			memoryData;
			for ( uint32 index = 0, count = selectedActors.size(); index < count; ++index )
			{			
				// Serialize actor to memory		
				ActorRef_t				actor = selectedActors[index];
				CMemoryWriter			memoryWriter( memoryData );
				actor->Serialize( memoryWriter );

				// Spawn new actor and serialize data from memory
				ActorRef_t				newActor = g_World->SpawnActor( actor->GetClass(), actor->GetActorLocation(), actor->GetActorRotation() );
				CMemoryReading			memoryReading( memoryData );
				newActor->Serialize( memoryReading );
			}
		}

		// Delete
		if ( ImGui::MenuItem( "Delete", "", nullptr, bSelectedActors ) )
		{
			std::vector<ActorRef_t>		selectedActors = g_World->GetSelectedActors();
			for ( uint32 index = 0, count = selectedActors.size(); index < count; ++index )
			{
				g_World->DestroyActor( selectedActors[index] );
			}
		}

		ImGui::EndPopup();
	}
}