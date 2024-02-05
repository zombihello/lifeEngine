/**
 * @file
 * @addtogroup Core Core
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

#ifndef DELEGATE_H
#define DELEGATE_H

#include <functional>
#include <list>

#include "Core/Core.h"
#include "Core/Misc/Object.h"
#include "Core/System/ThreadingBase.h"

/**
 * @ingroup Core
 * Multicast delegate
 */
template< typename... TParamTypes >
class TMulticastDelegate
{
public:
	/**
	 * Typedef of delegate type
	 */
	typedef std::function<void( TParamTypes... )>		DelegateType_t;

	/**
	 * Add delegate
	 * @param InDelegate	Delegate
	 */
	FORCEINLINE DelegateType_t* Add( const DelegateType_t& InDelegate )
	{
		CScopeLock		scopeLock( criticalSection );
		delegates.push_back( InDelegate );
		return &delegates.back();
	}

	/**
	 * Remove delegate
	 * @param InDelegate		Delegate
	 */
	FORCEINLINE void Remove( DelegateType_t*& InDelegate )
	{
		if ( !InDelegate )
		{
			return;
		}

		CScopeLock		scopeLock( criticalSection );
		for ( auto itDelegate = delegates.begin(), itDelegateEnd = delegates.end(); itDelegate != itDelegateEnd; ++itDelegate )
		{
			if ( &( *itDelegate ) == InDelegate )
			{
				delegates.erase( itDelegate );
				InDelegate = nullptr;
				return;
			}
		}
	}

	/**
	 * Broadcast all delegates
	 * @param[in] InParams Params for call delegate
	 */
	FORCEINLINE void Broadcast( TParamTypes... InParams ) const
	{
		// Call delegates
		CScopeLock		scopeLock( criticalSection );	
		for ( auto itDelegate = delegates.begin(), itDelegateEnd = delegates.end(); itDelegate != itDelegateEnd; ++itDelegate )
		{
			( *itDelegate )( InParams... );
		}
	}

private:
	std::list< DelegateType_t >		delegates;				/**< List of delegates */
	mutable CCriticalSection		criticalSection;		/**< Critical section for thread safe broadcast */
};

/**
 * @ingroup Core
 * Single cast delegate
 */
template< typename... TParamTypes >
class TDelegate
{
public:
	/**
	 * Typedef of delegate type
	 */
	typedef std::function<void( TParamTypes... )>		DelegateType_t;
	
	/**
	 * Bind delegate
	 * 
	 * @param InDelegate	Delegate
	 */
	FORCEINLINE void Bind( const DelegateType_t& InDelegate )
	{
		CScopeLock		scopeLock( criticalSection );
		delegate = InDelegate;
	}

	/**
	 * Unbind delegate
	 */
	FORCEINLINE void Unbind()
	{
		CScopeLock		scopeLock( criticalSection );
		delegate = nullptr;
	}

	/**
	 * Execute delegate
	 * 
	 * @param InParams Params for call delegate
	 */
	FORCEINLINE void Execute( TParamTypes... InParams )
	{
		if ( !delegate )
		{
			return;
		}

		CScopeLock		scopeLock( criticalSection );
		delegate( InParams... );
	}

private:
	DelegateType_t				delegate;				/**< Delegate */
	mutable CCriticalSection	criticalSection;		/**< Critical section for thread safe execute */
};

/**
 * @ingroup Core
 * Macro for declare multi cast delegate
 * 
 * @param[in] InDelegateName Delegate name
 * @param[in] ... Other parameters of delegate
 */
#define DECLARE_MULTICAST_DELEGATE( InDelegateName, ... )	\
	typedef TMulticastDelegate< __VA_ARGS__ >			InDelegateName;

 /**
  * @ingroup Core
  * Macro for declare single cast delegate
  *
  * @param[in] InDelegateName Delegate name
  * @param[in] ... Other parameters of delegate
  */
#define DECLARE_DELEGATE( InDelegateName, ... )	\
	typedef TDelegate< __VA_ARGS__ >					InDelegateName;

#endif // !DELEGATE_H