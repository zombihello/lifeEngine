/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef RENDERINGTHREAD_H
#define RENDERINGTHREAD_H

#include "Containers/RingBuffer.h"
#include "System/ThreadingBase.h"

/**
 * @ingroup Engine
 * Whether the renderer is currently running in a separate thread.
 * If this is false, then all rendering commands will be executed immediately instead of being enqueued in the rendering command buffer.
 */
extern bool				GIsThreadedRendering;

/**
 * @ingroup Engine
 * ID of rendering thread
 */
extern uint32			GRenderingThreadId;

/**
 * @ingroup Engine
 * The rendering command queue
 */
extern FRingBuffer		GRenderCommandBuffer;

/**
 * @ingroup Engine
 * Is current thread is render
 *
 * @return True if called from the rendering thread
 */
FORCEINLINE bool IsInRenderingThread()
{
	return ( GRenderingThreadId == 0 ) || ( appGetCurrentThreadId() == GRenderingThreadId );
}

/**
 * @ingroup Engine
 * The parent class of commands stored in the rendering command queue
 */
class FRenderCommand
{
public:
	/**
	 * Destructor
	 */
	virtual ~FRenderCommand() {}

	/**
	 * Execute render command
	 * @return Return size of render command
	 */
	virtual uint32 Execute() = 0;

	/**
	 * Get size of render command
	 * @return Return size of render command
	 */
	virtual uint32 GetSize() const = 0;

	/**
	 * Get TCHAR describe command
	 * @return return describe command in TCHAR format 
	 */
	virtual const tchar* DescribeCommand() const = 0;

	/**
	 * Get ANSI describe command
	 * @return return describe command in ANSI format 
	 */
	virtual const char* DescribeCommandChar() const = 0;

	/**
	 * Overrload operator of new
	 * 
	 * @param[in] InSize Size
	 * @param[in] InAllocation Allocation context in ring buffer
	 */
	FORCEINLINE void* operator new( size_t InSize, const FRingBuffer::AllocationContext& InAllocation )
	{
		return InAllocation.GetAllocation();
	}

	/**
	 * Overrload operator of delete
	 * 
	 * @param[in] InPtr Pointer to data
	 * @param[in] InAllocation Allocation context in ring buffer
	 */
	FORCEINLINE void operator delete( void* InPtr, const FRingBuffer::AllocationContext& InAllocation )
	{}
};

/**
 * @ingroup Engine
 * A rendering command that simply consumes space in the rendering command queue
 */
class FSkipRenderCommand : public FRenderCommand
{
public:
	/**
	 * Constructor
	 * 
	 * @param[in] InNumSkipBytes Number skip bytes
	 */
	FSkipRenderCommand( uint32 InNumSkipBytes );

	/**
	 * Execute render command
	 * @return Return size of render command
	 */
	virtual uint32 Execute() override;

	/**
	 * Get size of render command
	 * @return Return size of render command
	 */
	virtual uint32 GetSize() const override;

	/**
	 * Get TCHAR describe command
	 * @return return describe command in TCHAR format
	 */
	virtual const tchar* DescribeCommand() const override;

	/**
	 * Get ANSI describe command
	 * @return return describe command in ANSI format
	 */
	virtual const char* DescribeCommandChar() const override;

private:
	uint32			numSkipBytes;			/**< Number skip bytes */
};

//
// Macros for using render commands.
//

/**
 * @ingroup Engine
 * Send render command to render thread
 * 
 * @param[in] InTypeName Type name of render command
 * @param[in] InParam Parameters of render command
 */
#define SEND_RENDER_COMMAND( InTypeName, InParam ) \
	{ \
		check( IsInGameThread() ); \
		if ( GIsThreadedRendering ) \
		{ \
			FRingBuffer::AllocationContext			allocationContext( GRenderCommandBuffer, sizeof( InTypeName ) ); \
			if ( allocationContext.GetAllocatedSize() < sizeof( InTypeName ) ) \
			{ \
				check( allocationContext.GetAllocatedSize() >= sizeof( FSkipRenderCommand ) ); \
				new( allocationContext ) FSkipRenderCommand( allocationContext.GetAllocatedSize() ); \
				allocationContext.Commit(); \
				new( FRingBuffer::AllocationContext( GRenderCommandBuffer, sizeof(InTypeName) ) ) InTypeName InParam; \
			} \
			else \
			{ \
				new( allocationContext ) InTypeName InParam; \
			} \
		} \
		else \
		{ \
			InTypeName		InTypeName##Command InParam; \
			InTypeName##Command.Execute(); \
		} \
	}

/**
 * @ingroup Engine
 * Declares a rendering command type with 0 parameters
 * 
 * @param[in] InTypeName Type name of render command
 * @param[in] InCode Executable code in render thread
 */
#define UNIQUE_RENDER_COMMAND( InTypeName, InCode ) \
	class InTypeName : public FRenderCommand \
	{ \
	public: \
		virtual uint32 Execute() override \
		{ \
			InCode; \
			return sizeof( *this ); \
		} \
		virtual uint32 GetSize() const override \
		{ \
			return sizeof( *this ); \
		} \
		virtual const tchar* DescribeCommand() const override \
		{ \
			return TEXT( #InTypeName ); \
		} \
		virtual const char* DescribeCommandChar() const override \
		{ \
			return #InTypeName ; \
		} \
	}; \
	SEND_RENDER_COMMAND( InTypeName, );

/**
 * @ingroup Engine
 * Declares a rendering command type with 1 parameters
 * 
 * @param[in] InTypeName Type name of render command
 * @param[in] InParamType1 Type of param 1
 * @param[in] InParamName1 Name of param 1
 * @param[in] InParamValue1 Value of param 1
 * @param[in] InCode Executable code in render thread
 */
#define UNIQUE_RENDER_COMMAND_ONEPARAMETER( InTypeName, InParamType1, InParamName1, InParamValue1, InCode ) \
	class InTypeName : public FRenderCommand \
	{ \
	public: \
		InTypeName( InParamType1 In##InParamName1 ) : \
			InParamName1( In##InParamName1 ) \
		{} \
		virtual uint32 Execute() override \
		{ \
			InCode; \
			return sizeof( *this ); \
		} \
		virtual uint32 GetSize() const override \
		{ \
			return sizeof( *this ); \
		} \
		virtual const tchar* DescribeCommand() const override \
		{ \
			return TEXT( #InTypeName ); \
		} \
		virtual const char* DescribeCommandChar() const override \
		{ \
			return #InTypeName ; \
		} \
	private: \
		InParamType1			InParamName1; \
	}; \
	SEND_RENDER_COMMAND( InTypeName, ( InParamValue1 ) );

 /**
  * @ingroup Engine
  * Declares a rendering command type with 2 parameters
  *
  * @param[in] InTypeName Type name of render command
  * @param[in] InParamType1 Type of param 1
  * @param[in] InParamName1 Name of param 1
  * @param[in] InParamValue1 Value of param 1
  * @param[in] InParamType2 Type of param 2
  * @param[in] InParamName2 Name of param 2
  * @param[in] InParamValue2 Value of param 2
  * @param[in] InCode Executable code in render thread
  */
#define UNIQUE_RENDER_COMMAND_TWOPARAMETER( InTypeName, InParamType1, InParamName1, InParamValue1, InParamType2, InParamName2, InParamValue2, InCode ) \
	class InTypeName : public FRenderCommand \
	{ \
	public: \
		InTypeName( InParamType1 In##InParamName1, InParamType2 In##InParamName2 ) : \
			InParamName1( In##InParamName1 ), \
			InParamName2( In##InParamName2 ) \
		{} \
		virtual uint32 Execute() override \
		{ \
			InCode; \
			return sizeof( *this ); \
		} \
		virtual uint32 GetSize() const override \
		{ \
			return sizeof( *this ); \
		} \
		virtual const tchar* DescribeCommand() const override \
		{ \
			return TEXT( #InTypeName ); \
		} \
		virtual const char* DescribeCommandChar() const override \
		{ \
			return #InTypeName ; \
		} \
	private: \
		InParamType1			InParamName1; \
		InParamType2			InParamName2; \
	}; \
	SEND_RENDER_COMMAND( InTypeName, ( InParamValue1, InParamValue2 ) );

	/**
	 * @ingroup Engine
	 * Declares a rendering command type with 3 parameters
	 *
	 * @param[in] InTypeName Type name of render command
	 * @param[in] InParamType1 Type of param 1
	 * @param[in] InParamName1 Name of param 1
	 * @param[in] InParamValue1 Value of param 1
	 * @param[in] InParamType2 Type of param 2
	 * @param[in] InParamName2 Name of param 2
	 * @param[in] InParamValue2 Value of param 2
	 * @param[in] InParamType3 Type of param 3
	 * @param[in] InParamName3 Name of param 3
	 * @param[in] InParamValue3 Value of param 3
	 * @param[in] InCode Executable code in render thread
	 */
#define UNIQUE_RENDER_COMMAND_THREEPARAMETER( InTypeName, InParamType1, InParamName1, InParamValue1, InParamType2, InParamName2, InParamValue2, InParamType3, InParamName3, InParamValue3, InCode ) \
	class InTypeName : public FRenderCommand \
	{ \
	public: \
		InTypeName( InParamType1 In##InParamName1, InParamType2 In##InParamName2, InParamType3 In##InParamName3 ) : \
			InParamName1( In##InParamName1 ), \
			InParamName2( In##InParamName2 ), \
			InParamName3( In##InParamName3 ) \
		{} \
		virtual uint32 Execute() override \
		{ \
			InCode; \
			return sizeof( *this ); \
		} \
		virtual uint32 GetSize() const override \
		{ \
			return sizeof( *this ); \
		} \
		virtual const tchar* DescribeCommand() const override \
		{ \
			return TEXT( #InTypeName ); \
		} \
		virtual const char* DescribeCommandChar() const override \
		{ \
			return #InTypeName ; \
		} \
	private: \
		InParamType1			InParamName1; \
		InParamType2			InParamName2; \
		InParamType3			InParamName3; \
	}; \
	SEND_RENDER_COMMAND( InTypeName, ( InParamValue1, InParamValue2, InParamValue3 ) );

/**
 * @ingroup Engine
 * @brief The rendering thread runnable object
 */
class FRenderingThread : public FRunnable
{
public:
	/**
	 * @brief Initialize
	 *
	 * Allows per runnable object initialization. NOTE: This is called in the
	 * context of the thread object that aggregates this, not the thread that
	 * passes this runnable to a new thread.
	 *
	 * @return True if initialization was successful, false otherwise
	 */
	virtual bool Init() override;

	/**
	 * @brief Run
	 *
	 * This is where all per object thread work is done. This is only called
	 * if the initialization was successful.
	 *
	 * @return The exit code of the runnable object
	 */
	virtual uint32 Run() override;

	/**
	 * @brief Stop
	 *
	 * This is called if a thread is requested to terminate early
	 */
	virtual void Stop() override;

	/**
	 * @brief Exit
	 *
	 * Called in the context of the aggregating thread to perform any cleanup.
	 */
	virtual void Exit() override;
};

/** 
 * @ingroup Engine
 * Starts the rendering thread
 */
extern void StartRenderingThread();

/** 
 * @ingroup Engine
 * Stops the rendering thread
 */
extern void StopRenderingThread();

#endif // !RENDERINGTHREAD_H
