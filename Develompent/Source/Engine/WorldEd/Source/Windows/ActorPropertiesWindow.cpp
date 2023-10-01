#include "Containers/StringConv.h"
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/UIGlobals.h"
#include "System/World.h"
#include "System/Package.h"
#include "ImGUI/ImGUIEngine.h"
#include "ImGUI/imgui_internal.h"
#include "Windows/ActorPropertiesWindow.h"
#include "System/EditorEngine.h"

/*
==================
CActorPropertiesWindow::CActorPropertiesWindow
==================
*/
CActorPropertiesWindow::CActorPropertiesWindow( const std::wstring& InName )
	: CImGUILayer( InName )
	, rootPropertyNode( nullptr )
	, actorsSelectedDelegate( nullptr )
	, actorsUnselectedDelegate( nullptr )
	, createdNewMapDelegate( nullptr )
	, loadedMapDelegate( nullptr )
{
	actorsSelectedDelegate = EditorDelegates::onActorsSelected.Add(		std::bind( &CActorPropertiesWindow::OnActorsUnSelected, this, std::placeholders::_1 ) );
	actorsUnselectedDelegate = EditorDelegates::onActorsUnselected.Add(	std::bind( &CActorPropertiesWindow::OnActorsUnSelected, this, std::placeholders::_1 ) );
	createdNewMapDelegate = EditorDelegates::onEditorCreatedNewMap.Add(	std::bind( &CActorPropertiesWindow::OnMapChanged,		this						) );
	loadedMapDelegate = EditorDelegates::onEditorLoadedMap.Add(			std::bind( &CActorPropertiesWindow::OnMapChanged,		this						) );
}

/*
==================
CActorPropertiesWindow::~CActorPropertiesWindow
==================
*/
CActorPropertiesWindow::~CActorPropertiesWindow()
{
	DestroyPropertyTree();

	if ( actorsSelectedDelegate )
	{
		EditorDelegates::onActorsSelected.Remove( actorsSelectedDelegate );
	}

	if ( actorsUnselectedDelegate )
	{
		EditorDelegates::onActorsUnselected.Remove( actorsUnselectedDelegate );
	}

	if ( createdNewMapDelegate )
	{
		EditorDelegates::onEditorCreatedNewMap.Remove( createdNewMapDelegate );
	}

	if ( loadedMapDelegate )
	{
		EditorDelegates::onEditorLoadedMap.Remove( loadedMapDelegate );
	}
}

/*
==================
CActorPropertiesWindow::OnTick
==================
*/
void CActorPropertiesWindow::OnTick()
{
	if ( rootPropertyNode )
	{
		rootPropertyNode->Tick();
	}
}

/*
==================
CActorPropertiesWindow::OnActorsUnSelected
==================
*/
void CActorPropertiesWindow::OnActorsUnSelected( const std::vector<ActorRef_t>& InActors )
{
	// Destroy the property tree created earlier
	DestroyPropertyTree();

	// Getting an array of selected actors (we ignore InActors because it may not contain all of the selected actors)
	const std::vector<ActorRef_t>&		selectedActors = g_World->GetSelectedActors();
	if ( selectedActors.empty() )
	{
		return;
	}

	// Init root property node
	rootPropertyNode = new CObjectPropertyNode();
	for ( uint32 index = 0, count = selectedActors.size(); index < count; ++index )
	{
		rootPropertyNode->AddObject( selectedActors[index] );
	}
	rootPropertyNode->InitNode( nullptr, nullptr, -1, INDEX_NONE );
}

/*
==================
CActorPropertiesWindow::OnMapChanged
==================
*/
void CActorPropertiesWindow::OnMapChanged()
{
	DestroyPropertyTree();
}

/*
==================
CActorPropertiesWindow::DestroyPropertyTree
==================
*/
void CActorPropertiesWindow::DestroyPropertyTree()
{
	if ( rootPropertyNode )
	{
		delete rootPropertyNode;
		rootPropertyNode = nullptr;
	}
}