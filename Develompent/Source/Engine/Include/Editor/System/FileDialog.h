/**
 * @file
 * @addtogroup Editor Editor
 *
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

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <string>

#include "Core/Misc/Types.h"
#include "Core/System/Package.h"
#include "Core/CoreDefines.h"

/**
 * @ingroup Editor
 * @brief Open/Save dialog options
 */
class CFileDialogSetup
{
public:
	/**
	 * @brief File name filter
	 */
	struct FileNameFilter
	{
		/**
		 * @brief Constructor
		 */
		FORCEINLINE FileNameFilter()
		{}

		/**
		 * @brief Constructor
		 * 
		 * @param InFilter			Filter
		 * @param InDescription		Description
		 */
		FORCEINLINE FileNameFilter( const std::wstring& InFilter, const std::wstring& InDescription )
			: filter( InFilter )
			, description( InDescription )
		{}

		std::wstring	filter;			/**< Filter (ex. *.png;*.bmp) */
		std::wstring	description;	/**< Description */
	};

	/**
	 * @brief Constructor
	 */
	FORCEINLINE CFileDialogSetup()
		: bMultiselection( false )
	{}

	/**
	 * @brief Add format
	 * 
	 * @param InImporterInfo	Asset importer info
	 * @param InDescription		Description
	 */
	void AddFormat( const CAssetFactory::AssetImporterInfo& InImporterInfo, const std::wstring& InDescription );

	/**
	 * @brief Add format
	 * 
	 * @param InExtension		File extension
	 * @param InDescription		File description
	 */
	FORCEINLINE void AddFormat( const std::wstring& InExtension, const std::wstring& InDescription )
	{
		AddFormat( FileNameFilter( InExtension, InDescription ) );
	}

	/**
	 * @brief Add format
	 * @param InFilerInfo		File name's filer info
	 */
	FORCEINLINE void AddFormat( const FileNameFilter& InFilerInfo )
	{
		fileNameFilters.push_back( InFilerInfo );
	}

	/**
	 * @brief Enable/Disable multiple files (only open)
	 * @param InMultiselection		Is need use multiselection of files (only open)
	 */
	FORCEINLINE void SetMultiselection( bool InMultiselection )
	{
		bMultiselection = InMultiselection;
	}

	/**
	 * @brief Set dialog's title
	 * @param InTitle	Dialog's title
	 */
	FORCEINLINE void SetTitle( const std::wstring& InTitle )
	{
		title = InTitle;
	}

	/**
	 * @brief Set initial directory
	 * 
	 * @param InPath	Path to directory
	 */
	FORCEINLINE void SetDirectory( const std::wstring& InPath )
	{
		directoryPath = InPath;
	}

	/**
	 * @brief Set initial file name
	 * 
	 * @param InFileName	Initial file name
	 */
	FORCEINLINE void SetFileName( const std::wstring& InFileName )
	{
		fileName = InFileName;
	}

	/**
	 * @brief Set default file extension
	 * @param InDefaultExtension	Default extension (without '.')
	 */
	FORCEINLINE void SetDefaultExtension( const std::wstring& InDefaultExtension )
	{
		defaultExtension = InDefaultExtension;
	}

	/**
	 * @brief Is need use multiple selection files (only open)
	 * @return Return TRUE if need use multiple selection files, otherwise will returning FALSE
	 */
	FORCEINLINE bool IsMultiselection() const
	{
		return bMultiselection;
	}

	/**
	 * @brief Get window's title
	 * @return Return window's title
	 */
	FORCEINLINE const std::wstring& GetTitle() const
	{
		return title;
	}

	/**
	 * @brief Get initial path to the directory
	 * @return Return initial path to the directory
	 */
	FORCEINLINE const std::wstring& GetDirectory() const
	{
		return directoryPath;
	}

	/**
	 * @brief Get initial file name
	 * @return Return initial file name
	 */
	FORCEINLINE const std::wstring& GetFileName() const
	{
		return fileName;
	}

	/**
	 * @brief Get array of formats
	 * @return Return array of formats
	 */
	FORCEINLINE const std::vector<FileNameFilter>& GetFormats() const
	{
		return fileNameFilters;
	}

	/**
	 * @brief Get default file extension
	 * @return Return default file extension
	 */
	FORCEINLINE const std::wstring& GetDefaultExtension() const
	{
		return defaultExtension;
	}

private:
	bool							bMultiselection;	/**< Is use multiselection */
	std::wstring					title;				/**< Dialog window's title */
	std::wstring					directoryPath;		/**< Initial path to directory */
	std::wstring					fileName;			/**< Initial file name */
	std::wstring					defaultExtension;	/**< Default file extension */
	std::vector<FileNameFilter>	fileNameFilters;	/**< Array of file name filters */
};

/**
 * @ingroup Editor
 * @brief Result of file open operation
 */
struct OpenFileDialogResult
{
	std::vector<std::wstring>				files;				/**< Selected files */
};

/**
 * @ingroup Editor
 * @brief Result of file save operation
 */
struct SaveFileDialogResult
{
	CFileDialogSetup::FileNameFilter		selectedFormat;		/**< Selected format */
	std::vector<std::wstring>				files;				/**< Output files */
};

/**
 * @ingroup Editor
 * @note Need implement on each platform
 * @brief Create a "Open File" dialog
 *
 * @param InSetup		File dialog setup
 * @param OutResult		File dialog result
 * @return Return TRUE when user clicks OK, otherwise will returning FALSE
 */
bool Sys_ShowOpenFileDialog( const CFileDialogSetup& InSetup, OpenFileDialogResult& OutResult );

/**
 * @ingroup Editor
 * @note Need implement on each platform
 * @brief Create a "Save File" dialog
 *
 * @param InSetup		File dialog setup
 * @param OutResult		File dialog result
 * @return Return TRUE when user clicks OK, otherwise will returning FALSE
 */
bool Sys_ShowSaveFileDialog( const CFileDialogSetup& InSetup, SaveFileDialogResult& OutResult );

#endif // !FILEDIALOG_H