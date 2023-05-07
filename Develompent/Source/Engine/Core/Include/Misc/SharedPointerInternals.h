/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SHAREDPOINTERINTERNALS_H
#define SHAREDPOINTERINTERNALS_H

#include "Misc/Types.h"
#include "System/ThreadingBase.h"
#include "Core.h"

// Forward declarations
template< class ObjectType > class TSharedPtr;
template< class ObjectType > class TWeakPtr;
template< class ObjectType > class TSharedFromThis;

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
		template< typename OtherType >
		FORCEINLINE TReferenceController( OtherType* InObject )
			: sharedReferenceCount( 1 )
			, weakReferenceCount( 1 )
			, object( ( ObjectType* )InObject )
		{}

		/**
		 * @brief Destructor
		 */
		virtual ~TReferenceController()
		{
			Assert( !object && !sharedReferenceCount && !weakReferenceCount );
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
		 * @brief Get count of weak references
		 * @return Return count of weak references
		 */
		FORCEINLINE uint32 GetWeakReferenceCount() const
		{
			return weakReferenceCount;
		}

		/**
		 * @brief Adds a shared reference to this counter
		 */
		FORCEINLINE void AddSharedReference()
		{
			Sys_InterlockedIncrement( ( int32* )&sharedReferenceCount );
		}

		/**
		 * @brief Release shared reference
		 */
		FORCEINLINE void ReleaseSharedReference()
		{
			// If we already release shared reference, we nothing do
			if ( sharedReferenceCount <= 0 )
			{
				return;
			}

			if ( sharedReferenceCount == 1 )
			{
				// Last shared reference was released!  Destroy the referenced object.
				DestroyObject();

				// Clear shared reference count
				Sys_InterlockedDecrement( ( int32* )&sharedReferenceCount );

				// No more shared referencers, so decrement the weak reference count by one.  When the weak
				// reference count reaches zero, this object will be deleted.
				ReleaseWeakReference();
			}
			else
			{
				Sys_InterlockedDecrement( ( int32* )&sharedReferenceCount );
			}
		}

		/**
		 * @brief Adds a weak reference to this counter
		 */
		FORCEINLINE void AddWeakReference()
		{
			Sys_InterlockedIncrement( ( int32* )&weakReferenceCount );
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

			Sys_InterlockedIncrement( ( int32* )&sharedReferenceCount );
			return true;
		}

		/**
		 * @brief Releases a weak reference to this counter
		 */
		FORCEINLINE void ReleaseWeakReference()
		{
			if ( !Sys_InterlockedDecrement( ( int32* )&weakReferenceCount ) )
			{
				delete this;
			}
		}

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

		/**
		 * @brief Get object
		 * @return Return object
		 */
		FORCEINLINE ObjectType* GetObject() const
		{
			return object;
		}

		// Non-copyable
		TReferenceController( const TReferenceController& ) = delete;
		TReferenceController& operator=( const TReferenceController& ) = delete;

	private:
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
		 * @brief Move constructor creates no new references
		 * @param InSharedReference		Shared reference
		 */
		template< typename OtherType >
		FORCEINLINE explicit TSharedReferencer( TSharedReferencer<OtherType>&& InSharedReference )
			: referenceController( ( TReferenceController<ObjectType>* )InSharedReference.referenceController )
		{
			InSharedReference.referenceController = nullptr;
		}

		/**
		 * @brief Move constructor creates no new references
		 * @param InSharedReference		Shared reference
		 */
		FORCEINLINE explicit TSharedReferencer( TSharedReferencer&& InSharedReference )
			: referenceController( InSharedReference.referenceController )
		{
			InSharedReference.referenceController = nullptr;
		}

		/**
		 * @brief Constructor of move
		 * @param InReferenceController		Reference controller
		 */
		template< typename OtherType >
		FORCEINLINE explicit TSharedReferencer( TReferenceController<OtherType>*&& InReferenceController )
			: referenceController( ( TReferenceController<ObjectType>* )InReferenceController )
		{
			InReferenceController = nullptr;
		}

		/**
		 * @brief Constructor of move
		 * @param InReferenceController		Reference controller
		 */
		FORCEINLINE explicit TSharedReferencer( TReferenceController<ObjectType>*&& InReferenceController )
			: referenceController( InReferenceController )
		{
			InReferenceController = nullptr;
		}

		/**
		 * @brief Copy constructor creates a new reference to the existing object
		 * @param InSharedReference		Shared reference
		 */
		template< typename OtherType >
		FORCEINLINE explicit TSharedReferencer( const TSharedReferencer<OtherType>& InSharedReference )
			: referenceController( ( TReferenceController<ObjectType>* )InSharedReference.referenceController )
		{
			// If the incoming reference had an object associated with it, then go ahead and increment the
			// shared reference count
			if ( referenceController != nullptr )
			{
				referenceController->AddSharedReference();
			}
		}

		/**
		 * @brief Copy constructor creates a new reference to the existing object
		 * @param InSharedReference		Shared reference
		 */
		FORCEINLINE explicit TSharedReferencer( const TSharedReferencer& InSharedReference )
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
		 * @brief Creates a shared referencer object from a weak referencer object. This will only result
		 * in a valid object reference if the object already has at least one other shared referencer
		 *
		 * @param InWeakReference	Weak reference
		 */
		template< typename OtherType >
		FORCEINLINE explicit TSharedReferencer( const TWeakReferencer<OtherType>& InWeakReference )
			: referenceController( ( TReferenceController<ObjectType>* )InWeakReference.referenceController )
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
		FORCEINLINE explicit TSharedReferencer( const TWeakReferencer<ObjectType>& InWeakReference )
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
		template< typename OtherType >
		FORCEINLINE explicit TSharedReferencer( TWeakReferencer<OtherType>&& InWeakReference )
			: referenceController( ( TReferenceController<ObjectType>* )InWeakReference.referenceController )
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
		 * @brief Creates a shared referencer object from a weak referencer object. This will only result
		 * in a valid object reference if the object already has at least one other shared referencer
		 *
		 * @param InWeakReference	Weak reference
		 */
		FORCEINLINE explicit TSharedReferencer( TWeakReferencer<ObjectType>&& InWeakReference )
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
		virtual ~TSharedReferencer()
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
		template< typename OtherType >
		FORCEINLINE TSharedReferencer& operator=( const TSharedReferencer<OtherType>& InSharedReference )
		{
			*this = ( TSharedReferencer )InSharedReference;
			return *this;
		}

		/**
		 * @brief Override operator =
		 * @param InSharedReference		Shared reference
		 */
		FORCEINLINE TSharedReferencer& operator=( const TSharedReferencer& InSharedReference )
		{
			// Make sure we're not be reassigned to ourself!
			if ( InSharedReference.referenceController != referenceController )
			{
				// First, add a shared reference to the new object
				if ( InSharedReference.referenceController != nullptr )
				{
					InSharedReference.referenceController->AddSharedReference();
				}

				// Release shared reference to the old object
				if ( referenceController != nullptr )
				{
					referenceController->ReleaseSharedReference();
				}

				// Assume ownership of the assigned reference counter
				referenceController = InSharedReference.referenceController;
			}

			return *this;
		}

		/**
		 * @brief Move assignment operator adds no references to the assigned object.  If this counter was previously
		 * referencing an object, that reference will be released
		 * @param InSharedReference		Shared reference
		 */
		template< typename OtherType >
		FORCEINLINE TSharedReferencer& operator=( TSharedReferencer<OtherType>&& InSharedReference )
		{
			*this = ( TSharedReferencer&& )InSharedReference;
			return *this;
		}

		/**
		 * @brief Move assignment operator adds no references to the assigned object.  If this counter was previously
		 * referencing an object, that reference will be released
		 * @param InSharedReference		Shared reference
		 */
		FORCEINLINE TSharedReferencer& operator=( TSharedReferencer&& InSharedReference )
		{
			// Make sure we're not be reassigned to ourself!
			auto		newReferenceController = InSharedReference.referenceController;
			auto		oldReferenceController = referenceController;
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
		 * @brief Move assignment operator adds no references to the assigned object
		 * 
		 * @param InReferenceController		Reference controller
		 */
		template< typename OtherType >
		FORCEINLINE TSharedReferencer& operator=( TReferenceController<OtherType>*&& InReferenceController )
		{
			*this = ( TReferenceController<ObjectType>*&& )InReferenceController;
			return *this;
		}

		/**
		 * @brief Move assignment operator adds no references to the assigned object
		 *
		 * @param InReferenceController		Reference controller
		 */
		FORCEINLINE TSharedReferencer& operator=( TReferenceController<ObjectType>*&& InReferenceController )
		{
			// Make sure we're not be reassigned to ourself!
			auto		newReferenceController = InReferenceController;
			auto		oldReferenceController = referenceController;
			if ( newReferenceController != oldReferenceController )
			{
				// Assume ownership of the assigned reference counter
				InReferenceController = nullptr;
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
		 * @brief Returns the number of weak references to this object
		 * @return Number of weak references to the object
		 */
		FORCEINLINE uint32 GetWeakReferenceCount() const
		{
			return referenceController != nullptr ? referenceController->GetWeakReferenceCount() : 0;
		}

		/**
		 * @brief Is unique shared reference
		 * @return Return TRUE if there is only one shared reference to the object, and this is it!
		 */
		FORCEINLINE bool IsUnique() const
		{
			return GetSharedReferenceCount() == 1;
		}

		/**
		 * @brief Get object
		 * @return Return object
		 */
		FORCEINLINE ObjectType* GetObject() const
		{
			return IsValid() ? referenceController->GetObject() : nullptr;
		}

		// Declare other smart pointer types as friends as needed
		template< class OtherType > friend class TSharedReferencer;
		template< class OtherType > friend class TWeakReferencer;

	private:
		mutable TReferenceController<ObjectType>*		referenceController;	/**< Pointer to the reference controller for the object */
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
		template< typename OtherType >
		FORCEINLINE explicit TWeakReferencer( const TWeakReferencer<OtherType>& InWeakRefCountPointer )
			: referenceController( ( TReferenceController<ObjectType>* )InWeakRefCountPointer.referenceController )
		{
			// If the weak referencer has a valid controller, then go ahead and add a weak reference to it!
			if ( referenceController != nullptr )
			{
				referenceController->AddWeakReference();
			}
		}

		/**
		 * @brief Construct a weak referencer object from another weak referencer
		 * @param InWeakRefCountPointer		Weak referencer
		 */
		FORCEINLINE explicit TWeakReferencer( const TWeakReferencer& InWeakRefCountPointer )
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
		template< typename OtherType >
		FORCEINLINE explicit TWeakReferencer( const TSharedReferencer<OtherType>& InSharedRefCountPointer )
			: referenceController( ( TReferenceController<ObjectType>* )InSharedRefCountPointer.referenceController )
		{
			// If the shared referencer had a valid controller, then go ahead and add a weak reference to it!
			if ( referenceController != nullptr )
			{
				referenceController->AddWeakReference();
			}
		}

		/**
		 * @brief Construct a weak referencer object from a shared referencer object
		 * @param InSharedRefCountPointer		Shared referencer
		 */
		FORCEINLINE explicit TWeakReferencer( const TSharedReferencer<ObjectType>& InSharedRefCountPointer )
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
		 * @param InSharedRefCountPointer		Shared referencer
		 */
		template< typename OtherType >
		FORCEINLINE explicit TWeakReferencer( TWeakReferencer<OtherType>&& InWeakRefCountPointer )
			: referenceController( ( TReferenceController<ObjectType>* )InWeakRefCountPointer.referenceController )
		{
			InWeakRefCountPointer.referenceController = nullptr;
		}

		/**
		 * @brief Construct a weak referencer object from an rvalue weak referencer
		 * @param InSharedRefCountPointer		Shared referencer
		 */
		FORCEINLINE explicit TWeakReferencer( TWeakReferencer&& InWeakRefCountPointer )
			: referenceController( InWeakRefCountPointer.referenceController )
		{
			InWeakRefCountPointer.referenceController = nullptr;
		}

		/**
		 * @brief Destructor
		 */
		virtual ~TWeakReferencer()
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
		template< typename OtherType >
		FORCEINLINE TWeakReferencer& operator=( const TWeakReferencer<OtherType>& InWeakReference )
		{
			AssignReferenceController( InWeakReference.referenceController );
			return *this;
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
		template< typename OtherType >
		FORCEINLINE TWeakReferencer& operator=( const TSharedReferencer<OtherType>& InSharedReference )
		{
			AssignReferenceController( InSharedReference.referenceController );
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
		template< typename OtherType >
		FORCEINLINE TWeakReferencer& operator=( TWeakReferencer<OtherType>&& InWeakReference )
		{
			*this = ( TWeakReferencer&& )InWeakReference;
			return *this;
		}

		/**
		 * @brief Assignment operator from an rvalue weak referencer object.  If this counter was previously referencing an
		 * object, that reference will be released
		 *
		 * @param InWeakReference	Weak reference
		 */
		FORCEINLINE TWeakReferencer& operator=( TWeakReferencer<ObjectType>&& InWeakReference )
		{
			auto		oldReferenceController = referenceController;
			referenceController = InWeakReference.referenceController;
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

		// Declare other smart pointer types as friends as needed
		template< class OtherType > friend class TSharedReferencer;
		template< class OtherType > friend class TWeakReferencer;

	private:
		/**
		 * @brief Assigns a new reference controller to this counter object, first adding a reference to it, then
		 * releasing the previous object
		 *
		 * @param InNewReferenceController		New reference controller
		 */
		template< typename OtherType >
		FORCEINLINE void AssignReferenceController( TReferenceController<OtherType>* InNewReferenceController )
		{
			// Only proceed if the new reference counter is different than our current
			if ( ( TReferenceController<ObjectType>* )InNewReferenceController != referenceController )
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
				referenceController = ( TReferenceController<ObjectType>* )InNewReferenceController;
			}
		}

		mutable TReferenceController<ObjectType>*		referenceController;	/**< Pointer to the reference controller for the object */
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

	/**
	 * @brief Templated helper function (const) that creates a shared pointer from an object instance
	 * 
	 * @param InSharedPtr		Pointer to shared ptr
	 * @param InShareable		Shareable object
	 */
	template< class SharedPtrType, class OtherType >
	FORCEINLINE void EnableSharedFromThis( const TSharedPtr<SharedPtrType>* InSharedPtr, const TSharedFromThis<OtherType>* InShareable )
	{
		if ( InShareable != nullptr )
		{
			InShareable->UpdateWeakReferenceInternal( InSharedPtr );
		}
	}

	/**
	 * @brief Templated helper function that creates a shared pointer from an object instance
	 * 
	 * @param InSharedPtr		Pointer to shared ptr
	 * @param InShareable		Shareable object
	 */
	template< class SharedPtrType, class OtherType >
	FORCEINLINE void EnableSharedFromThis( TSharedPtr<SharedPtrType>* InSharedPtr, const TSharedFromThis<OtherType>* InShareable )
	{
		if ( InShareable != nullptr )
		{
			InShareable->UpdateWeakReferenceInternal( InSharedPtr );
		}
	}

	/**
	 * @brief Templated helper catch-all function, accomplice to the above helper functions
	 */
	FORCEINLINE void EnableSharedFromThis( ... ) {}
}

#endif // !SHAREDPOINTERINTERNALS_H