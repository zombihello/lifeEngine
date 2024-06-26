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
CLinkerSave::GetLinker
==================
*/
CLinker* CLinkerSave::GetLinker() const
{
	return ( CLinker* )this;
}

/*
==================
CLinkerSave::SetFilterEditorOnly
==================
*/
void CLinkerSave::SetFilterEditorOnly( bool InFilterEditorOnly )
{
	saver->SetFilterEditorOnly( InFilterEditorOnly );
	arIsFilterEditorOnly = InFilterEditorOnly;
}

/*
==================
CLinkerSave::SetCookingTarget
==================
*/
void CLinkerSave::SetCookingTarget( CBaseTargetPlatform* InCookingTarget )
{
#if WITH_EDITOR
	saver->SetCookingTarget( InCookingTarget );
	cookingTargetPlatform = InCookingTarget;

	// When we do cooking want serialize properties untagged format for faster loading in a shipping build
	if ( cookingTargetPlatform )
	{
		saver->SetWantBinaryPropertySerialization( true );
		arWantBinaryPropertySerialization = true;
	}
#endif // WITH_EDITOR
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

/*
==================
CLinkerSave::operator<<
==================
*/
CArchive& CLinkerSave::operator<<( class CName& InValue )
{
	uint32	nameIndex = nameIndices[InValue.GetIndex()];
	if ( nameIndex == INDEX_NONE )
	{
		Sys_Error( TEXT( "Name \"%s\" is not mapped when saving %s (object: %s, property: %s)" ), InValue.ToString().c_str(), arPath.c_str() );
	}

	uint32	number = InValue.GetNumber();
	*this << nameIndex << number;
	return *this;
}

/*
==================
CLinkerSave::operator<<
==================
*/
CArchive& CLinkerSave::operator<<( const class CName& InValue )
{
	*this << const_cast<CName*>( &InValue );
	return *this;
}