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
	std::vector< CConfigValue >		configActions = g_Config.GetValue( CT_Input, TEXT( "InputSystem.InputSettings" ), TEXT( "Actions" ) ).GetArray();
	for ( uint32 indexAction = 0, countActions = configActions.size(); indexAction < countActions; ++indexAction )
	{
		// Get JSON object of action item
		const CConfigValue&		configAction = configActions[ indexAction ];
		Assert( configAction.GetType() == CConfigValue::T_Object );
		CConfigObject			configObject = configAction.GetObject();

		// Get name of action
		SInputAction			inputAction;
		inputAction.name = configObject.GetValue( TEXT( "Name" ) ).GetString();
		if ( inputAction.name.empty() )
		{
			continue;
		}

		// Get buttons
		std::vector< CConfigValue >		configButtons = configObject.GetValue( TEXT( "Buttons" ) ).GetArray();
		for ( uint32 indexButton = 0, countButtons = configButtons.size(); indexButton < countButtons; ++indexButton )
		{
			// Get JSON item
			const CConfigValue& configButton = configButtons[ indexButton ];
			Assert( configButton.GetType() == CConfigValue::T_String );
			std::wstring		buttonName = configButton.GetString();

			// Get button code from name of button
			EButtonCode			buttonCode = Sys_GetButtonCodeByName( buttonName.c_str() );
			if ( buttonCode == BC_None )
			{
				Warnf( TEXT( "Action '%s': unknown button name '%s'\n" ), inputAction.name.c_str(), buttonName.c_str() );
				continue;
			}
			inputAction.buttons.push_back( buttonCode );
		}

		// If array of buttons not empty - add to input action map
		if ( !inputAction.buttons.empty() )
		{
			inputActionMap.insert( std::make_pair( inputAction.name, inputAction ) );
		}	
	}

	// Axis
	std::vector< CConfigValue >		configArrayAxis = g_Config.GetValue( CT_Input, TEXT( "InputSystem.InputSettings" ), TEXT( "Axis" ) ).GetArray();
	for ( uint32 indexAxis = 0, countAxis = configArrayAxis.size(); indexAxis < countAxis; ++indexAxis )
	{
		// Get JSON object of action item
		const CConfigValue&		configAxis = configArrayAxis[ indexAxis ];
		Assert( configAxis.GetType() == CConfigValue::T_Object );
		CConfigObject			configObject = configAxis.GetObject();

		// Get name of axis
		SInputAxis			inputAxis;
		inputAxis.name = configObject.GetValue( TEXT( "Name" ) ).GetString();
		if ( inputAxis.name.empty() )
		{
			continue;
		}

		// Get buttons
		std::vector< CConfigValue >		configButtons = configObject.GetValue( TEXT( "Buttons" ) ).GetArray();
		for ( uint32 indexButton = 0, countButtons = configButtons.size(); indexButton < countButtons; ++indexButton )
		{
			// Get JSON item
			const CConfigValue&		configButton = configButtons[ indexButton ];
			Assert( configAxis.GetType() == CConfigValue::T_Object );
			CConfigObject			configButtonObject = configButton.GetObject();
			
			std::wstring			buttonName = configButtonObject.GetValue( TEXT( "Name" ) ).GetString();
			float					scale = configButtonObject.GetValue( TEXT( "Scale" ) ).GetNumber();

			// Get button code from name of button
			EButtonCode			buttonCode = Sys_GetButtonCodeByName( buttonName.c_str() );
			if ( buttonCode == BC_None )
			{
				Warnf( TEXT( "Axis '%s' with scale %f: unknown button name '%s'\n" ), inputAxis.name.c_str(), scale, buttonName.c_str() );
				continue;
			}
			inputAxis.buttons.push_back( std::make_pair( buttonCode, scale ) );
		}

		// If array of buttons not empty - add to input axis map
		if ( !inputAxis.buttons.empty() )
		{
			inputAxisMap.insert( std::make_pair( inputAxis.name, inputAxis ) );
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
			const SInputAxis::PairAxisButton_t&		pairAxisButton	= it->second.buttons[ indexButton ];
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