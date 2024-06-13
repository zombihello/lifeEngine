/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTFILEPARSER_H
#define SCRIPTFILEPARSER_H

#include "Scripts/ScriptFieldStubs.h"
#include "Scripts/ScriptFileContext.h"
#include "Scripts/ScriptTokenStream.h"

/**
 * @ingroup WorldEd
 * @brief Script file parser
 */
class CScriptFileParser
{
public:
	/**
	 * @brief Constructor
	 * @param OutStubs	Output script system stubs
	 */
	CScriptFileParser( CScriptSystemStub& OutStubs );

	/**
	 * @brief Parse the script file definitions
	 *
	 * @param InFileName			File name
	 * @param InCode				Source code from the file
	 * @return Return TRUE if file has been successfully parsed, otherwise returns FALSE
	 */
	bool ParseFile( const std::wstring& InFileName, const std::string& InCode );

	/**
	 * @brief Emit parsing error
	 *
	 * @param InContext		Script file context
	 * @param InMessage		Message
	 */
	void EmitError( const ScriptFileContext* InContext, const std::wstring& InMessage );

	/**
	 * @brief Start class definition
	 *
	 * @param InContext				Class context
	 * @param InSuperClassContext	Context of a super class type
	 * @param InClassName			Class name
	 * @param InClassSuperName		Super class name
	 * @param InFlags				Flags (see EScriptStubFlags)
	 */
	void StartClass( const ScriptFileContext* InContext, const ScriptFileContext* InSuperClassContext, const std::string_view& InClassName, const std::string_view& InClassSuperName, uint32 InFlags );

	/**
	 * @brief End definition of current class/enum/struct
	 *
	 * @param InLine			Number of line where is the end of definition
	 * @param InScopeStart		Context of the scope start
	 * @param InScopeEnd		Context of the scope end
	 */
	void EndDefinition( int32 InLine, const ScriptFileContext* InScopeStart, const ScriptFileContext* InScopeEnd );

	/**
	 * @brief Start function definition
	 *
	 * @param InContext				Function context
	 * @param InReturnTypeContext	Context of a return type
	 * @param InFunctionName		Function name
	 * @param InReturnTypeName		Return type name
	 * @param InFlags				Flags (see EScriptStubFlags)
	 */
	void StartFunction( const ScriptFileContext* InContext, const ScriptFileContext* InReturnTypeContext, const std::string_view& InFunctionName, const std::string_view& InReturnTypeName, uint32 InFlags );

	/**
	 * @brief Get function code tokens
	 * @return Return function code tokens
	 */
	CScriptTokenStream& GetFunctionCodeTokens();

	/**
	 * @brief Add to current definition a property
	 *
	 * @param InContext			Property context
	 * @param InTypeContext		Type context
	 * @param InPropertyName	Property name
	 * @param InTypeName		Type name
	 * @param InIsFunctionParam	Is this property function parameter
	 */
	void AddProperty( const ScriptFileContext* InContext, const ScriptFileContext* InTypeContext, const std::string_view& InPropertyName, const std::string_view& InTypeName, bool InIsFunctionParam );

	/**
	 * @brief Pop context
	 */
	void PopContext();

private:
	bool					bHasError;		/**< There were a parsing errors */
	CScriptSystemStub&		stubs;			/**< Output stubs */
	ScriptClassStubPtr_t	currentClass;	/**< Opened class */
};

#endif // !SCRIPTFILEPARSER_H