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
	 * @param OutRegistrarClass				Output code to register the class in the system
	 * @param OutExportedClass				Output exported class into C++ code
	 * @param OutNativeFuncsTable			Output table with all native functions in the class
	 * @param OutRegisterNativeFuncsTable	Output code to register native functions in the system
	 */
	void ExportClass( const ScriptClassStubPtr_t& InClassStub, std::wstring& OutRegistrarClass, std::wstring& OutExportedClass, std::wstring& OutNativeFuncsTable, std::wstring& OutRegisterNativeFuncsTable );

	/**
	 * @brief Export native struct
	 *
	 * @param InStructStub			Struct stub
	 * @param OutRegistrarStruct	Output code to register the struct in the system
	 */
	void ExportStruct( const ScriptStructStubPtr_t& InStructStub, std::wstring& OutRegistrarStruct );

	/**
	 * @brief Export native enum
	 *
	 * @param InEnumStub			Enum stub
	 * @param OutRegistrarEnum		Output code to register the enum in the system
	 */
	void ExportEnum( const ScriptEnumStubPtr_t& InEnumStub, std::wstring& OutRegistrarEnum );

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

	/**
	 * @brief Generate code to register a class in the system
	 * 
	 * @param InClassStub		Class stub
	 * @return Return a string with code to register a class in the system
	 */
	FORCEINLINE std::wstring GenerateRegistrarClass( const ScriptClassStubPtr_t& InClassStub )
	{
		return L_Sprintf( TEXT( "\tCObject::StaticAddToAutoInitializeRegistrants( ( CObject* (*)() )&%s::StaticClass );" ), InClassStub->GetName().c_str() );
	}

	/**
	 * @brief Generate code to register a struct in the system
	 *
	 * @param InStructStub		Struct stub
	 * @return Return a string with code to register a struct in the system
	 */
	FORCEINLINE std::wstring GenerateRegistrarStruct( const ScriptStructStubPtr_t& InStructStub )
	{
		return L_Sprintf( TEXT( "\tCObject::StaticAddToAutoInitializeRegistrants( ( CObject* (*)() )&%s::StaticStruct );" ), InStructStub->GetName().c_str() );
	}

	/**
	 * @brief Generate code to register a enum in the system
	 *
	 * @param InEnumStub		Enum stub
	 * @return Return a string with code to register a enum in the system
	 */
	FORCEINLINE std::wstring GenerateRegistrarEnum( const ScriptEnumStubPtr_t& InEnumStub )
	{
		return L_Sprintf( TEXT( "\tCObject::StaticAddToAutoInitializeRegistrants( ( CObject* (*)() )&Enum::%s::StaticEnum );" ), InEnumStub->GetName().c_str() );
	}

	bool							bNativeClassesChanged;	/**< Is have been changed native classes */
	std::wstring					includeDir;				/**< Directory with includes where will be saved new headers */
	std::wstring					packageName;			/**< Script package name */
	const CScriptSystemStub&		stubs;					/**< Script system stub */
};

#endif // !NATIVECLASSEXPORTER_H