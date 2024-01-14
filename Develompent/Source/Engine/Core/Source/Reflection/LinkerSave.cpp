#include "Misc/CoreGlobals.h"
#include "Reflection/LinkerSave.h"
#include "Reflection/ObjectGC.h"
#include "System/BaseFileSystem.h"

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
	// Create file saver and mark archive as binary file
	saver = g_FileSystem->CreateFileWriter( InFilename, AW_NoFail );
	saver->SetType( AT_BinaryFile );
	SetType( AT_BinaryFile );

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