/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTCODEBLOCK_H
#define SCRIPTCODEBLOCK_H

#include <string>
#include <vector>

#include "Scripts/ScriptFileContext.h"

/**
 * @ingroup WorldEd
 * @brief A script code block
 */
class CScriptCodeBlock
{
public:
	/**
	 * @brief Constructor
	 * @param InContext		Source context
	 * @param InDebugName	Debug name
	 */
	CScriptCodeBlock( const ScriptFileContext& InContext, const std::wstring InDebugName = TEXT( "" ) );

	/**
	 * @brief Destructor
	 */
	~CScriptCodeBlock();

	/**
	 * @brief Glue together two code blocks
	 * 
	 * @param InNodeRoot	Root code block	
	 * @param InNextMode	Next code block after InNodeRoot
	 */
	static void Glue( CScriptCodeBlock* InNodeRoot, CScriptCodeBlock* InNextMode );

	/**
	 * @brief Dump block to log
	 */
	void Print() const;

	/**
	 * @brief Set debug name
	 * @param InDebugName	Debug name for the code block
	 */
	FORCEINLINE void SetDebugName( const std::wstring& InDebugName )
	{
		debugName = InDebugName;
	}

	/**
	 * @brief Get bytecode
	 * @return Return bytecode
	 */
	FORCEINLINE std::vector<byte>& GetBytecode()
	{
		return bytecode;
	}

	/**
	 * @brief Get bytecode
	 * @return Return bytecode
	 */
	FORCEINLINE const std::vector<byte>& GetBytecode() const
	{
		return bytecode;
	}

	/**
	 * @brief Get next code block
	 * @return Return next code block
	 */
	FORCEINLINE CScriptCodeBlock* GetNext() const
	{
		return nextCodeBlock;
	}

	/**
	 * @brief Get last code block
	 * @return Return last code block
	 */
	FORCEINLINE CScriptCodeBlock* GetLast() const
	{
		return lastCodeBlock;
	}

	/**
	 * @brief Is empty code block
	 * @return Return TRUE if bytecode of the code block is empty, otherwise FALSE
	 */
	FORCEINLINE bool IsEmpty() const
	{
		return bytecode.empty();
	}

	/**
	 * @brief Get debug name
	 * @return Return debug name
	 */
	FORCEINLINE const std::wstring& GetDebugName() const
	{
		return debugName;
	}

	/**
	 * @brief Get source context
	 * @return Return source context
	 */
	FORCEINLINE const ScriptFileContext& GetContext() const
	{
		return context;
	}

private:
	std::wstring			debugName;			/**< Debug name of the code block for dump log */
	std::vector<byte>		bytecode;			/**< Bytecode */
	ScriptFileContext		context;			/**< Source context */
	CScriptCodeBlock*		nextCodeBlock;		/**< Next code block */
	CScriptCodeBlock*		lastCodeBlock;		/**< Last code block */

};

#endif // !SCRIPTCODEBLOCK_H