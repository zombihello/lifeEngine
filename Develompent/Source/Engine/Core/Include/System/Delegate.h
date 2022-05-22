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
#include "Misc/Object.h"
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
	 * Typedef of delegate type
	 */
	typedef std::function<void( TParamTypes... )>		FDelegateType;

	/**
	 * Add delegate
	 * @param InDelegate	Delegate
	 */
	FORCEINLINE FDelegateType* Add( const FDelegateType& InDelegate )
	{
		FScopeLock		scopeLock( criticalSection );
		delegates.push_back( InDelegate );
		return &delegates.back();
	}

	/**
	 * Remove delegate
	 * @param InDelegate		Delegate
	 */
	FORCEINLINE void Remove( FDelegateType*& InDelegate )
	{
		if ( !InDelegate )
		{
			return;
		}

		FScopeLock		scopeLock( criticalSection );
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
		FScopeLock		scopeLock( criticalSection );	
		for ( auto itDelegate = delegates.begin(), itDelegateEnd = delegates.end(); itDelegate != itDelegateEnd; ++itDelegate )
		{
			( *itDelegate )( InParams... );
		}
	}

private:
	std::list< FDelegateType >		delegates;				/**< List of delegates */
	mutable FCriticalSection		criticalSection;		/**< Critical section for thread safe broadcast */
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
	typedef std::function<void( TParamTypes... )>		FDelegateType;
	
	/**
	 * Bind delegate
	 * 
	 * @param InDelegate	Delegate
	 */
	FORCEINLINE void Bind( const FDelegateType& InDelegate )
	{
		FScopeLock		scopeLock( criticalSection );
		delegate = InDelegate;
	}

	/**
	 * Unbind delegate
	 */
	FORCEINLINE void Unbind()
	{
		FScopeLock		scopeLock( criticalSection );
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

		FScopeLock		scopeLock( criticalSection );
		delegate( InParams... );
	}

private:
	FDelegateType				delegate;				/**< Delegate */
	mutable FCriticalSection	criticalSection;		/**< Critical section for thread safe execute */
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