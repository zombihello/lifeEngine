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

	bool							bNativeClassesChanged;	/**< Is have been changed native classes */
	std::wstring					includeDir;				/**< Directory with includes where will be saved new headers */
	std::wstring					packageName;			/**< Script package name */
	const CScriptSystemStub&		stubs;					/**< Script system stub */
};

#endif // !NATIVECLASSEXPORTER_H