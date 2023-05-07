#include "Containers/StringConv.h"
#include "Windows/ActorClassesWindow.h"

/** Selection color */
#define ACTORCLASSES_SELECTCOLOR					ImVec4( 0.f, 0.43f, 0.87f, 1.f )

/*
==================
CActorClassesWindow::CClassNode::CClassNode
==================
*/
CActorClassesWindow::CClassNode::CClassNode( CActorClassesWindow* InOwner, CClass* InClass /* = nullptr */ )
	: bSelected( false )
	, id( 0 )
	, lclass( InClass )
	, parent( nullptr )
	, owner( InOwner )
{
	Assert( InOwner );
}

/*
==================
CActorClassesWindow::CClassNode::~CClassNode
==================
*/
CActorClassesWindow::CClassNode::~CClassNode()
{
	// Delete all child nodes
	for ( auto itNode = children.begin(), itNodeEnd = children.end(); itNode != itNodeEnd; ++itNode )
	{
		delete* itNode;
	}
}

/*
==================
CActorClassesWindow::CClassNode::AddChild
==================
*/
void CActorClassesWindow::CClassNode::AddChild( CClassNode* InNode )
{
	Assert( InNode );

	// If parent this node, do nothing
	if ( InNode->parent == this )
	{
		return;
	}

	// If parent exist, we remove from parent this node
	if ( InNode->parent )
	{
		InNode->parent->RemoveChild( InNode );
	}

	// Add to current node this child
	InNode->parent = this;
	InNode->id = children.size();
	children.push_back( InNode );
}

/*
==================
CActorClassesWindow::CClassNode::RemoveChild
==================
*/
void CActorClassesWindow::CClassNode::RemoveChild( CClassNode* InNode, bool InIsDeleteNode /* = false */ )
{
	Assert( InNode );

	// If parent not this node, do nothing
	if ( InNode->parent != this )
	{
		return;
	}

	// Find node and remove he
	for ( auto itNode = children.begin(), itNodeEnd = children.end(); itNode != itNodeEnd; ++itNode )
	{
		if ( *itNode == InNode )
		{
			children.erase( itNode );
			break;
		}
	}

	// If need delete child node, do it!
	if ( InIsDeleteNode )
	{
		delete InNode;
	}
	// Else we reset parent in InNode
	else
	{
		InNode->parent = nullptr;
		InNode->id = 0;
	}
}

/*
==================
CActorClassesWindow::CClassNode::Find
==================
*/
CActorClassesWindow::CClassNode* CActorClassesWindow::CClassNode::Find( CClass* InClass, bool InIsRecursive /* = true */ ) const
{
	if ( lclass == InClass )
	{
		return const_cast<CClassNode*>( this );
	}

	// Find class node in child elements
	CClassNode* result = nullptr;
	for ( auto itNode = children.begin(), itNodeEnd = children.end(); itNode != itNodeEnd && !result; ++itNode )
	{
		CClassNode* node = *itNode;

		// We've found the right node!
		if ( node->lclass == InClass )
		{
			result = node;
			break;
		}

		// Else if we find node recursive - look in child nodes
		if ( InIsRecursive )
		{
			result = node->Find( InClass, InIsRecursive );
		}
	}

	return result;
}

/*
==================
CActorClassesWindow::CClassNode::GetChild
==================
*/
CActorClassesWindow::CClassNode* CActorClassesWindow::CClassNode::GetChild( uint32 InIndex ) const
{
	Assert( InIndex >= 0 && InIndex < children.size() );

	uint32		index = 0;
	for ( auto itNode = children.begin(), itNodeEnd = children.end(); itNode != itNodeEnd; ++itNode, ++index )
	{
		if ( index == InIndex )
		{
			return *itNode;
		}
	}

	return nullptr;
}

/*
==================
CActorClassesWindow::CClassNode::Tick
==================
*/
void CActorClassesWindow::CClassNode::Tick()
{
	if ( lclass )
	{
		// Set style for selected node
		bool	bNeedPopStyleColor = false;
		if ( bSelected )
		{
			bNeedPopStyleColor = true;
			ImGui::PushStyleColor( ImGuiCol_Header, ACTORCLASSES_SELECTCOLOR );
			ImGui::PushStyleColor( ImGuiCol_HeaderActive, ACTORCLASSES_SELECTCOLOR );
			ImGui::PushStyleColor( ImGuiCol_HeaderHovered, ACTORCLASSES_SELECTCOLOR );
		}

		bool bTreeNode = ImGui::TreeNodeEx( TCHAR_TO_ANSI( lclass->GetName().c_str() ), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick );
		
		// Item event handles
		ProcessEvents();

		// Pop selection node style
		if ( bNeedPopStyleColor )
		{
			ImGui::PopStyleColor( 3 );
		}

		// Draw tree
		if ( bTreeNode )
		{
			for ( auto itNode = children.begin(), itNodeEnd = children.end(); itNode != itNodeEnd; ++itNode )
			{
				( *itNode )->Tick();
			}
			ImGui::TreePop();
		}
	}
}

/*
==================
CActorClassesWindow::CClassNode::ProcessEvents
==================
*/
void CActorClassesWindow::CClassNode::ProcessEvents()
{
	// If we clicked on item then select him
	if ( !ImGui::IsItemToggledOpen() && ImGui::IsItemClicked() )
	{
		owner->root->SetSelect( false );
		SetSelect( true, false );
	}

	// Reset selection if click was not on node
	if ( ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) || ImGui::IsMouseDown( ImGuiMouseButton_Right ) ) )
	{
		owner->root->SetSelect( false );
	}
}

/*
==================
CActorClassesWindow::CClassNode::GetSelectedClasses
==================
*/
void CActorClassesWindow::CClassNode::GetSelectedClasses( std::vector<CClass*>& OutSelectedNodes, bool InIsIgnoreChildren /* = false */ ) const
{
	if ( bSelected )
	{
		OutSelectedNodes.push_back( GetClass() );
	}

	if ( !bSelected || !InIsIgnoreChildren )
	{
		for ( auto it = children.begin(), itEnd = children.end(); it != itEnd; ++it )
		{
			( *it )->GetSelectedClasses( OutSelectedNodes, InIsIgnoreChildren );
		}
	}
}


/*
==================
CActorClassesWindow::CActorClassesWindow
==================
*/
CActorClassesWindow::CActorClassesWindow( const std::wstring& InName, CClass* InBaseClass /* = nullptr */ )
	: CImGUILayer( InName )
{
	// Add base class to tree
	root = new CClassNode( this, InBaseClass );

	// Build tree of child classes of InBaseClass
	const std::unordered_map<std::wstring, const CClass*>&		classesTable = CClass::StaticGetRegisteredClasses();
	for ( auto itClass = classesTable.begin(), itClassEnd = classesTable.end(); itClass != itClassEnd; ++itClass )
	{
		CClass*			lclass = const_cast<CClass*>( itClass->second );

		// If current class isn't child of InBaseClass, we skip it
		if ( !lclass->IsA( InBaseClass ) )
		{
			continue;
		}

		// Iterate over all class ancestors and add nodes
		CClassNode*			newClassNode = nullptr;
		for ( CClass* tempClass = lclass; tempClass; tempClass = tempClass->GetSuperClass() )
		{
			CClassNode*		classNode = root->Find( tempClass );

			// If node already exist in tree, skip
			if ( classNode && classNode->GetClass() == lclass )
			{
				break;
			}
			// If we found parent node, add child to him
			else if ( classNode )
			{
				classNode->AddChild( newClassNode );
				break;
			}
			// Otherwise we make new node for tempClass
			else
			{
				CClassNode*			tempNewClassNode = newClassNode;
				newClassNode = new CClassNode( this, tempClass );
				if ( tempNewClassNode )
				{
					newClassNode->AddChild( tempNewClassNode );
				}
			}
		}
	}
}

/*
==================
CActorClassesWindow::~CActorClassesWindow
==================
*/
CActorClassesWindow::~CActorClassesWindow()
{
	delete root;
}

/*
==================
CActorClassesWindow::OnTick
==================
*/
void CActorClassesWindow::OnTick()
{
	root->Tick();
}