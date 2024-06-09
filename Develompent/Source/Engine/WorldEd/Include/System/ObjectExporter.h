/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef OBJECTEXPORTER_H
#define OBJECTEXPORTER_H

#include "Misc/Platform.h"
#include "Reflection/Object.h"

/**
 * @ingroup WorldEd
 * @brief Object exporter
 */
class CObjectExporter
{
public:
	/**
	 * @brief Constructor
	 */
	CObjectExporter();

	/**
	 * @brief Export object to JSON
	 * 
	 * @param InObject		The object to export
	 * @param OutString		The string to output JSON
	 * @param InPortFlags	Export flags (see EPropertyPortFlags)
	 */
	void Export( CObject* InObject, std::wstring& OutString, uint32 InPortFlags = PPF_None );

	/**
	 * @brief Export objects to JSON
	 *
	 * @param InObjects		The object array to export
	 * @param OutString		The string to output JSON
	 * @param InPortFlags	Export flags (see EPropertyPortFlags)
	 */
	void Export( const std::vector<CObject*>& InObjects, std::wstring& OutString, uint32 InPortFlags = PPF_None );

private:
	/**
	 * @brief Emits the starting line for a subobject definition
	 * 
	 * @param InObject		The object to emit the subobject block for
	 * @param OutString		The string to output JSON
	 */
	void EmitBeginObject( CObject* InObject, std::wstring& OutString );

	/**
	 * @brief Export an object's subobjects, its components, and its properties
	 * 
	 * @param InObject		The object to export 
	 * @param OutString		The string to output JSON
	 */
	void ExportObjectInner( CObject* InObject, std::wstring& OutString );

	/**
	 * @brief Emits the ending line for a subobject definition
	 * @param OutString		The string to output JSON
	 */
	void EmitEndObject( std::wstring& OutString );

	/**
	 * @brief Export an object's subobjects, its components, and its properties
	 * This function wrapper for call sequence EmitBeginObject -> ExportObjectInner -> EmitEndObject
	 * 
	 * @param InObject		The object to export
	 * @param OutString		The string to output JSON
	 */
	FORCEINLINE void ExportObject( CObject* InObject, std::wstring& OutString )
	{
		EmitBeginObject( InObject, OutString );
			ExportObjectInner( InObject, OutString );
		EmitEndObject( OutString );
	}

	/**
	 * @brief Get string with spaces
	 * 
	 * @param InNumSpaces	Number of spaces that must be in the string
	 * @return Return string with spaces
	 */
	FORCEINLINE std::wstring GetNumSpaces( uint32 InNumSpaces ) const
	{
		return std::wstring( InNumSpaces, TEXT( ' ' ) );
	}

	uint32		textIndent;			/**< Current indentation of spaces of the exported text */
	uint32		portFlags;			/**< Export/import flags (see EPropertyPortFlags) */
	CObject*	exportRootScope;	/**< The root object of export */
};

#endif // !OBJECTEXPORTER_H