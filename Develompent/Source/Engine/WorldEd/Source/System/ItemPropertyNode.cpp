#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Misc/UIGlobals.h"
#include "System/ItemPropertyNode.h"
#include "System/ObjectPropertyNode.h"
#include "ImGUI/ImGUIEngine.h"
#include "ImGUI/imgui.h"
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
		CObjectPropertyNode* parentObjectNode		= FindObjectItemParent();
		CObject*			 objectZero				= parentObjectNode->GetObject( 0 );
		CClass*				 theClass				= property->GetClass();
		bool				 bPropertyIsDisabled	= property->HasAnyFlags( CPF_Const ) || !objectZero->CanEditProperty( property );
		bool				 bPropertyInArray		= Cast<CArrayProperty>( property->GetOuter() );
		byte*				 valueData				= addresses[0];

		// Disable property if need		
		if ( bPropertyIsDisabled )
		{
			ImGui::BeginDisabled( true );
		}

		// Object property
		if ( theClass->HasAnyCastFlags( CASTCLASS_CObjectProperty ) )
		{
			ImGui::Dummy( ImVec2( InItemWidthSpacing, 0.f ) );
			ImGui::SameLine();
			if ( ImGui::CollapsingHeader( CString::Format( TEXT( "%s##%p" ), property->GetName().c_str(), this ), true ) )
			{
				TickChildren( InItemWidthSpacing + ImGui::GetStyle().ItemSpacing.x );
			}
		}
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
				bPropertyIsChanged |= ImGui::Checkbox( TCHAR_TO_ANSI( CString::Format( TEXT( "##%s_%p" ), property->GetName().c_str(), this ).c_str() ), ( bool* )valueData );
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
					bPropertyIsChanged = ImGui::DragInt( TCHAR_TO_ANSI( CString::Format( TEXT( "##%s_%p" ), property->GetName().c_str(), this ).c_str() ), &intValue, 1.f, 0, 255 );
					if ( bPropertyIsChanged )
					{
						*valueData = intValue;
					}
				}
				// Otherwise it's enum
				else
				{
					bPropertyIsChanged = ImGui::SelectEnum( CString::Format( TEXT( "##%s_%p" ), property->GetName().c_str(), this ), cenum, *valueData );
				}
			}

			// Int property
			else if ( theClass->HasAnyCastFlags( CASTCLASS_CIntProperty ) )
			{
				bPropertyIsChanged = ImGui::DragInt( TCHAR_TO_ANSI( CString::Format( TEXT( "##%s_%p" ), property->GetName().c_str(), this ).c_str() ), ( int32* )valueData );
			}

			// Float property
			else if ( theClass->HasAnyCastFlags( CASTCLASS_CFloatProperty ) )
			{
				bPropertyIsChanged = ImGui::DragFloat( TCHAR_TO_ANSI( CString::Format( TEXT( "##%s_%p" ), property->GetName().c_str(), this ).c_str() ), ( float* )valueData );
			}

			// Color property
			else if ( theClass->HasAnyCastFlags( CASTCLASS_CColorProperty ) )
			{
				Vector4D	color = ( ( CColor* )valueData )->ToNormalizedVector4D();
				if ( ImGui::ColorEdit4( TCHAR_TO_ANSI( CString::Format( TEXT( "##%s_%p" ), property->GetName().c_str(), this ).c_str() ), ( float* )&color, ImGuiColorEditFlags_NoInputs ) )
				{
					*( CColor* )valueData = color;
					bPropertyIsChanged = true;
				}
			}

			// Vector property
			else if ( theClass->HasAnyCastFlags( CASTCLASS_CVectorProperty ) )
			{
				CVectorProperty*	vectorProperty = ExactCast<CVectorProperty>( property );
				bPropertyIsChanged |= ImGui::DragVectorFloat( CString::Format( TEXT( "%s_%p" ), property->GetName().c_str(), this ), *( Vector* )valueData, vectorProperty->GetDefaultComponentValue(), 0.1f );
			}

			// Rotator property
			else if ( theClass->HasAnyCastFlags( CASTCLASS_CRotatorProperty ) )
			{
				Vector		rotation = ( ( CRotator* )valueData )->ToEuler();
				bPropertyIsChanged |= ImGui::DragVectorFloat( CString::Format( TEXT( "%s_%p" ), property->GetName().c_str(), this ), rotation, 0.f, 0.1f );
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

				bPropertyIsChanged |= ImGui::SelectAsset( CString::Format( TEXT( "%s_%p" ), property->GetName().c_str(), this ), TEXT( "" ), assetReference );
				if ( bPropertyIsChanged )
				{
					*( TAssetHandle<CAsset>* )valueData = g_PackageManager->FindAsset( assetReference, assetProperty->GetAssetType() );
				}
			}

			// Array property
			else if ( theClass->HasAnyCastFlags( CASTCLASS_CArrayProperty ) )
			{
				if ( !childNodes.empty() )
				{
					ImGui::Text("%i elements", childNodes.size());
					ImGui::Columns( 1 );
					ImGui::Dummy( ImVec2( InItemWidthSpacing + ImGui::GetStyle().ItemSpacing.x, 0.f ) );
					ImGui::SameLine();
					if ( ImGui::CollapsingHeader( CString::Format( TEXT( "##%p" ), this ), true ) )
					{
						TickChildren( InItemWidthSpacing + ImGui::GetStyle().ItemSpacing.x * 2.f );
					}
				}
				else
				{
					ImGui::Text( "Empty array" );
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
					memcpy( addresses[index], valueData, property->GetElementSize() );
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