/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SHAREDPOINTER_H
#define SHAREDPOINTER_H

#include "Misc/SharedPointerInternals.h"
#include "Misc/Template.h"

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
	 * @param InSharedPtr	Shared ptr
	 */
	template< typename OtherType >
	FORCEINLINE TSharedPtr( const TSharedPtr<OtherType>& InSharedPtr )
		: sharedReferenceCount( InSharedPtr.sharedReferenceCount )
	{}

	/**
	 * @brief Constructor
	 * @param InSharedPtr	Shared ptr
	 */
	FORCEINLINE TSharedPtr( const TSharedPtr& InSharedPtr )
		: sharedReferenceCount( InSharedPtr.sharedReferenceCount )
	{}

	/**
	 * @brief Constructor
	 * @param InWeakPtr		Weak ptr
	 */
	template< typename OtherType >
	FORCEINLINE TSharedPtr( const TWeakPtr<OtherType>& InWeakPtr )
		: sharedReferenceCount( InWeakPtr.weakReferenceCount )
	{}

	/**
	 * @brief Constructor
	 * @param InWeakPtr		Weak ptr
	 */
	FORCEINLINE TSharedPtr( const TWeakPtr<ObjectType>& InWeakPtr )
		: sharedReferenceCount( InWeakPtr.weakReferenceCount )
	{}

	/**
	 * @brief Constructor
	 * @param InSharedPtr	Shared ptr
	 */
	template< typename OtherType >
	FORCEINLINE TSharedPtr( TSharedPtr<OtherType>&& InSharedPtr )
		:  sharedReferenceCount( MoveTemp( InSharedPtr.sharedReferenceCount ) )
	{}

	/**
	 * @brief Constructor
	 * @param InSharedPtr	Shared ptr
	 */
	FORCEINLINE TSharedPtr( TSharedPtr&& InSharedPtr )
		: sharedReferenceCount( MoveTemp( InSharedPtr.sharedReferenceCount ) )
	{}

	/**
	 * @brief Overloaded operator =
	 * @return Return reference to current object
	 */
	FORCEINLINE TSharedPtr& operator=( std::nullptr_t )
	{
		Reset();
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
		sharedReferenceCount = InSharedPtr.sharedReferenceCount;
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 *
	 * @param InSharedPtr	Shared ptr
	 * @return Return reference to current object
	 */
	FORCEINLINE TSharedPtr& operator=( const TSharedPtr& InSharedPtr )
	{
		sharedReferenceCount = InSharedPtr.sharedReferenceCount;
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 *
	 * @param InWeakPtr		Weak ptr
	 * @return Return reference to current object
	 */
	template< typename OtherType >
	FORCEINLINE TSharedPtr& operator=( const TWeakPtr<OtherType>& InWeakPtr )
	{
		sharedReferenceCount = InWeakPtr.weakReferenceCount;
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
		sharedReferenceCount = InWeakPtr.weakReferenceCount;
		return *this;
	}

	/**
	 * @brief Overloaded operator 
	 * 
	 * @param InSharedPtr	Shared ptr
	 * @return Return reference to current object
	 */
	template< typename OtherType >
	FORCEINLINE TSharedPtr& operator=( TSharedPtr<OtherType>&& InSharedPtr )
	{
		if ( this != ( TSharedPtr<ObjectType>* )&InSharedPtr )
		{
			sharedReferenceCount = MoveTemp( InSharedPtr.sharedReferenceCount );
		}
		return *this;
	}

	/**
	 * @brief Overloaded operator
	 *
	 * @param InSharedPtr	Shared ptr
	 * @return Return reference to current object
	 */
	FORCEINLINE TSharedPtr& operator=( TSharedPtr&& InSharedPtr )
	{
		if ( this !=  &InSharedPtr )
		{
			sharedReferenceCount = MoveTemp( InSharedPtr.sharedReferenceCount );
		}
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
		return Get() == InSharedPtr.Get();
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
		return Get() != InSharedPtr.Get();
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
	 * @brief Get type hash
	 * @return Return hash of type
	 */
	FORCEINLINE uint64 GetTypeHash() const
	{
		return sharedReferenceCount.GetTypeHash();
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
	 * @brief Returns the number of weak references to this object
	 * @return Number of weak references to the object
	 */
	FORCEINLINE uint32 GetWeakReferenceCount() const
	{
		return sharedReferenceCount.GetWeakReferenceCount();
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
		return sharedReferenceCount.IsUnique();
	}

	// Friend function for make shared ptr
	template< typename ObjectType, typename... ArgTypes >
	friend TSharedPtr<ObjectType> MakeSharedPtr( ArgTypes&&... InArgs );

	// Declare other smart pointer types as friends as needed
	template< class OtherType > friend class TSharedPtr;
	template< class OtherType > friend class TWeakPtr;

protected:
	/**
	 * @brief Constructor
	 * @param InObject		Object
	 */
	template< typename OtherType >
	FORCEINLINE TSharedPtr( OtherType* InObject )
		: sharedReferenceCount( MoveTemp( SharedPointerInternals::NewReferenceController( ( ObjectType* )InObject ) ) )
	{
		// If the object happens to be derived from TSharedFromThis, the following method
		// will prime the object with a weak pointer to itself
		SharedPointerInternals::EnableSharedFromThis( this, InObject );
	}

	SharedPointerInternals::TSharedReferencer<ObjectType>		sharedReferenceCount;		/**< Shared reference count */
};

/**
 * @ingroup Core
 * @brief Make shared pointer
 *
 * @param InArgs	Arguments for construct object
 * @return Return created shared pointer with allocated object
 */
template< typename ObjectType, typename... ArgTypes >
FORCEINLINE TSharedPtr<ObjectType> MakeSharedPtr( ArgTypes&&... InArgs )
{
	return TSharedPtr<ObjectType>( new ObjectType( InArgs... ) );
}

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
	 * @brief Constructs an empty TWeakPtr
	 */
	FORCEINLINE TWeakPtr( std::nullptr_t )
	{}

	/**
	 * @brief Constructor of move
	 */
	template< typename OtherType >
	FORCEINLINE TWeakPtr( TWeakPtr<OtherType>&& InWeakPtr )
		: weakReferenceCount( MoveTemp( InWeakPtr.weakReferenceCount ) )
	{}

	/**
	 * @brief Constructor of move
	 */
	FORCEINLINE TWeakPtr( TWeakPtr&& InWeakPtr )
		: weakReferenceCount( MoveTemp( InWeakPtr.weakReferenceCount ) )
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
	 * @brief Constructs a weak pointer from a shared pointer
	 * @param InSharedPtr  The shared pointer to create a weak pointer from
	 */
	FORCEINLINE TWeakPtr( const TSharedPtr<ObjectType>& InSharedPtr )
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
	 * @brief Constructs a weak pointer from a weak pointer of another type
	 * @param  InWeakPtr  The weak pointer to create a weak pointer from
	 */
	FORCEINLINE TWeakPtr( const TWeakPtr& InWeakPtr )
		: weakReferenceCount( InWeakPtr.weakReferenceCount )
	{}

	/**
	 * @brief Override operator =
	 * @return Return reference to current object
	 */
	FORCEINLINE TWeakPtr& operator=( std::nullptr_t )
	{
		Reset();
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
		weakReferenceCount = InWeakPtr.weakReferenceCount;
		return *this;
	}

	/**
	 * @brief Assignment operator adds a weak reference to the object referenced by the specified weak pointer.
	 * This assignment operator is intended to allow derived-to-base conversions.
	 *
	 * @param InWeakPtr  The weak pointer for the object to assign
	 */
	FORCEINLINE TWeakPtr& operator=( const TWeakPtr& InWeakPtr )
	{
		weakReferenceCount = InWeakPtr.weakReferenceCount;
		return *this;
	}

	/**
	 * @brief Assignment operator sets this weak pointer from a shared pointer
	 * @param InSharedPtr The shared pointer used to assign to this weak pointer
	 */
	template< typename OtherType >
	FORCEINLINE TWeakPtr& operator=( const TSharedPtr<OtherType >& InSharedPtr )
	{
		weakReferenceCount = InSharedPtr.sharedReferenceCount;
		return *this;
	}

	/**
	 * @brief Assignment operator sets this weak pointer from a shared pointer
	 * @param InSharedPtr The shared pointer used to assign to this weak pointer
	 */
	FORCEINLINE TWeakPtr& operator=( const TSharedPtr<ObjectType>& InSharedPtr )
	{
		weakReferenceCount = InSharedPtr.sharedReferenceCount;
		return *this;
	}

	/**
	 * @brief Overloaded operator
	 *
	 * @param InWeakPtr	Weak ptr
	 * @return Return reference to current object
	 */
	template< typename OtherType >
	FORCEINLINE TWeakPtr& operator=( TWeakPtr<OtherType>&& InWeakPtr )
	{
		if ( this != ( TWeakPtr<ObjectType>* )&InWeakPtr )
		{
			weakReferenceCount = MoveTemp( InWeakPtr.weakReferenceCount );
		}
		return *this;
	}

	/**
	 * @brief Overloaded operator
	 *
	 * @param InWeakPtr	Weak ptr
	 * @return Return reference to current object
	 */
	FORCEINLINE TWeakPtr& operator=( TWeakPtr&& InWeakPtr )
	{
		if ( this != &InWeakPtr )
		{
			weakReferenceCount = MoveTemp( InWeakPtr.weakReferenceCount );
		}
		return *this;
	}

	/**
	 * @brief Overloaded operator ==
	 *
	 * @param InWeakPtr		Weak ptr
	 * @return Returning TRUE if pointers is equal, else returning FALSE
	 */
	template< typename OtherType >
	FORCEINLINE bool operator==( const TWeakPtr<OtherType>& InWeakPtr ) const
	{
		return Pin().Get() == InWeakPtr.Pin().Get();
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
		return Pin().Get() == InSharedPtr.Get();
	}

	/**
	 * @brief Overloaded operator !=
	 *
	 * @param InWeakPtr		Weak ptr
	 * @return Returning TRUE if pointers is not equal, else returning FALSE
	 */
	template< typename OtherType >
	FORCEINLINE bool operator!=( const TWeakPtr<OtherType>& InWeakPtr ) const
	{
		return Pin().Get() != InWeakPtr.Pin().Get();
	}

	/**
	 * @brief Overloaded operator !=
	 *
	 * @param InSharedPtr	Shared ptr
	 * @return Returning TRUE if pointers is equal, else returning FALSE
	 */
	template< typename OtherType >
	FORCEINLINE bool operator!=( const TSharedPtr<OtherType>& InSharedPtr ) const
	{
		return Pin().Get() != InSharedPtr.Get();
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
	 * @brief Converts this weak pointer to a shared pointer
	 * @return Return shared pointer for this object (will only be valid if still referenced!)
	 */
	FORCEINLINE TSharedPtr<ObjectType> Pin() const
	{
		return IsValid() ? TSharedPtr<ObjectType>( *this ) : TSharedPtr<ObjectType>();
	}

	/**
	 * @brief Get type hash
	 * @return Return hash of type
	 */
	FORCEINLINE uint64 GetTypeHash() const
	{
		return weakReferenceCount.GetTypeHash();
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

	// Declare other smart pointer types as friends as needed
	template< class OtherType > friend class TSharedPtr;
	template< class OtherType > friend class TWeakPtr;

protected:
	SharedPointerInternals::TWeakReferencer<ObjectType>		weakReferenceCount;		/**< Weak reference count */
};

/**
 * @ingroup Core
 * @brief Derive your class from TSharedFromThis to enable access to a TSharedPtr directly from an object
 * instance that's already been allocated
 */
template< class ObjectType >
class TSharedFromThis
{
public:
	/**
	 * @brief Provides access to a shared reference to this object. Note that is only valid to call
	 * this after a shared pointer to the object has already been created.
	 * Also note that it is illegal to call this in the object's destructor.
	 *
	 * @return Returns this object as a shared pointer
	 */
	TSharedPtr<ObjectType> AsShared()
	{
		TSharedPtr<ObjectType>	sharedThis = weakThis.Pin();

		//
		// If the following assert goes off, it means one of the following:
		//
		//     - You tried to request a shared pointer before the object was ever assigned to one. (e.g. constructor)
		//     - You tried to request a shared pointer while the object is being destroyed (destructor chain)
		//
		// To fix this, make sure you create at least one shared reference to your object instance before requested,
		// and also avoid calling this function from your object's destructor.
		//
		check( sharedThis.Get() == this );

		// Now that we've verified the shared pointer is valid, we return it!
		return sharedThis;
	}

	/**
	 * @brief Provides access to a shared reference to this object (const.)  Note that is only valid to call
	 * this after a shared pointer to the object has already been created.
	 * Also note that it is illegal to call this in the object's destructor.
	 *
	 * @return Returns this object as a shared pointer (const)
	 */
	TSharedPtr<const ObjectType> AsShared() const
	{
		TSharedPtr<const ObjectType>	sharedThis = weakThis.Pin();

		//
		// If the following assert goes off, it means one of the following:
		//
		//     - You tried to request a shared pointer before the object was ever assigned to one. (e.g. constructor)
		//     - You tried to request a shared pointer while the object is being destroyed (destructor chain)
		//
		// To fix this, make sure you create at least one shared reference to your object instance before requested,
		// and also avoid calling this function from your object's destructor.
		//
		check( sharedThis.Get() == this );

		// Now that we've verified the shared pointer is valid, we return it!
		return sharedThis;
	}

	/**
	 * @brief Provides a weak reference to this object. Note that is only valid to call
	 * this after a shared pointer to the object has already been created.
	 * Also note that it is illegal to call this in the object's destructor.
	 *
	 * @return Returns this object as a shared pointer
	 */
	TWeakPtr<ObjectType> AsWeak()
	{
		TWeakPtr<ObjectType>	result = weakThis;

		//
		// If the following assert goes off, it means one of the following:
		//
		//     - You tried to request a weak pointer before the object was ever assigned to a shared pointer. (e.g. constructor)
		//     - You tried to request a weak pointer while the object is being destroyed (destructor chain)
		//
		// To fix this, make sure you create at least one shared reference to your object instance before requested,
		// and also avoid calling this function from your object's destructor.
		//
		check( result.Pin().Get() == this );

		// Now that we've verified the pointer is valid, we'll return it!
		return result;
	}

	/**
	 * @brief Provides a weak reference to this object (const.). Note that is only valid to call
	 * this after a shared pointer to the object has already been created.
	 * Also note that it is illegal to call this in the object's destructor.
	 *
	 * @return Returns this object as a shared pointer (const.)
	 */
	TWeakPtr<const ObjectType> AsWeak() const
	{
		TWeakPtr<const ObjectType>		result = weakThis;

		//
		// If the following assert goes off, it means one of the following:
		//
		//     - You tried to request a weak pointer before the object was ever assigned to a shared pointer. (e.g. constructor)
		//     - You tried to request a weak pointer while the object is being destroyed (destructor chain)
		//
		// To fix this, make sure you create at least one shared reference to your object instance before requested,
		// and also avoid calling this function from your object's destructor.
		//
		check( result.Pin().Get() == this );

		// Now that we've verified the pointer is valid, we'll return it!
		return result;
	}

protected:
	/**
	 * @brief Provides access to a shared reference to an object, given the object's 'this' pointer. Uses
	 * the 'this' pointer to derive the object's actual type, then casts and returns an appropriately
	 * typed shared reference.  Intentionally declared 'protected', as should only be called when the
	 * 'this' pointer can be passed.
	 *
	 * @param InThisPtr		Pointer to this ptr
	 * @return Returns this object as a shared pointer
	 */
	template< class OtherType >
	FORCEINLINE static TSharedPtr<OtherType> SharedThis( OtherType* InThisPtr )
	{
		return ( TSharedPtr<OtherType> )InThisPtr->AsShared();
	}

	/**
	 * @brief Provides access to a shared reference to an object, given the object's 'this' pointer. Uses
	 * the 'this' pointer to derive the object's actual type, then casts and returns an appropriately
	 * typed shared reference.  Intentionally declared 'protected', as should only be called when the
	 * 'this' pointer can be passed.
	 *
	 * @param InThisPtr		Pointer to this ptr
	 * @return Returns this object as a shared pointer (const)
	 */
	template< class OtherType >
	FORCEINLINE static TSharedPtr<const OtherType> SharedThis( const OtherType* InThisPtr )
	{
		return ( TSharedPtr<const OtherType> )InThisPtr->AsShared();
	}

public:		// Ideally this would be private, but template sharing problems prevent it
	/**
	 * @brief Freshens the internal weak pointer object using
	 * the supplied object pointer along with the authoritative shared reference to the object.
	 * Note that until this function is called, calls to AsShared() will result in an empty pointer
	 * 
	 * @warning Internal use only, do not call this method
	 * 
	 * @param InSharedPtr	Pointer to shared ptr
	 */
	template< class SharedPtrType >
	FORCEINLINE void UpdateWeakReferenceInternal( const TSharedPtr<SharedPtrType>* InSharedPtr ) const
	{
		if ( !weakThis.IsValid() )
		{
			weakThis = TSharedPtr<ObjectType>( *InSharedPtr );
		}
	}

	/**
	 * @brief Checks whether our referenced instance is valid (ie, whether it's safe to call AsShared)
	 * @return If this returns false, it means that your instance has either:
	 *  - Not yet been assigned to a shared pointer (via MakeShared or MakeShareable).
	 *  - Is currently within its constructor (so the shared instance isn't yet available).
	 *  - Is currently within its destructor (so the shared instance is no longer available).
	 */
	FORCEINLINE bool DoesSharedInstanceExist() const
	{
		return weakThis.IsValid();
	}

protected:
	/**
	 * @brief Hidden stub constructor
	 */
	TSharedFromThis() 
	{}

	/**
	 * @brief Hidden stub copy constructor
	 */
	TSharedFromThis( TSharedFromThis const& ) {}

	/**
	 * @brief Hidden stub assignment operator
	 */
	FORCEINLINE TSharedFromThis& operator=( TSharedFromThis const& )
	{
		return *this;
	}

	/**
	 * @brief Hidden destructor
	 */
	~TSharedFromThis() {}

private:
	mutable TWeakPtr<ObjectType>		weakThis;	/**< Weak reference to ourselves */
};

#endif // SHAREDPOINTER_H