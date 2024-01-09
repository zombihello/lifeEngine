#include "System/ObjectPropertyNode.h"
#include "System/CategoryPropertyNode.h"
#include "ImGUI/imgui.h"
#include "ImGUI/ImGUIExtension.h"

/*
==================
CObjectPropertyNode::CObjectPropertyNode
==================
*/
CObjectPropertyNode::CObjectPropertyNode()
	: baseClass( nullptr )
{}

/*
==================
CObjectPropertyNode::~CObjectPropertyNode
==================
*/
CObjectPropertyNode::~CObjectPropertyNode()
{}

/*
==================
CObjectPropertyNode::InitNodeInternal
==================
*/
void CObjectPropertyNode::InitNodeInternal()
{
	// Find an appropriate base class
	SetBestBaseClass();
}

/*
==================
CObjectPropertyNode::GetObject
==================
*/
CObject* CObjectPropertyNode::GetObject( uint32 InIndex ) const
{
	Assert( InIndex >= 0 && InIndex < objects.size() );
	uint32	index = 0;
	for ( auto it = objects.begin(), itEnd = objects.end(); it != itEnd; ++it, ++index )
	{
		if ( index == InIndex )
		{
			return *it;
		}
	}

	return nullptr;
}

/*
==================
CObjectPropertyNode::GetReadAddress
==================
*/
bool CObjectPropertyNode::GetReadAddress( CPropertyNode* InNode, std::vector<byte *>& OutAddresses ) const
{
	// Are any objects selected for property editing?
	if ( objects.empty() )
	{
		return false;
	}

	CProperty*		nodeProperty = InNode->GetProperty();
	if ( !nodeProperty )
	{
		return false;
	}
	bool			bAllPropertiesIsSame = true;

	// If this item is the child of an array, return FALSE if there is a different number
	// of items in the array in different objects, when multi-selecting
	if ( Cast<CArrayProperty>( nodeProperty->GetOuter() ) )
	{
		CPropertyNode*		parentNode = InNode->GetParentNode();
		Assert( parentNode );
		CObject*			objectTmp = GetObject( 0 );
		uint32				arraySize = ( ( std::vector<byte>* )parentNode->GetValueBaseAddress( ( byte* )objectTmp ) )->size();
		for ( uint32 index = 1, count = GetNumObjects(); index < count; ++index )
		{
			objectTmp = GetObject( index );
			if ( ( ( std::vector<byte>* )parentNode->GetValueBaseAddress( ( byte* )objectTmp ) )->size() != arraySize )
			{
				bAllPropertiesIsSame = false;
			}
		}
	}

	byte*	baseAddress = InNode->GetValueBaseAddress( ( byte* )GetObject( 0 ) );
	if ( baseAddress )
	{
		// If the item is an array itself, return FALSE if there are a different number of
		// items in the array in different objects, when multi-selecting
		if ( Cast<CArrayProperty>( nodeProperty ) )
		{
			CObject*	objectTmp = GetObject( 0 );
			uint32		arraySize = ( ( std::vector<byte>* )InNode->GetValueBaseAddress( ( byte* )objectTmp ) )->size();
			for ( uint32 index = 1, count = GetNumObjects(); index < count; ++index )
			{
				CObject*	objectTmp = GetObject( index );
				if ( ( ( std::vector<byte>* )InNode->GetValueBaseAddress( ( byte* )objectTmp ) )->size() != arraySize )
				{
					bAllPropertiesIsSame = false;
				}
			}
		}
		// If the item is an object itself, return FALSE if there are a different values (e.g base object isn't NULL and other is NULL or vice versa)
		else if ( Cast<CObjectProperty>( nodeProperty ) )
		{
			CObject*	objectBase = *( CObject** )baseAddress;
			for ( uint32 index = 1, count = GetNumObjects(); index < count; ++index )
			{
				CObject*	objectTmp = GetObject( index );
				CObject*	objectCurrent = *( CObject** )InNode->GetValueBaseAddress( ( byte* )objectTmp );
				if ( ( bool )objectBase != ( bool )objectCurrent )
				{
					bAllPropertiesIsSame = false;
				}
			}
		}
	}

	// Write addresses to the output
	for ( auto it = objects.begin(), itEnd = objects.end(); it != itEnd; ++it )
	{
		CObject*	object = *it;
		OutAddresses.push_back( InNode->GetValueBaseAddress( ( byte* )object ) );
	}

	// Everything checked out and we have usable addresses
	return bAllPropertiesIsSame;
}

/*
==================
CObjectPropertyNode::GetValueBaseAddress
==================
*/
byte* CObjectPropertyNode::GetValueBaseAddress( byte* InBaseAddress ) const
{
	return InBaseAddress;
}

/*
==================
CObjectPropertyNode::InitChildNodes
==================
*/
void CObjectPropertyNode::InitChildNodes()
{
	Assert( baseClass );

	// Assemble a list of category names by iterating over all properties of baseClass
	std::unordered_set<CName, CName::HashFunction>	categories;
	std::vector<CProperty*>							baseClassProperties; 	
	baseClass->GetProperties( baseClassProperties );
	
	for ( uint32 index = 0, count = baseClassProperties.size(); index < count; ++index )
	{
		// We ignore property if it not have neither CPF_Edit nor CPF_EditConst
		CProperty*		property = baseClassProperties[index];
		if ( property->HasAnyFlags( CPF_Edit ) || property->HasAnyFlags( CPF_EditConst ) )
		{
			categories.insert( baseClassProperties[index]->GetCategory() );
		}
	}

	// Create and init category node
	for ( auto it = categories.begin(), itEnd = categories.end(); it != itEnd; ++it )
	{
		CCategoryPropertyNode*		categroyPropertyNode = new CCategoryPropertyNode();
		categroyPropertyNode->SetCategoryName( *it );
		categroyPropertyNode->InitNode( this, nullptr, -1, INDEX_NONE );

		// Add this node to array of child nodes
		AddChildNode( categroyPropertyNode );
	}
}

/*
==================
CObjectPropertyNode::SetBestBaseClass
==================
*/
void CObjectPropertyNode::SetBestBaseClass()
{
	baseClass = nullptr;
	for ( auto it = objects.begin(), itEnd = objects.end(); it != itEnd; ++it )
	{
		CObject*	object = *it;
		CClass*		objectClass = object->GetClass();
		Assert( object && objectClass );

		// Initialize with the class of the first object we encounter
		if ( !baseClass )
		{
			baseClass = objectClass;
		}

		// If we've encountered an object that's not a subclass of the current best baseclass,
		// climb up a step in the class hierarchy
		while ( !objectClass->IsChildOf( baseClass ) )
		{
			baseClass = baseClass->GetSuperClass();
		}
	}
}

/*
==================
CObjectPropertyNode::GetObjectNode
==================
*/
CObjectPropertyNode* CObjectPropertyNode::GetObjectNode()
{
	return this;
}