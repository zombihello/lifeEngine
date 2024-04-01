/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef DELEGATE_H
#define DELEGATE_H

#include <functional>
#include <list>

#include "Core.h"
#include "Reflection/Object.h"
#include "Threading.h"

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
		CScopeLock		scopeLock( mutex );
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

		CScopeLock		scopeLock( mutex );
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
		CScopeLock		scopeLock( mutex );
		for ( auto itDelegate = delegates.begin(), itDelegateEnd = delegates.end(); itDelegate != itDelegateEnd; ++itDelegate )
		{
			( *itDelegate )( InParams... );
		}
	}

private:
	std::list< DelegateType_t >		delegates;	/**< List of delegates */
	mutable CThreadMutex			mutex;		/**< Critical section for thread safe broadcast */
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
		CScopeLock		scopeLock( mutex );
		delegate = InDelegate;
	}

	/**
	 * Unbind delegate
	 */
	FORCEINLINE void Unbind()
	{
		CScopeLock		scopeLock( mutex );
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

		CScopeLock		scopeLock( mutex );
		delegate( InParams... );
	}

private:
	DelegateType_t			delegate;				/**< Delegate */
	mutable CThreadMutex	mutex;		/**< Critical section for thread safe execute */
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