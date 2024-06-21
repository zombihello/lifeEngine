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
#include "Reflection/ObjectPackage.h"
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
	 * 
	 * @param InStubs			Stubs
	 * @param InScriptPackage	Script package
	 */
	CScriptEnvironmentBuilder( CScriptSystemStub& InStubs, CObjectPackage* InScriptPackage );

	/**
	 * @brief Build environment
	 */
	bool Build();

private:
	/**
	 * @brief Add to the system stubs intrinsic types
	 */
	void AddIntrinsicTypes();

	/**
	 * @brief Create type definitions
	 * @return Return TRUE if all types was successfully created, otherwise FALSE
	 */
	bool CreateTypes();

	/**
	 * @brief Sort class stubs so that base class is always before its children
	 * @return Return TRUE if no errors in hierarchy order, otherwise FALSE
	 */
	bool CreateHierarchyOrder();

	/**
	 * @brief Create CClass from stub
	 * 
	 * @param InClassStub	Class stub
	 * @return Return TRUE if CClass was successfully created, otherwise FALSE
	 */
	bool CreateClass( CScriptClassStub& InClassStub );

	/**
	 * @brief Create functions in classes
	 * @return Return TRUE if all functions was successfully created, otherwise FALSE
	 */
	bool CreateFunctions();

	/**
	 * @brief Create function from stub
	 *
	 * @param InClassStub		Class stub
	 * @param InFunctionStub	Function stub
	 * @return Return TRUE if function was successfully created, otherwise FALSE
	 */
	bool CreateFunction( CScriptClassStub& InClassStub, CScriptFunctionStub& InFunctionStub );

	/**
	 * @brief Bind supper function connections
	 * @return Return TRUE if all functions was successfully binded, otherwise FALSE
	 */
	bool BindSuperFunctions();

	/**
	 * @brief Bind super function
	 *
	 * @param InClassStub		Class stub
	 * @param InFunctionStub	Function stub
	 * @return Return TRUE if function was successfully binded, otherwise FALSE
	 */
	bool BindSuperFunction( CScriptClassStub& InClassStub, CScriptFunctionStub& InFunctionStub );

	CScriptSystemStub&	stubs;			/**< Stubs */
	CObjectPackage*		scriptPackage;	/**< Script package */
};

#endif // !SCRIPTENVIRONMENTBUILDER_H