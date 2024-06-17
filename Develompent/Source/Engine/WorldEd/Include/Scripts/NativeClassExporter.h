/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef NATIVECLASSEXPORTER_H
#define NATIVECLASSEXPORTER_H

#include "Scripts/ScriptFieldStubs.h"

/**
 * @ingroup WorldEd
 * @brief Used to export native classes from LifeScript to C++ headers/sources
 */
class CNativeClassExporter
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InIncludeDir		Directory with includes where will be saved new headers
	 * @param InPackageName		Script package name
	 * @param InStubs			Script system stubs
	 */
	CNativeClassExporter( const std::wstring& InIncludeDir, const std::wstring& InPackageName, const CScriptSystemStub& InStubs );

	/**
	 * @brief Export native classes
	 */
	void ExportClasses();

	/**
	 * @brief Is have been changed native classes
	 * @return Return TRUE if native classes have been changed, otherwise FALSE
	 */
	FORCEINLINE bool IsNativeClassesChanged() const
	{
		return bNativeClassesChanged;
	}

private:
	/**
	 * @brief Export native class
	 * @param InClassStub	Class stub
	 */
	void ExportClass( const ScriptClassStubPtr_t& InClassStub );

	/**
	 * @brief Generate C++ body for header
	 * 
	 * @param InClassStub	Class stub
	 * @return Return final C++ text for past into header
	 */
	std::wstring GenerateCppTextBody( const ScriptClassStubPtr_t& InClassStub );

	/**
	 * @brief Generate a string in the format CLASS_Something|CLASS_Something which represents all class flags that are set for the specified
	 * class which need to be exported as part of the DECLARE_CLASS macro
	 * 
	 * @param InClassStub	Class stub
	 * @return Return a class flags in a string in the format CLASS_Something|CLASS_Something which represents all class flags
	 */
	std::wstring GenerateClassFlags( const ScriptClassStubPtr_t& InClassStub );

	/**
	 * @brief Generate within class macro
	 * 
	 * @param InClassStub	Class stub
	 * @return Return a string with within class macro
	 */
	std::wstring GenerateWithinClass( const ScriptClassStubPtr_t& InClassStub );

	/**
	 * @brief Generate native functions
	 *
	 * @param InClassStub	Class stub
	 * @return Return a string with native functions
	 */
	std::wstring GenerateNativeFunctions( const ScriptClassStubPtr_t& InClassStub );

	/**
	 * @brief Generate register of native functions
	 *
	 * @param InClassStub	Class stub
	 * @return Return a string with register of native functions
	 */
	std::wstring GenerateRegisterNativeFunctions( const ScriptClassStubPtr_t& InClassStub );

	bool							bNativeClassesChanged;	/**< Is have been changed native classes */
	std::wstring					includeDir;				/**< Directory with includes where will be saved new headers */
	std::wstring					packageName;			/**< Script package name */
	const CScriptSystemStub&		stubs;					/**< Script system stub */
};

#endif // !NATIVECLASSEXPORTER_H