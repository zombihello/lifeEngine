#include "System/ObjectExporter.h"
#include "System/Archive.h"
#include "Reflection/Class.h"
#include "Reflection/Property.h"

/*
==================
CObjectExporter::CObjectExporter
==================
*/
CObjectExporter::CObjectExporter()
	: textIndent( 0 )
	, portFlags( PPF_None )
	, exportRootScope( nullptr )
{}

/*
==================
CObjectExporter::Export
==================
*/
void CObjectExporter::Export( CObject* InObject, std::wstring& OutString, uint32 InPortFlags /* = PPF_None */ )
{
	// If object isn't valid do nothing
	if ( !InObject )
	{
		Warnf( TEXT( "Can't export object because it isn't valid\n" ) );
		return;
	}

	// Reset text indent and OutString
	textIndent = 0;
	exportRootScope = InObject;
	portFlags = InPortFlags;
	OutString.clear();

	// Open main scope
	OutString	= TEXT( "{\n" );
	textIndent	+= 3;

	// Open section of exported objects
	OutString	+= L_Sprintf( TEXT( "%s\"ExportedObjects\": [\n" ), GetNumSpaces( textIndent ).c_str() );
	textIndent	+= 3;

	// Export the object
	ExportObject( InObject, OutString );

	// Close section of exported objects
	textIndent	-= 3;
	OutString	+= L_Sprintf( TEXT( "\n%s]\n" ), GetNumSpaces( textIndent ).c_str() );

	// Close main scope
	textIndent	-= 3;
	OutString	+= TEXT( "}" );
}

/*
==================
CObjectExporter::Export
==================
*/
void CObjectExporter::Export( const std::vector<CObject*>& InObjects, std::wstring& OutString, uint32 InPortFlags /* = PPF_None */ )
{
	// Reset text indent and OutString
	textIndent			= 0;
	portFlags =			 InPortFlags;
	OutString.clear();

	// Open main scope
	OutString	= TEXT( "{\n" );
	textIndent	+= 3;

	// Open section of exported objects
	OutString	+= L_Sprintf( TEXT( "%s\"ExportedObjects\": [" ), GetNumSpaces( textIndent ).c_str() );
	textIndent	+= 3;

	bool	bMoreThanOneObject = false;
	for ( uint32 index = 0, count = InObjects.size(); index < count; ++index )
	{
		// If the object isn't valid do nothing
		CObject*	object	= InObjects[index];
		exportRootScope		= object;
		if ( !object )
		{
			Warnf( TEXT( "Can't export object %i because it isn't valid\n" ), index );
			continue;
		}

		// Export the object
		if ( bMoreThanOneObject )
		{
			OutString += TEXT( "," );
		}
		OutString += TEXT( "\n" );
		ExportObject( object, OutString );
		bMoreThanOneObject = true;
	}

	// Close section of exported objects
	textIndent	-= 3;
	OutString	+= L_Sprintf( TEXT( "\n%s]\n" ), GetNumSpaces( textIndent ).c_str() );

	// Close main scope
	textIndent	-= 3;
	OutString	+= TEXT( "}" );
}

/*
==================
CObjectExporter::EmitBeginObject
==================
*/
void CObjectExporter::EmitBeginObject( CObject* InObject, std::wstring& OutString )
{
	// Open the object section
	OutString += L_Sprintf( TEXT( "%s{\n" ),					GetNumSpaces( textIndent ).c_str(), InObject->GetClass()->GetPathName().c_str() );
	textIndent += 3;

	OutString += L_Sprintf( TEXT( "%s\"Class\": \"%s\",\n" ),	GetNumSpaces( textIndent ).c_str(), InObject->GetClass()->GetPathName().c_str() );
	OutString += L_Sprintf( TEXT( "%s\"Name\": \"%s\"" ),		GetNumSpaces( textIndent ).c_str(), InObject->GetName().c_str() );
}

/*
==================
CObjectExporter::EmitEndObject
==================
*/
void CObjectExporter::EmitEndObject( std::wstring& OutString )
{
	// Close JSON section
	textIndent -= 3;
	OutString += L_Sprintf( TEXT( "\n%s}" ), GetNumSpaces( textIndent ).c_str() );
}

/*
==================
CObjectExporter::ExportObjectInner
==================
*/
void CObjectExporter::ExportObjectInner( CObject* InObject, std::wstring& OutString )
{
	// Grab all subobjects in the object
	std::vector<CObject*>						subObjects;
	TArchiveObjectReferenceCollector<CObject>	objectReferenceCollector( &subObjects, InObject );
	InObject->Serialize( objectReferenceCollector );

	// Export all subobjects
	OutString	+= L_Sprintf( TEXT( ",\n%s\"Objects\": [" ), GetNumSpaces( textIndent ).c_str() );
	textIndent	+= 3;
	{
		for ( uint32 index = 0, count = subObjects.size(); index < count; ++index )
		{
			OutString += TEXT( "\n" );
			ExportObject( subObjects[index], OutString );
			if ( index + 1 < count )
			{
				OutString += TEXT( "," );
			}
		}
	}
	textIndent -= 3;
	OutString += L_Sprintf( TEXT( "\n%s],\n" ), GetNumSpaces( textIndent ).c_str() );

	// Export data properties
	OutString	+= L_Sprintf( TEXT( "%s\"Data\": [" ), GetNumSpaces( textIndent ).c_str() );
	textIndent	+= 3;
	{
		bool						bMoreThanOneProperty = false;
		std::vector<CProperty*>		properties;
		InObject->GetClass()->GetProperties( properties );
		for ( uint32 index = 0, count = properties.size(); index < count; ++index )
		{
			CProperty*		property = properties[index];
			std::wstring	valueString;
			
			property->ExportProperty( valueString, ( byte* )InObject, exportRootScope, portFlags );
			if ( !valueString.empty() )
			{
				if ( bMoreThanOneProperty )
				{
					OutString += TEXT( "," );
				}
				OutString				+= L_Sprintf( TEXT( "\n%s\"%s\": %s" ), GetNumSpaces( textIndent ).c_str(), property->GetName().c_str(), valueString.c_str() );			
				bMoreThanOneProperty	= true;
			}
		}
	}
	textIndent	-= 3;
	OutString	+= L_Sprintf( TEXT( "\n%s]" ), GetNumSpaces( textIndent ).c_str() );
}