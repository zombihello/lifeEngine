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
		: sharedReferenceCount( MoveTemp( SharedPointerInternals::NewReferenceController( ( ObjectType* ) InObject ) ) )
	{}

	SharedPointerInternals::TSharedReferencer<ObjectType>		sharedReferenceCount;		/**< Shared reference count */
};

/**
 * @ingroup Core
 * @brief Make shared pointer
 *
 * @param InArgs	Arguments for construct object
 * @return Return created shared pointer from InObject
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

	// Declare other smart pointer types as friends as needed
	template< class OtherType > friend class TSharedPtr;
	template< class OtherType > friend class TWeakPtr;

protected:
	SharedPointerInternals::TWeakReferencer<ObjectType>		weakReferenceCount;		/**< Weak reference count */
};

#endif // SHAREDPOINTER_H