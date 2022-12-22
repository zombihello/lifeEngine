#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Windows/PhysicsMaterialEditorWindow.h"
#include "ImGUI/imgui_internal.h"

CPhysicsMaterialEditorWindow::CPhysicsMaterialEditorWindow( const TSharedPtr<CPhysicsMaterial>& InPhysMaterial )
	: CImGUILayer( CString::Format( TEXT( "Physics Material Editor - %s" ), InPhysMaterial->GetAssetName().c_str() ) )
	, physMaterial( InPhysMaterial )
{
	flags |= LF_DestroyOnHide;

	// Subscribe to event when assets try destroy of editing audio bank and reload. It need is block
	assetsCanDeleteHandle = SEditorDelegates::onAssetsCanDelete.Add( std::bind( &CPhysicsMaterialEditorWindow::OnAssetsCanDelete, this, std::placeholders::_1, std::placeholders::_2	) );
}

CPhysicsMaterialEditorWindow::~CPhysicsMaterialEditorWindow()
{
	// Unsubscribe from event when assets try destroy and reload
	SEditorDelegates::onAssetsCanDelete.Remove( assetsCanDeleteHandle );
}

void CPhysicsMaterialEditorWindow::Init()
{
	CImGUILayer::Init();
	SetSize( Vector2D( 365.f, 200.f ) );
}

void CPhysicsMaterialEditorWindow::OnTick()
{
	ImGui::Columns( 2, 0, false );

	// Input static friction
	{
		float		staticFriction = physMaterial->GetStaticFriction();
		ImGui::Text( "Static Friction:" );
		ImGui::NextColumn();
		if ( ImGui::InputFloat( "##StaticFriction", &staticFriction ) )
		{
			physMaterial->SetStaticFriction( staticFriction );
		}	
		ImGui::NextColumn();
	}

	// Input dynamic friction
	{	
		float		dynamicFriction = physMaterial->GetDynamicFriction();
		ImGui::Text( "Dynamic Friction:" );
		ImGui::NextColumn();
		if ( ImGui::InputFloat( "##DynamicFriction", &dynamicFriction ) )
		{
			physMaterial->SetDynamicFriction( dynamicFriction );
		}	
		ImGui::NextColumn();
	}

	// Input density
	{
		float		density = physMaterial->GetDensity();
		ImGui::Text( "Density:" );
		ImGui::NextColumn();
		if ( ImGui::InputFloat( "##Density", &density ) )
		{
			physMaterial->SetDensity( density );
		}
		ImGui::NextColumn();
	}

	// Input restitution
	{
		float		restitution = physMaterial->GetDensity();
		ImGui::Text( "Restitution:" );
		ImGui::NextColumn();
		if ( ImGui::InputFloat( "##Restitution", &restitution ) )
		{
			physMaterial->SetRestitution( restitution );
		}
		ImGui::NextColumn();
	}

	// Combobox of surface type
	{
		ImGui::Text( "Surface Type:" );
		ImGui::NextColumn();

		// Find current surface index
		uint32		surfaceIndex = 0;
		for ( uint32 index = 0, count = GSurfaceTypeNames.size(); index < count; ++index )
		{
			if ( GSurfaceTypeNames[index].second == physMaterial->GetSurfaceType() )
			{
				surfaceIndex = index;
				break;
			}
		}

		// Draw combobox with all surface types
		if ( ImGui::BeginCombo( "##SurfaceTypeCombo", TCHAR_TO_ANSI( GSurfaceTypeNames[surfaceIndex].first.c_str() ) ) )
		{
			for ( uint32 index = 0, count = GSurfaceTypeNames.size(); index < count; ++index )
			{
				bool	bDummy = false;
				if ( ImGui::Selectable( TCHAR_TO_ANSI( GSurfaceTypeNames[index].first.c_str() ), &bDummy ) )
				{
					physMaterial->SetSurfaceType( GSurfaceTypeNames[index].second );
				}
			}

			ImGui::EndCombo();
		}
	}

	ImGui::EndColumns();
}

void CPhysicsMaterialEditorWindow::OnAssetsCanDelete( const std::vector<TSharedPtr<CAsset>>& InAssets, SCanDeleteAssetResult& OutResult )
{
	// If in InAssets exist audio bank who is editing now - need is block
	for ( uint32 index = 0, count = InAssets.size(); index < count; ++index )
	{
		TSharedPtr<CAsset>		assetRef = InAssets[index];
		if ( assetRef == physMaterial )
		{
			OutResult.Set( false );
			return;
		}
	}
}