/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Core/Misc/CoreGlobals.h"
#include "Core/System/BaseFileSystem.h"
#include "Core/System/BaseWindow.h"
#include "Editor/System/FileDialog.h"

/*
==================
Sys_ShowFileInExplorer
==================
*/
void Sys_ShowFileInExplorer( const std::wstring& InPath )
{
	CFilename		filename( g_FileSystem->ConvertToAbsolutePath( InPath ) );
	Sys_CreateProc( TEXT( "explorer.exe" ), g_FileSystem->IsDirectory( filename.GetFullPath() ) ? filename.GetFullPath().c_str() : filename.GetPath().c_str(), true, false, false, 0 );
}

/*
==================
Sys_ShowOpenFileDialog
==================
*/
bool Sys_ShowOpenFileDialog( const CFileDialogSetup& InSetup, OpenFileDialogResult& OutResult )
{
	OPENFILENAME		fileDialogSettings;
	Sys_Memzero( &fileDialogSettings, sizeof( OPENFILENAME ) );
	fileDialogSettings.lStructSize = sizeof( OPENFILENAME );

	// Flags
	fileDialogSettings.hwndOwner	= g_Window ? ( HWND )g_Window->GetHandle() : nullptr;
	fileDialogSettings.Flags		|= ( InSetup.IsMultiselection() ) ? OFN_ALLOWMULTISELECT : 0;
	fileDialogSettings.Flags		|= OFN_EXPLORER | OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST | OFN_EXTENSIONDIFFERENT;

	// File name buffer
	std::wstring		fileNameBuffer;
	fileNameBuffer.resize( 64 * 1024 );
	fileDialogSettings.lpstrFile	= fileNameBuffer.data();
	fileDialogSettings.nMaxFile		= fileNameBuffer.size();

	// Format filters
	std::wstring		filterBuffer;
	const std::vector<CFileDialogSetup::FileNameFilter>		fileNameFilters = InSetup.GetFormats();
	if ( fileNameFilters.empty() )
	{
		fileDialogSettings.nFilterIndex		= 0;
		fileDialogSettings.lpstrFilter		= TEXT( "All Formats (*.*)" );
	}
	else	// We are going to filter only those files with the same exact filename
	{
		std::wstring		allSupportedFormats;
		for ( uint32 index = 0, count = fileNameFilters.size(); index < count; ++index )
		{
			const CFileDialogSetup::FileNameFilter&	fileNameFilter = fileNameFilters[index];
			if ( index > 0 )
			{
				allSupportedFormats += TEXT( "; " );
			}
			allSupportedFormats += fileNameFilter.filter;
			
			filterBuffer		+= CString::Format( TEXT( "%s (%s)" ), fileNameFilter.description.c_str(), fileNameFilter.filter.c_str() );
			filterBuffer.push_back( TEXT( '\0' ) );
			filterBuffer		+= fileNameFilter.filter;
			filterBuffer.push_back( TEXT( '\0' ) );
		}
		filterBuffer			+= CString::Format( TEXT( "All Supported Formats (%s)" ), allSupportedFormats.c_str() );
		filterBuffer.push_back( TEXT( '\0' ) );
		filterBuffer			+= allSupportedFormats;
		filterBuffer.push_back( TEXT( '\0' ) );

		fileDialogSettings.nFilterIndex		= 0;
		fileDialogSettings.lpstrFilter		= filterBuffer.data();
	}

	// Default file extension
	std::wstring		defaultFileExtension = InSetup.GetDefaultExtension();
	if ( !defaultFileExtension.empty() )
	{
		fileDialogSettings.lpstrDefExt = defaultFileExtension.data();
	}

	// Title text
	std::wstring		title = InSetup.GetTitle();
	if ( title.empty() )
	{
		fileDialogSettings.lpstrTitle	= InSetup.IsMultiselection() ? TEXT( "Open Files" ) : TEXT( "Open File" );
	}
	else
	{
		fileDialogSettings.lpstrTitle	= title.data();
	}

	// Preserve the directory around the calls
	std::wstring		absolutePathToDirectory = g_FileSystem->ConvertToAbsolutePath( InSetup.GetDirectory() );
	fileDialogSettings.lpstrInitialDir	= absolutePathToDirectory.data();

	// Open file dialog
	std::wstring		originalCurrentDir = g_FileSystem->GetCurrentDirectory();
	if ( !GetOpenFileNameW( &fileDialogSettings ) )
	{
		return false;
	}
	SetCurrentDirectoryW( originalCurrentDir.c_str() );

	// Extract file paths
	{
		std::vector<std::wstring>		parts;

		// Parse file names
		tchar*	pPos	= fileNameBuffer.data();
		tchar*	pStart	= fileNameBuffer.data();
		while ( true )
		{
			if ( *pPos == 0 )
			{
				if ( pPos == pStart )
				{
					break;
				}
				
				parts.push_back( std::wstring( pStart ) );
				pStart	= pPos + 1;
				pPos	+= 1;
			}
			else
			{
				++pPos;
			}
		}

		if ( !parts.empty() )
		{
			// If single paths
			if ( parts.size() == 1 )
			{
				OutResult.files.push_back( parts[0] );
			}
			// When multiple paths are selected the first entry in the list is the directory
			else
			{
				std::wstring	basePath = parts[0];
				for ( uint32 index = 1, count = parts.size(); index < count; ++index )
				{
					OutResult.files.push_back( basePath + PATH_SEPARATOR + parts[index] );
				}
			}
		}
	}

	return true;
}

/*
==================
Sys_ShowSaveFileDialog
==================
*/
bool Sys_ShowSaveFileDialog( const CFileDialogSetup& InSetup, SaveFileDialogResult& OutResult )
{
	OPENFILENAME		fileDialogSettings;
	Sys_Memzero( &fileDialogSettings, sizeof( OPENFILENAME ) );
	fileDialogSettings.lStructSize = sizeof( OPENFILENAME );

	// Flags
	fileDialogSettings.hwndOwner	= g_Window ? ( HWND )g_Window->GetHandle() : nullptr;
	fileDialogSettings.Flags		|= OFN_EXPLORER | OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST | OFN_NOTESTFILECREATE | OFN_OVERWRITEPROMPT | OFN_EXTENSIONDIFFERENT;

	// File name buffer
	std::wstring		fileNameBuffer;
	fileNameBuffer.resize( 64 * 1024 );
	fileDialogSettings.lpstrFile	= fileNameBuffer.data();
	fileDialogSettings.nMaxFile		= fileNameBuffer.size();

	// Format filters
	std::wstring		filterBuffer;
	const std::vector<CFileDialogSetup::FileNameFilter>		fileNameFilters = InSetup.GetFormats();
	if ( fileNameFilters.empty() )
	{
		fileDialogSettings.nFilterIndex		= 0;
		fileDialogSettings.lpstrFilter		= TEXT( "All Formats (*.*)" );
	}
	else	// We are going to filter only those files with the same exact filename
	{
		std::wstring		allSupportedFormats;
		for ( uint32 index = 0, count = fileNameFilters.size(); index < count; ++index )
		{
			const CFileDialogSetup::FileNameFilter&	fileNameFilter = fileNameFilters[index];
			if ( index > 0 )
			{
				allSupportedFormats += TEXT( "; " );
			}
			allSupportedFormats += fileNameFilter.filter;
			
			filterBuffer		+= CString::Format( TEXT( "%s (%s)" ), fileNameFilter.description.c_str(), fileNameFilter.filter.c_str() );
			filterBuffer.push_back( TEXT( '\0' ) );
			filterBuffer		+= fileNameFilter.filter;
			filterBuffer.push_back( TEXT( '\0' ) );
		}
		filterBuffer			+= CString::Format( TEXT( "All Supported Formats (%s)" ), allSupportedFormats.c_str() );
		filterBuffer.push_back( TEXT( '\0' ) );
		filterBuffer			+= allSupportedFormats;
		filterBuffer.push_back( TEXT( '\0' ) );

		fileDialogSettings.nFilterIndex		= 0;
		fileDialogSettings.lpstrFilter		= filterBuffer.data();
	}

	// Default file extension
	std::wstring		defaultFileExtension = InSetup.GetDefaultExtension();
	if ( !defaultFileExtension.empty() )
	{
		fileDialogSettings.lpstrDefExt = defaultFileExtension.data();
	}

	// Title text
	std::wstring		title = InSetup.GetTitle();
	if ( title.empty() )
	{
		fileDialogSettings.lpstrTitle	= InSetup.IsMultiselection() ? TEXT( "Save Files" ) : TEXT( "Save File" );
	}
	else
	{
		fileDialogSettings.lpstrTitle	= title.data();
	}

	// Preserve the directory around the calls
	std::wstring		absolutePathToDirectory = g_FileSystem->ConvertToAbsolutePath( InSetup.GetDirectory() );
	fileDialogSettings.lpstrInitialDir	= absolutePathToDirectory.data();

	// Open file dialog
	std::wstring		originalCurrentDir = g_FileSystem->GetCurrentDirectory();
	if ( !GetSaveFileNameW( &fileDialogSettings ) )
	{
		return false;
	}
	SetCurrentDirectoryW( originalCurrentDir.c_str() );

	// Get selected format
	if ( fileDialogSettings.nFilterIndex && fileDialogSettings.nFilterIndex <= fileNameFilters.size() )
	{
		OutResult.selectedFormat = fileNameFilters[fileDialogSettings.nFilterIndex - 1];
	}
	else if ( !fileNameFilters.empty() )
	{
		OutResult.selectedFormat = fileNameFilters[0];
	}

	// Extract file paths
	{
		std::vector<std::wstring>		parts;

		// Parse file names
		tchar*	pPos	= fileNameBuffer.data();
		tchar*	pStart	= fileNameBuffer.data();
		while ( true )
		{
			if ( *pPos == 0 )
			{
				if ( pPos == pStart )
				{
					break;
				}
				
				parts.push_back( std::wstring( pStart ) );
				pStart	= pPos + 1;
				pPos	+= 1;
			}
			else
			{
				++pPos;
			}
		}

		if ( !parts.empty() )
		{
			// If single paths
			if ( parts.size() == 1 )
			{
				OutResult.files.push_back( parts[0] );
			}
			// When multiple paths are selected the first entry in the list is the directory
			else
			{
				std::wstring	basePath = parts[0];
				for ( uint32 index = 1, count = parts.size(); index < count; ++index )
				{
					OutResult.files.push_back( basePath + PATH_SEPARATOR + parts[index] );
				}
			}
		}
	}

	return true;
}