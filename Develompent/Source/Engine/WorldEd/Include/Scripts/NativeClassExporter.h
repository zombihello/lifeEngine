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
	 * 
	 * @param InClassStub					Class stub
	 * @param OutExportedClass				Output exported class into C++ code
	 * @param OutNativeFuncsTable			Output table with all native functions in the class
	 * @param OutRegisterNativeFuncsTable	Output code to register native functions in the system
	 */
	void ExportClass( const ScriptClassStubPtr_t& InClassStub, std::wstring& OutExportedClass, std::wstring& OutNativeFuncsTable, std::wstring& OutRegisterNativeFuncsTable );

	/**
	 * @brief Save C++ header to file
	 * 
	 * @param InBuffer	Buffer to save
	 * @param InPath	Path to file
	 */
	void SaveCppHeader( const std::wstring& InBuffer, const std::wstring& InPath );

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
	 * @param InClassStub					Class stub
	 * @param OutNativeFuncsTable			Output table with all native functions in the class
	 * @param OutRegisterNativeFuncsTable	Output code to register native functions in the system
	 * @return Return a string with native functions
	 */
	std::wstring GenerateNativeFunctions( const ScriptClassStubPtr_t& InClassStub, std::wstring& OutNativeFuncsTable, std::wstring& OutRegisterNativeFuncsTable );

	bool							bNativeClassesChanged;	/**< Is have been changed native classes */
	std::wstring					includeDir;				/**< Directory with includes where will be saved new headers */
	std::wstring					packageName;			/**< Script package name */
	const CScriptSystemStub&		stubs;					/**< Script system stub */
};

#endif // !NATIVECLASSEXPORTER_H