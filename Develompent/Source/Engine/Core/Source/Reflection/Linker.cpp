#include "Logger/LoggerMacros.h"
#include "Misc/CoreGlobals.h"
#include "Reflection/Linker.h"
#include "Reflection/ObjectGC.h"
#include "System/BaseFileSystem.h"

/*
==================
PackageFileSummary::PackageFileSummary
==================
*/
PackageFileSummary::PackageFileSummary()
{
	Sys_Memzero( this, sizeof( *this ) );
}

/*
==================
operator<<
==================
*/
CArchive& operator<<( CArchive& InArchive, PackageFileSummary& InValue )
{
	InArchive << InValue.tag;

	// Only keep loading/saving if we match the magic
	if ( InValue.tag == PACKAGE_FILE_TAG )
	{
		InArchive << InValue.engineVersion;
		InArchive << InValue.fileVersion;
		InArchive << InValue.exportCount;
		InArchive << InValue.exportOffset;
		InArchive << InValue.importCount;
		InArchive << InValue.importOffset;
		InArchive << InValue.guid;
	}
	else
	{
		Warnf( TEXT( "%s: Failed to get correct summary tag! 0x%X not 0x%X\n" ), InArchive.GetPath().c_str(), InValue.tag, PACKAGE_FILE_TAG );
		AssertMsg( false, TEXT( "%s: Bad summary tag" ), InArchive.GetPath().c_str() );
	}

	return InArchive;
}


/*
==================
CLinker::CLinker
==================
*/
CLinker::CLinker( CObjectPackage* InRoot, const tchar* InFilename )
	: linkerRoot( InRoot )
	, filename( InFilename )
{
	Assert( linkerRoot );
	Assert( InFilename );
}


/*
==================
CLinkerLoad::CLinkerLoad
==================
*/
CLinkerLoad::CLinkerLoad( CObjectPackage* InRoot, const tchar* InFilename )
	: CLinker( InRoot, InFilename )
	, CArchive( InFilename )
{}


/*
==================
CLinkerSave::CLinkerSave
==================
*/
CLinkerSave::CLinkerSave( CObjectPackage* InRoot, const tchar* InFilename )
	: CLinker( InRoot, InFilename )
	, CArchive( InFilename )
	, saver( nullptr )
{
	// Create file saver
	saver = g_FileSystem->CreateFileWriter( InFilename, AW_NoFail );

	// Set main summary info
	summary.tag				= PACKAGE_FILE_TAG;
	summary.engineVersion	= ENGINE_VERSION;
	summary.fileVersion		= VER_PACKAGE_LATEST;

	// Allocate indeces
	objectIndices.resize( CObjectGC::Get().GetReservedSizeAllocatedObjects() );
}

/*
==================
CLinkerSave::~CLinkerSave
==================
*/
CLinkerSave::~CLinkerSave()
{
	Detach();
}

/*
==================
CLinkerSave::Detach
==================
*/
void CLinkerSave::Detach()
{
	if ( saver )
	{
		delete saver;
		saver = nullptr;
	}
}

/*
==================
CLinkerSave::IsSaving
==================
*/
bool CLinkerSave::IsSaving() const
{
	return true;
}

/*
==================
CLinkerSave::Tell
==================
*/
uint32 CLinkerSave::Tell()
{
	return saver->Tell();
}

/*
==================
CLinkerSave::Seek
==================
*/
void CLinkerSave::Seek( uint32 InPosition )
{
	return saver->Seek( InPosition );
}

/*
==================
CLinkerSave::Flush
==================
*/
void CLinkerSave::Flush()
{
	saver->Flush();
}

/*
==================
CLinkerSave::IsEndOfFile
==================
*/
bool CLinkerSave::IsEndOfFile()
{
	return saver->IsEndOfFile();
}

/*
==================
CLinkerSave::GetSize
==================
*/
uint32 CLinkerSave::GetSize()
{
	return saver->GetSize();
}

/*
==================
CLinkerSave::Serialize
==================
*/
void CLinkerSave::Serialize( void* InBuffer, uint32 InSize )
{
	saver->Serialize( InBuffer, InSize );
}

/*
==================
CLinkerSave::operator<<
==================
*/
CArchive& CLinkerSave::operator<<( class CObject*& InValue )
{
	if ( !InValue )
	{
		return *this << CPackageIndex();
	}
	return *this << objectIndices[InValue->GetIndex()];
}