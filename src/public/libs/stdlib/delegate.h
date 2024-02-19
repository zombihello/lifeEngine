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

#ifndef DELEGATE_H
#define DELEGATE_H

#include <vector>

#include "core/platform.h"
#include "core/threading.h"

/**
 * @ingroup stdlib
 * @brief Multicast delegate interface
 */
template<typename... TParamTypes>
class IMulticastDelegate
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IMulticastDelegate() {}

	/**
	 * @brief Delegate type for functions
	 */
	typedef void ( *DelegateFuncFn_t )( void* /* pUserData */, TParamTypes... /* params */ );

	/**
	 * @brief Function delegate info
	 */
	struct FuncDelegate
	{
		void*				pUserData;	/**< Pointer to user data */
		DelegateFuncFn_t	pFunc;		/**< Pointer to delegate function */
	};

	/**
	 * @brief Add function delegate
	 * 
	 * @param pUserData		Pointer to user data
	 * @param pDelegate		Function delegate
	 * @return Return pointer to delegate handle
	 */
	virtual FuncDelegate* AddFunc( DelegateFuncFn_t pDelegateFn, void* pUserData = nullptr ) = 0;

	/**
	 * @brief Remove all function delegates
	 */
	virtual void RemoveAll() = 0;

	/**
	 * @brief Remove function delegate
	 * @param pDelegate		Delegate handle
	 */
	virtual void RemoveFunc( FuncDelegate* pDelegate ) = 0;

	/**
	 * @brief Broadcast all delegates
	 * @param params	Params for call delegate
	 */
	virtual void Broadcast( TParamTypes... params ) const = 0;
};

/**
 * @ingroup stdlib
 * @brief Single cast delegate interface
 */
template<typename... TParamTypes>
class IDelegate
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IDelegate() {}

	/**
	 * @brief Delegate type for functions
	 */
	typedef void ( *DelegateFuncFn_t )( void* /* pUserData */, TParamTypes... /* params */ );

	/**
	 * @brief Bind function delegate
	 *
	 * @param pDelegate		Delegate
	 * @param pUserData		Pointer to user data
	 */
	virtual void BindFunc( DelegateFuncFn_t pDelegateFn, void* pUserData = nullptr ) = 0;

	/**
	 * @brief Unbind delegate
	 */
	virtual void Unbind() = 0;

	/**
	 * @brief Execute delegate
	 * @param params	Params for call delegate
	 */
	virtual void Execute( TParamTypes... params ) = 0;
};

/**
 * @ingroup stdlib
 * @brief Multicast delegate
 */
template<typename... TParamTypes>
class TMulticastDelegate : public IMulticastDelegate<TParamTypes...>
{
public:
	/**
	 * @brief Add function delegate
	 *
	 * @param pUserData		Pointer to user data
	 * @param pDelegate		Function delegate
	 * @return Return pointer to delegate handle
	 */
	virtual FuncDelegate* AddFunc( DelegateFuncFn_t pDelegateFn, void* pUserData = nullptr ) override
	{
		CScopeLock		scopeLock( mutex );
		return &funcDelegates.emplace_back( FuncDelegate{ pUserData, pDelegateFn } );
	}

	/**
	 * @brief Remove all function delegates
	 */
	virtual void RemoveAll() override
	{
		CScopeLock		scopeLock( mutex );
		funcDelegates.clear();
	}

	/**
	 * @brief Remove function delegate
	 * @param pDelegate		Delegate handle
	 */
	virtual void RemoveFunc( FuncDelegate* pDelegate ) override
	{
		if ( pDelegate )
		{
			CScopeLock		scopeLock( mutex );
			for ( uint32 index = 0, count = ( uint32 )funcDelegates.size(); index < count; ++index )
			{
				if ( &funcDelegates[index] == pDelegate )
				{
					funcDelegates.erase( funcDelegates.begin() + index );
					return;
				}
			}
		}
	}

	/**
	 * @brief Broadcast all delegates
	 * @param params	Params for call delegate
	 */
	virtual void Broadcast( TParamTypes... params ) const override
	{
		CScopeLock		scopeLock( mutex );
		for ( uint32 index = 0, count = ( uint32 )funcDelegates.size(); index < count; ++index )
		{
			const FuncDelegate&		funcDelegate = funcDelegates[index];
			( *funcDelegate.pFunc )( funcDelegate.pUserData, params... );
		}
	}

private:
	std::vector<FuncDelegate>		funcDelegates;	/**< Function delegates */
	mutable CThreadMutex			mutex;			/**< Mutex for thread safe broadcast */
};

/**
 * @ingroup stdlib
 * @brief Single cast delegate
 */
template<typename... TParamTypes>
class TDelegate : public IDelegate<TParamTypes...>
{
public:
	/**
	 * @brief Function delegate info
	 */
	struct FuncDelegate
	{
		void*				pUserData;	/**< Pointer to user data */
		DelegateFuncFn_t	pFunc;		/**< Pointer to delegate function */
	};

	/**
	 * @brief Bind function delegate
	 *
	 * @param pDelegate		Delegate
	 * @param pUserData		Pointer to user data
	 */
	virtual void BindFunc( DelegateFuncFn_t pDelegateFn, void* pUserData = nullptr ) override
	{
		CScopeLock				scopeLock( mutex );
		funcDelegate.pFunc		= pDelegateFn;
		funcDelegate.pUserData	= pUserData;
	}

	/**
	 * @brief Unbind delegate
	 */
	virtual void Unbind() override
	{
		CScopeLock		scopeLock( mutex );
		L_memzero( &funcDelegate, sizeof( FuncDelegate ) );
	}

	/**
	 * @brief Execute delegate
	 * @param params	Params for call delegate
	 */
	virtual void Execute( TParamTypes... params ) override
	{
		CScopeLock		scopeLock( mutex );
		( *funcDelegate.pFunc )( funcDelegate.pUserData, params... );
	}

private:
	FuncDelegate			funcDelegate;	/**< Function delegate */
	mutable CThreadMutex	mutex;			/**< Mutex for thread safe broadcast */
};

/**
 * @ingroup stdlib
 * @brief Macro for declare multicast delegate interface
 *
 * @param DelegateInterfaceName		Delegate interface name
 * @param ...						Other parameters of delegate
 */
#define DECLARE_MULTICAST_DELEGATE_INTERFACE( DelegateInterfaceName, ... )	\
	typedef IMulticastDelegate<__VA_ARGS__>			DelegateInterfaceName;

/**
 * @ingroup stdlib
 * @brief Macro for declare multicast delegate
 *
 * @param DelegateName	Delegate name
 * @param ...			Other parameters of delegate
 */
#define DECLARE_MULTICAST_DELEGATE( DelegateName, ... )	\
	typedef TMulticastDelegate<__VA_ARGS__>			DelegateName;

/**
 * @ingroup stdlib
 * @brief Macro for declare single cast delegate interface
 *
 * @param DelegateInterfaceName		Delegate interface name
 * @param ...						Other parameters of delegate
 */
#define DECLARE_DELEGATE_INTERFACE( DelegateInterfaceName, ... )	\
	typedef IDelegate<__VA_ARGS__>					DelegateInterfaceName;

/**
 * @ingroup stdlib
 * @brief Macro for declare single cast delegate
 *
 * @param DelegateName	Delegate name
 * @param ...			Other parameters of delegate
 */
#define DECLARE_DELEGATE( DelegateName, ... )	\
	typedef TDelegate<__VA_ARGS__>					DelegateName;

#endif // !DELEGATE_H