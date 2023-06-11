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
	for ( auto it = objectFields.begin(), itEnd = objectFields.end(); it != itEnd; ++it )
	{
		if ( ImGui::CollapsingHeader( it->first.c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			for ( auto itObj = it->second.begin(), itObjEnd = it->second.end(); itObj != itObjEnd; ++itObj )
			{
				( *itObj ).Tick();
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

	// Getting an array of selected actors (we ignore InActors because it may not contain all of the selected actors)
	const std::vector<ActorRef_t>&						selectedActors = g_World->GetSelectedActors();
	if ( selectedActors.empty() )
	{
		return;
	}

	// Create a set to store the fields present in each actor
	std::unordered_map<const SFieldDescription*, std::list<CObject*>>	commonFields;

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
			GetAllObjectFields( component->GetDataDescMap(), component, commonFields );
		}
	}

	// Checking for the presence of components in other actors
	for ( uint32 actorIdx = 1, actorNum = selectedActors.size(); actorIdx < actorNum; ++actorIdx )
	{
		ActorRef_t															actor = selectedActors[actorIdx];
		std::unordered_map<const SFieldDescription*, std::list<CObject*>>	actorFields;

		// Get all fields of actor
		GetAllObjectFields( actor->GetDataDescMap(), actor, actorFields );

		// Get all fields of each component
		const std::vector<ActorComponentRef_t>&			components = actor->GetComponents();
		for ( uint32 index = 0, count = components.size(); index < count; ++index )
		{
			ActorComponentRef_t		component = components[index];
			GetAllObjectFields( component->GetDataDescMap(), component, actorFields );
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
	}

	// Add fields to final map
	for ( auto it = commonFields.begin(); it != commonFields.end(); ++it )
	{
		objectFields[it->first->category].push_back( CObjectField( it->second, it->first ) );
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
		if ( ImGui::Checkbox( field->name, &bValue ) )
		{
			SetObjectValue( bValue );
		}
		break;
	}

	case FT_Float:
	{
		float	value = *DataMap::DataFieldAccess<float>( objectZero, field );
		if ( ImGui::InputFloat( field->name, &value ) )
		{
			SetObjectValue( value );
		}
		break;
	}

	case FT_Color:
	{
		Vector4D	color = DataMap::DataFieldAccess<CColor>( objectZero, field )->ToNormalizedVector4D();
		if ( ImGui::ColorPicker4( field->name, ( float* )&color ) )
		{
			SetObjectValue<CColor>( color );
		}
		break;
	}
	}
}