#include "Misc/EngineGlobals.h"
#include "System/PropertyNode.h"
#include "System/ObjectPropertyNode.h"
#include "System/World.h"

/*
==================
NotifyFindObjectItemParent
==================
*/
static FORCEINLINE CObjectPropertyNode* NotifyFindObjectItemParent( CPropertyNode* InNode )
{
	Assert( InNode );
	CObjectPropertyNode*	result = nullptr;
	CPropertyNode*			parentNode = InNode->GetParentNode();
	if ( parentNode )
	{
		result = parentNode->FindObjectItemParent();
	}

	return result;
}

/*
==================
CPropertyNode::CPropertyNode
==================
*/
CPropertyNode::CPropertyNode()
	: parentNode( nullptr )
	, property( nullptr )
	, propertyOffset( 0 )
	, arrayIndex( INDEX_NONE )
{}

/*
==================
CPropertyNode::~CPropertyNode
==================
*/
CPropertyNode::~CPropertyNode()
{
	DestroyTree();
}

/*
==================
CPropertyNode::InitNode
==================
*/
void CPropertyNode::InitNode( CPropertyNode* InParentNode, CProperty* InProperty, uint32 InPropertyOffset, uint32 InArrayIndex, bool InAllowChildren /* = true */ )
{
	// Destroy the previous tree
	DestroyTree();
	Assert( InParentNode != this );

	// Init node's data
	parentNode		= InParentNode;
	property		= InProperty;
	propertyOffset	= InPropertyOffset;
	arrayIndex		= InArrayIndex;

	// Init node by child class
	InitNodeInternal();

	// Rebuild children if it allowed
	if ( InAllowChildren )
	{
		RebuildChildren();
	}
}

/*
==================
CPropertyNode::InitNodeInternal
==================
*/
void CPropertyNode::InitNodeInternal()
{}

/*
==================
CPropertyNode::RebuildChildren
==================
*/
void CPropertyNode::RebuildChildren()
{
	// Destroy the previous tree and init child nodes
	DestroyTree();
	InitChildNodes();
}

/*
==================
CPropertyNode::Tick
==================
*/
void CPropertyNode::Tick( float InItemWidthSpacing /* = 0.f */ )
{
	TickChildren( InItemWidthSpacing );
}

/*
==================
CPropertyNode::TickChildren
==================
*/
void CPropertyNode::TickChildren( float InItemWidthSpacing /* = 0.f */ )
{
	for ( uint32 index = 0, count = childNodes.size(); index < count; ++index )
	{
		CPropertyNode*		childNode = childNodes[index];
		childNode->Tick( InItemWidthSpacing );
	}
}

/*
==================
CPropertyNode::NotifyPostChange
==================
*/
void CPropertyNode::NotifyPostChange( const PropertyChangedEvenet& InPropertyChangedEvent )
{
	CObjectPropertyNode*	objectNode		= FindObjectItemParent();
	CProperty*				currentProperty	= InPropertyChangedEvent.property;

	// Call PostEditChange on the object chain
	while ( true )
	{
		for ( uint32 index = 0, count = objectNode->GetNumObjects(); index < count; ++index )
		{
			CObject*				object = objectNode->GetObject( index );
			PropertyChangedEvenet	changedEvent = InPropertyChangedEvent;
			if ( currentProperty != InPropertyChangedEvent.property )
			{
				// Parent object node property. Reset other internals and leave the event type as unspecified
				changedEvent = PropertyChangedEvenet( currentProperty );
			}
			object->PostEditChangeProperty( changedEvent );
		}

		// Pass this property to the parent's PostEditChange call
		currentProperty = objectNode->GetProperty();
		
		// Traverse up a level in the nested object tree
		objectNode = NotifyFindObjectItemParent( objectNode );
		if ( !objectNode )
		{
			// We've hit the root
			break;
		}
	}

	g_World->MarkDirty();
}

/*
==================
CPropertyNode::DestroyTree
==================
*/
void CPropertyNode::DestroyTree()
{
	for ( uint32 index = 0, count = childNodes.size(); index < count; ++index )
	{
		CPropertyNode*	childNode = childNodes[index];
		Assert( childNode );
		delete childNode;
	}
	childNodes.clear();
}

/*
==================
CPropertyNode::GetReadAddress
==================
*/
bool CPropertyNode::GetReadAddress( CPropertyNode* InNode, std::vector<byte*>& OutAddresses ) const
{
	if ( parentNode )
	{
		return parentNode->GetReadAddress( InNode, OutAddresses );
	}
	return false;
}

/*
==================
CPropertyNode::GetValueBaseAddress
==================
*/
byte* CPropertyNode::GetValueBaseAddress( byte* InBaseAddress ) const
{
	if ( parentNode )
	{
		return parentNode->GetValueAddress( InBaseAddress );
	}
	return false;
}

/*
==================
CPropertyNode::GetValueAddress
==================
*/
byte* CPropertyNode::GetValueAddress( byte* InBaseAddress ) const
{
	return GetValueBaseAddress( InBaseAddress );
}

/*
==================
CPropertyNode::FindObjectItemParent
==================
*/
CObjectPropertyNode* CPropertyNode::FindObjectItemParent()
{
	CPropertyNode*			currentNode = this;
	CObjectPropertyNode*	foundObjectNode = nullptr;
	while ( true )
	{
		foundObjectNode = currentNode->GetObjectNode();
		if ( foundObjectNode )
		{
			break;
		}
		currentNode = currentNode->GetParentNode();
	}

	return foundObjectNode;
}

/*
==================
CPropertyNode::GetObjectNode
==================
*/
CObjectPropertyNode* CPropertyNode::GetObjectNode()
{
	return nullptr;
}

/*
==================
CPropertyNode::GetCategoryNode
==================
*/
CCategoryPropertyNode* CPropertyNode::GetCategoryNode()
{
	return nullptr;
}