#include <unordered_set>

#include "System/Config.h"
#include "Misc/EngineGlobals.h"
#include "Logger/LoggerMacros.h"
#include "Components/InputComponent.h"
#include "System/InputSystem.h"

IMPLEMENT_CLASS( CInputComponent )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CInputComponent )

/*
==================
CInputComponent::BeginPlay
==================
*/
void CInputComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get mapping of buttons
	// Actions
	const CJsonValue*				configActionsValue	= CConfig::Get().GetValue( CT_Input, TEXT( "InputSystem.InputSettings" ), TEXT( "Actions" ) );
	const std::vector<CJsonValue>*	configActions		= configActionsValue ? configActionsValue->GetArray() : nullptr;
	if ( configActions )
	{
		for ( uint32 indexAction = 0, countActions = configActions->size(); indexAction < countActions; ++indexAction )
		{
			// Get JSON object of action item
			const CJsonValue&		configAction = configActions->at( indexAction );
			const CJsonObject*		configObject = configAction.GetObject();
			if ( !configObject )
			{
				Warnf( TEXT( "Invalid input %i action\n" ), indexAction );
				continue;
			}

			// Get name of action
			InputAction			inputAction;
			const CJsonValue*	inputActionName = configObject->GetValue( TEXT( "Name" ) );
			inputAction.name	= inputActionName ? inputActionName->GetString() : TEXT( "" );
			if ( inputAction.name.empty() )
			{
				Warnf( TEXT( "Invalid input %i action\n" ), indexAction );
				continue;
			}

			// Get buttons
			const CJsonValue*					inputActionButtons = configObject->GetValue( TEXT( "Buttons" ) );
			const std::vector<CJsonValue>*		configButtons = inputActionButtons ? inputActionButtons->GetArray() : nullptr;
			if ( configButtons )
			{
				for ( uint32 indexButton = 0, countButtons = configButtons->size(); indexButton < countButtons; ++indexButton )
				{
					// Get JSON item
					const CJsonValue&	configButton = configButtons->at( indexButton );
					std::wstring		buttonName = configButton.GetString();
					if ( buttonName.empty() )
					{
						Warnf( TEXT( "Invalid button name %i at input action '%s'\n" ), indexButton, inputAction.name.c_str() );
						continue;
					}

					// Get button code from name of button
					EButtonCode			buttonCode = Sys_GetButtonCodeByName( buttonName.c_str() );
					if ( buttonCode == BC_None )
					{
						Warnf( TEXT( "Action '%s': unknown button name '%s'\n" ), inputAction.name.c_str(), buttonName.c_str() );
						continue;
					}
					inputAction.buttons.push_back( buttonCode );
				}
			}

			// If array of buttons not empty - add to input action map
			if ( !inputAction.buttons.empty() )
			{
				inputActionMap.insert( std::make_pair( inputAction.name, inputAction ) );
			}	
		}
	}

	// Axis
	const CJsonValue*				configAxisValue = CConfig::Get().GetValue( CT_Input, TEXT( "InputSystem.InputSettings" ), TEXT( "Axis" ) );
	const std::vector<CJsonValue>*	configArrayAxis = configAxisValue ? configAxisValue->GetArray() : nullptr;
	if ( configArrayAxis )
	{
		for ( uint32 indexAxis = 0, countAxis = configArrayAxis->size(); indexAxis < countAxis; ++indexAxis )
		{
			// Get JSON object of action item
			const CJsonValue&		configAxis = configArrayAxis->at( indexAxis );
			const CJsonObject*		configObject = configAxis.GetObject();
			if ( !configObject )
			{
				Warnf( TEXT( "Invalid input %i axis\n" ), indexAxis );
				continue;
			}

			// Get name of axis
			InputAxis			inputAxis;
			const CJsonValue*	inputAxisName = configObject->GetValue( TEXT( "Name" ) );
			inputAxis.name		= inputAxisName ? inputAxisName->GetString() : TEXT( "" );
			if ( inputAxis.name.empty() )
			{
				Warnf( TEXT( "Invalid input %i axis\n" ), indexAxis );
				continue;
			}

			// Get buttons
			const CJsonValue*					inputAxisButtons = configObject->GetValue( TEXT( "Buttons" ) );
			const std::vector<CJsonValue>*		configButtons = inputAxisButtons ? inputAxisButtons->GetArray() : nullptr;
			if ( configButtons )
			{
				for ( uint32 indexButton = 0, countButtons = configButtons->size(); indexButton < countButtons; ++indexButton )
				{
					// Get JSON item
					const CJsonValue&		configButton = configButtons->at( indexButton );
					const CJsonObject*		configButtonObject = configButton.GetObject();
					if ( !configButtonObject )
					{
						Warnf( TEXT( "Invalid %i button at input axis '%s'\n" ), indexButton, inputAxis.name.c_str() );
						continue;
					}
			
					const CJsonValue*	axisButtonName = configButtonObject->GetValue( TEXT( "Name" ) );
					const CJsonValue*	axisButtonScale = configButtonObject->GetValue( TEXT( "Scale" ) );
					if ( !axisButtonName || !axisButtonScale )
					{
						Warnf( TEXT( "Invalid %i button at input axis '%s'\n" ), indexButton, inputAxis.name.c_str() );
						continue;
					}

					// Get button code from name of button
					std::wstring		buttonName = axisButtonName->GetString();
					float				scale = axisButtonScale->GetNumber();
					EButtonCode			buttonCode = Sys_GetButtonCodeByName( buttonName.c_str() );
					if ( buttonCode == BC_None )
					{
						Warnf( TEXT( "Axis '%s' with scale %f: unknown button name '%s'\n" ), inputAxis.name.c_str(), scale, buttonName.c_str() );
						continue;
					}
					inputAxis.buttons.push_back( std::make_pair( buttonCode, scale ) );
				}
			}

			// If array of buttons not empty - add to input axis map
			if ( !inputAxis.buttons.empty() )
			{
				inputAxisMap.insert( std::make_pair( inputAxis.name, inputAxis ) );
			}
		}
	}
}

/**
 * Enumeration of flags for call input action
 */
enum EInputActionFlags
{
	IAF_None		= 0,								/**< No call */
	IAF_Pressed		= 1 << 0,							/**< Call event of pressed */
	IAF_Released	= 1 << 1,							/**< Call event of released */
	IAF_All			= IAF_Pressed | IAF_Released		/**< Need call press and release events */
};

/*
==================
CInputComponent::TickComponent
==================
*/
void CInputComponent::TickComponent( float InDeltaTime )
{
	Super::TickComponent( InDeltaTime );
	
	// Find triggered actions and execute it
	for ( InputActionMap_t::iterator it = inputActionMap.begin(), itEnd = inputActionMap.end(); it != itEnd; ++it )
	{
		uint32		executeFlags = IAF_None;
		for ( uint32 indexButton = 0, countButtons = it->second.buttons.size(); indexButton < countButtons; ++indexButton )
		{
			switch ( g_InputSystem->GetButtonEvent( it->second.buttons[ indexButton ] ) )
			{
			case BE_Pressed:	executeFlags |= IAF_Pressed;	break;
			case BE_Released:	executeFlags |= IAF_Released;	break;
			}

			if ( executeFlags & IAF_All )
			{
				break;
			}
		}

		// If not need execute action - continue to next step
		if ( executeFlags & IAF_None )
		{
			continue;
		}

		// Call delegate for pressed buttons
		if ( executeFlags & IAF_Pressed )
		{
			it->second.inputActionDelegate[ IE_Pressed ].Execute();
		}

		// Call delegate for released buttons
		if ( executeFlags & IAF_Released )
		{
			it->second.inputActionDelegate[ IE_Released ].Execute();
		}
	}

	// Find triggered axis and execute it
	for ( InputAxisMap_t::iterator it = inputAxisMap.begin(), itEnd = inputAxisMap.end(); it != itEnd; ++it )
	{
		std::unordered_set< float >		triggeredScales;
		for ( uint32 indexButton = 0, countButtons = it->second.buttons.size(); indexButton < countButtons; ++indexButton )
		{
			const InputAxis::PairAxisButton_t&		pairAxisButton	= it->second.buttons[ indexButton ];
			EButtonEvent							buttonEvent		= g_InputSystem->GetButtonEvent( pairAxisButton.first );
			switch ( buttonEvent )
			{
			case BE_Pressed:
			case BE_Released:
			case BE_Scrolled:
			case BE_Moved:
			{
				float		value = pairAxisButton.second;
				if ( buttonEvent == BE_Moved )
				{
					value *= g_InputSystem->GetMouseOffset( pairAxisButton.first ) * g_InputSystem->GetMouseSensitivity();
				}

				triggeredScales.insert( value );
				break;
			}

			default:	continue;
			}
		}

		// Trigger all scales
		for ( auto itTriggerScale = triggeredScales.begin(), itTriggerScaleEnd = triggeredScales.end(); itTriggerScale != itTriggerScaleEnd; ++itTriggerScale )
		{
			it->second.inputAxisDelegate.Execute( *itTriggerScale );
		}
	}
}