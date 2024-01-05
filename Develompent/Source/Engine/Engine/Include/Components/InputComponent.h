/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef INPUTCOMPONENT_H
#define INPUTCOMPONENT_H

#include <vector>
#include <unordered_map>

#include "System/Delegate.h"
#include "System/ButtonCode.h"
#include "System/BaseWindow.h"
#include "Misc/CoreGlobals.h"
#include "Components/ActorComponent.h"

/**
 * @ingroup Engine
 * Declare delegate of process input action
 */
DECLARE_DELEGATE( COnInputActionDelegate );

/**
 * @ingroup Engine
 * Declare delegate of process input axis
 */
DECLARE_DELEGATE( COnInputAxisDelegate, float );

/**
 * @ingroup Engine
 * Enumeration of type input events
 */
enum EInputEvent
{
	IE_Pressed,		/**< Press event */
	IE_Released,	/**< Release event */
	IE_Num = 2		/**< Num of events */
};

/**
 * @ingroup Engine
 * Component for work inputs in actors
 */
class CInputComponent : public CActorComponent
{
	DECLARE_CLASS( CInputComponent, CActorComponent, 0, 0, TEXT( "Engine" ) )
	NO_DEFAULT_CONSTRUCTOR( CInputComponent )

public:
	/**
	 * Begins Play for the component.
	 * Called when the owning Actor begins play or when the component is created if the Actor has already begun play.
	 */
	virtual void BeginPlay() override;

	/**
	 * Function called every frame on this ActorComponent. Override this function to implement custom logic to be executed every frame.
	 *
	 * @param[in] InDeltaTime The time since the last tick.
	 */
	virtual void TickComponent( float InDeltaTime );

	/**
	 * Show cursor
	 */
	FORCEINLINE void ShowCursor()
	{
		g_Window->ShowCursor();
	}

	/**
	 * Hide cursor
	 */
	FORCEINLINE void HideCursor()
	{
		g_Window->HideCursor();
	}

	/**
	 * Bind to action
	 * 
	 * @param InName Name action
	 * @param InTriggerEvent Button event when triggered this action
	 * @param InDelegate Delegate
	 */
	FORCEINLINE void BindAction( const std::wstring& InName, EInputEvent InTriggerEvent, const COnInputActionDelegate::DelegateType_t& InDelegate )
	{
		InputActionMap_t::iterator		it = inputActionMap.find( InName );
		if ( it == inputActionMap.end() )
		{
			return;
		}

		Assert( InTriggerEvent != IE_Num );
		it->second.inputActionDelegate[ InTriggerEvent ].Bind( InDelegate );
	}

	/**
	 * Bind to axis
	 *
	 * @param InName Name axis
	 * @param InDelegate Delegate
	 */
	FORCEINLINE void BindAxis( const std::wstring& InName, const COnInputAxisDelegate::DelegateType_t& InDelegate )
	{
		InputAxisMap_t::iterator		it = inputAxisMap.find( InName );
		if ( it == inputAxisMap.end() )
		{
			return;
		}
		it->second.inputAxisDelegate.Bind( InDelegate );
	}

private:
	/**
	 * Struct description of input action
	 */
	struct InputAction
	{
		std::wstring					name;								/**< Name of action */
		std::vector< EButtonCode >		buttons;							/**< Buttons for trigger action */
		COnInputActionDelegate			inputActionDelegate[ IE_Num ];		/**< Array of delegates per input event */
	};

	/**
	 * Struct description of input axis
	 */
	struct InputAxis
	{
		/**
		 * Typedef of pair EButtonCode and float (scale)
		 */
		typedef std::pair< EButtonCode, float >			PairAxisButton_t;

		std::wstring							name;				/**< Name of action */
		std::vector< PairAxisButton_t >			buttons;			/**< Buttons for trigger action */
		COnInputAxisDelegate					inputAxisDelegate;	/**< Array of delegates per input event */
	};

	/**
	 * Typedef of input action map
	 */
	typedef std::unordered_map< std::wstring, InputAction >		InputActionMap_t;

	/**
	 * Typedef of input axis map
	 */
	typedef std::unordered_map< std::wstring, InputAxis >			InputAxisMap_t;

	InputActionMap_t			inputActionMap;		/**< Input action map */
	InputAxisMap_t				inputAxisMap;		/**< Input axis map */
};

#endif // !INPUTCOMPONENT_H