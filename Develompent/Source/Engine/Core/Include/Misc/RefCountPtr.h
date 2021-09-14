/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef REFCOUNTPTR_H
#define REFCOUNTPTR_H

#include "Core.h"
#include "Misc/Types.h"

/**
 * @ingroup Core
 * @brief Reference-counting pointer class
 */
template< typename PtrType >
class TRefCountPtr
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE							TRefCountPtr() : ptr( nullptr )
	{}

	/**
	 * @brief Constructor
	 * @param[in] InPtr Pointer to PtrType value
	 */
	FORCEINLINE							TRefCountPtr( PtrType* InPtr ) : ptr( InPtr )
	{
		if ( ptr )
		{
			ptr->AddRef();
		}
	}

	/**
	 * @brief Constructor of copy
	 * @param[in] InCopy Copy
	 */
	FORCEINLINE							TRefCountPtr( const TRefCountPtr& InCopy ) : ptr( InCopy.ptr )
	{
		if ( ptr )
		{
			ptr->AddRef();
		}
	}

	/**
	 * @brief Constructor of copy
	 * @param[in] InCopy Copy
	 */
	template< typename BasePtrType >
	FORCEINLINE							TRefCountPtr( const TRefCountPtr< BasePtrType >& InCopy ) : ptr( static_cast< PtrType* >( InCopy.GetPtr() ) )
	{
		if ( ptr )
		{
			ptr->AddRef();
		}
	}

	/**
	 * @brief Destructor
	 */
	FORCEINLINE							~TRefCountPtr()
	{
		if ( ptr )
		{
			ptr->ReleaseRef();
		}
	}

	/**
	 * @brief Overloaded operator =
	 * @param[in] InCopy Pointer to PtrType value
	 * 
	 * @return Return reference to current object
	 */
	FORCEINLINE TRefCountPtr&			operator=( PtrType* InPtr )
	{
		if ( ptr )
		{
			ptr->ReleaseRef();
		}

		ptr = InPtr;

		if ( ptr )
		{
			ptr->AddRef();
		}
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 * @param[in] InCopy Copy
	 *
	 * @return Return reference to current object
	 */
	FORCEINLINE TRefCountPtr&			operator=( const TRefCountPtr& InCopy )
	{
		if ( ptr )
		{
			ptr->ReleaseRef();
		}

		ptr = InCopy.ptr;

		if ( ptr )
		{
			ptr->AddRef();
		}
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 * @param[in] InCopy Copy
	 *
	 * @return Return reference to current object
	 */
	template< typename BasePtrType >
	FORCEINLINE TRefCountPtr&			operator=( const TRefCountPtr< BasePtrType >& InCopy )
	{
		if ( ptr )
		{
			ptr->ReleaseRef();
		}

		ptr = static_cast< PtrType* >( InCopy.GetPtr() );

		if ( ptr )
		{
			ptr->AddRef();
		}
		return *this;
	}

	/**
	 * @brief Overloaded operator ==
	 * @param[in] InRight Right operand
	 *
	 * @return Returning true if pointers is equal, else returning false
	 */
	FORCEINLINE bool					operator==( const TRefCountPtr& InRight ) const 
	{ 
		return ptr == InRight.ptr; 
	}

	/**
	 * @brief Overloaded operator !=
	 * @param[in] InRight Right operand
	 *
	 * @return Returning true if pointers is not equal, else returning false
	 */
	FORCEINLINE bool					operator!=( const TRefCountPtr& InRight ) const 
	{ 
		return ptr != InRight.ptr; 
	}

	/**
	 * @brief Overloaded operator bool
	 *
	 * @return Returning true if pointer not null, else returning false
	 */
	FORCEINLINE							operator bool() const 
	{ 
		return ptr; 
	}

	/**
	 * @brief Overloaded operator PtrType&
	 *
	 * @return Returning reference on value
	 */
	FORCEINLINE							operator PtrType&() const
	{
		return *ptr;
	}

	/**
	 * @brief Overloaded operator PtrType*
	 * 
	 * @return Returning pointer to value
	 */
	FORCEINLINE							operator PtrType*() const
	{
		return ptr;
	}

	/**
	 * @brief Overloaded operator PtrType*&
	 *
	 * @return Returning reference to pointer value
	 */
	FORCEINLINE							operator PtrType*&()
	{
		return ptr;
	}

	/**
	 * @brief Overloaded operator ->
	 *
	 * @return Return pointer to PtrType value
	 */
	FORCEINLINE PtrType*				operator->() const
	{
		check( ptr );
		return ptr;
	}

	/**
	 * @brief Overloaded operator *
	 *
	 * @return Return reference to PtrType value
	 */
	FORCEINLINE PtrType&				operator*()
	{
		check( ptr );
		return *ptr;
	}

	/**
	 * @brief Overloaded operator &
	 *
	 * @return Return reference to pointer of PtrType value
	 */
	FORCEINLINE PtrType**				operator&()
	{
		check( ptr );
		return &ptr;
	}

	/**
	 * Safe release
	 */
	FORCEINLINE void SafeRelease()
	{
		*this = nullptr;
	}

	/**
	 * @brief Is valid pointer
	 *
	 * @return Returning true if pointer is valid, else returning false
	 */
	FORCEINLINE bool					IsValid() const 
	{ 
		return ptr; 
	}

	/**
	 * @brief Get reference count
	 *
	 * @return Return reference count
	 */
	FORCEINLINE uint32					GetRefCount() const
	{
		if ( !ptr )
		{
			return 0;
		}

		return ptr->GetRefCount();
	}

	/**
	 * @brief Get pointer
	 *
	 * @return Return pointer, if not set returned nullptr
	 */
	FORCEINLINE PtrType*				GetPtr() const 
	{ 
		return ptr; 
	}

private:
	PtrType*			ptr;		/**< Pointer to value */
};

#endif // !REFCOUNTPTR_H
