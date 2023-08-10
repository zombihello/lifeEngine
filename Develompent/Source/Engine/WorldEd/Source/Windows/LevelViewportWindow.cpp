#include "Misc/WorldEdGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/UIGlobals.h"
#include "Logger/LoggerMacros.h"
#include "Windows/LevelViewportWindow.h"
#include "Windows/DialogWindow.h"
#include "Render/EditorLevelViewportClient.h"
#include "System/EditorEngine.h"
#include "System/World.h"
#include "System/BaseFileSystem.h"
#include "System/ActorFactory.h"
#include "Render/RenderingThread.h"
#include "System/AssetsImport.h"

/** Macro size button in menu bar */
#define LEVELVIEWPORT_MENUBAR_BUTTONSIZE	ImVec2( 16.f, 16.f )

/*
==================
CLevelViewportWindow::CLevelViewportWindow
==================
*/
CLevelViewportWindow::CLevelViewportWindow( const std::wstring& InName, bool InVisibility /* = true */, ELevelViewportType InViewportType /* = LVT_Perspective */ )
	: CImGUILayer( InName )
	, bGuizmoUsing( false )
	, guizmoOperationType( ImGuizmo::TRANSLATE )
	, guizmoModeType( ImGuizmo::LOCAL )
	, viewportCursorPos( 0.f, 0.f )
	, viewportWidget( true, &viewportClient )
{
	flags |= ImGuiWindowFlags_MenuBar;

	viewportClient.SetViewportType( InViewportType );
	SetVisibility( InVisibility );
	SetPadding( Vector2D( 0.f, 0.f ) );
}

/*
==================
CLevelViewportWindow::Init
==================
*/
void CLevelViewportWindow::Init()
{
	CImGUILayer::Init();
	viewportWidget.Init();
}

/*
==================
CLevelViewportWindow::OnTick
==================
*/
void CLevelViewportWindow::OnTick()
{
	// We set focus on window if mouse tracking is working and window not taken focus
	if ( !IsFocused() && viewportClient.GetMouseTrackingType() != CEditorLevelViewportClient::MT_None )
	{
		ImGui::SetWindowFocus();
	}
	
	// Menu bar
	if ( ImGui::BeginMenuBar() )
	{
		// Tool 'Select objects'
		if ( ImGui::ImageButton( g_ImGUIEngine->LockTexture( g_EditorEngine->GetIcon( IT_ToolSelect ).ToSharedPtr()->GetTexture2DRHI() ), guizmoOperationType == ImGuizmo::NONE, LEVELVIEWPORT_MENUBAR_BUTTONSIZE ) )
		{
			guizmoOperationType = ImGuizmo::NONE;
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Select objects" );
		}

		// Tool 'Select and translate objects'
		if ( ImGui::ImageButton( g_ImGUIEngine->LockTexture( g_EditorEngine->GetIcon( IT_ToolTranslate ).ToSharedPtr()->GetTexture2DRHI() ), guizmoOperationType == ImGuizmo::TRANSLATE, LEVELVIEWPORT_MENUBAR_BUTTONSIZE ) )
		{
			guizmoOperationType = ImGuizmo::TRANSLATE;
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Select and translate objects" );
		}

		// Tool 'Select and rotate objects'
		if ( ImGui::ImageButton( g_ImGUIEngine->LockTexture( g_EditorEngine->GetIcon( IT_ToolRotate ).ToSharedPtr()->GetTexture2DRHI() ), guizmoOperationType == ImGuizmo::ROTATE, LEVELVIEWPORT_MENUBAR_BUTTONSIZE ) )
		{
			guizmoOperationType = ImGuizmo::ROTATE;
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Select and rotate objects" );
		}

		// Tool 'Select and scale objects'
		if ( ImGui::ImageButton( g_ImGUIEngine->LockTexture( g_EditorEngine->GetIcon( IT_ToolScale ).ToSharedPtr()->GetTexture2DRHI() ), guizmoOperationType == ImGuizmo::SCALE, LEVELVIEWPORT_MENUBAR_BUTTONSIZE ) )
		{
			guizmoOperationType = ImGuizmo::SCALE;
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Select and scale objects" );
		}

		// Play standalone game
		ImGui::Separator();
		if ( ImGui::ImageButton( g_ImGUIEngine->LockTexture( g_EditorEngine->GetIcon( IT_PlayStandaloneGame ).ToSharedPtr()->GetTexture2DRHI() ), LEVELVIEWPORT_MENUBAR_BUTTONSIZE ) )
		{
			if ( g_World->IsDirty() )
			{
				OpenPopup<CDialogWindow>( TEXT( "Warning" ), CString::Format( TEXT( "Map not saved.\nFor launch standalone game need it save" ) ), CDialogWindow::BT_Ok );
			}
			else
			{
				Sys_CreateProc( g_FileSystem->GetExePath().c_str(), CString::Format( TEXT( "-map %s" ), g_World->GetFilePath().c_str() ).c_str(), false, false, false, 0 );
			}
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Play game in standalone" );
		}

		ImGui::EndMenuBar();
	}

	// Draw viewport widget
	viewportCursorPos = ImVec2( ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x, ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y );
	viewportWidget.Tick();

	// Draw popup menu
	DrawPopupMenu();

	// Draw transform gizmos if has selected actors
	std::vector<ActorRef_t>		selectedActors = g_World->GetSelectedActors();
	if ( selectedActors.size() > 0 )
	{
		bool			bOrhtoViewportType	= viewportClient.GetViewportType() != LVT_Perspective;
		bool			bMultiSelection		= selectedActors.size() > 1;

		ImGuizmo::SetID( viewportClient.GetViewportType() );
		ImGuizmo::SetOrthographic( bOrhtoViewportType );
		ImGuizmo::AllowAxisFlip( bOrhtoViewportType );
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect( ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight() );

		// Calculate operation flags for orthographic projections
		uint32				guizmoOperationFlags	= guizmoOperationType;
		switch ( viewportClient.GetViewportType() )
		{
			// Ortho XY
		case LVT_OrthoXY:
			switch ( guizmoOperationType )
			{
			case ImGuizmo::TRANSLATE:	guizmoOperationFlags = ImGuizmo::TRANSLATE_X | ImGuizmo::TRANSLATE_Y;	break;
			case ImGuizmo::ROTATE:		guizmoOperationFlags = ImGuizmo::ROTATE_SCREEN;							break;
			case ImGuizmo::SCALE:		guizmoOperationFlags = ImGuizmo::SCALE_X | ImGuizmo::SCALE_Y;			break;
			}
			break;

			// Ortho XZ
		case LVT_OrthoXZ:
			switch ( guizmoOperationType )
			{
			case ImGuizmo::TRANSLATE:	guizmoOperationFlags = ImGuizmo::TRANSLATE_X | ImGuizmo::TRANSLATE_Z;	break;
			case ImGuizmo::ROTATE:		guizmoOperationFlags = ImGuizmo::ROTATE_SCREEN;							break;
			case ImGuizmo::SCALE:		guizmoOperationFlags = ImGuizmo::SCALE_X | ImGuizmo::SCALE_Z;			break;
			}
			break;

			// Ortho YZ
		case LVT_OrthoYZ:
			switch ( guizmoOperationType )
			{
			case ImGuizmo::TRANSLATE:	guizmoOperationFlags = ImGuizmo::TRANSLATE_Y | ImGuizmo::TRANSLATE_Z;	break;
			case ImGuizmo::ROTATE:		guizmoOperationFlags = ImGuizmo::ROTATE_SCREEN;							break;
			case ImGuizmo::SCALE:		guizmoOperationFlags = ImGuizmo::SCALE_Y | ImGuizmo::SCALE_Z;			break;
			}
			break;
		}

		CSceneView*	sceneView				= viewportClient.CalcSceneView( viewportWidget.GetSize().x, viewportWidget.GetSize().y );
		ActorRef_t	actorCenter				= selectedActors[selectedActors.size() - 1];
		Matrix		actorTransformMatrix	= actorCenter->GetActorTransform().ToMatrix();
		Matrix		deltaMatrix;

		// Get snap size
		bool		bUseSnap = false;
		float		gridSize = 0.f;
		switch ( guizmoOperationType )
		{
			// Translate
			case ImGuizmo::TRANSLATE:
				bUseSnap = g_EditorEngine->GetConstraints().IsUseSnapTranslation();
				if ( bUseSnap )
				{
					gridSize = g_EditorEngine->GetConstraints().GetGridTranslationSize();
				}
				break;

			// Rotate
			case ImGuizmo::ROTATE:
				bUseSnap = g_EditorEngine->GetConstraints().IsUseSnapRotation();
				if ( bUseSnap )
				{
					gridSize = g_EditorEngine->GetConstraints().GetGridRotationSize();
				}
				break;

			// Scale
			case ImGuizmo::SCALE:
				bUseSnap = g_EditorEngine->GetConstraints().IsUseSnapScale();
				if ( bUseSnap )
				{
					gridSize = g_EditorEngine->GetConstraints().GetGridScalingSize();
				}
				break;
		}

		// Draw gizmo and apply changes to actor
		Vector		snapSize;
		if ( bUseSnap )
		{
			snapSize = Vector( gridSize, gridSize, gridSize );
		}

		ImGuizmo::Manipulate( glm::value_ptr( sceneView->GetViewMatrix() ), glm::value_ptr( sceneView->GetProjectionMatrix() ), ( ImGuizmo::OPERATION )guizmoOperationFlags, !bOrhtoViewportType ? guizmoModeType : ImGuizmo::WORLD, glm::value_ptr( actorTransformMatrix ), glm::value_ptr( deltaMatrix ), bUseSnap ? &snapSize.x : nullptr );
		bGuizmoUsing = ImGuizmo::IsUsing();
		if ( bGuizmoUsing )
		{
			// Decompose matrix to components: location, rotation and scale
			Vector	location;
			Vector	rotation;
			Vector	scale;
			ImGuizmo::DecomposeMatrixToComponents( glm::value_ptr( actorTransformMatrix ), nullptr, nullptr, &scale.x );
			ImGuizmo::DecomposeMatrixToComponents( glm::value_ptr( deltaMatrix ), &location.x, &rotation.x, nullptr );

			// Apply new transformation to actors
			for ( uint32 index = 0, count = selectedActors.size(); index < count; ++index )
			{
				ActorRef_t		actor = selectedActors[index];
				switch ( guizmoOperationType )
				{
					// Translate
				case ImGuizmo::TRANSLATE:
					actor->AddActorLocation( location );
					break;

					// Rotate
				case ImGuizmo::ROTATE:
				{
					Quaternion		deltaRotation =  SMath::AnglesToQuaternionZYX( rotation );
					actor->AddActorRotation( deltaRotation );
					if ( bMultiSelection && actor != actorCenter )
					{
						Matrix		translateMatrix = SMath::TranslateMatrix( actorCenter->GetActorLocation() );
						actor->SetActorLocation( translateMatrix * SMath::QuaternionToMatrix( deltaRotation ) * SMath::InverseMatrix( translateMatrix ) * Vector4D( actor->GetActorLocation(), 1.f ) );
					}
					break;
				}

					// Scale
				case ImGuizmo::SCALE:
				{
					Vector		deltaScale = scale * ( 1.f / actor->GetActorScale() );
					actor->SetActorScale( actor->GetActorScale() * deltaScale );			
					if ( bMultiSelection && actor != actorCenter )
					{
						deltaScale -= Vector( 1.f, 1.f, 1.f );
						actor->AddActorLocation( -actorCenter->GetActorLocation() );
						actor->AddActorLocation( deltaScale * actor->GetActorLocation() );
						actor->AddActorLocation( actorCenter->GetActorLocation() );
					}
					break;
				}
				}
			}

			// Mark world as dirty
			g_World->MarkDirty();
		}

		delete sceneView;
	}
}

/*
==================
CLevelViewportWindow::DrawPopupMenu
==================
*/
void CLevelViewportWindow::DrawPopupMenu()
{
	if ( viewportClient.IsAllowContextMenu() && ImGui::BeginPopupContextWindow( "", ImGuiMouseButton_Right ) )
	{
		// Convert from screen space to world space
		Vector			location = viewportClient.ScreenToWorld( Vector2D( viewportCursorPos.x, viewportCursorPos.y ), GetSizeX(), GetSizeY() );
		if ( g_EditorEngine->GetConstraints().IsUseSnapTranslation() )
		{
			location = SMath::GridSnap( location, g_EditorEngine->GetConstraints().GetGridTranslationSize() );
		}

		// Spawn actor by class
		CClass*			actorClass = g_EditorEngine->GetActorClassesWindow()->GetCurrentClass();
		if ( ImGui::MenuItem( TCHAR_TO_ANSI( CString::Format( TEXT( "Spawn %s" ), actorClass->GetName().c_str() ).c_str() ) ) )
		{
			// Spawn new actor
			g_World->SpawnActor( actorClass, location );
		}

		// Spawn actor by asset from content browser
		std::wstring	assetReference = g_EditorEngine->GetContentBrowserWindow()->GetSelectedAssetReference();
		if ( !assetReference.empty() )
		{
			std::wstring		dummy;
			EAssetType			assetType;
			if ( ParseReferenceToAsset( assetReference, dummy, dummy, assetType ) && g_ActorFactory.IsRegistered( assetType ) && ImGui::MenuItem( TCHAR_TO_ANSI( CString::Format( TEXT( "Spawn %s" ), assetReference.c_str() ).c_str() ) ) )
			{
				// Spawn new actor
				TAssetHandle<CAsset>		asset = g_PackageManager->FindAsset( assetReference, assetType );
				if ( asset.IsAssetValid() )
				{
					g_ActorFactory.Spawn( asset, location );
				}
			}
		}
		ImGui::EndPopup();
	}
}

/*
==================
CLevelViewportWindow::OnVisibilityChanged
==================
*/
void CLevelViewportWindow::OnVisibilityChanged( bool InNewVisibility )
{
	viewportWidget.SetEnabled( InNewVisibility );
}

/*
==================
CLevelViewportWindow::ProcessEvent
==================
*/
void CLevelViewportWindow::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	// Process ImGUI events
	viewportWidget.ProcessEvent( InWindowEvent );
	
	// We process events only when viewport widget is hovered
	if ( viewportWidget.IsHovered() )
	{
		// Process event in viewport client
		viewportClient.ProcessEvent( InWindowEvent );

		switch ( InWindowEvent.type )
		{
			// If pressed left mouse button, we try select actor in world
		case SWindowEvent::T_MouseReleased:
#if ENABLE_HITPROXY
			if ( !bGuizmoUsing && InWindowEvent.events.mouseButton.code == BC_MouseLeft )
			{
				// Update hit proxies id in all actors
				g_World->UpdateHitProxiesId();

				// Render hit proxies to render target
				viewportClient.DrawHitProxies( const_cast< CViewport* >( &viewportWidget.GetViewport() ) );

				// Wait until we rendering scene
				FlushRenderingCommands();

				CHitProxyId		hitProxyId = viewportClient.GetHitProxyId( viewportCursorPos.x, viewportCursorPos.y );
				bool			bControlDown = g_InputSystem->IsKeyDown( BC_KeyLControl ) || g_InputSystem->IsKeyDown( BC_KeyRControl );

				if ( !bControlDown )
				{
					g_World->UnselectAllActors();
				}

				if ( hitProxyId.IsValid() )
				{
					uint32			index = hitProxyId.GetIndex();
					ActorRef_t		actor = g_World->GetActor( index > 0 ? index - 1 : index );

					if ( bControlDown && actor->IsSelected() )
					{
						g_World->UnselectActor( actor );
						Logf( TEXT( "(%f;%f) Unselected actor '%s'\n" ), viewportCursorPos.x, viewportCursorPos.y, actor->GetName().c_str() );
					}
					else
					{
						g_World->SelectActor( actor );
						Logf( TEXT( "(%f;%f) Selected actor '%s'\n" ), viewportCursorPos.x, viewportCursorPos.y, actor->GetName().c_str() );
					}
				}
			}
#endif // ENABLE_HITPROXY
			break;

			// Key released events
		case SWindowEvent::T_KeyReleased:
			switch ( InWindowEvent.events.key.code )
			{
				// Unselect all actors
			case BC_KeyEscape:
				g_World->UnselectAllActors();
				break;
			}
			break;
		}
	}
}