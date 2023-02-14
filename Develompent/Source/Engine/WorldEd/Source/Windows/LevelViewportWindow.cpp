#include "Misc/WorldEdGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/UIGlobals.h"
#include "Logger/LoggerMacros.h"
#include "Windows/LevelViewportWindow.h"
#include "System/EditorEngine.h"
#include "System/World.h"
#include "Render/RenderingThread.h"
#include "System/AssetsImport.h"

/** Table pathes to icons */
static const tchar* GLevelViewportIconPaths[] =
{
	TEXT( "Tool_Select.png" ),		// IT_ToolSelect
	TEXT( "Tool_Translate.png" ),	// IT_ToolTranslate
	TEXT( "Tool_Rotate.png" ),		// IT_ToolRotate
	TEXT( "Tool_Scale.png" )		// IT_ToolScale
};
static_assert( ARRAY_COUNT( GLevelViewportIconPaths ) == CLevelViewportWindow::IT_Num, "Need full init GLevelViewportIconPaths array" );

/** Macro size button in menu bar */
#define LEVELVIEWPORT_MENUBAR_BUTTONSIZE	ImVec2( 16.f, 16.f )

/** Selection color */
#define LEVELVIEWPORT_SELECTCOLOR			ImVec4( 0.f, 0.43f, 0.87f, 1.f )

/** Is need pop style color of a button */
static bool GImGui_ButtonNeedPopStyleColor = false;

static void ImGui_ButtonSetButtonSelectedStyle()
{
	if ( !GImGui_ButtonNeedPopStyleColor )
	{
		GImGui_ButtonNeedPopStyleColor = true;
		ImGui::PushStyleColor( ImGuiCol_Button,			LEVELVIEWPORT_SELECTCOLOR );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered,	LEVELVIEWPORT_SELECTCOLOR );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive,	LEVELVIEWPORT_SELECTCOLOR );
	}
}

static void ImGui_ButtonPopStyleColor()
{
	if ( GImGui_ButtonNeedPopStyleColor )
	{
		ImGui::PopStyleColor( 3 );
		GImGui_ButtonNeedPopStyleColor = false;
	}
}

CLevelViewportWindow::CLevelViewportWindow( const std::wstring& InName, bool InVisibility /* = true */, ELevelViewportType InViewportType /* = LVT_Perspective */ )
	: CImGUILayer( InName )
	, bGuizmoUsing( false )
	, guizmoOperationType( ImGuizmo::TRANSLATE )
	, guizmoModeType( ImGuizmo::LOCAL )
	, viewportScreenPos( 0.f, 0.f )
	, viewportWidget( true, &viewportClient )
{
	flags |= ImGuiWindowFlags_MenuBar;

	viewportClient.SetViewportType( InViewportType );
	SetVisibility( InVisibility );
	SetPadding( Vector2D( 0.f, 0.f ) );
}

void CLevelViewportWindow::Init()
{
	CImGUILayer::Init();
	viewportWidget.Init();

	// Loading icons
	std::wstring	errorMsg;
	PackageRef_t	package = GPackageManager->LoadPackage( TEXT( "" ), true );
	check( package );

	for ( uint32 index = 0; index < IT_Num; ++index )
	{
		const std::wstring				assetName = CString::Format( TEXT( "LevelViewportWindow_%X" ), index );
		TAssetHandle<CTexture2D>&		assetHandle = icons[index];
		assetHandle = package->Find( assetName );
		if ( !assetHandle.IsAssetValid() )
		{
			std::vector<TSharedPtr<CAsset>>		result;
			if ( !CTexture2DImporter::Import( appBaseDir() + TEXT( "Engine/Editor/Icons/" ) + GLevelViewportIconPaths[index], result, errorMsg ) )
			{
				LE_LOG( LT_Warning, LC_Editor, TEXT( "Fail to load level viewport window icon '%s' for type 0x%X. Message: %s" ), GLevelViewportIconPaths[index], index, errorMsg.c_str() );
				assetHandle = GEngine->GetDefaultTexture();
			}
			else
			{
				TSharedPtr<CTexture2D>		texture2D = result[0];
				texture2D->SetAssetName( assetName );
				assetHandle = texture2D->GetAssetHandle();
				package->Add( assetHandle );
			}
		}
	}
}

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
		// Tool 'Select'
		ImGui::ImageButton( GImGUIEngine->LockTexture( icons[IT_ToolSelect].ToSharedPtr()->GetTexture2DRHI() ), LEVELVIEWPORT_MENUBAR_BUTTONSIZE );
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Tool 'Select'" );
		}

		// Tool 'Translate'
		if ( guizmoOperationType == ImGuizmo::TRANSLATE )
		{
			ImGui_ButtonSetButtonSelectedStyle();
		}
		if ( ImGui::ImageButton( GImGUIEngine->LockTexture( icons[IT_ToolTranslate].ToSharedPtr()->GetTexture2DRHI() ), LEVELVIEWPORT_MENUBAR_BUTTONSIZE ) )
		{
			guizmoOperationType = ImGuizmo::TRANSLATE;
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Tool 'Translate'" );
		}
		ImGui_ButtonPopStyleColor();

		// Tool 'Rotate'
		if ( guizmoOperationType == ImGuizmo::ROTATE )
		{
			ImGui_ButtonSetButtonSelectedStyle();
		}
		if ( ImGui::ImageButton( GImGUIEngine->LockTexture( icons[IT_ToolRotate].ToSharedPtr()->GetTexture2DRHI() ), LEVELVIEWPORT_MENUBAR_BUTTONSIZE ) )
		{
			guizmoOperationType = ImGuizmo::ROTATE;
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Tool 'Rotate'" );
		}
		ImGui_ButtonPopStyleColor();

		// Tool 'Scale'
		if ( guizmoOperationType == ImGuizmo::SCALE )
		{
			ImGui_ButtonSetButtonSelectedStyle();
		}
		if ( ImGui::ImageButton( GImGUIEngine->LockTexture( icons[IT_ToolScale].ToSharedPtr()->GetTexture2DRHI() ), LEVELVIEWPORT_MENUBAR_BUTTONSIZE ) )
		{
			guizmoOperationType = ImGuizmo::SCALE;
		}
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( "Tool 'Scale'" );
		}
		ImGui_ButtonPopStyleColor();

		ImGui::EndMenuBar();
	}

	// Draw viewport widget
	viewportScreenPos = ImGui::GetCursorScreenPos();
	viewportWidget.Tick();

	// Draw transform gizmos if has selected actors
	std::vector<ActorRef_t>		selectedActors = GWorld->GetSelectedActors();
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

		// Draw gizmo and apply changes to actor
		ImGuizmo::Manipulate( glm::value_ptr( sceneView->GetViewMatrix() ), glm::value_ptr( sceneView->GetProjectionMatrix() ), ( ImGuizmo::OPERATION )guizmoOperationFlags, !bOrhtoViewportType ? guizmoModeType : ImGuizmo::WORLD, glm::value_ptr( actorTransformMatrix ), glm::value_ptr( deltaMatrix ) );
		bGuizmoUsing = ImGuizmo::IsUsing();
		if ( bGuizmoUsing )
		{
			// Decompose matrix to components: location, rotation and scale
			float	location[3];
			float	rotation[3];
			float	scale[3];
			ImGuizmo::DecomposeMatrixToComponents( glm::value_ptr( actorTransformMatrix ), nullptr, nullptr, scale );
			ImGuizmo::DecomposeMatrixToComponents( glm::value_ptr( deltaMatrix ), location, rotation, nullptr );

			// Apply new transformation to actors
			for ( uint32 index = 0, count = selectedActors.size(); index < count; ++index )
			{
				ActorRef_t		actor = selectedActors[index];
				switch ( guizmoOperationType )
				{
					// Translate
				case ImGuizmo::TRANSLATE:
					actor->AddActorLocation( Vector( location[0], location[1], location[2] ) );
					break;

					// Rotate
				case ImGuizmo::ROTATE:
				{
					Quaternion		deltaRotation = SMath::AnglesToQuaternionXYZ( rotation[0], rotation[1], rotation[2] );
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
					actor->SetActorScale( Vector( scale[0], scale[1], scale[2] ) );
					break;
				}
			}
		}

		delete sceneView;
	}
}

void CLevelViewportWindow::OnVisibilityChanged( bool InNewVisibility )
{
	viewportWidget.SetEnabled( InNewVisibility );
}

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
				GWorld->UpdateHitProxiesId();

				// Render hit proxies to render target
				viewportClient.DrawHitProxies( const_cast< CViewport* >( &viewportWidget.GetViewport() ) );

				// Wait until we rendering scene
				FlushRenderingCommands();

				Vector2D		cursorPosition( ImGui::GetMousePos().x - viewportScreenPos.x, ImGui::GetMousePos().y - viewportScreenPos.y );
				CHitProxyId		hitProxyId = viewportClient.GetHitProxyId( cursorPosition.x, cursorPosition.y );
				bool			bControlDown = GInputSystem->IsKeyDown( BC_KeyLControl ) || GInputSystem->IsKeyDown( BC_KeyRControl );

				if ( !bControlDown )
				{
					GWorld->UnselectAllActors();
				}

				if ( hitProxyId.IsValid() )
				{
					uint32			index = hitProxyId.GetIndex();
					ActorRef_t		actor = GWorld->GetActor( index > 0 ? index - 1 : index );

					if ( bControlDown && actor->IsSelected() )
					{
						GWorld->UnselectActor( actor );
						LE_LOG( LT_Log, LC_Editor, TEXT( "(%f;%f) Unselected actor '%s'" ), cursorPosition.x, cursorPosition.y, actor->GetName() );
					}
					else
					{
						GWorld->SelectActor( actor );
						LE_LOG( LT_Log, LC_Editor, TEXT( "(%f;%f) Selected actor '%s'" ), cursorPosition.x, cursorPosition.y, actor->GetName() );
					}
				}
			}
#endif // ENABLE_HITPROXY
			break;

			// If pressed 'Escape' we unselect all actors
		case SWindowEvent::T_KeyReleased:
			if ( InWindowEvent.events.key.code == BC_KeyEscape )
			{
				GWorld->UnselectAllActors();
			}
			break;
		}
	}
}