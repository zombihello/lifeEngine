/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTVAR_H
#define SCRIPTVAR_H

#include <type_traits>

#include "Misc/Types.h"
#include "Misc/EngineGlobals.h"
#include "ScriptEngine.h"
#include "Containers/StringConv.h"

/**
 * @ingroup Engine
 * @brief Class for work with script values
 */
template< typename Type, bool InIsPointer = std::is_pointer< Type >::value, bool InIsScriptObject = std::is_base_of< class ScriptObject, Type >::value >
class ScriptVar
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE			ScriptVar()
	{}

	/**
	 * @brief Constructor of copy
	 * @param[in] InCopy Copy
	 */
	FORCEINLINE			ScriptVar( const ScriptVar& InCopy )
	{
		handle.SetValue< InIsScriptObject >( InCopy.handle.GetAddress< InIsScriptObject >() );
	}

	/**
	 * @brief Initialize value
	 * 
	 * @param[in] InValueID Index of global value in script module
	 * @param[in] InModuleName Script module name
	 */
	FORCEINLINE void		Init( int32 InValueID, const tchar* InModuleName )
	{
		void*		address = GScriptEngine->GetASScriptEngine()->GetModule( TCHAR_TO_ANSI( InModuleName ) )->GetAddressOfGlobalVar( InValueID );		
		handle.SetValue< InIsScriptObject >( address );
	}

	/**
	 * @brief Initialize value
	 * 
	 * @param[in] InValueID Index of property in script object
	 * @param[in] InScriptObject Pointer to script object
	 */
	FORCEINLINE void		Init( int32 InValueID, asIScriptObject* InScriptObject )
	{
		check( InScriptObject );
		void*		address = InScriptObject->GetAddressOfProperty( InValueID );
		handle.SetValue< InIsScriptObject >( address );
	}

	/**
	 * @brief Initialize value
	 * @param[in] InAddress Address on script variable
	 */
	FORCEINLINE void		Init( void* InAddress )
	{
		handle.SetValue< InIsScriptObject >( InAddress );
	}

	/**
	 * @brief Overrload operator of copy
	 * @param[in] InCopy Copy
	 */
	FORCEINLINE ScriptVar&	operator=( const ScriptVar& InCopy )
	{
		handle.SetValue< InIsScriptObject >( InCopy.handle.GetAddress() );
		return *this;
	}

	/**
	 * @brief Get value
	 * @return Return pointer on script value. If not initialize or empty return nullptr
	 */
	FORCEINLINE Type*		GetValue()
	{
		return handle.GetValue();
	}

	/**
	 * @brief Operator for cast to Type*
	 * @return Pointer on script value
	 */
	FORCEINLINE				operator Type*()
	{
		return handle.GetValue();
	}

	/**
	 * @brief Operator for cast to Type
	 * @return Script value
	 */
	FORCEINLINE				operator Type()
	{
		return *handle.GetValue();
	}

	/**
	 * @brief Operator ->
	 * @return Pointer on script value
	 */
	FORCEINLINE Type*		operator->()
	{
		return handle.GetValue();
	}

private:
	/**
	 * @brief Class of description script value
	 */
	template< bool >
	class Handle
	{
	public:
		/**
		 * @brief Constructor
		 */
		FORCEINLINE				Handle() : isNeedFree( false ), value( nullptr )
		{}

		/**
		 * @brief Destructor
		 */
		FORCEINLINE				~Handle()
		{
			if ( isNeedFree )
			{
				delete value;
			}
		}

		/**
		 * @brief Set pointer to script value for simple and C++ types (float, int, std::string, etc)
		 *
		 * @param[in] InAddress Pointer to script value
		 */
		template< bool InIsScriptObject >
		void						SetValue( void* InAddress )
		{
			value = ( Type* )InAddress;
			isNeedFree = false;
		}

		/**
		 * @brief Set pointer to script value for classes derived from ScriptObject
		 *
		 * @param[in] InAddress Pointer to script value
		 */
		template<>
		void						SetValue< true >( void* InAddress )
		{
			asIScriptObject*		scriptObject = ( asIScriptObject* )InAddress;
			if ( !scriptObject )
			{
				return;
			}

			Type*			userData = ( Type* )scriptObject->GetUserData();
			if ( !userData )
			{
				value = new Type( ( asIScriptObject* )InAddress );
				isNeedFree = true;
			}
			else
			{
				value = userData;
				userData->GetHandle()->AddRef();
			}		
		}

		/**
		 * @brief Get value
		 * @return Return pointer on script value. If not initialize or empty return nullptr
		 */
		FORCEINLINE Type*			GetValue()
		{
			return value;
		}

		/**
		 * @brief Get address on script variable for simple and C++ types (float, int, std::string, etc) 
		 * @return Address on script variable
		 */
		template< bool InIsScriptObject >
		FORCEINLINE void*			GetAddress() const
		{
			return value;
		}

		/**
		 * @brief Get address on script variable for script object
		 * @return Address on script variable
		 */
		template<>
		FORCEINLINE void*			GetAddress< true >() const
		{
			return value->GetHandle();
		}

	private:
		bool			isNeedFree;		/**< Is need free memory when object destroyed */
		Type*			value;			/**< Pointer to script value */
	};

	/**
	 * @brief Class of description pointer to script value
	 */
	template<>
	class Handle< true >
	{
	public:
		/**
		 * @brief Constructor
		 */
		FORCEINLINE						Handle() : pointer( nullptr )
		{}

		/**
		 * @brief Set pointer to script value
		 *
		 * @param[in] InAddress Pointer to handle script value
		 */
		template< bool InIsScriptObject >
		void							SetValue( void* InAddress )
		{
			pointer = InAddress;
			if ( !pointer )
			{
				return;
			}

			void*		address = nullptr;
			memcpy( &address, pointer, sizeof( void* ) );
			if ( !address )
			{
				return;
			}

			value.SetValue< InIsScriptObject >( address );
		}

		/**
		 * @brief Get value
		 * @return Return pointer on script value. If not initialize or empty return nullptr
		 */
		FORCEINLINE Type*				GetValue()
		{
			return GetValueInternal< InIsScriptObject >();
		}

		/**
		 * @brief Get address on script variable
		 * @return Address on script variable
		 */
		template< bool InIsScriptObject >
		FORCEINLINE void*				GetAddress() const
		{
			return pointer;
		}

	private:
		/**
		 * @brief Internal of getting value for simple type (int, float, etc)
		 *
		 * @return Return pointer on script value. If not initialize or empty return nullptr
		 */
		template< bool InIsScriptObject >
		FORCEINLINE Type*				GetValueInternal()
		{
			return ( Type* )pointer;
		}

		/**
		 * @brief Internal of getting value for classes derived from ScriptObject
		 *
		 * @return Return pointer on script value. If not initialize or empty return nullptr
		 */
		template<>
		FORCEINLINE Type*				GetValueInternal< true >()
		{
			if ( !pointer )
			{
				return nullptr;
			}

			// If the pointer refers to another script object, you need to update the stored values
			if ( !value.GetValue() || ( class asIScriptObject* )pointer != value.GetValue()->GetHandle() )
			{
				void*			address = nullptr;
				memcpy( &address, pointer, sizeof( void* ) );

				value.SetValue< InIsScriptObject >( address );
			}

			return value.GetValue();
		}

		void*				pointer;		/**< Address to pointer script variable */
		Handle< false >		value;			/**< Handle on script value */
	};

	Handle< InIsPointer >		handle;			/**< Handle of script variable */
};

#endif // !SCRIPTVAR_H
