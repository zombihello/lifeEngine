/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef NATIVECLASSEXPORTER_H
#define NATIVECLASSEXPORTER_H

#include <string>

#include "System/Archive.h"
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
	 * @param InSourceDir		Directory with sources where will be saved new sources
	 * @param InStub			Script system stubs
	 */
	CNativeClassExporter( const std::wstring& InIncludeDir, const std::wstring& InSourceDir, const CScriptSystemStub& InStub );

	/**
	 * @brief Export native classes
	 */
	void ExportClasses();

	/**
	 * @brief Is have been changed native classes
	 * @return Return TRUE if native classes have been changed, otherwise returns FALSE
	 */
	FORCEINLINE bool IsNativeClassesChanged() const
	{
		return bNativeClassesChanged;
	}

private:
	/**
	 * @brief Export native class
	 * 
	 * @param InOutFileData	Output file data
	 * @param InClassStub	Class stub
	 */
	void ExportClass( std::wstring& InOutFileData, const TSharedPtr<CScriptClassStub>& InClassStub );

	bool							bNativeClassesChanged;	/**< Is have been changed native classes */
	std::wstring					includeDir;				/**< Directory with includes where will be saved new headers */
	std::wstring					sourceDir;				/**< Directory with sources where will be saved new sources */
	const CScriptSystemStub*		stubs;					/**< Script system stub */
};

#endif // !NATIVECLASSEXPORTER_H