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
	if ( properties.IsEmpty() )
	{
		return;
	}
	properties.Tick();
}

/*
==================
CActorPropertiesWindow::GetAllPropertiesFromObject
==================
*/
void CActorPropertiesWindow::GetAllPropertiesFromObject( CObject* InObject, CObjectProperties& OutObjectProperties ) const
{
	// Get all properties of object
	Assert( InObject );
	if ( InObject->GetNumProperties() == 0 )
	{
		return;
	}

	// Get all object's properties
	std::unordered_map<CName, std::vector<TSharedPtr<PropertyHandler>>, CName::HashFunction>&	objectProperties = OutObjectProperties.GetProperties();
	{
		std::vector<CProperty*>		tempProperties;
		InObject->GetProperties( tempProperties );
		for ( uint32 index = 0, count = tempProperties.size(); index < count; ++index )
		{
			CProperty*					property = tempProperties[index];
			objectProperties[property->GetCategory()].push_back( MakeSharedPtr<PropertyHandler>( property ) );
		}
	}

	// Get all properties from components
	for ( auto itObjectProperties = objectProperties.begin(); itObjectProperties != objectProperties.end(); ++itObjectProperties )
	{
		std::vector<TSharedPtr<PropertyHandler>>&		properties = itObjectProperties->second;
		for ( uint32 index = 0; index < properties.size(); ++index )
		{
			// If in property handler objectProperties is a valid then we get all it's properties 
			const TSharedPtr<PropertyHandler>&			propertyHandler = properties[index];
			if ( propertyHandler->objectProperties )
			{
				UPropertyValue		propertyValue;
				propertyHandler->property->GetPropertyValue( ( byte* )InObject, propertyValue );
				Assert( propertyValue.objectValue );

				if ( propertyValue.objectValue->GetNumProperties() > 0 )
				{
					GetAllPropertiesFromObject( propertyValue.objectValue, *propertyHandler->objectProperties );
				}
			}
		}
	}

	// If the array of object properties is not empty, then remember the pointer to it
	if ( !objectProperties.empty() )
	{
		OutObjectProperties.GetObjects().push_back( InObject );
	}
}

/*
==================
CActorPropertiesWindow::RemoveMissingProperties
==================
*/
bool CActorPropertiesWindow::RemoveMissingProperties( const std::vector<TSharedPtr<PropertyHandler>>& InArrayA, std::vector<TSharedPtr<PropertyHandler>>& InOutArrayB ) const
{
	bool	bResult = false;
	for ( uint32 propIdx = 0; propIdx < InOutArrayB.size(); )
	{
		bool										bFound = false;
		const TSharedPtr<PropertyHandler>&			dstPropertyHandler = InOutArrayB[propIdx];
		for ( uint32 checkPropIdx = 0, countCheckProps = InArrayA.size(); checkPropIdx < countCheckProps; ++checkPropIdx )
		{
			const TSharedPtr<PropertyHandler>&		srcPropertyHandler = InArrayA[checkPropIdx];
			if ( dstPropertyHandler->property == srcPropertyHandler->property )
			{
				// If the property is object then we recursive remove missing properties in the objectProperties
				if ( dstPropertyHandler->objectProperties )
				{
					if ( RemoveMissingProperties( srcPropertyHandler->objectProperties->GetProperties(), dstPropertyHandler->objectProperties->GetProperties() ) )
					{
						// Size array of objects in the local CObjectProperties must be 1, because it contain only one object
						Assert( srcPropertyHandler->objectProperties->GetObjects().size() == 1 );
						dstPropertyHandler->objectProperties->GetObjects().push_back( srcPropertyHandler->objectProperties->GetObjects()[0] );
						bFound = true;
					}
				}
				else
				{
					bFound = true;
				}
				break;
			}
		}

		if ( !bFound )
		{
			InOutArrayB.erase( InOutArrayB.begin() + propIdx );
		}
		else
		{
			bResult = true;
			++propIdx;
		}
	}

	return bResult;
}

/*
==================
CActorPropertiesWindow::RemoveMissingProperties
==================
*/
bool CActorPropertiesWindow::RemoveMissingProperties( const std::unordered_map<CName, std::vector<TSharedPtr<PropertyHandler>>, CName::HashFunction>& InArrayA, std::unordered_map<CName, std::vector<TSharedPtr<PropertyHandler>>, CName::HashFunction>& InOutArrayB ) const
{
	bool		bWereMatches = false;
	for ( auto itBProperties = InOutArrayB.begin(); itBProperties != InOutArrayB.end(); )
	{
		auto	itAProperties = InArrayA.find( itBProperties->first );
		if ( itAProperties == InArrayA.end() )
		{
			itBProperties = InOutArrayB.erase( itBProperties );
			continue;
		}

		if ( RemoveMissingProperties( itAProperties->second, itBProperties->second ) )
		{
			bWereMatches = true;
			++itBProperties;
			continue;
		}
		else
		{
			itBProperties = InOutArrayB.erase( itBProperties );
		}
	}

	return bWereMatches;
}

/*
==================
CActorPropertiesWindow::OnActorsUnSelected
==================
*/
void CActorPropertiesWindow::OnActorsUnSelected( const std::vector<ActorRef_t>& InActors )
{
	// Clear old object properties
	properties.Clear();

	// Getting an array of selected actors (we ignore InActors because it may not contain all of the selected actors)
	const std::vector<ActorRef_t>&		selectedActors = g_World->GetSelectedActors();
	if ( selectedActors.empty() )
	{
		return;
	}

	// Initialization of the set by the components of the first actor
	GetAllPropertiesFromObject( selectedActors[0], properties );

	// Checking for the presence of components in other actors
	for ( uint32 actorIdx = 1, actorNum = selectedActors.size(); actorIdx < actorNum; ++actorIdx )
	{
		// Get all properties in the actor
		ActorRef_t			actor = selectedActors[actorIdx];
		CObjectProperties	localActorProperties;
		GetAllPropertiesFromObject( actor, localActorProperties );

		// Remove properties that are not present in the current actor
		if ( RemoveMissingProperties( localActorProperties.GetProperties(), properties.GetProperties() ) )
		{
			properties.GetObjects().push_back( actor );
		}
	}
}

/*
==================
CActorPropertiesWindow::OnMapChanged
==================
*/
void CActorPropertiesWindow::OnMapChanged()
{
	// Clear object properties
	properties.Clear();
}


/*
==================
CActorPropertiesWindow::CObjectProperties::SetPropertyValue
==================
*/
void CActorPropertiesWindow::CObjectProperties::SetPropertyValue( CProperty* InProperty, const UPropertyValue& InPropertyValue )
{
	for ( uint32 objIdx = 0, countObjs = objects.size(); objIdx < countObjs; ++objIdx )
	{
		CObject*	object = objects[objIdx];
		InProperty->SetPropertyValue( ( byte* )object, InPropertyValue );
		object->PostEditChangeProperty( InProperty, PCT_ValueSet );
	}
}

/*
==================
CActorPropertiesWindow::CObjectProperties::TickProperty
==================
*/
void CActorPropertiesWindow::CObjectProperties::TickProperty( float InItemWidthSpacing, bool InApplySpacingToCategories, const TSharedPtr<PropertyHandler>& InPropertyHandler )
{
	bool			bPropertyIsChanged = false;
	CObject*		objectZero = objects[0];
	CClass*			theClass = InPropertyHandler->property->GetClass();
	bool			bPropertyIsDisabled = InPropertyHandler->property->HasAnyFlags( CPF_Const ) || !objectZero->CanEditProperty( InPropertyHandler->property );
	bool			bPropertyIsObject = theClass->HasAnyCastFlags( CASTCLASS_CObjectProperty );
	UPropertyValue	propertyValue;

	// Draw name of property
	if ( bPropertyIsDisabled )
	{
		ImGui::BeginDisabled( true );
	}
	if ( !bPropertyIsObject )
	{
		ImGui::Dummy( ImVec2( InItemWidthSpacing, 0.f ) );
		ImGui::SameLine();
		ImGui::Text( TCHAR_TO_ANSI( InPropertyHandler->property->GetCName().ToString().c_str() ) );
		if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
		{
			ImGui::SetTooltip( TCHAR_TO_ANSI( InPropertyHandler->property->GetDescription().c_str() ) );
		}
		
		ImGui::NextColumn();
	}

	// Draw value of property
	// Bool property
	if ( theClass->HasAnyCastFlags( CASTCLASS_CBoolProperty ) )
	{
		InPropertyHandler->property->GetPropertyValue( ( byte* )objectZero, propertyValue );
		bPropertyIsChanged = ImGui::Checkbox( TCHAR_TO_ANSI( CString::Format( TEXT( "##%s_%p" ), InPropertyHandler->property->GetName().c_str(), this ).c_str() ), &propertyValue.boolValue );
	}

	// Byte property
	else if ( theClass->HasAnyCastFlags( CASTCLASS_CByteProperty ) )
	{
		InPropertyHandler->property->GetPropertyValue( ( byte* )objectZero, propertyValue );
		CByteProperty*		byteProperty = ExactCast<CByteProperty>( InPropertyHandler->property );
		Assert( byteProperty );

		// Get cenum. If it null then byte property is simple byte field
		CEnum*				cenum = byteProperty->GetEnum();
		if ( !cenum )
		{
			int32	intValue = propertyValue.byteValue;
			bPropertyIsChanged = ImGui::DragInt( TCHAR_TO_ANSI( CString::Format( TEXT( "##%s_%p" ), InPropertyHandler->property->GetName().c_str(), this ).c_str() ), &intValue, 1.f, 0, 255 );
			if ( bPropertyIsChanged )
			{
				propertyValue.byteValue = intValue;
			}
		}
		// Otherwise it's enum
		else
		{
			bPropertyIsChanged = ImGui::SelectEnum( CString::Format( TEXT( "##%s_%p" ), InPropertyHandler->property->GetName().c_str(), this ), cenum, propertyValue.byteValue );
		}
	}

	// Int property
	else if ( theClass->HasAnyCastFlags( CASTCLASS_CIntProperty ) )
	{
		InPropertyHandler->property->GetPropertyValue( ( byte* )objectZero, propertyValue );
		bPropertyIsChanged = ImGui::DragInt( TCHAR_TO_ANSI( CString::Format( TEXT( "##%s_%p" ), InPropertyHandler->property->GetName().c_str(), this ).c_str() ), &propertyValue.intValue );
	}

	// Float property
	else if ( theClass->HasAnyCastFlags( CASTCLASS_CFloatProperty ) )
	{
		InPropertyHandler->property->GetPropertyValue( ( byte* )objectZero, propertyValue );
		bPropertyIsChanged = ImGui::DragFloat( TCHAR_TO_ANSI( CString::Format( TEXT( "##%s_%p" ), InPropertyHandler->property->GetName().c_str(), this ).c_str() ), &propertyValue.floatValue );
	}

	// Color property
	else if ( theClass->HasAnyCastFlags( CASTCLASS_CColorProperty ) )
	{
		InPropertyHandler->property->GetPropertyValue( ( byte* )objectZero, propertyValue );
		
		Vector4D	color = propertyValue.colorValue.ToNormalizedVector4D();
		if ( ImGui::ColorEdit4( TCHAR_TO_ANSI( CString::Format( TEXT( "##%s_%p" ), InPropertyHandler->property->GetName().c_str(), this ).c_str() ), ( float* )&color, ImGuiColorEditFlags_NoInputs ) )
		{
			propertyValue.colorValue = color;
			bPropertyIsChanged = true;
		}	
	}

	// Vector property
	else if ( theClass->HasAnyCastFlags( CASTCLASS_CVectorProperty ) )
	{
		CVectorProperty*	vectorProperty = ExactCast<CVectorProperty>( InPropertyHandler->property );
		Assert( vectorProperty );

		vectorProperty->GetPropertyValue( ( byte* )objectZero, propertyValue );
		bPropertyIsChanged |= ImGui::DragVectorFloat( CString::Format( TEXT( "%s_%p" ), InPropertyHandler->property->GetName().c_str(), this ), propertyValue.vectorValue, vectorProperty->GetDefaultComponentValue(), 0.1f );
	}

	// Rotator property
	else if ( theClass->HasAnyCastFlags( CASTCLASS_CRotatorProperty ) )
	{
		InPropertyHandler->property->GetPropertyValue( ( byte* )objectZero, propertyValue );
		Vector		rotation = propertyValue.rotatorValue.ToEuler();

		bPropertyIsChanged |= ImGui::DragVectorFloat( CString::Format( TEXT( "%s_%p" ), InPropertyHandler->property->GetName().c_str(), this ), rotation, 0.f, 0.1f );
		if ( bPropertyIsChanged )
		{
			propertyValue.rotatorValue = CRotator::MakeFromEuler( rotation );
		}
	}

	// Asset property
	else if ( theClass->HasAnyCastFlags( CASTCLASS_CAssetProperty ) )
	{
		CAssetProperty*		assetProperty = ExactCast<CAssetProperty>( InPropertyHandler->property );
		Assert( assetProperty );
	
		assetProperty->GetPropertyValue( ( byte* )objectZero, propertyValue );
		std::wstring		assetReference;
		MakeReferenceToAsset( propertyValue.assetValue, assetReference );
	
		bPropertyIsChanged |= ImGui::SelectAsset( CString::Format( TEXT( "%s_%p" ), InPropertyHandler->property->GetName().c_str(), this ), TEXT( "" ), assetReference );
		if ( bPropertyIsChanged )
		{
			propertyValue.assetValue = g_PackageManager->FindAsset( assetReference, assetProperty->GetAssetType() );
		}
	}

	// Object property
	else if ( theClass->HasAnyCastFlags( CASTCLASS_CObjectProperty ) )
	{
		Assert( InPropertyHandler->objectProperties );
		ImGui::Columns( 1 );
		ImGui::Dummy( ImVec2( InItemWidthSpacing, 0.f ) );
		ImGui::SameLine();
		
		if ( ImGui::CollapsingHeader( CString::Format( TEXT( "%s##%p" ), InPropertyHandler->property->GetName().c_str(), this ), true ) )
		{
			InPropertyHandler->objectProperties->Tick( InItemWidthSpacing + ImGui::GetStyle().ItemSpacing.x, true );
		}

		ImGui::Columns( 2 );
		ImGui::NextColumn();
	}

	// Unknown property
	else
	{
		ImGui::TextColored( g_ImGUIEngine->GetStyleColor( IGC_ErrorColor ), "Unknown type" );
	}
	ImGui::NextColumn();

	if ( bPropertyIsDisabled )
	{
		ImGui::EndDisabled();
	}

	if ( bPropertyIsChanged )
	{
		SetPropertyValue( InPropertyHandler->property, propertyValue );
		g_World->MarkDirty();
	}
}

/*
==================
CActorPropertiesWindow::CObjectProperties::Tick
==================
*/
void CActorPropertiesWindow::CObjectProperties::Tick( float InItemWidthSpacing /*= 0.f*/, bool InApplySpacingToCategories /*= false*/)
{
	float		currentSpacing = InItemWidthSpacing;
	for ( auto itCategory = properties.begin(), itCategoryEnd = properties.end(); itCategory != itCategoryEnd; ++itCategory )
	{
		if ( InApplySpacingToCategories )
		{
			ImGui::Dummy( ImVec2( InItemWidthSpacing, 0.f ) );
			ImGui::SameLine();
		}
		
		if ( ImGui::CollapsingHeader( CString::Format( TEXT( "%s##%p" ), itCategory->first.ToString().c_str(), this ), true, ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			ImGui::Columns( 2 );			
			const std::vector<TSharedPtr<PropertyHandler>>&		categoryProperties = itCategory->second;
			for ( uint32 propIdx = 0, countProps = categoryProperties.size(); propIdx < countProps; ++propIdx )
			{
				const TSharedPtr<PropertyHandler>&				propertyHandler = categoryProperties[propIdx];
				Assert( propertyHandler );
				TickProperty( InItemWidthSpacing + ImGui::GetStyle().ItemSpacing.x, InApplySpacingToCategories, propertyHandler );
			}
			ImGui::Columns( 1 );
		}
	}
}