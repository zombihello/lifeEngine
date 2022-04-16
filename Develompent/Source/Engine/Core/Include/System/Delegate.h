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
	 * Typedef of function delegate for callback
	 */
	typedef void ( *FDelegateFunction )( TParamTypes... );

	/**
	 * Typedef of method delegate for callback
	 */
	typedef void ( LObject::*FDelegateMethod )( TParamTypes... );

	/**
	 * Add delegate
	 * @param[in] InDelegate Delegate
	 */
	FORCEINLINE void Add( const FDelegateFunction& InDelegate )
	{
		FScopeLock		scopeLock( criticalSection );
		delegateFunctions.push_back( InDelegate );
	}

	/**
	 * Add delegate
	 * 
	 * @param[in] InObject Object
	 * @param[in] InDelegate Delegate
	 */
	FORCEINLINE void Add( LObject* InObject, const FDelegateMethod& InDelegate )
	{
		FScopeLock		scopeLock( criticalSection );
		delegateMethods.push_back( FDelegateMethodInfo{ InObject, InDelegate } );
	}

	/**
	 * Add delegate
	 *
	 * @param[in] InObject Object
	 * @param[in] InDelegate Delegate
	 */
	template< class TClass, typename TMethod >
	FORCEINLINE void Add( TClass* InObject, const TMethod& InDelegate )
	{
		Add( ( LObject* )InObject, ( FDelegateMethod )InDelegate );
	}

	/**
	 * Remove delegate
	 * @param[in] InDelegate Delegate
	 */
	FORCEINLINE void Remove( const FDelegateFunction& InDelegate )
	{
		FScopeLock		scopeLock( criticalSection );
		for ( uint32 index = 0, count = delegateFunctions.size(); index < count; ++index )
		{
			if ( delegateFunctions[ index ] == InDelegate )
			{
				delegateFunctions.erase( delegateFunctions.begin() + index );
				return;
			}
		}
	}

	/**
	 * Remove delegate
	 * 
	 * @param[in] InObject Object
	 * @param[in] InDelegate Delegate
	 */
	FORCEINLINE void Remove( LObject* InObject, const FDelegateMethod& InDelegate )
	{
		FScopeLock		scopeLock( criticalSection );
		for ( uint32 index = 0, count = delegateMethods.size(); index < count; ++index )
		{
			const FDelegateMethodInfo&		delegateInfo = delegateMethods[ index ];
			if ( delegateInfo.object == InObject && delegateInfo.method == InDelegate )
			{
				delegateMethods.erase( delegateMethods.begin() + index );
				return;
			}
		}
	}

	/**
	 * Remove delegate
	 *
	 * @param[in] InObject Object
	 * @param[in] InDelegate Delegate
	 */
	template< class TClass, typename TMethod >
	FORCEINLINE void Remove( TClass* InObject, const TMethod& InDelegate )
	{
		Remove( ( LObject* )InObject, ( FDelegateMethod )InDelegate );
	}

	/**
	 * Broadcast all delegates
	 * @param[in] InParams Params for call delegate
	 */
	FORCEINLINE void Broadcast( TParamTypes... InParams ) const
	{
		FScopeLock		scopeLock( criticalSection );
		
		// Call function delegates
		if ( !delegateFunctions.empty() )
		{
			for ( uint32 index = 0, count = delegateFunctions.size(); index < count; ++index )
			{
				delegateFunctions[ index ]( InParams... );
			}
		}

		// Call method delegates
		if ( !delegateMethods.empty() )
		{
			for ( uint32 index = 0, count = delegateMethods.size(); index < count; ++index )
			{
				const FDelegateMethodInfo& delegateInfo = delegateMethods[ index ];
				( delegateInfo.object->*delegateInfo.method )( InParams... );
			}
		}
	}

private:
	/**
	 * Struct of info about delegate method
	 */
	struct FDelegateMethodInfo
	{
		LObject*			object;		/**< Pointer to object */
		FDelegateMethod		method;		/**< Pointer to method */
	};

	std::vector< FDelegateFunction >			delegateFunctions;		/**< Array of function delegates */
	std::vector< FDelegateMethodInfo >			delegateMethods;		/**< Array of method delegates */
	mutable FCriticalSection					criticalSection;		/**< Critical section for thread safe broadcast */
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
	 * Typedef of function delegate for callback
	 */
	typedef void ( *FDelegateFunction )( TParamTypes... );

	/**
	 * Typedef of method delegate for callback
	 */
	typedef void ( LObject::*FDelegateMethod )( TParamTypes... );

	/**
	 * Constructor
	 */
	FORCEINLINE TDelegate()
		: type( DT_None ), delegate( nullptr )
	{}

	/**
	 * Destructor
	 */
	FORCEINLINE ~TDelegate()
	{
		Unbind();
	}

	/**
	 * Bind delegate
	 * 
	 * @param InDelegate Delegate
	 */
	FORCEINLINE void BindFunction( const FDelegateFunction& InDelegate )
	{
		FScopeLock		scopeLock( criticalSection );
		if ( type != DT_Function )
		{
			Unbind();
		}

		if ( !delegate )
		{
			delegate = new FDelegateFunction;
		}

		*static_cast< FDelegateFunction* >( delegate ) = InDelegate;
		type = DT_Function;
	}

	/**
	 * Bind delegate
	 *
	 * @param InObject Object
	 * @param InDelegate Delegate
	 */
	FORCEINLINE void BindMethod( LObject* InObject, const FDelegateMethod& InDelegate )
	{
		FScopeLock		scopeLock( criticalSection );
		if ( type != DT_Method )
		{
			Unbind();
		}

		if ( !delegate )
		{
			delegate = new FDelegateMethodInfo();
		}

		*static_cast< FDelegateMethodInfo* >( delegate ) = FDelegateMethodInfo{ InObject, InDelegate };
		type = DT_Method;
	}

	/**
	 * Bind delegate
	 *
	 * @param InObject Object
	 * @param InDelegate Delegate
	 */
	template< class TClass, typename TMethod >
	FORCEINLINE void BindMethod( TClass* InObject, const TMethod& InDelegate )
	{
		BindMethod( ( LObject* )InObject, ( FDelegateMethod )InDelegate );
	}

	/**
	 * Unbind delegate
	 */
	FORCEINLINE void Unbind()
	{
		FScopeLock		scopeLock( criticalSection );
		if ( !delegate )
		{
			return;
		}

		switch ( type )
		{
		case DT_Function:		delete static_cast< FDelegateFunction* >( delegate );		break;
		case DT_Method:			delete static_cast< FDelegateMethodInfo* >( delegate );		break;
		}

		delegate = nullptr;
		type = DT_None;
	}

	/**
	 * Execute delegate
	 * 
	 * @param InParams Params for call delegate
	 */
	FORCEINLINE void Execute( TParamTypes... InParams )
	{
		FScopeLock		scopeLock( criticalSection );
		if ( !delegate )
		{
			return;
		}

		switch ( type )
		{
		case DT_Function:
			( *static_cast< FDelegateFunction* >( delegate ) )( InParams... );
			break;

		case DT_Method:
			FDelegateMethodInfo*		delegateInfo = ( FDelegateMethodInfo* )delegate;
			( delegateInfo->object->*delegateInfo->method )( InParams... );
			break;
		}
	}

private:
	/**
	 * Enumeration of delegate type
	 */
	enum EDelegateType
	{
		DT_Function,							/**< Function delegate */
		DT_Method,								/**< Method delegate */
		DT_None									/**< Not valid delegate type */
	};

	/**
	 * Struct of info about delegate method
	 */
	struct FDelegateMethodInfo
	{
		LObject*			object;				/**< Pointer to object */
		FDelegateMethod		method;				/**< Pointer to method */
	};

	EDelegateType		type;					/**< Delegate type */
	void*				delegate;				/**< Pointer to delegate */
	FCriticalSection	criticalSection;		/**< Critical section for thread safe execute */
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