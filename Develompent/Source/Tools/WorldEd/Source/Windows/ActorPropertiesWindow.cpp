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
#include "Core/Misc/CoreGlobals.h"
#include "Core/System/Package.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/System/World.h"
#include "UI/Misc/UIGlobals.h"
#include "UI/ImGUI/ImGUIEngine.h"
#include "UI/ImGUI/imgui_internal.h"
#include "Misc/WorldEdGlobals.h"
#include "Windows/ActorPropertiesWindow.h"

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