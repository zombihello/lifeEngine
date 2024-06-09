#include "Misc/StringConv.h"
#include "Misc/UIGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "System/ItemPropertyNode.h"
#include "System/ObjectPropertyNode.h"
#include "System/CategoryPropertyNode.h"
#include "System/EditorEngine.h"
#include "ImGUI/ImGUIEngine.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_stdlib.h"
#include "ImGUI/ImGUIExtension.h"

/*
==================
CItemPropertyNode::CItemPropertyNode
==================
*/
CItemPropertyNode::CItemPropertyNode()
{}

/*
==================
CItemPropertyNode::~CItemPropertyNode
==================
*/
CItemPropertyNode::~CItemPropertyNode()
{}

/*
==================
CItemPropertyNode::Tick
==================
*/
void CItemPropertyNode::Tick( float InItemWidthSpacing /* = 0.f */ )
{
	std::vector<byte*>		addresses;
	if ( GetReadAddress( this, addresses ) )
	{
		// Get all data for tick property node
		CObjectPropertyNode* parentObjectNode				= FindObjectItemParent();
		CObject*			 objectZero						= parentObjectNode->GetObject( 0 );
		CClass*				 theClass						= property->GetClass();
		bool				 bPropertyIsDisabled			= property->HasAnyFlags( CPF_Const ) || property->HasAnyFlags( CPF_EditConst ) || !objectZero->CanEditProperty( property );
		bool				 bPropertyInArray				= Cast<CArrayProperty>( property->GetOuter() ) || arrayIndex != INDEX_NONE;
		bool				 bPropertyIsStaticArray			= property->GetArraySize() > 1 && arrayIndex == INDEX_NONE;
		byte*				 valueData						= addresses[0];

		// Disable property if need		
		if ( bPropertyIsDisabled )
		{
			ImGui::BeginDisabled( true );
		}

		// Struct and Object property
		if ( theClass->HasAnyCastFlags( CASTCLASS_CStructProperty ) || theClass->HasAnyCastFlags( CASTCLASS_CObjectProperty ) )
		{
			// If out property in an array then we not draw collapsing header
			if ( !bPropertyInArray )
			{
				ImGui::Dummy( ImVec2( InItemWidthSpacing, 0.f ) );
				ImGui::SameLine();
			}
			if ( bPropertyInArray || ImGui::CollapsingHeader( L_Sprintf( TEXT( "%s##%p" ), property->GetName().c_str(), this ), true ) )
			{
				TickChildren( InItemWidthSpacing + ImGui::GetStyle().ItemSpacing.x );
			}
		}

		// Static and Dynamic Array property
		else if ( bPropertyIsStaticArray || theClass->HasAnyCastFlags( CASTCLASS_CArrayProperty ) )
		{
			bool		bPropertyIsChanged	= false;
			bool		bFixedArraySize		= property->HasAnyFlags( CPF_EditFixedSize ) || bPropertyIsStaticArray;
			uint32		elementIdToRemove	= -1;
			CProperty*	innerProperty		= !bPropertyIsStaticArray ? ExactCast<CArrayProperty>( property )->GetInnerProperty() : property;
			
			// If array size was changed and the node have not equal number of children then we rebuild it (only for dynamic array)
			if ( !bPropertyIsStaticArray && ( ( std::vector<byte>* )valueData )->size() / innerProperty->GetElementSize() != childNodes.size() )
			{
				RebuildChildren();
			}

			ImGui::CollapsingArrayHeaderResult	arrayCollapseResult = ImGui::CollapsingArrayHeader( 
																						L_Sprintf( TEXT( "##%p" ), this ),																			// InStrId
																						property->GetCName().ToString(),																					// InLabel
																						!bFixedArraySize ? ImGuiCollapsingArrayHeaderFlags_AllButtons : ImGuiCollapsingArrayHeaderFlags_RemoveButton,		// InFlags
																						InItemWidthSpacing,																									// InItemWidthSpacing
																						L_Sprintf( TEXT( "%i elements" ), childNodes.size() ),														// InMessage
																						property->GetDescription(),																							// InLabelToolTip
																						TEXT( "Add a new element to array" ),																				// InAddButtonToolTip
																						!bFixedArraySize ? TEXT( "Remove all elements from array" ) : TEXT( "Clear all elements in the array" ),			// InRemoveButtonToolTip
																						true );																												// InIgnoreDisabled
		
			if ( arrayCollapseResult.bIsOpened )
			{
				float	innerArrayItemSpacing = InItemWidthSpacing + ImGui::GetStyle().ItemSpacing.x;
				for ( uint32 index = 0, count = childNodes.size(); index < count; ++index )
				{
					CPropertyNode*		childNode = childNodes[index];
					ImGui::CollapsingArrayHeaderResult	elementCollapseResult = ImGui::CollapsingArrayHeader(
																						L_Sprintf( TEXT( "##%p_%i" ), this, index ),												// InStrId
																						L_Sprintf( TEXT( "%i" ), index ),															// InLabel
																						ImGuiCollapsingArrayHeaderFlags_RemoveButton,													// InFlags
																						innerArrayItemSpacing,																			// InItemWidthSpacing
																						TEXT( "" ),																						// InMessage
																						TEXT( "" ),																						// InLabelToolTip
																						TEXT( "" ),																						// InAddButtonToolTip
																						!bFixedArraySize ? TEXT( "Remove element from array" ) : TEXT( "Clear element in the array" ),	// InRemoveButtonToolTip
																						true );																							// InIgnoreDisabled
					if ( elementCollapseResult.bIsOpened )
					{
						childNode->Tick( innerArrayItemSpacing + ImGui::GetStyle().ItemSpacing.x * 2.f );
					}

					if ( elementCollapseResult.bPressedRemove )
					{
						elementIdToRemove = index;
					}
				}
			}

			// If was pressed add button then we add a new element if array isn't static (only for dynamic array)
			if ( !bPropertyIsStaticArray && !bFixedArraySize && arrayCollapseResult.bPressedAdd )
			{
				bPropertyIsChanged = true;
				for ( uint32 index = 0, count = addresses.size(); index < count; ++index )
				{
					std::vector<byte>*		vectorData = ( std::vector<byte>* )addresses[index];
					vectorData->resize( vectorData->size() + innerProperty->GetElementSize() );
				}
			}

			// If was pressed remove button then we remove all elements from the array or one element
			if ( arrayCollapseResult.bPressedRemove || elementIdToRemove != -1 )
			{
				bPropertyIsChanged = true;
				for ( uint32 index = 0, count = addresses.size(); index < count; ++index )
				{
					std::vector<byte>*	vectorData	= ( std::vector<byte>* )addresses[index];
	
					// Clear/Remove whole array
					if ( arrayCollapseResult.bPressedRemove )
					{
						if ( !bPropertyIsStaticArray )
						{
							if ( !bFixedArraySize )
							{
								vectorData->clear();
							}
							else
							{
								Memory::Memzero( vectorData->data(), vectorData->size() );
							}
						}
						else
						{
							Memory::Memzero( addresses[index], innerProperty->GetArraySize() * innerProperty->GetElementSize() );
						}
					}
					// Clear/Remove only one array's element
					else if ( elementIdToRemove != -1 )
					{
						uint32		dataOffset = elementIdToRemove * innerProperty->GetElementSize();
						if ( !bPropertyIsStaticArray )
						{
							if ( !bFixedArraySize )
							{
								vectorData->erase( vectorData->begin() + dataOffset, vectorData->begin() + dataOffset + innerProperty->GetElementSize() );
							}
							else
							{
								Memory::Memzero( vectorData->data() + dataOffset, innerProperty->GetElementSize() );
							}
						}
						else
						{
							Memory::Memzero( addresses[index] + dataOffset, innerProperty->GetElementSize() );
						}
					}
				}
			}

			if ( bPropertyIsChanged )
			{
				// Rebuild children because array size was changed (only for dynamic array)
				if ( !bFixedArraySize )
				{
					RebuildChildren();
				}

				// Notify all object if property was changed
				NotifyPostChange( PropertyChangedEvenet( !bPropertyInArray ? property : Cast<CArrayProperty>( property->GetOuter() ), PCT_ValueSet ) );
			}
		}
		// Other simple property types
		else
		{
			bool	bPropertyIsChanged = false;
			if ( !bPropertyInArray )
			{
				ImGui::Columns( 2 );
				ImGui::Dummy( ImVec2( InItemWidthSpacing, 0.f ) );
				ImGui::SameLine();

				// Draw name of property
				ImGui::Text( TCHAR_TO_ANSI( property->GetCName().ToString().c_str() ) );
				if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
				{
					ImGui::SetTooltip( TCHAR_TO_ANSI( property->GetDescription().c_str() ) );
				}
				ImGui::NextColumn();
			}
			else
			{
				ImGui::Dummy( ImVec2( InItemWidthSpacing, 0.f ) );
				ImGui::SameLine();
			}

			// Draw value of property
			// Bool property
			if ( theClass->HasAnyCastFlags( CASTCLASS_CBoolProperty ) )
			{
				bPropertyIsChanged |= ImGui::Checkbox( TCHAR_TO_ANSI( L_Sprintf( TEXT( "##%s_%p" ), property->GetName().c_str(), this ).c_str() ), ( bool* )valueData );
			}

			// Byte property
			else if ( theClass->HasAnyCastFlags( CASTCLASS_CByteProperty ) )
			{
				// Get cenum. If it null then byte property is simple byte field
				CByteProperty*	byteProperty = ExactCast<CByteProperty>( property );
				CEnum*			cenum = byteProperty->GetEnum();
				if ( !cenum )
				{
					int32	intValue = *( int32* )valueData;
					bPropertyIsChanged = ImGui::DragInt( TCHAR_TO_ANSI( L_Sprintf( TEXT( "##%s_%p" ), property->GetName().c_str(), this ).c_str() ), &intValue, 1.f, 0, 255 );
					if ( bPropertyIsChanged )
					{
						*valueData = intValue;
					}
				}
				// Otherwise it's enum
				else
				{
					bPropertyIsChanged = ImGui::SelectEnum( L_Sprintf( TEXT( "##%s_%p" ), property->GetName().c_str(), this ), cenum, *valueData );
				}
			}

			// Int property
			else if ( theClass->HasAnyCastFlags( CASTCLASS_CIntProperty ) )
			{
				bPropertyIsChanged = ImGui::DragInt( TCHAR_TO_ANSI( L_Sprintf( TEXT( "##%s_%p" ), property->GetName().c_str(), this ).c_str() ), ( int32* )valueData );
			}

			// Float property
			else if ( theClass->HasAnyCastFlags( CASTCLASS_CFloatProperty ) )
			{
				bPropertyIsChanged = ImGui::DragFloat( TCHAR_TO_ANSI( L_Sprintf( TEXT( "##%s_%p" ), property->GetName().c_str(), this ).c_str() ), ( float* )valueData );
			}

			// Color property
			else if ( theClass->HasAnyCastFlags( CASTCLASS_CColorProperty ) )
			{
				Vector4D	color = ( ( CColor* )valueData )->ToNormalizedVector4D();
				if ( ImGui::ColorEdit4( TCHAR_TO_ANSI( L_Sprintf( TEXT( "##%s_%p" ), property->GetName().c_str(), this ).c_str() ), ( float* )&color, ImGuiColorEditFlags_NoInputs ) )
				{
					*( CColor* )valueData = color;
					bPropertyIsChanged = true;
				}
			}

			// Vector property
			else if ( theClass->HasAnyCastFlags( CASTCLASS_CVectorProperty ) )
			{
				CVectorProperty*	vectorProperty = ExactCast<CVectorProperty>( property );
				bPropertyIsChanged |= ImGui::DragVectorFloat( L_Sprintf( TEXT( "%s_%p" ), property->GetName().c_str(), this ), *( Vector* )valueData, vectorProperty->GetDefaultComponentValue(), 0.1f );
			}

			// Rotator property
			else if ( theClass->HasAnyCastFlags( CASTCLASS_CRotatorProperty ) )
			{
				Vector		rotation = ( ( CRotator* )valueData )->ToEuler();
				bPropertyIsChanged |= ImGui::DragVectorFloat( L_Sprintf( TEXT( "%s_%p" ), property->GetName().c_str(), this ), rotation, 0.f, 0.1f );
				if ( bPropertyIsChanged )
				{
					*( CRotator* )valueData = CRotator::MakeFromEuler( rotation );
				}
			}

			// Asset property
			else if ( theClass->HasAnyCastFlags( CASTCLASS_CAssetProperty ) )
			{
				CAssetProperty*		assetProperty = ExactCast<CAssetProperty>( property );
				std::wstring		assetReference;
				MakeReferenceToAsset( *( TAssetHandle<CAsset>* )valueData, assetReference );

				bPropertyIsChanged |= ImGui::SelectAsset( L_Sprintf( TEXT( "%s_%p" ), property->GetName().c_str(), this ), TEXT( "" ), assetReference );
				if ( bPropertyIsChanged )
				{
					*( TAssetHandle<CAsset>* )valueData = g_PackageManager->FindAsset( assetReference, assetProperty->GetAssetType() );
				}
			}

			// String property
			else if ( theClass->HasAnyCastFlags( CASTCLASS_CStringProperty ) )
			{
				std::string			ansiString = TCHAR_TO_ANSI( ( ( std::wstring* )valueData )->c_str() );
				bPropertyIsChanged |= ImGui::InputText( TCHAR_TO_ANSI( L_Sprintf( TEXT( "##%p" ), this ).c_str() ), &ansiString );
				if ( bPropertyIsChanged )
				{
					*( std::wstring* )valueData = ANSI_TO_TCHAR( ansiString.c_str() );
				}
			}

			// Unknown property
			else
			{
				ImGui::TextColored( g_ImGUIEngine->GetStyleColor( IGC_ErrorColor ), "Unknown type" );
			}

			if ( bPropertyIsChanged )
			{
				// Copy the new value to all objects. We start at first index because the zero element was changed early
				for ( uint32 index = 1, count = addresses.size(); index < count; ++index )
				{
					Memory::Memcpy( addresses[index], valueData, property->GetElementSize() );
				}

				// Notify all object if property was changed
				NotifyPostChange( PropertyChangedEvenet( !bPropertyInArray ? property : Cast<CArrayProperty>( property->GetOuter() ), PCT_ValueSet ) );
			}

			if ( !bPropertyInArray )
			{
				ImGui::Columns( 1 );
			}
		}

		if ( bPropertyIsDisabled )
		{
			ImGui::EndDisabled();
		}
	}
}

/*
==================
CItemPropertyNode::GetValueBaseAddress
==================
*/
byte* CItemPropertyNode::GetValueBaseAddress( byte* InBaseAddress ) const
{
	CArrayProperty*			outerArrayProperty = Cast<CArrayProperty>( property->GetOuter() );
	if ( outerArrayProperty )
	{
		std::vector<byte>*	array = ( std::vector<byte>* )parentNode->GetValueBaseAddress( InBaseAddress );
		uint32				numElements = array->size() / outerArrayProperty->GetInnerProperty()->GetElementSize();
		if ( array && arrayIndex < numElements )
		{
			return array->data() + propertyOffset;
		}

		return nullptr;
	}
	else
	{
		byte*	address = parentNode->GetValueAddress( InBaseAddress );
		if ( address )
		{
			address += propertyOffset;
		}
		return address;
	}
}

/*
==================
CItemPropertyNode::GetValueAddress
==================
*/
byte* CItemPropertyNode::GetValueAddress( byte* InBaseAddress ) const
{
	byte*				result = GetValueBaseAddress( InBaseAddress );
	if ( result && property->GetClass()->HasAnyCastFlags( CASTCLASS_CArrayProperty ) )
	{
		result = ( byte* )( ( std::vector<byte>* )result )->data();
	}

	return result;
}

/*
==================
CItemPropertyNode::InitChildNodes
==================
*/
void CItemPropertyNode::InitChildNodes()
{
	Assert( property );
	CClass*		propertyClass = property->GetClass();

	// Property it's static array
	if ( property->GetArraySize() > 1 && arrayIndex == INDEX_NONE )
	{
		for ( uint32 index = 0, count = property->GetArraySize(); index < count; ++index )
		{
			CItemPropertyNode*	newItemNode = new CItemPropertyNode();
			newItemNode->InitNode( this, property, index * property->GetElementSize(), index );
			AddChildNode( newItemNode );
		}
	}
	// Struct property
	else if ( propertyClass->HasAnyCastFlags( CASTCLASS_CStructProperty ) )
	{ 
		CStructProperty*								structProperty = ExactCast<CStructProperty>( property );
		std::vector<CProperty*>							properties;
		structProperty->GetPropertyStruct()->GetProperties( properties );
		
		for ( uint32 index = 0, count = properties.size(); index < count; ++index )
		{
			// We ignore property if it not have neither CPF_Edit nor CPF_EditConst
			CProperty*		property = properties[index];
			if ( property->HasAnyFlags( CPF_Edit ) || property->HasAnyFlags( CPF_EditConst ) )
			{
				CItemPropertyNode*		itemNode = new CItemPropertyNode();
				itemNode->InitNode( this, property, property->GetOffset(), INDEX_NONE );
				AddChildNode( itemNode );
			}
		}
	}
	// Object property
	else if ( propertyClass->HasAnyCastFlags( CASTCLASS_CObjectProperty ) )
	{
		std::vector<byte*>		addresses;
		if ( GetReadAddress( this, addresses ) )
		{
			// Have a peek at the first one, and only build an objects node if we've got addresses
			CObject*	object = *( CObject** )addresses[0];
			if ( object )
			{
				// Verify it's not above in the hierarchy somewhere
				CObjectPropertyNode*	parentObjectNode = FindObjectItemParent();
				while ( parentObjectNode )
				{
					for ( uint32 index = 0, count = parentObjectNode->GetNumObjects(); index < count; ++index )
					{
						if ( parentObjectNode->GetObject( index ) == object )
						{
							// Stop the circular loop!
							return;
						}
					}

					CPropertyNode*	upwardNode = parentObjectNode->GetParentNode();
					parentObjectNode = !upwardNode ? nullptr : upwardNode->FindObjectItemParent();
				}

				CObjectPropertyNode*	newObjectNode = new CObjectPropertyNode();
				for ( uint32 index = 0, count = addresses.size(); index < count; ++index )
				{
					newObjectNode->AddObject( *( CObject** )addresses[index] );
				}
				newObjectNode->InitNode( this, property, -1, INDEX_NONE );
				AddChildNode( newObjectNode );
			}
		}
	}
	// Array property
	else if ( propertyClass->HasAnyCastFlags( CASTCLASS_CArrayProperty ) )
	{
		std::vector<byte*>	addresses;
		if ( GetReadAddress( this, addresses ) )
		{
			std::vector<byte>*	array = ( std::vector<byte>* )addresses[0];
			if ( array )
			{
				CArrayProperty*			arrayProperty = ExactCast<CArrayProperty>( property );
				CProperty*				innerProperty = arrayProperty->GetInnerProperty();
				for ( uint32 index = 0, count = array->size() / innerProperty->GetElementSize(); index < count; ++index )
				{
					CItemPropertyNode*	newItemNode = new CItemPropertyNode();
					newItemNode->InitNode( this, innerProperty, index * innerProperty->GetElementSize(), index );
					AddChildNode( newItemNode );
				}
			}
		}
	}
}