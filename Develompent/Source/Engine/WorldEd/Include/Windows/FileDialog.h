/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <string>

#include "Misc/Types.h"
#include "System/Package.h"
#include "CoreDefines.h"

/**
 * @ingroup WorldEd
 * @brief Open/Save dialog options
 */
class CFileDialogSetup
{
public:
	/**
	 * @brief File name filter
	 */
	struct SFileNameFilter
	{
		/**
		 * @brief Constructor
		 */
		FORCEINLINE SFileNameFilter()
		{}

		/**
		 * @brief Constructor
		 * 
		 * @param InFilter			Filter
		 * @param InDescription		Description
		 */
		FORCEINLINE SFileNameFilter( const std::wstring& InFilter, const std::wstring& InDescription )
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
	void AddFormat( const CAssetFactory::SAssetImporterInfo& InImporterInfo, const std::wstring& InDescription );

	/**
	 * @brief Add format
	 * 
	 * @param InExtension		File extension
	 * @param InDescription		File description
	 */
	FORCEINLINE void AddFormat( const std::wstring& InExtension, const std::wstring& InDescription )
	{
		AddFormat( SFileNameFilter( InExtension, InDescription ) );
	}

	/**
	 * @brief Add format
	 * @param InFilerInfo		File name's filer info
	 */
	FORCEINLINE void AddFormat( const SFileNameFilter& InFilerInfo )
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
	FORCEINLINE const std::vector<SFileNameFilter>& GetFormats() const
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
	std::vector<SFileNameFilter>	fileNameFilters;	/**< Array of file name filters */
};

/**
 * @ingroup WorldEd
 * @brief Result of file open operation
 */
struct SOpenFileDialogResult
{
	std::vector<std::wstring>				files;				/**< Selected files */
};

/**
 * @ingroup WorldEd
 * @brief Result of file save operation
 */
struct SSaveFileDialogResult
{
	CFileDialogSetup::SFileNameFilter		selectedFormat;		/**< Selected format */
	std::vector<std::wstring>				files;				/**< Output files */
};

/**
 * @ingroup WorldEd
 * @note Need implement on each platform
 * @brief Create a "Open File" dialog
 *
 * @param InSetup		File dialog setup
 * @param OutResult		File dialog result
 * @return Return TRUE when user clicks OK, otherwise will returning FALSE
 */
bool appShowOpenFileDialog( const CFileDialogSetup& InSetup, SOpenFileDialogResult& OutResult );

/**
 * @ingroup WorldEd
 * @note Need implement on each platform
 * @brief Create a "Save File" dialog
 *
 * @param InSetup		File dialog setup
 * @param OutResult		File dialog result
 * @return Return TRUE when user clicks OK, otherwise will returning FALSE
 */
bool appShowSaveFileDialog( const CFileDialogSetup& InSetup, SSaveFileDialogResult& OutResult );

#endif // !FILEDIALOG_H