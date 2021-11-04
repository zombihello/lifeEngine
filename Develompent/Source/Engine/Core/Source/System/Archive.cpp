#include "System/Archive.h"
#include "LEVersion.h"

FArchive::FArchive() :
	arVer( VER_PACKAGE_LATEST ),
	arType( AT_TextFile )
{}

void FArchive::SerializePackageHeader()
{
	// Serialize header package
	uint32		packageFileTag = PACKAGE_FILE_TAG;
	*this << packageFileTag;
	checkMsg( packageFileTag == PACKAGE_FILE_TAG, TEXT( "Unknown package file tag. Current package file tag is 0x%X, need 0x%X" ), packageFileTag, PACKAGE_FILE_TAG );

	*this << arVer;
	checkMsg( arVer >= VER_MinVersion, TEXT( "Min supported version %i, but current version is %i" ), VER_MinVersion, arVer );

	*this << arType;
}