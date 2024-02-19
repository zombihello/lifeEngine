/**
 * @file
 * @addtogroup stdlib stdlib
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SHAREDPTR_INTERNALS_H
#define SHAREDPTR_INTERNALS_H

#include "core/core.h"
#include "core/threading.h"

// Forward declarations
template<class TTObjectType> class TSharedPtr;
template<class TTObjectType> class TWeakPtr;
template<class TTObjectType> class TSharedFromThis;

/**
 * @ingroup stdlib
 * @brief SharedPointerInternals contains internal workings of shared and weak pointers
 */
namespace SharedPointerInternals
{
	// Forward declarations
	template<class TTObjectType> class TWeakReferencer;

	/**
	 * @ingroup stdlib
	 * @brief Reference controller
	 */
	template<class TObjectType>
	class TReferenceController
	{
	public:
		/**
		 * @brief Constructor
		 * @param pObject	Object
		 */
		template<typename TOtherType>
		FORCEINLINE TReferenceController( TOtherType* pObject )
			: sharedReferenceCount( 1 )
			, weakReferenceCount( 1 )
			, pObject( ( TObjectType* )pObject )
		{}

		/**
		 * @brief Destructor
		 */
		virtual ~TReferenceController()
		{
			Assert( !pObject && !sharedReferenceCount && !weakReferenceCount );
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
			if ( pObject )
			{
				delete pObject;
				pObject = nullptr;
			}
		}

		/**
		 * @brief Get object
		 * @return Return object
		 */
		FORCEINLINE TObjectType* GetObject() const
		{
			return pObject;
		}

		// Non-copyable
		TReferenceController( const TReferenceController& ) = delete;
		TReferenceController& operator=( const TReferenceController& ) = delete;

	private:
		uint32		sharedReferenceCount;	/**< Number of shared references to this object */
		uint32		weakReferenceCount;		/**< Number of weak references to this object */
		TObjectType* pObject;				/**< Object */
	};

	/**
	 * @ingroup stdlib
	 * @brief TSharedReferencer is a wrapper around a pointer to a reference controller that is used by either a
	 * TSharedPtr to keep track of a referenced object's lifetime
	 */
	template<class TObjectType>
	class TSharedReferencer
	{
	public:
		friend TWeakReferencer<TObjectType>;

		/**
		 * @brief Constructor for an empty shared referencer object
		 */
		FORCEINLINE TSharedReferencer()
			: pReferenceController( nullptr )
		{}

		/**
		 * @brief Move constructor creates no new references
		 * @param sharedReference		Shared reference
		 */
		template<typename TOtherType>
		FORCEINLINE explicit TSharedReferencer( TSharedReferencer<TOtherType>&& sharedReference )
			: pReferenceController( ( TReferenceController<TObjectType>* )sharedReference.pReferenceController )
		{
			sharedReference.pReferenceController = nullptr;
		}

		/**
		 * @brief Move constructor creates no new references
		 * @param sharedReference		Shared reference
		 */
		FORCEINLINE explicit TSharedReferencer( TSharedReferencer&& sharedReference )
			: pReferenceController( sharedReference.pReferenceController )
		{
			sharedReference.pReferenceController = nullptr;
		}

		/**
		 * @brief Constructor of move
		 * @param pReferenceController		Reference controller
		 */
		template<typename TOtherType>
		FORCEINLINE explicit TSharedReferencer( TReferenceController<TOtherType>*&& pReferenceController )
			: pReferenceController( ( TReferenceController<TObjectType>* )pReferenceController )
		{
			pReferenceController = nullptr;
		}

		/**
		 * @brief Constructor of move
		 * @param pReferenceController		Reference controller
		 */
		FORCEINLINE explicit TSharedReferencer( TReferenceController<TObjectType>*&& pReferenceController )
			: pReferenceController( pReferenceController )
		{
			pReferenceController = nullptr;
		}

		/**
		 * @brief Copy constructor creates a new reference to the existing object
		 * @param sharedReference		Shared reference
		 */
		template<typename TOtherType>
		FORCEINLINE explicit TSharedReferencer( const TSharedReferencer<TOtherType>& sharedReference )
			: pReferenceController( ( TReferenceController<TObjectType>* )sharedReference.pReferenceController )
		{
			// If the incoming reference had an object associated with it, then go ahead and increment the
			// shared reference count
			if ( pReferenceController != nullptr )
			{
				pReferenceController->AddSharedReference();
			}
		}

		/**
		 * @brief Copy constructor creates a new reference to the existing object
		 * @param sharedReference		Shared reference
		 */
		FORCEINLINE explicit TSharedReferencer( const TSharedReferencer& sharedReference )
			: pReferenceController( sharedReference.pReferenceController )
		{
			// If the incoming reference had an object associated with it, then go ahead and increment the
			// shared reference count
			if ( pReferenceController != nullptr )
			{
				pReferenceController->AddSharedReference();
			}
		}

		/**
		 * @brief Creates a shared referencer object from a weak referencer object. This will only result
		 * in a valid object reference if the object already has at least one other shared referencer
		 * @param weakReference		Weak reference
		 */
		template<typename TOtherType>
		FORCEINLINE explicit TSharedReferencer( const TWeakReferencer<TOtherType>& weakReference )
			: pReferenceController( ( TReferenceController<TObjectType>* )weakReference.pReferenceController )
		{
			// If the incoming reference had an object associated with it, then go ahead and increment the
			// shared reference count
			if ( pReferenceController != nullptr )
			{
				// Attempt to elevate a weak reference to a shared one. For this to work, the object this
				// weak counter is associated with must already have at least one shared reference. We'll
				// never revive a pointer that has already expired!
				if ( !pReferenceController->ConditionallyAddSharedReference() )
				{
					pReferenceController = nullptr;
				}
			}
		}

		/**
		 * @brief Creates a shared referencer object from a weak referencer object. This will only result
		 * in a valid object reference if the object already has at least one other shared referencer
		 * @param weakReference		Weak reference
		 */
		FORCEINLINE explicit TSharedReferencer( const TWeakReferencer<TObjectType>& weakReference )
			: pReferenceController( weakReference.pReferenceController )
		{
			// If the incoming reference had an object associated with it, then go ahead and increment the
			// shared reference count
			if ( pReferenceController != nullptr )
			{
				// Attempt to elevate a weak reference to a shared one. For this to work, the object this
				// weak counter is associated with must already have at least one shared reference. We'll
				// never revive a pointer that has already expired!
				if ( !pReferenceController->ConditionallyAddSharedReference() )
				{
					pReferenceController = nullptr;
				}
			}
		}

		/**
		 * @brief Creates a shared referencer object from a weak referencer object. This will only result
		 * in a valid object reference if the object already has at least one other shared referencer
		 * @param weakReference		Weak reference
		 */
		template<typename TOtherType>
		FORCEINLINE explicit TSharedReferencer( TWeakReferencer<TOtherType>&& weakReference )
			: pReferenceController( ( TReferenceController<TObjectType>* )weakReference.pReferenceController )
		{
			// If the incoming reference had an object associated with it, then go ahead and increment the
			// shared reference count
			if ( pReferenceController != nullptr )
			{
				// Attempt to elevate a weak reference to a shared one. For this to work, the object this
				// weak counter is associated with must already have at least one shared reference. We'll
				// never revive a pointer that has already expired!
				if ( !pReferenceController->ConditionallyAddSharedReference() )
				{
					pReferenceController = nullptr;
				}

				// Tell the reference counter object that we're no longer referencing the object with
				// this weak pointer
				weakReference.pReferenceController->ReleaseWeakReference();
				weakReference.pReferenceController = nullptr;
			}
		}

		/**
		 * @brief Creates a shared referencer object from a weak referencer object. This will only result
		 * in a valid object reference if the object already has at least one other shared referencer
		 * @param weakReference		Weak reference
		 */
		FORCEINLINE explicit TSharedReferencer( TWeakReferencer<TObjectType>&& weakReference )
			: pReferenceController( weakReference.pReferenceController )
		{
			// If the incoming reference had an object associated with it, then go ahead and increment the
			// shared reference count
			if ( pReferenceController != nullptr )
			{
				// Attempt to elevate a weak reference to a shared one.  For this to work, the object this
				// weak counter is associated with must already have at least one shared reference.  We'll
				// never revive a pointer that has already expired!
				if ( !pReferenceController->ConditionallyAddSharedReference() )
				{
					pReferenceController = nullptr;
				}

				// Tell the reference counter object that we're no longer referencing the object with
				// this weak pointer
				weakReference.pReferenceController->ReleaseWeakReference();
				weakReference.pReferenceController = nullptr;
			}
		}

		/**
		 * @brief Destructor
		 */
		virtual ~TSharedReferencer()
		{
			if ( pReferenceController != nullptr )
			{
				// Tell the reference counter object that we're no longer referencing the object with
				// this shared pointer
				pReferenceController->ReleaseSharedReference();
			}
		}

		/**
		 * @brief Override operator =
		 * @param sharedReference		Shared reference
		 */
		template<typename TOtherType>
		FORCEINLINE TSharedReferencer& operator=( const TSharedReferencer<TOtherType>& sharedReference )
		{
			*this = ( TSharedReferencer )sharedReference;
			return *this;
		}

		/**
		 * @brief Override operator =
		 * @param sharedReference		Shared reference
		 */
		FORCEINLINE TSharedReferencer& operator=( const TSharedReferencer& sharedReference )
		{
			// Make sure we're not be reassigned to ourself!
			if ( sharedReference.pReferenceController != pReferenceController )
			{
				// First, add a shared reference to the new object
				if ( sharedReference.pReferenceController != nullptr )
				{
					sharedReference.pReferenceController->AddSharedReference();
				}

				// Release shared reference to the old object
				if ( pReferenceController != nullptr )
				{
					pReferenceController->ReleaseSharedReference();
				}

				// Assume ownership of the assigned reference counter
				pReferenceController = sharedReference.pReferenceController;
			}

			return *this;
		}

		/**
		 * @brief Move assignment operator adds no references to the assigned object.  If this counter was previously
		 * referencing an object, that reference will be released
		 * @param sharedReference		Shared reference
		 */
		template<typename TOtherType>
		FORCEINLINE TSharedReferencer& operator=( TSharedReferencer<TOtherType>&& sharedReference )
		{
			*this = ( TSharedReferencer&& )sharedReference;
			return *this;
		}

		/**
		 * @brief Move assignment operator adds no references to the assigned object.  If this counter was previously
		 * referencing an object, that reference will be released
		 * @param sharedReference		Shared reference
		 */
		FORCEINLINE TSharedReferencer& operator=( TSharedReferencer&& sharedReference )
		{
			// Make sure we're not be reassigned to ourself!
			auto		newReferenceController = sharedReference.pReferenceController;
			auto		oldReferenceController = pReferenceController;
			if ( newReferenceController != oldReferenceController )
			{
				// Assume ownership of the assigned reference counter
				sharedReference.pReferenceController = nullptr;
				pReferenceController = newReferenceController;

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
		 * @param pReferenceController		Reference controller
		 */
		template<typename TOtherType>
		FORCEINLINE TSharedReferencer& operator=( TReferenceController<TOtherType>*&& pReferenceController )
		{
			*this = ( TReferenceController<TObjectType>*&& )pReferenceController;
			return *this;
		}

		/**
		 * @brief Move assignment operator adds no references to the assigned object
		 * @param pReferenceController		Reference controller
		 */
		FORCEINLINE TSharedReferencer& operator=( TReferenceController<TObjectType>*&& pReferenceController )
		{
			// Make sure we're not be reassigned to ourself!
			auto		newReferenceController = pReferenceController;
			auto		oldReferenceController = TSharedReferencer::pReferenceController;
			if ( newReferenceController != oldReferenceController )
			{
				// Assume ownership of the assigned reference counter
				pReferenceController = nullptr;
				TSharedReferencer::pReferenceController = newReferenceController;

				// Release shared reference to the old object
				if ( oldReferenceController != nullptr )
				{
					oldReferenceController->ReleaseSharedReference();
				}
			}

			return *this;
		}

		/**
		 * @brief Get hash
		 * @return Return hash
		 */
		FORCEINLINE hash_t GetHash() const
		{
			return ( hash_t )pReferenceController;
		}

		/**
		 * @brief Tests to see whether or not this shared counter contains a valid reference
		 * @return Return TRUE if reference is valid
		 */
		FORCEINLINE bool IsValid() const
		{
			return pReferenceController != nullptr && pReferenceController->GetSharedReferenceCount() > 0;
		}

		/**
		 * @brief Returns the number of shared references to this object
		 * @return Number of shared references to the object
		 */
		FORCEINLINE uint32 GetSharedReferenceCount() const
		{
			return pReferenceController != nullptr ? pReferenceController->GetSharedReferenceCount() : 0;
		}

		/**
		 * @brief Returns the number of weak references to this object
		 * @return Number of weak references to the object
		 */
		FORCEINLINE uint32 GetWeakReferenceCount() const
		{
			return pReferenceController != nullptr ? pReferenceController->GetWeakReferenceCount() : 0;
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
		FORCEINLINE TObjectType* GetObject() const
		{
			return IsValid() ? pReferenceController->GetObject() : nullptr;
		}

		// Declare other smart pointer types as friends as needed
		template<class TOtherType> friend class TSharedReferencer;
		template<class TOtherType> friend class TWeakReferencer;

	private:
		mutable TReferenceController<TObjectType>*		pReferenceController;	/**< Pointer to the reference controller for the object */
	};

	/**
	 * @brief TWeakReferencer is a wrapper around a pointer to a reference controller that is used
	 * by a TWeakPtr to keep track of a referenced object's lifetime
	 */
	template<class TObjectType>
	class TWeakReferencer
	{
	public:
		friend TSharedReferencer<TObjectType>;

		/**
		 * @brief Get hash
		 * @return Return hash
		 */
		FORCEINLINE hash_t GetTypeHash() const
		{
			return ( hash_t )pReferenceController;
		}

		/**
		 * @brief Default constructor with empty counter
		 */
		FORCEINLINE TWeakReferencer()
			: pReferenceController( nullptr )
		{}

		/**
		 * @brief Construct a weak referencer object from another weak referencer
		 * @param weakRefCountPointer		Weak referencer
		 */
		template<typename TOtherType>
		FORCEINLINE explicit TWeakReferencer( const TWeakReferencer<TOtherType>& weakRefCountPointer )
			: pReferenceController( ( TReferenceController<TObjectType>* )weakRefCountPointer.pReferenceController )
		{
			// If the weak referencer has a valid controller, then go ahead and add a weak reference to it!
			if ( pReferenceController != nullptr )
			{
				pReferenceController->AddWeakReference();
			}
		}

		/**
		 * @brief Construct a weak referencer object from another weak referencer
		 * @param weakRefCountPointer		Weak referencer
		 */
		FORCEINLINE explicit TWeakReferencer( const TWeakReferencer& weakRefCountPointer )
			: pReferenceController( weakRefCountPointer.pReferenceController )
		{
			// If the weak referencer has a valid controller, then go ahead and add a weak reference to it!
			if ( pReferenceController != nullptr )
			{
				pReferenceController->AddWeakReference();
			}
		}

		/**
		 * @brief Construct a weak referencer object from a shared referencer object
		 * @param sharedRefCountPointer		Shared referencer
		 */
		template<typename TOtherType>
		FORCEINLINE explicit TWeakReferencer( const TSharedReferencer<TOtherType>& sharedRefCountPointer )
			: pReferenceController( ( TReferenceController<TObjectType>* )sharedRefCountPointer.pReferenceController )
		{
			// If the shared referencer had a valid controller, then go ahead and add a weak reference to it!
			if ( pReferenceController != nullptr )
			{
				pReferenceController->AddWeakReference();
			}
		}

		/**
		 * @brief Construct a weak referencer object from a shared referencer object
		 * @param sharedRefCountPointer		Shared referencer
		 */
		FORCEINLINE explicit TWeakReferencer( const TSharedReferencer<TObjectType>& sharedRefCountPointer )
			: pReferenceController( sharedRefCountPointer.pReferenceController )
		{
			// If the shared referencer had a valid controller, then go ahead and add a weak reference to it!
			if ( pReferenceController != nullptr )
			{
				pReferenceController->AddWeakReference();
			}
		}

		/**
		 * @brief Construct a weak referencer object from an rvalue weak referencer
		 * @param weakRefCountPointer		Shared referencer
		 */
		template<typename TOtherType>
		FORCEINLINE explicit TWeakReferencer( TWeakReferencer<TOtherType>&& weakRefCountPointer )
			: pReferenceController( ( TReferenceController<TObjectType>* )weakRefCountPointer.pReferenceController )
		{
			weakRefCountPointer.pReferenceController = nullptr;
		}

		/**
		 * @brief Construct a weak referencer object from an rvalue weak referencer
		 * @param weakRefCountPointer		Shared referencer
		 */
		FORCEINLINE explicit TWeakReferencer( TWeakReferencer&& weakRefCountPointer )
			: pReferenceController( weakRefCountPointer.pReferenceController )
		{
			weakRefCountPointer.pReferenceController = nullptr;
		}

		/**
		 * @brief Destructor
		 */
		virtual ~TWeakReferencer()
		{
			if ( pReferenceController != nullptr )
			{
				// Tell the reference counter object that we're no longer referencing the object with
				// this weak pointer
				pReferenceController->ReleaseWeakReference();
			}
		}

		/**
		 * @brief Override operator =
		 * @param weakReference		Weak reference
		 */
		template<typename TOtherType>
		FORCEINLINE TWeakReferencer& operator=( const TWeakReferencer<TOtherType>& weakReference )
		{
			AssignReferenceController( weakReference.pReferenceController );
			return *this;
		}

		/**
		 * @brief Override operator =
		 * @param weakReference		Weak reference
		 */
		FORCEINLINE TWeakReferencer& operator=( const TWeakReferencer& weakReference )
		{
			AssignReferenceController( weakReference.pReferenceController );
			return *this;
		}

		/**
		 * @brief Override operator =
		 * @param sharedReference	Shared reference
		 */
		template< typename TOtherType >
		FORCEINLINE TWeakReferencer& operator=( const TSharedReferencer<TOtherType>& sharedReference )
		{
			AssignReferenceController( sharedReference.pReferenceController );
			return *this;
		}

		/**
		 * @brief Override operator =
		 * @param sharedReference	Shared reference
		 */
		FORCEINLINE TWeakReferencer& operator=( const TSharedReferencer<TObjectType>& sharedReference )
		{
			AssignReferenceController( sharedReference.pReferenceController );
			return *this;
		}

		/**
		 * @brief Assignment operator from an rvalue weak referencer object.  If this counter was previously referencing an
		 * object, that reference will be released
		 * @param weakReference		Weak reference
		 */
		template<typename TOtherType>
		FORCEINLINE TWeakReferencer& operator=( TWeakReferencer<TOtherType>&& weakReference )
		{
			*this = ( TWeakReferencer&& )weakReference;
			return *this;
		}

		/**
		 * @brief Assignment operator from an rvalue weak referencer object.  If this counter was previously referencing an
		 * object, that reference will be released
		 *
		 * @param weakReference		Weak reference
		 */
		FORCEINLINE TWeakReferencer& operator=( TWeakReferencer<TObjectType>&& weakReference )
		{
			auto		oldReferenceController = pReferenceController;
			pReferenceController = weakReference.pReferenceController;
			weakReference.pReferenceController = nullptr;

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
			return pReferenceController != nullptr && pReferenceController->GetSharedReferenceCount() > 0;
		}

		/**
		 * @brief Get object
		 * @return Return object
		 */
		FORCEINLINE TObjectType* GetObject() const
		{
			return IsValid() ? pReferenceController->GetObject() : nullptr;
		}

		// Declare other smart pointer types as friends as needed
		template<class TOtherType> friend class TSharedReferencer;
		template<class TOtherType> friend class TWeakReferencer;

	private:
		/**
		 * @brief Assigns a new reference controller to this counter object, first adding a reference to it, then
		 * releasing the previous object
		 * @param pNewReferenceController		New reference controller
		 */
		template<typename TOtherType>
		FORCEINLINE void AssignReferenceController( TReferenceController<TOtherType>* pNewReferenceController )
		{
			// Only proceed if the new reference counter is different than our current
			if ( ( TReferenceController<TObjectType>* )pNewReferenceController != pReferenceController )
			{
				// First, add a weak reference to the new object
				if ( pNewReferenceController != nullptr )
				{
					pNewReferenceController->AddWeakReference();
				}

				// Release weak reference to the old object
				if ( pReferenceController != nullptr )
				{
					pReferenceController->ReleaseWeakReference();
				}

				// Assume ownership of the assigned reference counter
				pReferenceController = ( TReferenceController<TObjectType>* )pNewReferenceController;
			}
		}

		mutable TReferenceController<TObjectType>*		pReferenceController;	/**< Pointer to the reference controller for the object */
	};

	/**
	 * @brief Creates a reference controller
	 * @param pObject		Object
	 */
	template<typename TObjectType>
	FORCEINLINE TReferenceController<TObjectType>* NewReferenceController( TObjectType* pObject )
	{
		return new TReferenceController<TObjectType>( pObject );
	}

	/**
	 * @brief Templated helper function (const) that creates a shared pointer from an object instance
	 * 
	 * @param pSharedPtr		Pointer to shared ptr
	 * @param pShareable		Shareable object
	 */
	template<class TSharedPtrType, class TOtherType>
	FORCEINLINE void EnableSharedFromThis( const TSharedPtr<TSharedPtrType>* pSharedPtr, const TSharedFromThis<TOtherType>* pShareable )
	{
		if ( pShareable != nullptr )
		{
			pShareable->UpdateWeakReferenceInternal( pSharedPtr );
		}
	}

	/**
	 * @brief Templated helper function that creates a shared pointer from an object instance
	 * 
	 * @param pSharedPtr		Pointer to shared ptr
	 * @param pShareable		Shareable object
	 */
	template<class TSharedPtrType, class TOtherType>
	FORCEINLINE void EnableSharedFromThis( TSharedPtr<TSharedPtrType>* pSharedPtr, const TSharedFromThis<TOtherType>* pShareable )
	{
		if ( pShareable != nullptr )
		{
			pShareable->UpdateWeakReferenceInternal( pSharedPtr );
		}
	}

	/**
	 * @brief Templated helper catch-all function, accomplice to the above helper functions
	 */
	FORCEINLINE void EnableSharedFromThis( ... ) {}
}

#endif // !SHAREDPTR_INTERNALS_H