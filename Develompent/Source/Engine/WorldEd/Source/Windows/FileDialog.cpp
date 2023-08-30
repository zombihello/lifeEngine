#include "Containers/String.h"
#include "Windows/FileDialog.h"

/*
==================
CFileDialogSetup::AddFormat
==================
*/
void CFileDialogSetup::AddFormat( const CAssetFactory::AssetImporterInfo& InImporterInfo, const std::wstring& InDescription )
{
	std::wstring	filter;
	for ( uint32 index = 0, count = InImporterInfo.supportedExtensions.size(); index < count; ++index )
	{
		if ( index > 0 )
		{
			filter += TEXT( ";" );
		}
		filter += CString::Format( TEXT( "*.%s" ), InImporterInfo.supportedExtensions[index].c_str() );
	}

	AddFormat( filter, InDescription );
}