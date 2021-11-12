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
#include "ThreadingBase.h"

/**
 * @ingroup Core
 * Multicast delegate
 */
template< typename... TParamTypes >
class TMulticastDelegate
{
public:
	/**
	 * Typedef of function delegate for callback
	 */
	typedef std::function< void( TParamTypes... ) >			FDelegate;

	/**
	 * Add delegate
	 * @param[in] InDelegate Delegate
	 */
	FORCEINLINE void Add( const FDelegate& InDelegate )
	{
		FScopeLock		scopeLock( criticalSection );
		delegates.push_back( InDelegate );
	}

	/**
	 * Remove delegate
	 * @param[in] InDelegate Delegate
	 */
	FORCEINLINE void Remove( const FDelegate& InDelegate )
	{
		FScopeLock		scopeLock( criticalSection );
		for ( auto it = delegates.begin(), itEnd = delegates.end(); it != itEnd; ++it )
		{
			if ( ( *it ) == InDelegate )
			{
				delegates.erase( it );
				criticalSection.Unlock();
				return;
			}
		}
	}

	/**
	 * Broadcast all delegates
	 * @param[in] InParams Params for call delegate
	 */
	FORCEINLINE void Broadcast( TParamTypes... InParams )
	{
		FScopeLock		scopeLock( criticalSection );
		for ( auto it = delegates.begin(), itEnd = delegates.end(); it != itEnd; ++it )
		{
			( *it )( InParams... );
		}
	}

private:
	std::vector< FDelegate >			delegates;			/**< Array of delegates */
	FCriticalSection					criticalSection;	/**< Critical section for thread safe broadcast */
};

/**
 * @ingroup Core
 * Macro for declare delegate
 * 
 * @param[in] InDelegateName Delegate name
 * @param[in] ... Other parameters of delegate
 */
#define DECLARE_DELEGATE( InDelegateName, ... )	\
	typedef TMulticastDelegate< __VA_ARGS__ >			InDelegateName;

#endif // !DELEGATE_H