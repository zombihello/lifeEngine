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
	 * @brief Create class from stub
	 * 
	 * @param InClassStub	Class stub
	 * @return Return TRUE if class was successfully created, otherwise returns FALSE
	 */
	bool CreateClass( CScriptClassStub& InClassStub );

	/**
	 * @brief Create class properties from stub
	 *
	 * @param InClassStub	Class stub
	 * @return Return TRUE if class properties were successfully created, otherwise returns FALSE
	 */
	bool CreateClassProperties( CScriptClassStub& InClassStub );

	/**
	 * @brief Create class property from stub
	 *
	 * @param InClassStub		Class stub
	 * @param InPropertyStub	Property stub
	 * @return Return TRUE if class property was successfully created, otherwise returns FALSE
	 */
	bool CreateClassProperty( CScriptClassStub& InClassStub, CScriptPropertyStub& InPropertyStub );

	/**
	 * @brief Bind class
	 *
	 * @param InClassStub	Class stub
	 * @return Return TRUE if class was successfully binded, otherwise returns FALSE
	 */
	bool BindClass( CScriptClassStub& InClassStub );

	CScriptSystemStub*	stubs;	/**< Stubs */

};

#endif // !SCRIPTENVIRONMENTBUILDER_H