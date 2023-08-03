#include "Containers/StringConv.h"
#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Windows/ActorPropertiesWindow.h"

/*
==================
CActorPropertiesWindow::CActorPropertiesWindow
==================
*/
CActorPropertiesWindow::CActorPropertiesWindow( const std::wstring& InName )
	: CImGUILayer( InName )
	, actorsSelectedDelegate( nullptr )
	, actorsUnselectedDelegate( nullptr )
{
	actorsSelectedDelegate = SEditorDelegates::onActorsSelected.Add(		std::bind( &CActorPropertiesWindow::OnActorsUnSelected, this, std::placeholders::_1 ) );
	actorsUnselectedDelegate = SEditorDelegates::onActorsUnselected.Add(	std::bind( &CActorPropertiesWindow::OnActorsUnSelected, this, std::placeholders::_1 ) );
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
		SEditorDelegates::onActorsSelected.Remove( actorsSelectedDelegate );
	}

	if ( actorsUnselectedDelegate )
	{
		SEditorDelegates::onActorsUnselected.Remove( actorsUnselectedDelegate );
	}
}

/*
==================
CActorPropertiesWindow::OnTick
==================
*/
void CActorPropertiesWindow::OnTick()
{
	const ImVec2&		itemSpacing = ImGui::GetStyle().ItemSpacing;
	if ( objectFields.empty() && componentsFields.empty() )
	{
		return;
	}

	// Actor properties
	ImGui::Separator();
	ImGui::Text( "Actor" );
	ImGui::Separator();

	for ( auto it = objectFields.begin(), itEnd = objectFields.end(); it != itEnd; ++it )
	{
		if ( it->second.empty() )
		{
			continue;
		}

		if ( ImGui::CollapsingHeader( it->first.c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			for ( auto itObj = it->second.begin(), itObjEnd = it->second.end(); itObj != itObjEnd; ++itObj )
			{
				ImGui::Dummy( ImVec2( itemSpacing.x, 0.f ) );
				ImGui::SameLine();
				( *itObj ).Tick();
			}
		}
	}

	// Components properties
	ImGui::Dummy( ImVec2( 0, itemSpacing.y ) );
	ImGui::Separator();
	ImGui::Text( "Components" );
	ImGui::Separator();

	for ( auto itComponent = componentsFields.begin(), itComponentEnd = componentsFields.end(); itComponent != itComponentEnd; ++itComponent )
	{
		if ( itComponent->second.empty() )
		{
			continue;
		}

		if ( ImGui::CollapsingHeader( itComponent->first.c_str() ) )
		{
			for ( auto itCategory = itComponent->second.begin(), itCategoryEnd = itComponent->second.end(); itCategory != itCategoryEnd; ++itCategory )
			{
				if ( itCategory->second.empty() )
				{
					continue;
				}

				ImGui::Dummy( ImVec2( itemSpacing.x, 0.f ) );
				ImGui::SameLine();
				if ( ImGui::CollapsingHeader( TCHAR_TO_ANSI( CString::Format( TEXT( "%s##%s" ), ANSI_TO_TCHAR( itCategory->first.c_str() ), ANSI_TO_TCHAR( itComponent->first.c_str() ) ).c_str() ) ) )
				{
					for ( auto itObj = itCategory->second.begin(), itObjEnd = itCategory->second.end(); itObj != itObjEnd; ++itObj )
					{
						ImGui::Dummy( ImVec2( itemSpacing.x * 2, 0.f ) );
						ImGui::SameLine();
						( *itObj ).Tick();
					}
				}
			}
		}
	}
}

/*
==================
CActorPropertiesWindow::OnActorsUnSelected
==================
*/
void CActorPropertiesWindow::OnActorsUnSelected( const std::vector<ActorRef_t>& InActors )
{
	// Clear old object properties
	objectFields.clear();
	componentsFields.clear();

	// Getting an array of selected actors (we ignore InActors because it may not contain all of the selected actors)
	const std::vector<ActorRef_t>&						selectedActors = g_World->GetSelectedActors();
	if ( selectedActors.empty() )
	{
		return;
	}

	// Create a set to store the fields present in each actor
	std::unordered_map<const SFieldDescription*, std::list<CObject*>>										commonFields;
	std::unordered_map<std::string, std::unordered_map<const SFieldDescription*, std::list<CObject*>>>		commonComponentsFileds;

	// Initialization of the set by the components of the first actor
	{
		ActorRef_t					actorZero = selectedActors[0];
	
		// Get all fields of actor
		GetAllObjectFields( actorZero->GetDataDescMap(), actorZero, commonFields );
	
		// Get all fields of each component
		const std::vector<ActorComponentRef_t>&			components = actorZero->GetComponents();
		for ( uint32 index = 0, count = components.size(); index < count; ++index )
		{
			ActorComponentRef_t		component = components[index];
			auto					it = commonComponentsFileds.insert( std::make_pair( TCHAR_TO_ANSI( component->GetName() ), std::unordered_map<const SFieldDescription*, std::list<CObject*>>() ) ).first;
			GetAllObjectFields( component->GetDataDescMap(), component, it->second );
		}
	}
	
	// Checking for the presence of components in other actors
	for ( uint32 actorIdx = 1, actorNum = selectedActors.size(); actorIdx < actorNum; ++actorIdx )
	{
		ActorRef_t																								actor = selectedActors[actorIdx];
		std::unordered_map<const SFieldDescription*, std::list<CObject*>>										actorFields;
		std::unordered_map<std::string, std::unordered_map<const SFieldDescription*, std::list<CObject*>>>		componentsFileds;

		// Get all fields of actor
		GetAllObjectFields( actor->GetDataDescMap(), actor, actorFields );
	
		// Get all fields of each component
		const std::vector<ActorComponentRef_t>&			components = actor->GetComponents();
		for ( uint32 index = 0, count = components.size(); index < count; ++index )
		{
			ActorComponentRef_t		component = components[index];
			auto					it = componentsFileds.insert( std::make_pair( TCHAR_TO_ANSI( component->GetName() ), std::unordered_map<const SFieldDescription*, std::list<CObject*>>() ) ).first;
			GetAllObjectFields( component->GetDataDescMap(), component, it->second );
		}
	
		// Remove components that are not present in the current actor
		for ( auto it = commonFields.begin(); it != commonFields.end(); )
		{
			auto	itActorField = actorFields.find( it->first );
			if ( itActorField == actorFields.end() )
			{
				it = commonFields.erase( it );
				continue;
			}
	
			// If we found a component then add pointer to objects to common
			for ( auto itActorObj = itActorField->second.begin(), itActorObjEnd = itActorField->second.end(); itActorObj != itActorObjEnd; ++itActorObj )
			{
				it->second.push_back( *itActorObj );
			}
	
			++it;
		}

		// TODO: BS yehor.pohuliaka - Need fix it
		/*for ( auto itComponentMap = commonComponentsFileds.begin(); itComponentMap != commonComponentsFileds.end(); )
		{
			for ( auto it = itComponentMap->second.begin(); it != itComponentMap->second.end(); )
			{
				auto	itComponentField = itComponentMap->second.find( it->first );
				if ( itComponentField == itComponentMap->second.end() )
				{
					it = itComponentMap->second.erase( it );
					continue;
				}

				// If we found a component then add pointer to objects to common
				for ( auto itComponentObj = itComponentField->second.begin(), itComponentObjEnd = itComponentField->second.end(); itComponentObj != itComponentObjEnd; ++itComponentObj )
				{
					it->second.push_back( *itComponentObj );
				}

				++it;
			}

			if ( itComponentMap->second.empty() )
			{
				itComponentMap = commonComponentsFileds.erase( itComponentMap );
				continue;
			}
			else
			{
				++itComponentMap;
			}
		}*/
	}
	
	// Add actor fields to final map
	for ( auto it = commonFields.begin(), itEnd = commonFields.end(); it != itEnd; ++it )
	{
		objectFields[it->first->category].push_back( CObjectField( it->first, it->second ) );
	}

	// Add components fields to final map
	for ( auto itComponent = commonComponentsFileds.begin(), itComponentEnd = commonComponentsFileds.end(); itComponent != itComponentEnd; ++itComponent )
	{
		auto	newMapIt = componentsFields.insert( std::make_pair( itComponent->first, MapObjectFields_t() ) ).first;
		for ( auto itField = itComponent->second.begin(), itFieldEnd = itComponent->second.end(); itField != itFieldEnd; ++itField )
		{
			newMapIt->second[itField->first->category].push_back( CObjectField( itField->first, itField->second ) );
		}
	}
}

/*
==================
CActorPropertiesWindow::CObjectField::Tick
==================
*/
void CActorPropertiesWindow::CObjectField::Tick()
{
	if ( !field || objects.empty() )
	{
		return;
	}
	CObject*		objectZero = *objects.begin();

	switch ( field->type )
	{
	case FT_Boolean:
	{
		bool	bValue = *DataMap::DataFieldAccess<bool>( objectZero, field );
		if ( ImGui::Checkbox( TCHAR_TO_ANSI( CString::Format( TEXT( "%s##%p" ), ANSI_TO_TCHAR( field->name ), this ).c_str() ), &bValue) )
		{
			SetObjectValue( bValue );
		}
		break;
	}

	case FT_Float:
	{
		float	value = *DataMap::DataFieldAccess<float>( objectZero, field );
		if ( ImGui::InputFloat( TCHAR_TO_ANSI( CString::Format( TEXT( "%s##%p" ), ANSI_TO_TCHAR( field->name ), this ).c_str() ), &value ) )
		{
			SetObjectValue( value );
		}
		break;
	}

	case FT_Color:
	{
		Vector4D	color = DataMap::DataFieldAccess<CColor>( objectZero, field )->ToNormalizedVector4D();
		if ( ImGui::ColorPicker4( TCHAR_TO_ANSI( CString::Format( TEXT( "%s##%p" ), ANSI_TO_TCHAR( field->name ), this ).c_str() ), ( float* )&color ) )
		{
			SetObjectValue<CColor>( color );
		}
		break;
	}
	}
}