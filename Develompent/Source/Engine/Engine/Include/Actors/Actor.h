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

#include "Math/Rect.h"
#include "Math/Color.h"
#include "System/Delegate.h"
#include "Reflection/ReflectionEnvironment.h"
#include "Reflection/Property.h"
#include "Render/Material.h"
#include "Render/HitProxies.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"

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
class CActorVar
{
public:
	/**
	 * Constructor
	 */
	CActorVar();

	/**
	 * Constructor of copy
	 *
	 * @param InCopy Copy
	 */
	CActorVar( const CActorVar& InCopy );

	/**
	 * Destructor
	 */
	FORCEINLINE ~CActorVar()
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
	FORCEINLINE void SetValueVector2D( const Vector2D& InValue )
	{
		if ( value && type != AVT_Vector2D )
		{
			Clear();
		}

		if ( !value )
		{
			value = new Vector2D();
		}

		*static_cast< Vector2D* >( value ) = InValue;
		type = AVT_Vector2D;
	}

	/**
	 * Set value vector 3D
	 *
	 * @param InValue Value
	 */
	FORCEINLINE void SetValueVector3D( const Vector& InValue )
	{
		if ( value && type != AVT_Vector3D )
		{
			Clear();
		}

		if ( !value )
		{
			value = new Vector();
		}

		*static_cast< Vector* >( value ) = InValue;
		type = AVT_Vector3D;
	}

	/**
	 * Set value vector 4D
	 */
	FORCEINLINE void SetValueVector4D( const Vector4D& InValue )
	{
		if ( value && type != AVT_Vector4D )
		{
			Clear();
		}

		if ( !value )
		{
			value = new Vector4D();
		}

		*static_cast< Vector4D* >( value ) = InValue;
		type = AVT_Vector4D;
	}

	/**
	 * Set value rect int32
	 *
	 * @param InValue Value
	 */
	FORCEINLINE void SetValueRectInt( const RectInt32_t& InValue )
	{
		if ( value && type != AVT_RectInt )
		{
			Clear();
		}

		if ( !value )
		{
			value = new RectInt32_t();
		}

		*static_cast< RectInt32_t* >( value ) = InValue;
		type = AVT_RectInt;
	}

	/**
	 * Set value rect float
	 *
	 * @param InValue Value
	 */
	FORCEINLINE void SetValueRectFloat( const RectFloat_t& InValue )
	{
		if ( value && type != AVT_RectFloat )
		{
			Clear();
		}

		if ( !value )
		{
			value = new RectFloat_t();
		}

		*static_cast< RectFloat_t* >( value ) = InValue;
		type = AVT_RectFloat;
	}

	/**
	 * Set value color
	 *
	 * @param InValue Value
	 */
	FORCEINLINE void SetValueColor( const CColor& InValue )
	{
		if ( value && type != AVT_Color )
		{
			Clear();
		}

		if ( !value )
		{
			value = new CColor();
		}

		*static_cast< CColor* >( value ) = InValue;
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
	FORCEINLINE void SetValueMaterial( const TAssetHandle<CMaterial>& InValue )
	{
		if ( value && type != AVT_Material )
		{
			Clear();
		}

		if ( !value )
		{
			value = new TAssetHandle<CMaterial>();
		}

		*static_cast< TAssetHandle<CMaterial>* >( value ) = InValue;
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
	FORCEINLINE Vector2D GetValueVector2D() const
	{
		if ( type != AVT_Vector2D )
		{
			return Vector2D();
		}
		return *static_cast< Vector2D* >( value );
	}

	/**
	 * Get value vector 3D
	 * @return Return value vector 3D
	 */
	FORCEINLINE Vector GetValueVector3D() const
	{
		if ( type != AVT_Vector3D )
		{
			return Vector();
		}
		return *static_cast< Vector* >( value );
	}

	/**
	 * Get value vector 4D
	 * @return Return value vector 4D
	 */
	FORCEINLINE Vector4D GetValueVector4D() const
	{
		if ( type != AVT_Vector4D )
		{
			return Vector4D();
		}
		return *static_cast< Vector4D* >( value );
	}

	/**
	 * Get value rect int
	 * @return Return value rect int
	 */
	FORCEINLINE RectInt32_t GetValueRectInt() const
	{
		if ( type != AVT_RectInt )
		{
			return RectInt32_t();
		}
		return *static_cast< RectInt32_t* >( value );
	}

	/**
	 * Get value rect float
	 * @return Return value rect float
	 */
	FORCEINLINE RectFloat_t GetValueRectFloat() const
	{
		if ( type != AVT_RectFloat )
		{
			return RectFloat_t();
		}
		return *static_cast< RectFloat_t* >( value );
	}

	/**
	 * Get value color
	 * @return Return value color
	 */
	FORCEINLINE CColor GetValueColor() const
	{
		if ( type != AVT_Color )
		{
			return CColor();
		}
		return *static_cast< CColor* >( value );
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
	FORCEINLINE TAssetHandle<CMaterial> GetValueMaterial() const
	{
		if ( type != AVT_Material )
		{
			return nullptr;
		}
		return *static_cast< TAssetHandle<CMaterial>* >( value );
	}

	/**
	 * Overload operator =
	 */
	FORCEINLINE CActorVar& operator=( const CActorVar& InRight )
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
 * @brief Delegate for called event when actor is destroyed
 */
DECLARE_MULTICAST_DELEGATE( COnActorDestroyed, class AActor* );

/**
 * @ingroup Engine
 * @brief Base class of all actors in world
 */
class AActor : public CObject
{
	DECLARE_CLASS( AActor, CObject, 0, 0 )

public:
	/**
	 * @brief Constructor
	 */
	AActor();

	/**
	 * @brief Called before destroying the object
	 * This is called immediately upon deciding to destroy the object, to allow the object to begin an
	 * asynchronous cleanup process
	 */
	virtual void BeginDestroy() override;

	/**
	 * @brief Overridable native event for when play begins for this actor
	 */
	virtual void BeginPlay();

	/**
	 * @brief Overridable native event for when end play for this actor
	 */
	virtual void EndPlay();

	/**
	 * @brief Function called every frame on this Actor. Override this function to implement custom logic to be executed every frame.
	 * 
	 * @param InDeltaTime	The time since the last tick.
	 */
	virtual void Tick( float InDeltaTime );

	/**
	 * @brief Serialize actor
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive );
	
	/**
	 * @brief Called when this actor is spawned
	 */
	virtual void Spawned();

	/**
	 * @brief Called when this actor is destroyed
	 */
	virtual void Destroyed();

	/**
	 * @brief Destroy this actor. Returns TRUE the actor is destroyed or already marked for destruction, FALSE if indestructible
	 * Destruction is latent. It occurs at the end of the tick
	 * @returns	Return TRUE if destroyed or already marked for destruction, FALSE if indestructible
	 */
	bool Destroy();

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
	virtual bool InitProperties( const std::vector< CActorVar >& InActorVars, class CCookPackagesCommandlet* InCooker );

	/**
	 * @brief Get path to icon of actor for exploer level in WorldEd
	 * @return Return path to actor icon from Sys_BaseDir()
	 */
	virtual std::wstring GetActorIcon() const;
#endif // WITH_EDITOR

	/**
	 * @brief Get array of owned components
	 * @return Return array owned components
	 */
	FORCEINLINE const std::vector<CActorComponent*>& GetComponents() const
	{
		return ownedComponents;
	}

	/**
	 * @brief Adds a delta to the location of this actor in world space
	 * 
	 * @param InDeltaLocation	Delta location
	 */
	FORCEINLINE void AddActorLocation( const Vector& InDeltaLocation )
	{
		if ( !rootComponent )	return;
		rootComponent->AddRelativeLocation( InDeltaLocation );
	}

	/**
	 * @brief Adds a delta to the rotation of this actor in world space
	 * 
	 * @param InDeltaRotation	Delta rotation
	 */
	FORCEINLINE void AddActorRotation( const Quaternion& InDeltaRotation )
	{
		if ( !rootComponent )	return;
		rootComponent->AddRelativeRotation( InDeltaRotation );
	}

	/**
	 * @brief Adds a delta to the rotation of this actor in world space
	 *
	 * @param InDeltaRotation	Delta rotation
	 */
	FORCEINLINE void AddActorRotation( const CRotator& InDeltaRotation )
	{
		if ( !rootComponent )	return;
		rootComponent->AddRelativeRotation( InDeltaRotation );
	}

	/**
	 * @brief Adds a delta to the scale of this actor in world space
	 * 
	 * @param InDeltaScale	Delta scale
	 */
	FORCEINLINE void AddActorScale( const Vector& InDeltaScale )
	{
		if ( !rootComponent )	return;
		rootComponent->AddRelativeScale( InDeltaScale );
	}

	/**
	 * @brief Set actor location in world space
	 * 
	 * @param InNewLocation		New actor location
	 */
	FORCEINLINE void SetActorLocation( const Vector& InNewLocation )
	{
		if ( !rootComponent )	return;
		rootComponent->SetRelativeLocation( InNewLocation );
	}

	/**
	 * @brief Set actor rotation in world space
	 * 
	 * @param InNewRotation		New actor rotation
	 */
	FORCEINLINE void SetActorRotation( const Quaternion& InNewRotation )
	{
		if ( !rootComponent )	return;
		rootComponent->SetRelativeRotation( InNewRotation );
	}

	/**
	 * @brief Set actor rotation in world space
	 *
	 * @param InNewRotation		New actor rotation
	 */
	FORCEINLINE void SetActorRotation( const CRotator& InNewRotation )
	{
		if ( !rootComponent )	return;
		rootComponent->SetRelativeRotation( InNewRotation );
	}

	/**
	 * @brief Set actor scale in world space
	 * 
	 * @param InNewScale	New actor scale
	 */
	FORCEINLINE void SetActorScale( const Vector& InNewScale )
	{
		if ( !rootComponent )	return;
		rootComponent->SetRelativeScale( InNewScale );
	}

	/**
	 * @brief Set static actor
	 * @param InIsStatic Is actor static
	 */
	FORCEINLINE void SetStatic( bool InIsStatic )
	{
		bNeedReinitCollision = InIsStatic != bIsStatic ? true : bNeedReinitCollision;
		bIsStatic = InIsStatic;
	}

#if ENABLE_HITPROXY
	/**
	 * @brief Set hit proxy id
	 * @param InIndex	Index
	 */
	FORCEINLINE void SetHitProxyId( uint32 InIndex )
	{
		hitProxyId.SetIndex( InIndex );
	}

	/**
	 * @brief Get hit proxy id
	 * @return Return hit proxy id
	 */
	FORCEINLINE const CHitProxyId& GetHitProxyId() const
	{
		return hitProxyId;
	}
#endif // ENABLE_HITPROXY

#if WITH_EDITOR
	/**
	 * @brief Set selected actor
	 * @param InIsSelected	Is selected this actor
	 */
	FORCEINLINE void SetSelected( bool InIsSelected )
	{
		bSelected = InIsSelected;
	}

	/**
	 * @brief Is selected actor
	 * @return Return TRUE if actor is selected, else returning FALSE
	 */
	FORCEINLINE bool IsSelected() const
	{
		return bSelected;
	}
#endif // WITH_EDITOR

	/**
	 * @brief Set visibility
	 *
	 * @param InNewVisibility New visibility
	 */
	FORCEINLINE void SetVisibility( bool InNewVisibility )
	{
		bVisibility = InNewVisibility;
	}

	/**
	 * @brief Get visibility
	 * @return Return visibility of actor
	 */
	FORCEINLINE bool IsVisibility() const
	{
		return bVisibility;
	}

	/**
	 * @brief Get actor location in world space
	 * @return Return actor location, if root component is not valid return zero vector
	 */
	FORCEINLINE Vector GetActorLocation() const
	{
		return rootComponent ? rootComponent->GetComponentLocation() : Math::vectorZero;
	}

	/**
	 * @brief Get actor rotation in world space
	 * @return Return actor rotation, if root component is not valid return zero rotator
	 */
	FORCEINLINE CRotator GetActorRotation() const
	{
		return rootComponent ? rootComponent->GetComponentRotation() : Math::quaternionZero;
	}

	/**
	 * @brief Get actor's quaternion rotation in world space
	 * @return Return actor's quaternion rotation, if root component is not valid return zero quaternion
	 */
	FORCEINLINE Quaternion GetActorQuat() const
	{
		return rootComponent ? rootComponent->GetComponentQuat() : Math::quaternionZero;
	}

	/**
	 * @brief Get actor scale in world space
	 * @return Return actor scale, if root component is not valid return one vector
	 */
	FORCEINLINE Vector GetActorScale() const
	{
		return rootComponent ? rootComponent->GetComponentScale() : Math::vectorOne;
	}

	/**
	 * @brief Get actor transform in world space
	 * @return Return actor transform, if root component is not valid return zero transform
	 */
	FORCEINLINE CTransform GetActorTransform() const
	{
		return rootComponent ? rootComponent->GetComponentTransform() : Math::transformZero;
	}

	/**
	 * @brief Get actor forward vector
	 * @return Return actor forward vector
	 */
	FORCEINLINE Vector GetActorForwardVector() const
	{
		if ( !rootComponent )
		{
			return Math::vectorForward;
		}
		else
		{
			const CTransform&	componentTransform = rootComponent->GetComponentTransform();
			return componentTransform.GetUnitAxis( A_Forward );
		}
	}

	/**
	 * @brief Get actor right vector
	 * @return Return actor right vector
	 */
	FORCEINLINE Vector GetActorRightVector() const
	{
		if ( !rootComponent )
		{
			return Math::vectorForward;
		}
		else
		{
			const CTransform&	componentTransform = rootComponent->GetComponentTransform();
			return componentTransform.GetUnitAxis( A_Right );
		}
	}

	/**
	 * @brief Get actor up vector
	 * @return Return actor up vector
	 */
	FORCEINLINE Vector GetActorUpVector() const
	{
		if ( !rootComponent )
		{
			return Math::vectorForward;
		}
		else
		{
			const CTransform&	componentTransform = rootComponent->GetComponentTransform();
			return componentTransform.GetUnitAxis( A_Up );
		}
	}

	/**
	 * @brief Get collision component
	 * @return Return collision component. If not exist return nullptr
	 */
	FORCEINLINE CPrimitiveComponent* GetCollisionComponent() const
	{
		return collisionComponent;
	}

	/**
	 * @brief Is static actor
	 * @return Return true if actor is static, else return false
	 */
	FORCEINLINE bool IsStatic() const
	{
		return bIsStatic;
	}

	/**
	 * @brief Is actor playing
	 * @return Return TRUE if actor is playing, else returning FALSE
	 */
	FORCEINLINE bool IsPlaying() const
	{
		return bBeginPlay;
	}

protected:
	/**
	 * @brief Create component and add to array of owned components
	 * If creating CSceneComponent, if RootComponent is nullptr, automatic new component setted to root
	 *
	 * @param InClass		Class component
	 * @param InName		Name component
	 * @param InEditorOnly	Is editor only component
	 * @return Return pointer to component
	 */
	CActorComponent* CreateComponent( CClass* InClass, const CName& InName, bool InEditorOnly = false );

	/**
	 * @brief Create component and add to array of owned components
	 *
	 * @param InName	Name component
	 * @return Return pointer to component
	 */
	template<typename TClass>
	FORCEINLINE TClass* CreateComponent( const CName& InName, bool InEditorOnly = false )
	{
		CActorComponent*	newComponent = CreateComponent( TClass::StaticClass(), InName, InEditorOnly );
		return ( TClass* )newComponent;
	}

	/**
	 * @brief Puts a component in to the OwnedComponents array of the Actor
	 * @param InComponent	Component
	 */
	void AddOwnedComponent( class CActorComponent* InComponent );

	/**
	 * @brief Removes a component from the OwnedComponents array of the Actor
	 * @param InComponent	Component
	 */
	void RemoveOwnedComponent( class CActorComponent* InComponent );

	/**
	 * @brief Clear array owned components
	 */
	void ResetOwnedComponents();

	/**
	 * @brief Get event when actor is destroyed
	 * @return Return event when actor is destroyed
	 */
	FORCEINLINE COnActorDestroyed& OnActorDestroyed() const
	{
		return onActorDestroyed;
	}

	/**
	 * @brief Get world where this actor is
	 * @return Return a pointer to world where this actor is
	 */
	FORCEINLINE CWorld* GetWorld() const
	{
		return worldPrivate ? worldPrivate : GetWorld_Uncached();
	}

	CSceneComponent*							rootComponent;			/**< Root component, default is null */
	CPrimitiveComponent*						collisionComponent;		/**< Collision component */

private:
	/**
	 * @brief Get world where this actor is
	 * This function use for cache pointer to world
	 * 
	 * @return Return a pointer to world where this actor is
	 */
	CWorld* GetWorld_Uncached() const;

	bool										bIsStatic;				/**< Is static actor */
	bool										bNeedReinitCollision;	/**< Is need reinit collision component */
	bool										bActorIsBeingDestroyed;	/**< Actor is being destroyed */
	bool										bBeginPlay;				/**< Is begin play for this actor */
	bool										bVisibility;			/**< Is actor visibility */

#if WITH_EDITOR
	bool										bSelected;				/**< Is selected this actor */
#endif // WITH_EDITOR

	class CWorld*								worldPrivate;			/**< Pointer to world where this actor is */
	std::vector<CActorComponent*>				ownedComponents;		/**< Owned components */
	mutable COnActorDestroyed					onActorDestroyed;		/**< Called event when actor is destroyed */

#if ENABLE_HITPROXY
	CHitProxyId									hitProxyId;				/**< Hit proxy id */
#endif // ENABLE_HITPROXY
};

#endif // !ACTOR_H