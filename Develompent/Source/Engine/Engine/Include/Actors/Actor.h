/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ACTOR_H
#define ACTOR_H

#include <vector>

#include "Misc/Object.h"
#include "Misc/RefCounted.h"
#include "Misc/EngineTypes.h"
#include "Math/Rect.h"
#include "Math/Color.h"
#include "Render/Material.h"
#include "Components/SceneComponent.h"

#if WITH_EDITOR
/**
 * @ingroup Engine
 * Enumeration of actor var type
 */
enum EActorVarType
{
	AVT_Unknown,	/**< Unknown type */
	AVT_Int,		/**< Int */
	AVT_Float,		/**< Float */
	AVT_Bool,		/**< Bool */
	AVT_Vector2D,	/**< Vector 2D */
	AVT_Vector3D,	/**< Vector 3D */
	AVT_Vector4D,	/**< Vector 4D */
	AVT_RectInt,	/**< Rectangle int */
	AVT_RectFloat,	/**< Rectangle float */
	AVT_Color,		/**< Color */
	AVT_String,		/**< String */
	AVT_Material	/**< Material */
};

/**
 * @ingroup Engine
 * Class for contain actor variable for initialize him when cooked map. Available only when WITH_EDITOR is 1
 */
class FActorVar
{
public:
	/**
	 * Constructor
	 */
	FActorVar();

	/**
	 * Constructor of copy
	 *
	 * @param InCopy Copy
	 */
	FActorVar( const FActorVar& InCopy );

	/**
	 * Destructor
	 */
	FORCEINLINE ~FActorVar()
	{
		Clear();
	}

	/**
	 * Clear value
	 */
	void Clear();

	/**
	 * Set name
	 *
	 * @param InName Name
	 */
	FORCEINLINE void SetName( const std::wstring& InName )
	{
		name = InName;
	}

	/**
	 * Set value int
	 *
	 * @param InValue Value
	 */
	FORCEINLINE void SetValueInt( int32 InValue )
	{
		if ( value && type != AVT_Int )
		{
			Clear();
		}

		if ( !value )
		{
			value = new int32;
		}

		*static_cast< int32* >( value ) = InValue;
		type = AVT_Int;
	}

	/**
	 * Set value float
	 *
	 * @param InValue Value
	 */
	FORCEINLINE void SetValueFloat( float InValue )
	{
		if ( value && type != AVT_Float )
		{
			Clear();
		}

		if ( !value )
		{
			value = new float;
		}

		*static_cast< float* >( value ) = InValue;
		type = AVT_Float;
	}

	/**
	 * Set value bool
	 *
	 * @param InValue Value
	 */
	FORCEINLINE void SetValueBool( bool InValue )
	{
		if ( value && type != AVT_Bool )
		{
			Clear();
		}

		if ( !value )
		{
			value = new bool;
		}

		*static_cast< bool* >( value ) = InValue;
		type = AVT_Bool;
	}

	/**
	 * Set value vector 2D
	 *
	 * @param InValue Value
	 */
	FORCEINLINE void SetValueVector2D( const FVector2D& InValue )
	{
		if ( value && type != AVT_Vector2D )
		{
			Clear();
		}

		if ( !value )
		{
			value = new FVector2D();
		}

		*static_cast< FVector2D* >( value ) = InValue;
		type = AVT_Vector2D;
	}

	/**
	 * Set value vector 3D
	 *
	 * @param InValue Value
	 */
	FORCEINLINE void SetValueVector3D( const FVector& InValue )
	{
		if ( value && type != AVT_Vector3D )
		{
			Clear();
		}

		if ( !value )
		{
			value = new FVector();
		}

		*static_cast< FVector* >( value ) = InValue;
		type = AVT_Vector3D;
	}

	/**
	 * Set value vector 4D
	 */
	FORCEINLINE void SetValueVector4D( const FVector4D& InValue )
	{
		if ( value && type != AVT_Vector4D )
		{
			Clear();
		}

		if ( !value )
		{
			value = new FVector4D();
		}

		*static_cast< FVector4D* >( value ) = InValue;
		type = AVT_Vector4D;
	}

	/**
	 * Set value rect int32
	 *
	 * @param InValue Value
	 */
	FORCEINLINE void SetValueRectInt( const FRectInt32& InValue )
	{
		if ( value && type != AVT_RectInt )
		{
			Clear();
		}

		if ( !value )
		{
			value = new FRectInt32();
		}

		*static_cast< FRectInt32* >( value ) = InValue;
		type = AVT_RectInt;
	}

	/**
	 * Set value rect float
	 *
	 * @param InValue Value
	 */
	FORCEINLINE void SetValueRectFloat( const FRectFloat& InValue )
	{
		if ( value && type != AVT_RectFloat )
		{
			Clear();
		}

		if ( !value )
		{
			value = new FRectFloat();
		}

		*static_cast< FRectFloat* >( value ) = InValue;
		type = AVT_RectFloat;
	}

	/**
	 * Set value color
	 *
	 * @param InValue Value
	 */
	FORCEINLINE void SetValueColor( const FColor& InValue )
	{
		if ( value && type != AVT_Color )
		{
			Clear();
		}

		if ( !value )
		{
			value = new FColor();
		}

		*static_cast< FColor* >( value ) = InValue;
		type = AVT_Color;
	}

	/**
	 * Set value string
	 *
	 * @param InValue Value
	 */
	FORCEINLINE void SetValueString( const std::wstring& InValue )
	{
		if ( value && type != AVT_String )
		{
			Clear();
		}

		if ( !value )
		{
			value = new std::wstring();
		}

		*static_cast< std::wstring* >( value ) = InValue;
		type = AVT_String;
	}

	/**
	 * Set value material
	 *
	 * @param InValue Value
	 */
	FORCEINLINE void SetValueMaterial( const FMaterialRef& InValue )
	{
		if ( value && type != AVT_Material )
		{
			Clear();
		}

		if ( !value )
		{
			value = new FMaterialRef();
		}

		*static_cast< FMaterialRef* >( value ) = InValue;
		type = AVT_Material;
	}

	/**
	 * Is valid
	 * @return Return true if var is valid, else return false
	 */
	FORCEINLINE bool IsValid() const
	{
		return type != AVT_Unknown && value;
	}

	/**
	 * Get name
	 * @return Return name
	 */
	FORCEINLINE const std::wstring& GetName() const
	{
		return name;
	}

	/**
	 * Get type
	 * @return Return type
	 */
	FORCEINLINE EActorVarType GetType() const
	{
		return type;
	}

	/**
	 * Get value int
	 * @return Return value int
	 */
	FORCEINLINE int32 GetValueInt() const
	{
		if ( type != AVT_Int )
		{
			return 0;
		}
		return *static_cast< int32* >( value );
	}

	/**
	 * Get value float
	 * @return Return value float
	 */
	FORCEINLINE float GetValueFloat() const
	{
		if ( type != AVT_Float )
		{
			return 0.f;
		}
		return *static_cast< float* >( value );
	}

	/**
	 * Get value number
	 * @return Return value number
	 */
	FORCEINLINE float GetValueNumber() const
	{
		if ( type != AVT_Float && type != AVT_Int )
		{
			return 0.f;
		}
		return *static_cast< float* >( value );
	}

	/**
	 * Get value bools
	 * @return Return value bool
	 */
	FORCEINLINE bool GetValueBool() const
	{
		if ( type != AVT_Bool )
		{
			return false;
		}
		return *static_cast< bool* >( value );
	}

	/**
	 * Get value vector 2D
	 * @return Return value vector 2D
	 */
	FORCEINLINE FVector2D GetValueVector2D() const
	{
		if ( type != AVT_Vector2D )
		{
			return FVector2D();
		}
		return *static_cast< FVector2D* >( value );
	}

	/**
	 * Get value vector 3D
	 * @return Return value vector 3D
	 */
	FORCEINLINE FVector GetValueVector3D() const
	{
		if ( type != AVT_Vector3D )
		{
			return FVector();
		}
		return *static_cast< FVector* >( value );
	}

	/**
	 * Get value vector 4D
	 * @return Return value vector 4D
	 */
	FORCEINLINE FVector4D GetValueVector4D() const
	{
		if ( type != AVT_Vector4D )
		{
			return FVector4D();
		}
		return *static_cast< FVector4D* >( value );
	}

	/**
	 * Get value rect int
	 * @return Return value rect int
	 */
	FORCEINLINE FRectInt32 GetValueRectInt() const
	{
		if ( type != AVT_RectInt )
		{
			return FRectInt32();
		}
		return *static_cast< FRectInt32* >( value );
	}

	/**
	 * Get value rect float
	 * @return Return value rect float
	 */
	FORCEINLINE FRectFloat GetValueRectFloat() const
	{
		if ( type != AVT_RectFloat )
		{
			return FRectFloat();
		}
		return *static_cast< FRectFloat* >( value );
	}

	/**
	 * Get value color
	 * @return Return value color
	 */
	FORCEINLINE FColor GetValueColor() const
	{
		if ( type != AVT_Color )
		{
			return FColor();
		}
		return *static_cast< FColor* >( value );
	}

	/**
	 * Get value string
	 * @return Return value string
	 */
	FORCEINLINE std::wstring GetValueString() const
	{
		if ( type != AVT_String )
		{
			return TEXT( "" );
		}
		return *static_cast< std::wstring* >( value );
	}

	/**
	 * Get value material
	 * @return Return value material
	 */
	FORCEINLINE FMaterialRef GetValueMaterial() const
	{
		if ( type != AVT_Material )
		{
			return nullptr;
		}
		return *static_cast< FMaterialRef* >( value );
	}

	/**
	 * Overload operator =
	 */
	FORCEINLINE FActorVar& operator=( const FActorVar& InRight )
	{
		name = InRight.name;
		switch ( InRight.type )
		{
		case AVT_Int:		SetValueInt( InRight.GetValueInt() );				break;
		case AVT_Float:		SetValueFloat( InRight.GetValueFloat() );			break;
		case AVT_Bool:		SetValueBool( InRight.GetValueBool() );				break;
		case AVT_Vector2D:	SetValueVector2D( InRight.GetValueVector2D() );		break;
		case AVT_Vector3D:	SetValueVector3D( InRight.GetValueVector3D() );		break;
		case AVT_Vector4D:	SetValueVector4D( InRight.GetValueVector4D() );		break;
		case AVT_RectInt:	SetValueRectInt( InRight.GetValueRectInt() );		break;
		case AVT_RectFloat:	SetValueRectFloat( InRight.GetValueRectFloat() );	break;
		case AVT_Color:		SetValueColor( InRight.GetValueColor() );			break;
		case AVT_String:	SetValueString( InRight.GetValueString() );			break;
		case AVT_Material:	SetValueMaterial( InRight.GetValueMaterial() );		break;

		default:
			type = AVT_Unknown;
			value = nullptr;
			break;
		}

		return *this;
	}

private:
	std::wstring		name;		/**< Name */
	EActorVarType		type;		/**< Type */
	void*				value;		/**< Value */
};
#endif // WITH_EDITOR

/**
 * @ingroup Engine
 * Base class of all actors in world
 */
class AActor : public LObject, public FRefCounted
{
	DECLARE_CLASS( AActor, LObject )

public:
	/**
	 * Constructor
	 */
	AActor();

	/**
	 * Destructor
	 */
	virtual ~AActor();

	/**
	 * Overridable native event for when play begins for this actor
	 */
	virtual void BeginPlay();

	/**
	 * Function called every frame on this Actor. Override this function to implement custom logic to be executed every frame.
	 * 
	 * @param[in] InDeltaTime The time since the last tick.
	 */
	virtual void Tick( float InDeltaTime );

	/**
	 * @brief Serialize actor
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class FArchive& InArchive );

	/**
	 * @brief Init physics body
	 */
	void InitPhysics();

	/**
	 * @brief Terminate physics body
	 */
	void TermPhysics();

	/**
	 * @brief Sync actor to physics body
	 */
	void SyncPhysics();

#if WITH_EDITOR
	/**
	 * @brief Initialize actor properties
	 * This method called only when actor spawned on cooking of map. Available only when WITH_EDITOR is 1
	 * 
	 * @param InActorVars Array of actor properties to init
	 * @param InCooker Pointer to cooker for cook any resources if need
	 * @return Return if properties inited succeed and all resources cooked is succeed, else return false
	 */
	virtual bool InitProperties( const std::vector< FActorVar >& InActorVars, class LCookPackagesCommandlet* InCooker );
#endif // WITH_EDITOR

	/**
	 * Get array of owned components
	 * @return Return array owned components
	 */
	FORCEINLINE const std::vector< LActorComponentRef >& GetComponents() const
	{
		return ownedComponents;
	}

	/**
	 * Adds a delta to the location of this actor in world space
	 * 
	 * @param[in] InDeltaLocation Delta location
	 */
	FORCEINLINE void AddActorLocation( const FVector& InDeltaLocation )
	{
		if ( !rootComponent )	return;
		rootComponent->AddRelativeLocation( InDeltaLocation );
	}

	/**
	 * Adds a delta to the rotation of this actor in world space
	 * 
	 * @param[in] InDeltaRotation Delta rotation
	 */
	FORCEINLINE void AddActorRotation( const FRotator& InDeltaRotation )
	{
		if ( !rootComponent )	return;
		rootComponent->AddRelativeRotate( InDeltaRotation );
	}

	/**
	 * Adds a delta to the scale of this actor in world space
	 * 
	 * @param[in] InDeltaScale Delta scale
	 */
	FORCEINLINE void AddActorScale( const FVector& InDeltaScale )
	{
		if ( !rootComponent )	return;
		rootComponent->AddRelativeScale( InDeltaScale );
	}

	/**
	 * Set actor location in world space
	 * 
	 * @param[in] InNewLocation New actor location
	 */
	FORCEINLINE void SetActorLocation( const FVector& InNewLocation )
	{
		if ( !rootComponent )	return;
		rootComponent->SetRelativeLocation( InNewLocation );
	}

	/**
	 * Set actor rotation in world space
	 * 
	 * @param[in] InNewRotation New actor rotation
	 */
	FORCEINLINE void SetActorRotation( const FRotator& InNewRotation )
	{
		if ( !rootComponent )	return;
		rootComponent->SetRelativeRotation( InNewRotation );
	}

	/**
	 * Set actor scale in world space
	 * 
	 * @param[in] InNewScale New actor scale
	 */
	FORCEINLINE void SetActorScale( const FVector& InNewScale )
	{
		if ( !rootComponent )	return;
		rootComponent->SetRelativeScale( InNewScale );
	}

	/**
	 * Set static actor
	 * @param InIsStatic Is actor static
	 */
	FORCEINLINE void SetStatic( bool InIsStatic )
	{
		bIsStatic = InIsStatic;
	}

	/**
	 * Get actor location in world space
	 * @return Return actor location, if root component is not valid return zero vector
	 */
	FORCEINLINE FVector GetActorLocation() const
	{
		return rootComponent ? rootComponent->GetComponentLocation() : FMath::vectorZero;
	}

	/**
	 * Get actor rotation in world space
	 * @return Return actor rotation, if root component is not valid return zero rotator
	 */
	FORCEINLINE FRotator GetActorRotation() const
	{
		return rootComponent ? rootComponent->GetComponentRotation() : FMath::rotatorZero;
	}

	/**
	 * Get actor scale in world space
	 * @return Return actor scale, if root component is not valid return one vector
	 */
	FORCEINLINE FVector GetActorScale() const
	{
		return rootComponent ? rootComponent->GetComponentScale() : FMath::vectorOne;
	}

	/**
	 * Get actor transform in world space
	 * @return Return actor transform, if root component is not valid return zero transform
	 */
	FORCEINLINE FTransform GetActorTransform() const
	{
		return rootComponent ? rootComponent->GetComponentTransform() : FMath::transformZero;
	}

	/**
	 * Get actor forward vector
	 * @return Return actor forward vector
	 */
	FORCEINLINE FVector GetActorForwardVector() const
	{
		return rootComponent ? rootComponent->GetForwardVector() : FMath::vectorForward;
	}

	/**
	 * Get actor right vector
	 * @return Return actor right vector
	 */
	FORCEINLINE FVector GetActorRightVector() const
	{
		return rootComponent ? rootComponent->GetRightVector() : FMath::vectorRight;
	}

	/**
	 * Get actor up vector
	 * @return Return actor up vector
	 */
	FORCEINLINE FVector GetActorUpVector() const
	{
		return rootComponent ? rootComponent->GetUpVector() : FMath::vectorUp;
	}

	/**
	 * Is static actor
	 * @return Return true if actor is static, else return false
	 */
	FORCEINLINE bool IsStatic() const
	{
		return bIsStatic;
	}

protected:
	/**
	 * Create component and add to array of owned components
	 * If creating LSceneComponent, if RootComponent is nullptr, automatic new component setted to root
	 *
	 * @param[in] InClass Class component
	 * @param[in] InName Name component
	 * @return Return pointer to component
	 */
	LActorComponentRef CreateComponent( LClass* InClass, const tchar* InName );

	/**
	 * Create component and add to array of owned components
	 *
	 * @param[in] InName Name component
	 * @return Return pointer to component
	 */
	template< typename TClass >
	FORCEINLINE TRefCountPtr< TClass > CreateComponent( const tchar* InName )
	{
		LActorComponent*		newComponent = CreateComponent( TClass::StaticClass(), InName );
		return ( TClass* )newComponent;
	}

	/**
	 * Puts a component in to the OwnedComponents array of the Actor
	 * @param[in] InComponent Component
	 */
	void AddOwnedComponent( class LActorComponent* InComponent );

	/**
	 * Removes a component from the OwnedComponents array of the Actor
	 * @param[in] InComponent Component
	 */
	void RemoveOwnedComponent( class LActorComponent* InComponent );

	/**
	 * Clear array owned components
	 */
	void ResetOwnedComponents();

	TRefCountPtr< LSceneComponent >				rootComponent;			/**< Root component, default is null */

private:
	bool										bIsStatic;				/**< Is static actor */
	std::vector< LActorComponentRef >			ownedComponents;		/**< Owned components */
};

#endif // !ACTOR_H