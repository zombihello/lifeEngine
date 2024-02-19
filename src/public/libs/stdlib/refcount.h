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

#ifndef REFCOUNT_H
#define REFCOUNT_H

#include "stdlib/hashing/fasthash.h"
#include "core/types.h"
#include "core/platform.h"
#include "core/threading.h"

/**
 * @ingroup stdlib
 * @brief Standard reference counted interface
 */
class IRefCounted
{
public:
	/**
	 * @brief Increment reference count
	 */
	virtual void AddRef() = 0;

	/**
	 * @brief Decrement reference count and delete self if no more references
	 */
	virtual void ReleaseRef() = 0;

	/**
	 * @brief Get reference count
	 * @return Return reference count
	 */
	virtual uint32 GetRefCount() const = 0;
};

/**
 * @ingroup stdlib
 * @brief Base classes to implement reference counting
 */
template<class TBaseClass>
class TRefCounted : public TBaseClass
{
public:
	/**
	 * @brief Constructor
	 */
	TRefCounted()
		: countReferences( 0 )
	{}

	/**
	 * @brief Destructor
	 */
	virtual	~TRefCounted()
	{
		Assert( !countReferences );
	}

	/**
	 * @brief Increment reference count
	 */
	virtual void AddRef() override
	{
		Sys_InterlockedIncrement( ( int32* )&countReferences );
	}

	/**
	 * @brief Decrement reference count and delete self if no more references
	 */
	virtual void ReleaseRef() override
	{
		if ( !countReferences || !Sys_InterlockedDecrement( ( int32* )&countReferences ) )
		{
			delete this;
		}
	}

	/**
	 * @brief Get reference count
	 * @return Return reference count
	 */
	virtual uint32 GetRefCount() const override
	{
		return countReferences;
	}

private:
	uint32	countReferences;	/**< Count references on object */
};

/**
 * @ingroup stdlib
 * @brief Reference-counting pointer
 */
template<typename TPtrType>
class TRefPtr
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE	TRefPtr()
		: pPtr( nullptr )
	{}

	/**
	 * @brief Constructor
	 * @param pPtr	Pointer to TPtrType value
	 */
	FORCEINLINE TRefPtr( TPtrType* pPtr ) 
		: pPtr( pPtr )
	{
		if ( pPtr )
		{
			pPtr->AddRef();
		}
	}

	/**
	 * @brief Constructor of copy
	 * @param copy	Copy
	 */
	FORCEINLINE TRefPtr( const TRefPtr& copy ) 
		: pPtr( copy.pPtr )
	{
		if ( pPtr )
		{
			pPtr->AddRef();
		}
	}

	/**
	 * @brief Constructor of copy
	 * @param copy		Copy
	 */
	template<typename TBasePtrType>
	FORCEINLINE TRefPtr( const TRefPtr<TBasePtrType>& copy ) 
		: pPtr( ( TPtrType* )( copy.GetPtr() ) )
	{
		if ( pPtr )
		{
			pPtr->AddRef();
		}
	}

	/**
	 * @brief Destructor
	 */
	FORCEINLINE ~TRefPtr()
	{
		if ( pPtr )
		{
			pPtr->ReleaseRef();
		}
	}

	/**
	 * @brief Hash function for STL containers
	 */
	struct HashFunction
	{
		/**
		 * @brief Calculate hash of TRefPtr
		 * 
		 * @param refPtr	Reference to object
		 * @return Return calculated hash
		 */
		FORCEINLINE std::size_t operator()( const TRefPtr& refPtr ) const
		{
			return FastHash( refPtr );
		}
	};

	/**
	 * @brief Overloaded operator =
	 * 
	 * @param pPtr		Pointer to TPtrType value
	 * @return Return reference to current object
	 */
	FORCEINLINE TRefPtr& operator=( TPtrType* pPtr )
	{
		if ( TRefPtr::pPtr )
		{
			TRefPtr::pPtr->ReleaseRef();
		}

		TRefPtr::pPtr = pPtr;

		if ( TRefPtr::pPtr )
		{
			TRefPtr::pPtr->AddRef();
		}
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 * 
	 * @param copy	Copy
	 * @return Return reference to current object
	 */
	FORCEINLINE TRefPtr& operator=( const TRefPtr& copy )
	{
		if ( pPtr )
		{
			pPtr->ReleaseRef();
		}

		pPtr = copy.pPtr;

		if ( pPtr )
		{
			pPtr->AddRef();
		}
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 * 
	 * @param copy	Copy
	 * @return Return reference to current object
	 */
	template<typename TBasePtrType>
	FORCEINLINE TRefPtr& operator=( const TRefPtr<TBasePtrType>& copy )
	{
		if ( pPtr )
		{
			pPtr->ReleaseRef();
		}

		pPtr = ( TPtrType* )( copy.GetPtr() );

		if ( pPtr )
		{
			pPtr->AddRef();
		}
		return *this;
	}

	/**
	 * @brief Overloaded operator ==
	 * 
	 * @param right		Right operand
	 * @return Return TRUE if pointers is equal, FALSE otherwise
	 */
	FORCEINLINE bool operator==( const TRefPtr& right ) const
	{
		return pPtr == right.pPtr;
	}

	/**
	 * @brief Overloaded operator ==
	 * 
	 * @param pRight	Right operand
	 * @return Return TRUE if pointers is equal, FALSE otherwise
	 */
	FORCEINLINE bool operator==( TPtrType* pRight ) const
	{
		return pPtr == pRight;
	}

	/**
	 * @brief Overloaded operator !=
	 * 
	 * @param right		Right operand
	 * @return Return TRUE if pointers is not equal, FALSE otherwise
	 */
	FORCEINLINE bool operator!=( const TRefPtr& right ) const
	{
		return pPtr != right.pPtr;
	}

	/**
	 * @brief Overloaded operator !=
	 * 
	 * @param pRight	Right operand
	 * @return Return TRUE if pointers is not equal, FALSE otherwise
	 */
	FORCEINLINE bool operator!=( TPtrType* pRight ) const
	{
		return pPtr != pRight;
	}

	/**
	 * @brief Overloaded operator bool
	 * @return Return TRUE if pointer not null, FALSE otherwise
	 */
	FORCEINLINE operator bool() const
	{
		return !!pPtr;
	}

	/**
	 * @brief Overloaded operator ptrint
	 * @return Return address
	 */
	FORCEINLINE operator ptrint() const
	{
		return ( ptrint )pPtr;
	}

	/**
	 * @brief Overloaded operator uptrint
	 * @return Return address
	 */
	FORCEINLINE operator uptrint() const
	{
		return ( uptrint )pPtr;
	}

	/**
	 * @brief Overloaded operator TPtrType&
	 * @return Return reference on value
	 */
	FORCEINLINE operator TPtrType&() const
	{
		return *pPtr;
	}

	/**
	 * @brief Overloaded operator TPtrType*
	 * @return Return pointer to value
	 */
	FORCEINLINE operator TPtrType*() const
	{
		return pPtr;
	}

	/**
	 * @brief Overloaded operator TPtrType*&
	 * @return Return reference to pointer value
	 */
	FORCEINLINE operator TPtrType*&()
	{
		return pPtr;
	}

	/**
	 * @brief Overloaded operator ->
	 * @return Return pointer to TPtrType value
	 */
	FORCEINLINE TPtrType* operator->() const
	{
		Assert( pPtr );
		return pPtr;
	}

	/**
	 * @brief Overloaded operator *
	 * @return Return reference to TPtrType value
	 */
	FORCEINLINE TPtrType& operator*()
	{
		Assert( pPtr );
		return *pPtr;
	}

	/**
	 * @brief Overloaded operator &
	 * @return Return reference to pointer of TPtrType value
	 */
	FORCEINLINE TPtrType** operator&()
	{
		Assert( pPtr );
		return &pPtr;
	}

	/**
	 * @brief Safe release
	 */
	FORCEINLINE void SafeRelease()
	{
		*this = nullptr;
	}

	/**
	 * @brief Is valid pointer
	 * @return Return TRUE if pointer is valid, FALSE otherwise
	 */
	FORCEINLINE bool IsValid() const
	{
		return !!pPtr;
	}

	/**
	 * @brief Get reference count
	 * @return Return reference count
	 */
	FORCEINLINE uint32 GetRefCount() const
	{
		if ( !pPtr )
		{
			return 0;
		}

		return pPtr->GetRefCount();
	}

	/**
	 * @brief Get pointer
	 * @return Return pointer, if not set will return NULL
	 */
	FORCEINLINE TPtrType* GetPtr() const
	{
		return pPtr;
	}

private:
	TPtrType*	pPtr;		/**< Pointer to value */
};

#endif // !REFCOUNT_H