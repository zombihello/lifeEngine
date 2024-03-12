/**
 * @file
 * @addtogroup studiorender studiorender
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

#ifndef ISTUDIORENDERCMD_H
#define ISTUDIORENDERCMD_H

#include "interfaces/interfaces.h"
#include "core/types.h"
#include "core/platform.h"
#include "studiorender/istudiocmdbuffer.h"
#include "studiorender/istudiorender.h"

/**
 * @ingroup studiorender
 * @brief Studio render command interface stored in the render command queue
 */
class IStudioRenderCmd
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IStudioRenderCmd() {}

	/**
	 * @brief Execute render command
	 * @return Return size of render command
	 */
	virtual uint32 Execute() = 0;

	/**
	 * @brief Get size of render command
	 * @return Return size of render command
	 */
	virtual uint32 GetSize() const = 0;

	/**
	 * @brief Overload operator new
	 * 
	 * @param size				Size
	 * @param studioCmdAlloc	Studio allocated chunk in the render command buffer
	 */
	FORCEINLINE void* operator new( size_t size, const StudioCmdAlloc& studioCmdAlloc )
	{
		return studioCmdAlloc.pAllocation;
	}

	/**
	 * @brief Overload operator delete
	 * 
	 * @param pPtr				Pointer to data
	 * @param studioCmdAlloc	Studio allocated chunk in the render command buffer
	 */
	FORCEINLINE void operator delete( void* pPtr, const StudioCmdAlloc& studioCmdAlloc )
	{}
};

/**
 * @ingroup studiorender
 * @brief A rendering command that simply consumes space in the render command queue
 */
class CStudioRenderCmd_Skip : public IStudioRenderCmd
{
public:
	/**
	 * @brief Constructor
	 * @param numSkipBytes		Number skip bytes
	 */
	CStudioRenderCmd_Skip( uint32 numSkipBytes )
		: numSkipBytes( numSkipBytes )
	{}

	/**
	 * @brief Execute render command
	 * @return Return size of render command
	 */
	virtual uint32 Execute() override
	{
		return numSkipBytes;
	}

	/**
	 * @brief Get size of render command
	 * @return Return size of render command
	 */
	virtual uint32 GetSize() const override
	{
		return numSkipBytes;
	}

private:
	uint32	numSkipBytes;			/**< Number skip bytes */
};

//
// Macros for using render commands
//

/**
 * @ingroup studiorender
 * @brief Send a render command to the render thread
 * @warning For use g_pStudioRender must be valid
 * 
 * @param TypeName	Render command type name
 * @param Params	Render command parameters
 */
#define SEND_RENDER_COMMAND( TypeName, Params ) \
	{ \
		/** Send a render command to the render thread if we're not in it */ \
		if ( !g_pStudioRender->IsInRenderThread() ) \
		{ \
			IStudioCmdBuffer*		pStudioCmdBuffer = g_pStudioRender->GetCommandBuffer(); \
			Assert( pStudioCmdBuffer ); \
			\
			/** Allocate chunk in the command buffer for our command */ \
			StudioCmdAlloc		studioCmdAlloc = pStudioCmdBuffer->GetAllocation( sizeof( TypeName ) ); \
			\
			/** If not enough space we skip this chunk and allocate in another */ \
			if ( studioCmdAlloc.allocatedSize < sizeof( TypeName ) ) \
			{ \
				Assert( studioCmdAlloc.allocatedSize >= sizeof( CStudioRenderCmd_Skip ) ); \
				new( studioCmdAlloc ) CStudioRenderCmd_Skip( studioCmdAlloc.allocatedSize ); \
				pStudioCmdBuffer->CommitAllocation( studioCmdAlloc ); \
				\
				/** Now we have enough space and can allocate our command */ \
				studioCmdAlloc = pStudioCmdBuffer->GetAllocation( sizeof( TypeName ) ); \
				new( studioCmdAlloc ) TypeName Params; \
				pStudioCmdBuffer->CommitAllocation( studioCmdAlloc ); \
			} \
			/** Otherwise if we have enough space allocate our command */ \
			else \
			{ \
				new( studioCmdAlloc ) TypeName Params; \
				pStudioCmdBuffer->CommitAllocation( studioCmdAlloc ); \
			} \
		} \
		/** Otherwise we are in the render thread and simple execute our command */ \
		else \
		{ \
			TypeName	TypeName##Command Params; \
			TypeName##Command.Execute(); \
		} \
	}

/**
 * @ingroup studiorender
 * @brief Declares a render command type with 0 parameters
 * @warning For use g_pStudioRender must be valid
 * 
 * @param TypeName	Render command type name
 * @param Code		Executable code in the render thread
 */
#define UNIQUE_RENDER_COMMAND( TypeName, Code ) \
	class TypeName : public IStudioRenderCmd \
	{ \
	public: \
		virtual uint32 Execute() override \
		{ \
			Code; \
			return sizeof( *this ); \
		} \
		virtual uint32 GetSize() const override \
		{ \
			return sizeof( *this ); \
		} \
	}; \
	SEND_RENDER_COMMAND( TypeName, );

/**
 * @ingroup studiorender
 * @brief Declares a render command type with 1 parameters
 * @warning For use g_pStudioRender must be valid
 * 
 * @param TypeName		Render command type name
 * @param ParamType1	Type of param 1
 * @param ParamName1	Name of param 1
 * @param ParamValue1	Value of param 1
 * @param Code			Executable code in the render thread
 */
#define UNIQUE_RENDER_COMMAND_ONEPARAMETER( TypeName, ParamType1, ParamName1, ParamValue1, Code ) \
	class TypeName : public IStudioRenderCmd \
	{ \
	public: \
		TypeName( ParamType1 ParamName1 ) \
			: ParamName1( ParamName1 ) \
		{} \
		virtual uint32 Execute() override \
		{ \
			Code; \
			return sizeof( *this ); \
		} \
		virtual uint32 GetSize() const override \
		{ \
			return sizeof( *this ); \
		} \
	private: \
		ParamType1	ParamName1; \
	}; \
	SEND_RENDER_COMMAND( TypeName, ( ParamValue1 ) );

 /**
  * @ingroup studiorender
  * @brief Declares a render command type with 2 parameters
  * @warning For use g_pStudioRender must be valid
  *
  * @param TypeName		Render command type name
  * @param ParamType1	Type of param 1
  * @param ParamName1	Name of param 1
  * @param ParamValue1	Value of param 1
  * @param ParamType2	Type of param 2
  * @param ParamName2	Name of param 2
  * @param ParamValue2	Value of param 2
  * @param Code			Executable code in the render thread
  */
#define UNIQUE_RENDER_COMMAND_TWOPARAMETER( TypeName, ParamType1, ParamName1, ParamValue1, ParamType2, ParamName2, ParamValue2, Code ) \
	class TypeName : public IStudioRenderCmd \
	{ \
	public: \
		TypeName( ParamType1 ParamName1, ParamType2 ParamName2 ) \
			: ParamName1( ParamName1 ) \
			, ParamName2( ParamName2 ) \
		{} \
		virtual uint32 Execute() override \
		{ \
			Code; \
			return sizeof( *this ); \
		} \
		virtual uint32 GetSize() const override \
		{ \
			return sizeof( *this ); \
		} \
	private: \
		ParamType1	ParamName1; \
		ParamType2	ParamName2; \
	}; \
	SEND_RENDER_COMMAND( TypeName, ( ParamValue1, ParamValue2 ) );

/**
 * @ingroup studiorender
 * @brief Declares a render command type with 3 parameters
 * @warning For use g_pStudioRender must be valid
 *
 * @param TypeName		Render command type name
 * @param ParamType1	Type of param 1
 * @param ParamName1	Name of param 1
 * @param ParamValue1	Value of param 1
 * @param ParamType2	Type of param 2
 * @param ParamName2	Name of param 2
 * @param ParamValue2	Value of param 2
 * @param ParamType3	Type of param 3
 * @param ParamName3	Name of param 3
 * @param ParamValue3	Value of param 3
 * @param Code			Executable code in the render thread
 */
#define UNIQUE_RENDER_COMMAND_THREEPARAMETER( TypeName, ParamType1, ParamName1, ParamValue1, ParamType2, ParamName2, ParamValue2, ParamType3, ParamName3, ParamValue3, Code ) \
	class TypeName : public IStudioRenderCmd \
	{ \
	public: \
		TypeName( ParamType1 ParamName1, ParamType2 ParamName2, ParamType3 ParamName3 ) \
			: ParamName1( ParamName1 ) \
			, ParamName2( ParamName2 ) \
			, ParamName3( ParamName3 ) \
		{} \
		virtual uint32 Execute() override \
		{ \
			Code; \
			return sizeof( *this ); \
		} \
		virtual uint32 GetSize() const override \
		{ \
			return sizeof( *this ); \
		} \
	private: \
		ParamType1	ParamName1; \
		ParamType2	ParamName2; \
		ParamType3	ParamName3; \
	}; \
	SEND_RENDER_COMMAND( TypeName, ( ParamValue1, ParamValue2, ParamValue3 ) );

/**
 * @ingroup studiorender
 * @brief Declares a render command type with 4 parameters
 * @warning For use g_pStudioRender must be valid
 *
 * @param TypeName		Render command type name
 * @param ParamType1	Type of param 1
 * @param ParamName1	Name of param 1
 * @param ParamValue1	Value of param 1
 * @param ParamType2	Type of param 2
 * @param ParamName2	Name of param 2
 * @param ParamValue2	Value of param 2
 * @param ParamType3	Type of param 3
 * @param ParamName3	Name of param 3
 * @param ParamValue3	Value of param 3
 * @param ParamType4	Type of param 4
 * @param ParamName4	Name of param 4
 * @param ParamValue4	Value of param 4
 * @param Code			Executable code in the render thread
 */
#define UNIQUE_RENDER_COMMAND_FOURPARAMETER( TypeName, ParamType1, ParamName1, ParamValue1, ParamType2, ParamName2, ParamValue2, ParamType3, ParamName3, ParamValue3, ParamType4, ParamName4, ParamValue4, Code ) \
	class TypeName : public IStudioRenderCmd \
	{ \
	public: \
		TypeName( InParamType1 ParamName1, ParamType2 ParamName2, ParamType3 ParamName3, ParamType4 ParamName4 ) \
			: ParamName1( ParamName1 ) \
			, ParamName2( ParamName2 ) \
			, ParamName3( ParamName3 ) \
			, ParamName4( ParamName4 ) \
		{} \
		virtual uint32 Execute() override \
		{ \
			Code; \
			return sizeof( *this ); \
		} \
		virtual uint32 GetSize() const override \
		{ \
			return sizeof( *this ); \
		} \
	private: \
		ParamType1	ParamName1; \
		ParamType2	ParamName2; \
		ParamType3	ParamName3; \
		ParamType4	ParamName4; \
	}; \
	SEND_RENDER_COMMAND( TypeName, ( ParamValue1, ParamValue2, ParamValue3, ParamValue4 ) );

/**
 * @ingroup studiorender
 * @brief Flush render commands
 * @warning For use g_pStudioRender must be valid
 */
FORCEINLINE void R_FlushRenderCommands()
{
	if ( !g_pStudioRender->IsInRenderThread() )
	{
		g_pStudioRender->GetCommandBuffer()->Flush();
	}
}

#endif // !ISTUDIORENDERCMD_H