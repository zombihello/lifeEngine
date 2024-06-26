#include "Misc/StringConv.h"
#include "System/CategoryPropertyNode.h"
#include "System/ObjectPropertyNode.h"
#include "System/ItemPropertyNode.h"
#include "ImGUI/imgui.h"
#include "ImGUI/ImGUIExtension.h"
#include "Reflection/FieldIterator.h"

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

	for ( TFieldIterator<CProperty> it( objectBaseClass ); it; ++it )
	{
		// We ignore property if it not have neither CPF_Edit nor CPF_EditConst
		CProperty*				property = *it;
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
	if ( categoryName == NAME_None || ImGui::CollapsingHeader( L_Sprintf( TEXT( "%s##%p" ), categoryName.ToString().c_str(), this ), true, ImGuiTreeNodeFlags_DefaultOpen ) )
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