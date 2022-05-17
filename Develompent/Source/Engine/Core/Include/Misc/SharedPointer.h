/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SHAREDPOINTER_H
#define SHAREDPOINTER_H

#include "Misc/Types.h"
#include "System/ThreadingBase.h"
#include "Core.h"

/**
 * @ingroup Core
 * @brief SharedPointerInternals contains internal workings of shared and weak pointers. You should
 * hopefully never have to use anything inside this namespace directly.
 */
namespace SharedPointerInternals
{
	// Forward declarations
	template< class ObjectType > class TWeakReferencer;

	/**
	 * @brief Reference controller
	 */
	template< class ObjectType >
	class TReferenceController
	{
	public:
		/**
		 * @brief Constructor
		 * @param InObject	Object
		 */
		FORCEINLINE explicit TReferenceController( ObjectType* InObject )
			: sharedReferenceCount( 1 )
			, weakReferenceCount( 1 )
			, object( InObject )
		{}
		
		/**
		 * @brief Destructor
		 */
		FORCEINLINE ~TReferenceController()
		{
			check( !object && !sharedReferenceCount && !weakReferenceCount );
		}

		/**
		 * @brief Get count of shared references
		 * @return Return count of shared references
		 */
		FORCEINLINE uint32 GetSharedReferenceCount() const
		{
			return sharedReferenceCount;
		}

		/**
		 * @brief Adds a shared reference to this counter
		 */
		FORCEINLINE void AddSharedReference()
		{
			appInterlockedIncrement( ( int32* )&sharedReferenceCount );
		}

		/**
		 * @brief Release shared reference
		 */
		FORCEINLINE void ReleaseSharedReference()
		{
			if ( !sharedReferenceCount || !appInterlockedDecrement( ( int32* )&sharedReferenceCount ) )
			{
				// Last shared reference was released!  Destroy the referenced object.
				DestroyObject();

				// No more shared referencers, so decrement the weak reference count by one.  When the weak
				// reference count reaches zero, this object will be deleted.
				ReleaseWeakReference();
			}
		}

		/**
		 * @brief Adds a weak reference to this counter
		 */
		FORCEINLINE void AddWeakReference()
		{
			appInterlockedIncrement( ( int32* )&weakReferenceCount );
		}

		/**
		 * @brief Adds a shared reference to this counter ONLY if there is already at least one reference
		 * @return Return TRUE if the shared reference was added successfully
		 */
		FORCEINLINE bool ConditionallyAddSharedReference()
		{
			if ( sharedReferenceCount == 0 )
			{
				// Never add a shared reference if the pointer has already expired
				return false;
			}

			appInterlockedIncrement( ( int32* )&sharedReferenceCount );
			return true;
		}

		/**
		 * @brief Releases a weak reference to this counter
		 */
		FORCEINLINE void ReleaseWeakReference()
		{
			if ( !weakReferenceCount || !appInterlockedDecrement( ( int32* )&weakReferenceCount ) )
			{
				delete this;
			}
		}

		/**
		 * @brief Get object
		 * @return Return object
		 */
		FORCEINLINE ObjectType* GetObject() const
		{
			return object;
		}

		// Non-copyable
		TReferenceController( const TReferenceController& )				= delete;
		TReferenceController& operator=( const TReferenceController& )	= delete;

	private:
		/**
		 * @brief Destroy object
		 */
		FORCEINLINE void DestroyObject()
		{
			if ( object )
			{
				delete object;
				object = nullptr;
			}
		}

		uint32		sharedReferenceCount;	/**< Number of shared references to this object */
		uint32		weakReferenceCount;		/**< Number of weak references to this object */
		ObjectType* object;					/**< Object */
	};

	/**
	 * @brief FSharedReferencer is a wrapper around a pointer to a reference controller that is used by either a
	 * TSharedPtr to keep track of a referenced object's lifetime
	 */
	template< class ObjectType >
	class TSharedReferencer
	{
	public:
		friend TWeakReferencer<ObjectType>;

		/**
		 * @brief Constructor for an empty shared referencer object
		 */
		FORCEINLINE TSharedReferencer()
			: referenceController( nullptr )
		{}

		/**
		 * @brief Constructor that counts a single reference to the specified object
		 * @param InReferenceController		Reference controller
		 */
		FORCEINLINE explicit TSharedReferencer( TReferenceController<ObjectType>* InReferenceController )
			: referenceController( InReferenceController )
		{}

		/**
		 * @brief Copy constructor creates a new reference to the existing object
		 * @param InSharedReference		Shared reference
		 */
		FORCEINLINE TSharedReferencer( const TSharedReferencer<ObjectType>& InSharedReference )
			: referenceController( InSharedReference.referenceController )
		{
			// If the incoming reference had an object associated with it, then go ahead and increment the
			// shared reference count
			if ( referenceController != nullptr )
			{
				referenceController->AddSharedReference();
			}
		}

		/**
		 * @brief Move constructor creates no new references
		 * @param InSharedReference		Shared reference
		 */
		FORCEINLINE TSharedReferencer( TSharedReferencer&& InSharedReference )
			: referenceController( InSharedReference.referenceController )
		{
			InSharedReference.referenceController = nullptr;
		}

		/**
		 * @brief Creates a shared referencer object from a weak referencer object. This will only result
		 * in a valid object reference if the object already has at least one other shared referencer
		 * 
		 * @param InWeakReference	Weak reference
		 */
		FORCEINLINE TSharedReferencer( const TWeakReferencer<ObjectType>& InWeakReference )
			: referenceController( InWeakReference.referenceController )
		{
			// If the incoming reference had an object associated with it, then go ahead and increment the
			// shared reference count
			if ( referenceController != nullptr )
			{
				// Attempt to elevate a weak reference to a shared one.  For this to work, the object this
				// weak counter is associated with must already have at least one shared reference.  We'll
				// never revive a pointer that has already expired!
				if ( !referenceController->ConditionallyAddSharedReference() )
				{
					referenceController = nullptr;
				}
			}
		}

		/**
		 * @brief Creates a shared referencer object from a weak referencer object. This will only result
		 * in a valid object reference if the object already has at least one other shared referencer
		 * 
		 * @param InWeakReference	Weak reference
		 */
		FORCEINLINE TSharedReferencer( TWeakReferencer<ObjectType>&& InWeakReference )
			: referenceController( InWeakReference.referenceController )
		{
			// If the incoming reference had an object associated with it, then go ahead and increment the
			// shared reference count
			if ( referenceController != nullptr )
			{
				// Attempt to elevate a weak reference to a shared one.  For this to work, the object this
				// weak counter is associated with must already have at least one shared reference.  We'll
				// never revive a pointer that has already expired!
				if ( !referenceController->ConditionallyAddSharedReference() )
				{
					referenceController = nullptr;
				}

				// Tell the reference counter object that we're no longer referencing the object with
				// this weak pointer
				InWeakReference.referenceController->ReleaseWeakReference();
				InWeakReference.referenceController = nullptr;
			}
		}

		/**
		 * @brief Destructor
		 */
		FORCEINLINE ~TSharedReferencer()
		{
			if ( referenceController != nullptr )
			{
				// Tell the reference counter object that we're no longer referencing the object with
				// this shared pointer
				referenceController->ReleaseSharedReference();
			}
		}

		/**
		 * @brief Override operator =
		 * @param InSharedReference		Shared reference
		 */
		FORCEINLINE TSharedReferencer& operator=( const TSharedReferencer<ObjectType>& InSharedReference )
		{
			*this = InSharedReference.referenceController;
			return *this;
		}

		/**
		 * @brief Move assignment operator adds no references to the assigned object.  If this counter was previously
		 * referencing an object, that reference will be released
		 * 
		 * @param InSharedReference		Shared reference
		 */
		FORCEINLINE TSharedReferencer& operator=( TSharedReferencer&& InSharedReference )
		{
			// Make sure we're not be reassigned to ourself!
			auto	newReferenceController = InSharedReference.referenceController;
			auto	oldReferenceController = referenceController;
			if ( newReferenceController != oldReferenceController )
			{
				// Assume ownership of the assigned reference counter
				InSharedReference.referenceController = nullptr;
				referenceController = newReferenceController;

				// Release shared reference to the old object
				if ( oldReferenceController != nullptr )
				{
					oldReferenceController->ReleaseSharedReference();
				}
			}

			return *this;
		}

		/**
		 * @brief Override operator =
		 * @param InReferenceController		Reference controller	
		 */
		FORCEINLINE TSharedReferencer& operator=( TReferenceController<ObjectType>* InReferenceController )
		{
			// Make sure we're not be reassigned to ourself!
			if ( InReferenceController != referenceController )
			{
				// First, add a shared reference to the new object
				if ( InReferenceController != nullptr )
				{
					InReferenceController->AddSharedReference();
				}

				// Release shared reference to the old object
				if ( referenceController != nullptr )
				{
					referenceController->ReleaseSharedReference();
				}

				// Assume ownership of the assigned reference counter
				referenceController = InReferenceController;
			}

			return *this;
		}

		/**
		 * @brief Get type hash
		 * @return Return hash of type
		 */
		FORCEINLINE uint64 GetTypeHash() const
		{
			return ( uint64 )referenceController;
		}

		/**
		 * @brief Tests to see whether or not this shared counter contains a valid reference
		 * @return Return TRUE if reference is valid
		 */
		FORCEINLINE bool IsValid() const
		{
			return referenceController != nullptr && referenceController->GetSharedReferenceCount() > 0;
		}

		/**
		 * @brief Returns the number of shared references to this object
		 * @return Number of shared references to the object
		 */
		FORCEINLINE uint32 GetSharedReferenceCount() const
		{
			return referenceController != nullptr ? referenceController->GetSharedReferenceCount() : 0;
		}

		/**
		 * @brief Is unique shared reference
		 * @return Return TRUE if there is only one shared reference to the object, and this is it!
		 */
		FORCEINLINE bool IsUnique() const
		{
			return GetSharedReferenceCount() == 0;
		}

		/**
		 * @brief Get object
		 * @return Return object
		 */
		FORCEINLINE ObjectType* GetObject() const
		{
			return IsValid() ? referenceController->GetObject() : nullptr;
		}

	private:
		TReferenceController<ObjectType>*		referenceController;	/**< Pointer to the reference controller for the object */
	};

	/**
	 * @brief TWeakReferencer is a wrapper around a pointer to a reference controller that is used
	 * by a TWeakPtr to keep track of a referenced object's lifetime
	 */
	template< class ObjectType >
	class TWeakReferencer
	{
	public:
		friend TSharedReferencer<ObjectType>;

		/**
		 * @brief Get type hash
		 * @return Return hash of type
		 */
		FORCEINLINE uint64 GetTypeHash() const
		{
			return ( uint64 )referenceController;
		}

		/**
		 * @brief Default constructor with empty counter
		 */
		FORCEINLINE TWeakReferencer()
			: referenceController( nullptr )
		{}

		/**
		 * @brief Construct a weak referencer object from another weak referencer
		 * @param InWeakRefCountPointer		Weak referencer
		 */
		FORCEINLINE TWeakReferencer( const TWeakReferencer& InWeakRefCountPointer )
			: referenceController( InWeakRefCountPointer.referenceController )
		{
			// If the weak referencer has a valid controller, then go ahead and add a weak reference to it!
			if ( referenceController != nullptr )
			{
				referenceController->AddWeakReference();
			}
		}

		/**
		 * @brief Construct a weak referencer object from a shared referencer object
		 * @param InSharedRefCountPointer		Shared referencer
		 */
		FORCEINLINE TWeakReferencer( const TSharedReferencer<ObjectType>& InSharedRefCountPointer )
			: referenceController( InSharedRefCountPointer.referenceController )
		{
			// If the shared referencer had a valid controller, then go ahead and add a weak reference to it!
			if ( referenceController != nullptr )
			{
				referenceController->AddWeakReference();
			}
		}

		/**
		 * @brief Construct a weak referencer object from an rvalue weak referencer
		 * @param InWeakRefCountPointer		Weak referencer
		 */
		FORCEINLINE TWeakReferencer( TWeakReferencer&& InWeakRefCountPointer )
			: referenceController( InWeakRefCountPointer.referenceController )
		{
			InWeakRefCountPointer.referenceController = nullptr;
		}

		/**
		 * @brief Destructor 
		 */
		FORCEINLINE ~TWeakReferencer()
		{
			if ( referenceController != nullptr )
			{
				// Tell the reference counter object that we're no longer referencing the object with
				// this weak pointer
				referenceController->ReleaseWeakReference();
			}
		}

		/**
		 * @brief Override operator =
		 * @param InWeakReference	Weak reference
		 */
		FORCEINLINE TWeakReferencer& operator=( const TWeakReferencer& InWeakReference )
		{
			AssignReferenceController( InWeakReference.referenceController );
			return *this;
		}

		/**
		 * @brief Override operator =
		 * @param InSharedReference		Shared reference
		 */
		FORCEINLINE TWeakReferencer& operator=( const TSharedReferencer<ObjectType>& InSharedReference )
		{
			AssignReferenceController( InSharedReference.referenceController );
			return *this;
		}

		/**
		 * @brief Assignment operator from an rvalue weak referencer object.  If this counter was previously referencing an
		 * object, that reference will be released
		 * 
		 * @param InWeakReference	Weak reference
		 */
		FORCEINLINE TWeakReferencer& operator=( TWeakReferencer&& InWeakReference )
		{
			auto	oldReferenceController = referenceController;
			referenceController				= InWeakReference.referenceController;
			
			InWeakReference.referenceController = nullptr;
			if ( oldReferenceController != nullptr )
			{
				oldReferenceController->ReleaseWeakReference();
			}

			return *this;
		}

		/**
		 * @brief Tests to see whether or not this weak counter contains a valid reference
		 * @return Return TRUE if reference is valid
		 */
		FORCEINLINE bool IsValid() const
		{
			return referenceController != nullptr && referenceController->GetSharedReferenceCount() > 0;
		}

		/**
		 * @brief Get object
		 * @return Return object
		 */
		FORCEINLINE ObjectType* GetObject() const
		{
			return IsValid() ? referenceController->GetObject() : nullptr;
		}

	private:
		/**
		 * @brief Assigns a new reference controller to this counter object, first adding a reference to it, then
		 * releasing the previous object
		 * 
		 * @param InNewReferenceController		New reference controller
		 */
		FORCEINLINE void AssignReferenceController( TReferenceController<ObjectType>* InNewReferenceController )
		{
			// Only proceed if the new reference counter is different than our current
			if ( InNewReferenceController != referenceController )
			{
				// First, add a weak reference to the new object
				if ( InNewReferenceController != nullptr )
				{
					InNewReferenceController->AddWeakReference();
				}

				// Release weak reference to the old object
				if ( referenceController != nullptr )
				{
					referenceController->ReleaseWeakReference();
				}

				// Assume ownership of the assigned reference counter
				referenceController = InNewReferenceController;
			}
		}

		TReferenceController<ObjectType>*		referenceController;	/**< Pointer to the reference controller for the object */
	};

	/**
	 * @brief Creates a reference controller
	 * @param InObject		Object
	 */
	template< typename ObjectType >
	FORCEINLINE TReferenceController<ObjectType>* NewReferenceController( ObjectType* InObject )
	{
		return new TReferenceController<ObjectType>( InObject );
	}
}

// Forward declaration
template< class ObjectType > class TWeakPtr;

/**
 * @ingroup Core
 * @brief Reference-counting pointer class
 */
template< class ObjectType >
class TSharedPtr
{
public:
	friend TWeakPtr<ObjectType>;

	/**
	 * @brief Hash function for STL containers
	 */
	struct FHashFunction
	{
		/**
		 * @brief Calculate hash of TSharedPtr
		 * @param InSharedPtr	Shared ptr
		 */
		FORCEINLINE std::size_t operator()( const TSharedPtr& InSharedPtr ) const
		{
			return InSharedPtr.sharedReferenceCount.GetTypeHash();
		}
	};

	/**
	 * @brief Constructor
	 */
	FORCEINLINE TSharedPtr()
	{}

	/**
	 * @brief Constructor
	 */
	FORCEINLINE TSharedPtr( std::nullptr_t )
	{}

	/**
	 * @brief Constructor
	 * @param InObject		Object
	 */
	template< typename OtherType >
	FORCEINLINE explicit TSharedPtr( OtherType* InObject )
		: sharedReferenceCount( SharedPointerInternals::NewReferenceController( InObject ) )
	{}

	/**
	 * @brief Constructor
	 * @param InSharedPtr	Shared ptr
	 */
	template< typename OtherType >
	FORCEINLINE explicit TSharedPtr( const TSharedPtr<OtherType>& InSharedPtr )
		: sharedReferenceCount( InSharedPtr.sharedReferenceCount )
	{}

	/**
	 * @brief Constructor
	 * @param InWeakPtr		Weak ptr
	 */
	FORCEINLINE explicit TSharedPtr( const TWeakPtr<ObjectType>& InWeakPtr )
		: sharedReferenceCount( InWeakPtr.weakReferenceCount )
	{}

	/**
	 * @brief Overloaded operator =
	 *
	 * @param InNullptr		std::nullptr
	 * @return Return reference to current object
	 */
	template< typename OtherType >
	FORCEINLINE TSharedPtr& operator=( std::nullptr_t )
	{
		Reset();
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 * 
	 * @param InObject	Object
	 * @return Return reference to current object
	 */
	template< typename OtherType >
	FORCEINLINE TSharedPtr& operator=( OtherType* InObject )
	{
		*this = TSharedPtr( InObject );
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 *
	 * @param InSharedPtr	Shared ptr
	 * @return Return reference to current object
	 */
	template< typename OtherType >
	FORCEINLINE TSharedPtr& operator=( const TSharedPtr<OtherType>& InSharedPtr )
	{
		sharedReferenceCount	= InSharedPtr.sharedReferenceCount;
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 *
	 * @param InWeakPtr		Weak ptr
	 * @return Return reference to current object
	 */
	FORCEINLINE TSharedPtr& operator=( const TWeakPtr<ObjectType>& InWeakPtr )
	{
		sharedReferenceCount	= InWeakPtr.weakReferenceCount;
		return *this;
	}

	/**
	 * @brief Overloaded operator ==
	 * 
	 * @param InObject	Object
	 * @return Returning TRUE if pointers is equal, else returning FALSE
	 */
	template< typename OtherType >
	FORCEINLINE bool operator==( OtherType* InObject ) const
	{
		return Get() == InObject;
	}

	/**
	 * @brief Overloaded operator ==
	 *
	 * @param InSharedPtr	Shared ptr
	 * @return Returning TRUE if pointers is equal, else returning FALSE
	 */
	template< typename OtherType >
	FORCEINLINE bool operator==( const TSharedPtr<OtherType>& InSharedPtr ) const
	{
		return Get() == InSharedPtr.object;
	}

	/**
	 * @brief Overloaded operator !=
	 * 
	 * @param InObject	Object
	 * @return Returning TRUE if pointers is not equal, else returning FALSE
	 */
	template< typename OtherType >
	FORCEINLINE bool operator!=( OtherType* InObject ) const
	{
		return Get() != InObject;
	}

	/**
	 * @brief Overloaded operator !=
	 *
	 * @param InSharedPtr	Shared ptr
	 * @return Returning TRUE if pointers is not equal, else returning FALSE
	 */
	template< typename OtherType >
	FORCEINLINE bool operator!=( const TSharedPtr<OtherType>& InSharedPtr ) const
	{
		return Get() != InSharedPtr.object;
	}

	/**
	 * @brief Overloaded operator bool
	 * @return Returning TRUE if pointer not null, else returning FALSE
	 */
	FORCEINLINE	operator bool() const
	{
		return IsValid();
	}

	/**
	 * @brief Overloaded operator ObjectType&
	 * @return Returning reference on value
	 */
	FORCEINLINE	operator ObjectType&() const
	{
		check( IsValid() );
		return *Get();
	}

	/**
	 * @brief Overloaded operator ObjectType*
	 * @return Returning pointer to value
	 */
	FORCEINLINE	operator ObjectType*() const
	{
		return Get();
	}

	/**
	 * @brief Overloaded operator ObjectType*&
	 * @return Returning reference to pointer value
	 */
	FORCEINLINE operator ObjectType*&()
	{
		return Get();
	}

	/**
	 * @brief Overloaded operator ->
	 * @return Return pointer to PtrType value
	 */
	FORCEINLINE ObjectType* operator->() const
	{
		check( IsValid() );
		return Get();
	}

	/**
	 * @brief Overloaded operator *
	 * @return Return reference to PtrType value
	 */
	FORCEINLINE ObjectType& operator*()
	{
		check( IsValid() );
		return *Get();
	}

	/**
	 * @brief Overloaded operator &
	 * @return Return reference to pointer of PtrType value
	 */
	FORCEINLINE ObjectType** operator&()
	{
		check( IsValid() );
		return &Get();
	}

	/**
	 * @brief Is valid pointer
	 * @return Returning TRUE if pointer is valid, else returning FALSE
	 */
	FORCEINLINE bool IsValid() const
	{
		return sharedReferenceCount.IsValid();
	}

	/**
	 * @brief Returns the number of shared references to this object
	 * @return Return number of shared references to the object
	 */
	FORCEINLINE uint32 GetSharedReferenceCount() const
	{
		return sharedReferenceCount.GetSharedReferenceCount();
	}

	/**
	 * @brief Resets this shared pointer, removing a reference to the object. If there are no other shared
	 * references to the object then it will be destroyed.
	 */
	FORCEINLINE void Reset()
	{
		*this = TSharedPtr<ObjectType>();
	}

	/**
	 * @brief Get pointer
	 * @return Return pointer, if not set returned NULL
	 */
	FORCEINLINE ObjectType* Get() const
	{
		return sharedReferenceCount.GetObject();
	}

	/**
	 * @brief Is unique shared reference
	 * @return Return TRUE if there is only one shared reference to the object, and this is it!
	 */
	FORCEINLINE bool IsUnique() const
	{
		return sharedReferenceCount->IsUnique();
	}

private:
	SharedPointerInternals::TSharedReferencer<ObjectType>		sharedReferenceCount;		/**< Shared reference count */
};

/**
 * @ingroup Core
 * @brief TWeakPtr is a non-intrusive reference-counted weak object pointer
 */
template< class ObjectType >
class TWeakPtr
{
public:
	friend TSharedPtr<ObjectType>;

	/**
	 * @brief Hash function for STL containers
	 */
	struct FHashFunction
	{
		/**
		 * @brief Calculate hash of TWeakPtr
		 * @param InWeakPtr		Weak ptr
		 */
		FORCEINLINE std::size_t operator()( const TWeakPtr& InWeakPtr ) const
		{
			return InWeakPtr.weakReferenceCount.GetTypeHash();
		}
	};

	/**
	 * @brief Constructs an empty TWeakPtr
	 */
	FORCEINLINE TWeakPtr()
	{}

	/**
	 * @brief Constructor an empty TWeakPtr
	 */
	FORCEINLINE TWeakPtr( std::nullptr_t )
	{}

	/**
	 * @brief Constructs a weak pointer from a shared pointer
	 * @param InSharedPtr  The shared pointer to create a weak pointer from
	 */
	template< typename OtherType >
	FORCEINLINE TWeakPtr( const TSharedPtr<OtherType>& InSharedPtr )
		: weakReferenceCount( InSharedPtr.sharedReferenceCount )
	{}

	/**
	 * @brief Constructs a weak pointer from a weak pointer of another type
	 * @param  InWeakPtr  The weak pointer to create a weak pointer from
	 */
	template< typename OtherType >
	FORCEINLINE TWeakPtr( const TWeakPtr<OtherType>& InWeakPtr )
		: weakReferenceCount( InWeakPtr.weakReferenceCount )
	{}

	/**
	 * @brief Constructor
	 * @param InWeakPtr		Weak ptr
	 */
	FORCEINLINE TWeakPtr( const TWeakPtr& InWeakPtr )
		: weakReferenceCount( InWeakPtr.weakReferenceCount )
	{}

	/**
	 * @brief Override operator =
	 */
	FORCEINLINE TWeakPtr& operator=( std::nullptr_t )
	{
		Reset();
		return *this;
	}

	/**
	 * @brief Assignment operator adds a weak reference to the object referenced by the specified weak pointer
	 * @param  InWeakPtr  The weak pointer for the object to assign
	 */
	FORCEINLINE TWeakPtr& operator=( const TWeakPtr& InWeakPtr )
	{
		weakReferenceCount	= InWeakPtr.weakReferenceCount;
		return *this;
	}

	/**
	 * @brief Assignment operator adds a weak reference to the object referenced by the specified weak pointer.
	 * This assignment operator is intended to allow derived-to-base conversions.
	 * 
	 * @param InWeakPtr  The weak pointer for the object to assign
	 */
	template< typename OtherType >
	FORCEINLINE TWeakPtr& operator=( const TWeakPtr<OtherType>& InWeakPtr )
	{
		weakReferenceCount	= InWeakPtr.weakReferenceCount;
		return *this;
	}

	/**
	 * @brief Assignment operator sets this weak pointer from a shared pointer
	 * @param InSharedPtr The shared pointer used to assign to this weak pointer
	 */
	template< typename OtherType >
	FORCEINLINE TWeakPtr& operator=( const TSharedPtr<OtherType >& InSharedPtr )
	{
		weakReferenceCount	= InSharedPtr.sharedReferenceCount;
		return *this;
	}

	/**
	 * @brief Converts this weak pointer to a shared pointer
	 * @return Return shared pointer for this object (will only be valid if still referenced!)
	 */
	FORCEINLINE TSharedPtr<ObjectType> Pin() const
	{
		return TSharedPtr<ObjectType>( *this );
	}

	/**
	 * @brief Checks to see if this weak pointer actually has a valid reference to an object
	 * @return Return TRUE if the weak pointer is valid and a pin operator would have succeeded
	 */
	FORCEINLINE const bool IsValid() const
	{
		return weakReferenceCount.IsValid();
	}

	/**
	 * @brief Resets this weak pointer, removing a weak reference to the object. If there are no other shared
	 * or weak references to the object, then the tracking object will be destroyed
	 */
	FORCEINLINE void Reset()
	{
		*this = TWeakPtr<ObjectType>();
	}

	/**
	 * @brief Has same object
	 * @return Returns TRUE if the object this weak pointer points to is the same as the specified object pointer
	 */
	FORCEINLINE bool HasSameObject( const void* InOtherPtr ) const
	{
		return Pin().Get() == InOtherPtr;
	}

private:
	SharedPointerInternals::TWeakReferencer<ObjectType>		weakReferenceCount;		/**< Weak reference count */
};

#endif // SHAREDPOINTER_H