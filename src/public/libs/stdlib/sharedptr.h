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

#ifndef SHAREDPTR_H
#define SHAREDPTR_H

#include "stdlib/sharedptr_internals.h"

// Forward declaration
template<class TObjectType> class TWeakPtr;

/**
 * @ingroup stdlib
 * @brief Reference-counting pointer class
 */
template<class TObjectType>
class TSharedPtr
{
public:
	friend TWeakPtr<TObjectType>;

	/**
	 * @brief Hash function for STL containers
	 */
	struct HashFunction
	{
		/**
		 * @brief Calculate hash of TSharedPtr
		 * 
		 * @param sharedPtr		Shared ptr
		 * @return Return calculated hash of TSharedPtr
		 */
		FORCEINLINE std::size_t operator()( const TSharedPtr& sharedPtr ) const
		{
			return sharedPtr.sharedReferenceCount.GetHash();
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
	 * @param sharedPtr		Shared ptr
	 */
	template<typename TOtherType>
	FORCEINLINE TSharedPtr( const TSharedPtr<TOtherType>& sharedPtr )
		: sharedReferenceCount( sharedPtr.sharedReferenceCount )
	{}

	/**
	 * @brief Constructor
	 * @param sharedPtr		Shared ptr
	 */
	FORCEINLINE TSharedPtr( const TSharedPtr& sharedPtr )
		: sharedReferenceCount( sharedPtr.sharedReferenceCount )
	{}

	/**
	 * @brief Constructor
	 * @param weakPtr		Weak ptr
	 */
	template<typename TOtherType>
	FORCEINLINE TSharedPtr( const TWeakPtr<TOtherType>& weakPtr )
		: sharedReferenceCount( weakPtr.weakReferenceCount )
	{}

	/**
	 * @brief Constructor
	 * @param weakPtr		Weak ptr
	 */
	FORCEINLINE TSharedPtr( const TWeakPtr<TObjectType>& weakPtr )
		: sharedReferenceCount( weakPtr.weakReferenceCount )
	{}

	/**
	 * @brief Constructor
	 * @param sharedPtr		Shared ptr
	 */
	template<typename TOtherType>
	FORCEINLINE TSharedPtr( TSharedPtr<TOtherType>&& sharedPtr )
		:  sharedReferenceCount( std::move( sharedPtr.sharedReferenceCount ) )
	{}

	/**
	 * @brief Constructor
	 * @param sharedPtr		Shared ptr
	 */
	FORCEINLINE TSharedPtr( TSharedPtr&& sharedPtr )
		: sharedReferenceCount( std::move( sharedPtr.sharedReferenceCount ) )
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
	 * @param sharedPtr		Shared ptr
	 * @return Return reference to current object
	 */
	template<typename TOtherType>
	FORCEINLINE TSharedPtr& operator=( const TSharedPtr<TOtherType>& sharedPtr )
	{
		sharedReferenceCount = sharedPtr.sharedReferenceCount;
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 *
	 * @param sharedPtr		Shared ptr
	 * @return Return reference to current object
	 */
	FORCEINLINE TSharedPtr& operator=( const TSharedPtr& sharedPtr )
	{
		sharedReferenceCount = sharedPtr.sharedReferenceCount;
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 *
	 * @param weakPtr		Weak ptr
	 * @return Return reference to current object
	 */
	template<typename TOtherType>
	FORCEINLINE TSharedPtr& operator=( const TWeakPtr<TOtherType>& weakPtr )
	{
		sharedReferenceCount = weakPtr.weakReferenceCount;
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 *
	 * @param weakPtr		Weak ptr
	 * @return Return reference to current object
	 */
	FORCEINLINE TSharedPtr& operator=( const TWeakPtr<TObjectType>& weakPtr )
	{
		sharedReferenceCount = weakPtr.weakReferenceCount;
		return *this;
	}

	/**
	 * @brief Overloaded operator 
	 * 
	 * @param sharedPtr		Shared ptr
	 * @return Return reference to current object
	 */
	template<typename TOtherType>
	FORCEINLINE TSharedPtr& operator=( TSharedPtr<TOtherType>&& sharedPtr )
	{
		if ( this != ( TSharedPtr<TObjectType>* )&sharedPtr )
		{
			sharedReferenceCount = std::move( sharedPtr.sharedReferenceCount );
		}
		return *this;
	}

	/**
	 * @brief Overloaded operator
	 *
	 * @param sharedPtr		Shared ptr
	 * @return Return reference to current object
	 */
	FORCEINLINE TSharedPtr& operator=( TSharedPtr&& sharedPtr )
	{
		if ( this != &sharedPtr )
		{
			sharedReferenceCount = std::move( sharedPtr.sharedReferenceCount );
		}
		return *this;
	}

	/**
	 * @brief Overloaded operator ==
	 *
	 * @param pObject	Object
	 * @return Returning TRUE if pointers is equal, else returning FALSE
	 */
	template<typename TOtherType>
	FORCEINLINE bool operator==( TOtherType* pObject ) const
	{
		return Get() == pObject;
	}

	/**
	 * @brief Overloaded operator ==
	 *
	 * @param sharedPtr		Shared ptr
	 * @return Returning TRUE if pointers is equal, else returning FALSE
	 */
	template<typename TOtherType>
	FORCEINLINE bool operator==( const TSharedPtr<TOtherType>& sharedPtr ) const
	{
		return Get() == sharedPtr.Get();
	}

	/**
	 * @brief Overloaded operator !=
	 *
	 * @param pObject	Object
	 * @return Returning TRUE if pointers is not equal, else returning FALSE
	 */
	template<typename TOtherType>
	FORCEINLINE bool operator!=( TOtherType* pObject ) const
	{
		return Get() != pObject;
	}

	/**
	 * @brief Overloaded operator !=
	 *
	 * @param sharedPtr		Shared ptr
	 * @return Returning TRUE if pointers is not equal, else returning FALSE
	 */
	template<typename TOtherType>
	FORCEINLINE bool operator!=( const TSharedPtr<TOtherType>& sharedPtr ) const
	{
		return Get() != sharedPtr.Get();
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
	FORCEINLINE TObjectType* operator->() const
	{
		Assert( IsValid() );
		return Get();
	}

	/**
	 * @brief Overloaded operator *
	 * @return Return reference to PtrType value
	 */
	FORCEINLINE TObjectType& operator*()
	{
		Assert( IsValid() );
		return *Get();
	}

	/**
	 * @brief Get hash
	 * @return Return hash
	 */
	FORCEINLINE hash_t GetHash() const
	{
		return sharedReferenceCount.GetHash();
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
		*this = TSharedPtr<TObjectType>();
	}

	/**
	 * @brief Get pointer
	 * @return Return pointer, if not set returned NULL
	 */
	FORCEINLINE TObjectType* Get() const
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
	template<typename TObjectType, typename... TArgTypes>
	friend TSharedPtr<TObjectType> MakeSharedPtr( TArgTypes&&... InArgs );

	// Declare other smart pointer types as friends as needed
	template<class TOtherType> friend class TSharedPtr;
	template<class TOtherType> friend class TWeakPtr;

protected:
	/**
	 * @brief Constructor
	 * @param pObject		Object
	 */
	template<typename TOtherType>
	FORCEINLINE TSharedPtr( TOtherType* pObject )
		: sharedReferenceCount( std::move( SharedPointerInternals::NewReferenceController( ( TObjectType* )pObject ) ) )
	{
		// If the object happens to be derived from TSharedFromThis, the following method
		// will prime the object with a weak pointer to itself
		SharedPointerInternals::EnableSharedFromThis( this, pObject );
	}

	SharedPointerInternals::TSharedReferencer<TObjectType>		sharedReferenceCount;		/**< Shared reference count */
};

/**
 * @ingroup stdlib
 * @brief Make shared pointer
 *
 * @param args		Arguments for construct object
 * @return Return created shared pointer with allocated object
 */
template<typename TObjectType, typename... TArgTypes>
FORCEINLINE TSharedPtr<TObjectType> MakeSharedPtr( TArgTypes&&... args )
{
	return TSharedPtr<TObjectType>( new TObjectType( args... ) );
}

/**
 * @ingroup stdlib
 * @brief TWeakPtr is a non-intrusive reference-counted weak object pointer
 */
template<class TObjectType>
class TWeakPtr
{
public:
	friend TSharedPtr<TObjectType>;

	/**
	 * @brief Hash function for STL containers
	 */
	struct HashFunction
	{
		/**
		 * @brief Calculate hash of TWeakPtr
		 * 
		 * @param weakPtr	Weak ptr
		 * @return Return calculated hash of TWeakPtr
		 */
		FORCEINLINE std::size_t operator()( const TWeakPtr& weakPtr ) const
		{
			return weakPtr.weakReferenceCount.GetHash();
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
	template<typename TOtherType>
	FORCEINLINE TWeakPtr( TWeakPtr<TOtherType>&& weakPtr )
		: weakReferenceCount( std::move( weakPtr.weakReferenceCount ) )
	{}

	/**
	 * @brief Constructor of move
	 */
	FORCEINLINE TWeakPtr( TWeakPtr&& weakPtr )
		: weakReferenceCount( std::move( weakPtr.weakReferenceCount ) )
	{}

	/**
	 * @brief Constructs a weak pointer from a shared pointer
	 * @param sharedPtr		The shared pointer to create a weak pointer from
	 */
	template<typename TOtherType>
	FORCEINLINE TWeakPtr( const TSharedPtr<TOtherType>& sharedPtr )
		: weakReferenceCount( sharedPtr.sharedReferenceCount )
	{}

	/**
	 * @brief Constructs a weak pointer from a shared pointer
	 * @param sharedPtr		The shared pointer to create a weak pointer from
	 */
	FORCEINLINE TWeakPtr( const TSharedPtr<TObjectType>& sharedPtr )
		: weakReferenceCount( sharedPtr.sharedReferenceCount )
	{}

	/**
	 * @brief Constructs a weak pointer from a weak pointer of another type
	 * @param weakPtr		The weak pointer to create a weak pointer from
	 */
	template<typename TOtherType>
	FORCEINLINE TWeakPtr( const TWeakPtr<TOtherType>& weakPtr )
		: weakReferenceCount( weakPtr.weakReferenceCount )
	{}

	/**
	 * @brief Constructs a weak pointer from a weak pointer of another type
	 * @param weakPtr		The weak pointer to create a weak pointer from
	 */
	FORCEINLINE TWeakPtr( const TWeakPtr& weakPtr )
		: weakReferenceCount( weakPtr.weakReferenceCount )
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
	 * @param weakPtr	The weak pointer for the object to assign
	 */
	template<typename TOtherType>
	FORCEINLINE TWeakPtr& operator=( const TWeakPtr<TOtherType>& weakPtr )
	{
		weakReferenceCount = weakPtr.weakReferenceCount;
		return *this;
	}

	/**
	 * @brief Assignment operator adds a weak reference to the object referenced by the specified weak pointer.
	 * This assignment operator is intended to allow derived-to-base conversions.
	 *
	 * @param weakPtr	The weak pointer for the object to assign
	 */
	FORCEINLINE TWeakPtr& operator=( const TWeakPtr& weakPtr )
	{
		weakReferenceCount = weakPtr.weakReferenceCount;
		return *this;
	}

	/**
	 * @brief Assignment operator sets this weak pointer from a shared pointer
	 * @param sharedPtr		The shared pointer used to assign to this weak pointer
	 */
	template<typename TOtherType>
	FORCEINLINE TWeakPtr& operator=( const TSharedPtr<TOtherType >& sharedPtr )
	{
		weakReferenceCount = sharedPtr.sharedReferenceCount;
		return *this;
	}

	/**
	 * @brief Assignment operator sets this weak pointer from a shared pointer
	 * @param sharedPtr		The shared pointer used to assign to this weak pointer
	 */
	FORCEINLINE TWeakPtr& operator=( const TSharedPtr<TObjectType>& sharedPtr )
	{
		weakReferenceCount = sharedPtr.sharedReferenceCount;
		return *this;
	}

	/**
	 * @brief Overloaded operator
	 *
	 * @param weakPtr	Weak ptr
	 * @return Return reference to current object
	 */
	template<typename TOtherType>
	FORCEINLINE TWeakPtr& operator=( TWeakPtr<TOtherType>&& weakPtr )
	{
		if ( this != ( TWeakPtr<TObjectType>* )&weakPtr )
		{
			weakReferenceCount = std::move( weakPtr.weakReferenceCount );
		}
		return *this;
	}

	/**
	 * @brief Overloaded operator
	 *
	 * @param weakPtr	Weak ptr
	 * @return Return reference to current object
	 */
	FORCEINLINE TWeakPtr& operator=( TWeakPtr&& weakPtr )
	{
		if ( this != &weakPtr )
		{
			weakReferenceCount = std::move( weakPtr.weakReferenceCount );
		}
		return *this;
	}

	/**
	 * @brief Overloaded operator ==
	 *
	 * @param weakPtr		Weak ptr
	 * @return Returning TRUE if pointers is equal, else returning FALSE
	 */
	template<typename TOtherType>
	FORCEINLINE bool operator==( const TWeakPtr<TOtherType>& weakPtr ) const
	{
		return Pin().Get() == weakPtr.Pin().Get();
	}

	/**
	 * @brief Overloaded operator ==
	 *
	 * @param sharedPtr		Shared ptr
	 * @return Returning TRUE if pointers is equal, else returning FALSE
	 */
	template<typename TOtherType>
	FORCEINLINE bool operator==( const TSharedPtr<TOtherType>& sharedPtr ) const
	{
		return Pin().Get() == sharedPtr.Get();
	}

	/**
	 * @brief Overloaded operator !=
	 *
	 * @param weakPtr		Weak ptr
	 * @return Returning TRUE if pointers is not equal, else returning FALSE
	 */
	template<typename TOtherType>
	FORCEINLINE bool operator!=( const TWeakPtr<TOtherType>& weakPtr ) const
	{
		return Pin().Get() != weakPtr.Pin().Get();
	}

	/**
	 * @brief Overloaded operator !=
	 *
	 * @param sharedPtr		Shared ptr
	 * @return Returning TRUE if pointers is equal, else returning FALSE
	 */
	template<typename TOtherType>
	FORCEINLINE bool operator!=( const TSharedPtr<TOtherType>& sharedPtr ) const
	{
		return Pin().Get() != sharedPtr.Get();
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
	FORCEINLINE TSharedPtr<TObjectType> Pin() const
	{
		return IsValid() ? TSharedPtr<TObjectType>( *this ) : TSharedPtr<TObjectType>();
	}

	/**
	 * @brief Get hash
	 * @return Return hash
	 */
	FORCEINLINE uint64 GetHash() const
	{
		return weakReferenceCount.GetHash();
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
		*this = TWeakPtr<TObjectType>();
	}

	/**
	 * @brief Has same object
	 * 
	 * @param pOtherPtr		Pointer to check
	 * @return Returns TRUE if the object this weak pointer points to is the same as the specified object pointer
	 */
	FORCEINLINE bool HasSameObject( const void* pOtherPtr ) const
	{
		return Pin().Get() == pOtherPtr;
	}

	// Declare other smart pointer types as friends as needed
	template<class TOtherType> friend class TSharedPtr;
	template<class TOtherType> friend class TWeakPtr;

protected:
	SharedPointerInternals::TWeakReferencer<TObjectType>		weakReferenceCount;		/**< Weak reference count */
};

/**
 * @ingroup stdlib
 * @brief Derive your class from TSharedFromThis to enable access to a TSharedPtr directly from an object
 * instance that's already been allocated
 */
template<class TObjectType>
class TSharedFromThis
{
public:
	/**
	 * @brief Provides access to a shared reference to this object
	 * 
	 * Note that is only valid to call this after a shared pointer to the object has already been created.
	 * Also note that it is illegal to call this in the object's destructor.
	 *
	 * @return Returns this object as a shared pointer
	 */
	TSharedPtr<TObjectType> AsShared()
	{
		TSharedPtr<TObjectType>	sharedThis = weakThis.Pin();

		// If the following assert goes off, it means one of the following:
		//
		//     - You tried to request a shared pointer before the object was ever assigned to one. (e.g. constructor)
		//     - You tried to request a shared pointer while the object is being destroyed (destructor chain)
		//
		// To fix this, make sure you create at least one shared reference to your object instance before requested,
		// and also avoid calling this function from your object's destructor
		Assert( sharedThis.Get() == this );

		// Now that we've verified the shared pointer is valid, we return it!
		return sharedThis;
	}

	/**
	 * @brief Provides access to a shared reference to this object (const)
	 * 
	 * Note that is only valid to call this after a shared pointer to the object has already been created.
	 * Also note that it is illegal to call this in the object's destructor.
	 *
	 * @return Returns this object as a shared pointer (const)
	 */
	TSharedPtr<const TObjectType> AsShared() const
	{
		TSharedPtr<const TObjectType>	sharedThis = weakThis.Pin();

		// If the following assert goes off, it means one of the following:
		//
		//     - You tried to request a shared pointer before the object was ever assigned to one. (e.g. constructor)
		//     - You tried to request a shared pointer while the object is being destroyed (destructor chain)
		//
		// To fix this, make sure you create at least one shared reference to your object instance before requested,
		// and also avoid calling this function from your object's destructor
		Assert( sharedThis.Get() == this );

		// Now that we've verified the shared pointer is valid, we return it!
		return sharedThis;
	}

	/**
	 * @brief Provides a weak reference to this object
	 * 
	 * Note that is only valid to call this after a shared pointer to the object has already been created.
	 * Also note that it is illegal to call this in the object's destructor.
	 *
	 * @return Returns this object as a shared pointer
	 */
	TWeakPtr<TObjectType> AsWeak()
	{
		TWeakPtr<TObjectType>	result = weakThis;

		// If the following assert goes off, it means one of the following:
		//
		//     - You tried to request a weak pointer before the object was ever assigned to a shared pointer. (e.g. constructor)
		//     - You tried to request a weak pointer while the object is being destroyed (destructor chain)
		//
		// To fix this, make sure you create at least one shared reference to your object instance before requested,
		// and also avoid calling this function from your object's destructor
		Assert( result.Pin().Get() == this );

		// Now that we've verified the pointer is valid, we'll return it!
		return result;
	}

	/**
	 * @brief Provides a weak reference to this object (const)
	 * 
	 * Note that is only valid to call this after a shared pointer to the object has already been created.
	 * Also note that it is illegal to call this in the object's destructor.
	 *
	 * @return Returns this object as a shared pointer (const.)
	 */
	TWeakPtr<const TObjectType> AsWeak() const
	{
		TWeakPtr<const TObjectType>		result = weakThis;

		// If the following assert goes off, it means one of the following:
		//
		//     - You tried to request a weak pointer before the object was ever assigned to a shared pointer. (e.g. constructor)
		//     - You tried to request a weak pointer while the object is being destroyed (destructor chain)
		//
		// To fix this, make sure you create at least one shared reference to your object instance before requested,
		// and also avoid calling this function from your object's destructor
		Assert( result.Pin().Get() == this );

		// Now that we've verified the pointer is valid, we'll return it!
		return result;
	}

protected:
	/**
	 * @brief Provides access to a shared reference to an object, given the object's 'this' pointer
	 * 
	 * Uses the 'this' pointer to derive the object's actual type, then casts and returns an appropriately
	 * typed shared reference.  Intentionally declared 'protected', as should only be called when the
	 * 'this' pointer can be passed.
	 *
	 * @param pThisPtr		Pointer to this ptr
	 * @return Returns this object as a shared pointer
	 */
	template<class TOtherType>
	FORCEINLINE static TSharedPtr<TOtherType> SharedThis( TOtherType* pThisPtr )
	{
		return ( TSharedPtr<TOtherType> )pThisPtr->AsShared();
	}

	/**
	 * @brief Provides access to a shared reference to an object, given the object's 'this' pointer
	 * 
	 * Uses the 'this' pointer to derive the object's actual type, then casts and returns an appropriately
	 * typed shared reference.  Intentionally declared 'protected', as should only be called when the
	 * 'this' pointer can be passed.
	 *
	 * @param pThisPtr		Pointer to this ptr
	 * @return Returns this object as a shared pointer (const)
	 */
	template<class TOtherType>
	FORCEINLINE static TSharedPtr<const TOtherType> SharedThis( const TOtherType* pThisPtr )
	{
		return ( TSharedPtr<const TOtherType> )pThisPtr->AsShared();
	}

public:		// Ideally this would be private, but template sharing problems prevent it
	/**
	 * @brief Freshens the internal weak pointer object using
	 * @warning Internal use only, do not call this method
	 * 
	 * The supplied object pointer along with the authoritative shared reference to the object.
	 * Note that until this function is called, calls to AsShared() will result in an empty pointer
	 * 
	 * @param pSharedPtr	Pointer to shared ptr
	 */
	template<class TSharedPtrType>
	FORCEINLINE void UpdateWeakReferenceInternal( const TSharedPtr<TSharedPtrType>* pSharedPtr ) const
	{
		if ( !weakThis.IsValid() )
		{
			weakThis = TSharedPtr<TObjectType>( *pSharedPtr );
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
	mutable TWeakPtr<TObjectType>		weakThis;	/**< Weak reference to ourselves */
};

#endif // SHAREDPOINTER_H