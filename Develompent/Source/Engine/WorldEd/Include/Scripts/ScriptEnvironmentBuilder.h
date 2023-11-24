/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTENVIRONMENTBUILDER_H
#define SCRIPTENVIRONMENTBUILDER_H

#include "Core.h"
#include "Scripts/ScriptFieldStubs.h"

/**
 * @ingroup WorldEd
 * @brief Script environment builder
 */
class CScriptEnvironmentBuilder
{
public:
	/**
	 * @brief Constructor
	 * @param InStubs	Stubs
	 */
	CScriptEnvironmentBuilder( CScriptSystemStub& InStubs );

	/**
	 * @brief Build environment
	 */
	bool Build();

private:
	/**
	 * @brief Create type definitions
	 * @return Return TRUE if all types was successfully created, otherwise returns FALSE
	 */
	bool CreateTypes();

	/**
	 * @brief Sort class stubs so that base class is always before its children
	 * @return Return TRUE if no errors in hierarchy order, otherwise returns FALSE
	 */
	bool CreateHierarchyOrder();

	/**
	 * @brief Create functions in classes
	 * @return Return TRUE if all functions was successfully created, otherwise returns FALSE
	 */
	bool CreateFunctions();

	/**
	 * @brief Create class from stub
	 * 
	 * @param InClassStub	Class stub
	 * @return Return TRUE if class was successfully created, otherwise returns FALSE
	 */
	bool CreateClass( CScriptClassStub& InClassStub );

	/**
	 * @brief Create function from stub
	 *
	 * @param InClassStub		Class stub
	 * @param InFunctionStub	Function stub
	 * @return Return TRUE if function was successfully created, otherwise returns FALSE
	 */
	bool CreateFunction( CScriptClassStub& InClassStub, CScriptFunctionStub& InFunctionStub );

	/**
	 * @brief Bind supper function connections
	 * @return Return TRUE if all functions was successfully binded, otherwise returns FALSE
	 */
	bool BindSuperFunctions();

	/**
	 * @brief Bind super function
	 * 
	 * @param InClassStub		Class stub
	 * @param InFunctionStub	Function stub
	 * @return Return TRUE if function was successfully binded, otherwise returns FALSE
	 */
	bool BindSuperFunction( CScriptClassStub& InClassStub, CScriptFunctionStub& InFunctionStub );

	CScriptSystemStub*	stubs;	/**< Stubs */
};

#endif // !SCRIPTENVIRONMENTBUILDER_H