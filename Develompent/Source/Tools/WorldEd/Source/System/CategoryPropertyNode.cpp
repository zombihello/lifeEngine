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

#include "Core/Containers/String.h"
#include "Core/Containers/StringConv.h"
#include "UI/ImGUI/imgui.h"
#include "UI/ImGUI/ImGUIExtension.h"
#include "System/CategoryPropertyNode.h"
#include "System/ObjectPropertyNode.h"
#include "System/ItemPropertyNode.h"

/*
==================
CCategoryPropertyNode::CCategoryPropertyNode
==================
*/
CCategoryPropertyNode::CCategoryPropertyNode()
{}

/*
==================
CCategoryPropertyNode::~CCategoryPropertyNode
==================
*/
CCategoryPropertyNode::~CCategoryPropertyNode()
{}

/*
==================
CCategoryPropertyNode::InitChildNodes
==================
*/
void CCategoryPropertyNode::InitChildNodes()
{
	// Get parent object node and create item nodes for each property
	CObjectPropertyNode*		objectNode = FindObjectItemParent();
	Assert( objectNode );
	CClass*						objectBaseClass = objectNode->GetObjectBaseClass();
	Assert( objectBaseClass );

	std::vector<CProperty*>		properties;
	objectBaseClass->GetProperties( properties );
	for ( uint32 index = 0, count = properties.size(); index < count; ++index )
	{
		// We ignore property if it not have neither CPF_Edit nor CPF_EditConst
		CProperty*				property = properties[index];
		if ( property->HasAnyFlags( CPF_Edit ) || property->HasAnyFlags( CPF_EditConst ) )
		{
			if ( property->GetCategory() == categoryName )
			{
				CItemPropertyNode* itemNode = new CItemPropertyNode();
				itemNode->InitNode( this, property, property->GetOffset(), INDEX_NONE );
				AddChildNode( itemNode );
			}
		}
	}
}

/*
==================
CCategoryPropertyNode::Tick
==================
*/
void CCategoryPropertyNode::Tick( float InItemWidthSpacing /* = 0.f */ )
{
	ImGui::Dummy( ImVec2( InItemWidthSpacing, 0.f ) );
	ImGui::SameLine();
	if ( categoryName == NAME_None || ImGui::CollapsingHeader( CString::Format( TEXT( "%s##%p" ), categoryName.ToString().c_str(), this ), true, ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		TickChildren( InItemWidthSpacing + ImGui::GetStyle().ItemSpacing.x );
	}
}

/*
==================
CCategoryPropertyNode::GetObjectNode
==================
*/
CCategoryPropertyNode* CCategoryPropertyNode::GetCategoryNode()
{
	return this;
}