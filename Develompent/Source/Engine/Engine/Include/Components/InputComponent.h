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
#include "Components/ActorComponent.h"

/**
 * @ingroup Engine
 * Declare delegate of process input action
 */
DECLARE_DELEGATE( FOnInputActionDelegate );

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
class LInputComponent : public LActorComponent
{
	DECLARE_CLASS( LInputComponent, LActorComponent )

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
	 * Bind to action
	 * 
	 * @param InName Name action
	 * @param InTriggerEvent Button event when triggered this action
	 * @param InDelegate Delegate
	 */
	FORCEINLINE void BindAction( const std::wstring& InName, EInputEvent InTriggerEvent, const FOnInputActionDelegate::FDelegateFunction& InDelegate )
	{
		FInputActionMap::iterator		it = inputActionMap.find( InName );
		if ( it == inputActionMap.end() )
		{
			return;
		}

		check( InTriggerEvent != IE_Num );
		it->second.inputActionDelegate[ InTriggerEvent ].BindFunction( InDelegate );
	}

	/**
	 * Bind to action
	 *
	 * @param InName Name action
	 * @param InTriggerEvent Button event when triggered this action
	 * @param InObject Object
	 * @param InDelegate Delegate
	 */
	FORCEINLINE void BindAction( const std::wstring& InName, EInputEvent InTriggerEvent, LObject* InObject, const FOnInputActionDelegate::FDelegateMethod& InDelegate )
	{
		FInputActionMap::iterator		it = inputActionMap.find( InName );
		if ( it == inputActionMap.end() )
		{
			return;
		}

		check( InTriggerEvent != IE_Num );
		it->second.inputActionDelegate[ InTriggerEvent ].BindMethod( InObject, InDelegate );
	}

	/**
	 * Bind to action
	 *
	 * @param InName Name action
	 * @param InTriggerEvent Button event when triggered this action
	 * @param InObject Object
	 * @param InDelegate Delegate
	 */
	template< class TClass, typename TMethod >
	FORCEINLINE void BindAction( const std::wstring& InName, EInputEvent InTriggerEvent, TClass* InObject, const TMethod& InDelegate )
	{
		BindAction( InName, InTriggerEvent, ( LObject* )InObject, ( FOnInputActionDelegate::FDelegateMethod )InDelegate );
	}

private:
	/**
	 * Struct description of input action
	 */
	struct FInputAction
	{
		std::wstring					name;								/**< Name of action */
		std::vector< EButtonCode >		buttons;							/**< Buttons for trigger action */
		FOnInputActionDelegate			inputActionDelegate[ IE_Num ];		/**< Array of delegates per input event */
	};

	/**
	 * Typedef of input action map
	 */
	typedef std::unordered_map< std::wstring, FInputAction >		FInputActionMap;

	FInputActionMap				inputActionMap;					/**< Input action map */
};

#endif // !INPUTCOMPONENT_H