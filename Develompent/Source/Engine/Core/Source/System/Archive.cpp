#include "System/Archive.h"
#include "LEVersion.h"

FArchive::FArchive() :
	arVer( VER_PACKAGE_LATEST ),
	arType( AT_TextFile )
{}

void FArchive::SerializeHeader()
{
	// Serialize header package
	uint32		archiveFileTag = ARCHIVE_FILE_TAG;
	*this << archiveFileTag;
	checkMsg( archiveFileTag == ARCHIVE_FILE_TAG, TEXT( "Unknown archive file tag. Current archive file tag is 0x%X, need 0x%X" ), archiveFileTag, ARCHIVE_FILE_TAG );

	*this << arVer;
	checkMsg( arVer >= VER_MinVersion, TEXT( "Min supported version %i, but current version is %i" ), VER_MinVersion, arVer );

	*this << arType;
}