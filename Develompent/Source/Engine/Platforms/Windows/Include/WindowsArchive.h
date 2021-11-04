/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WINDOWSARCHIVE_H
#define WINDOWSARCHIVE_H

#include <fstream>

#include "Core.h"
#include "System/Archive.h"

 /**
  * @ingroup WindowsPlatform
  * @brief The class for reading archive on Windows
  */
class FWindowsArchiveReading : public FArchive
{
public:
	/**
	 * @brief Constructor
	 * @param[in] InFile Link to file
	 */
									FWindowsArchiveReading( std::ifstream* InFile );

	/**
	 * @brief Destructor
	 */
									~FWindowsArchiveReading();

	/**
	 * @brief Serialize data
	 *
	 * @param[in] InBuffer Pointer to buffer for serialize
	 * @param[in] InSize Size of buffer
	 */
	virtual void					Serialize( void* InBuffer, uint32 InSize ) override;

	/**
	 * @brief Get current position in archive
	 * @return Current position in archive
	 */
	virtual uint32					Tell() override;

	/**
	 * @brief Set current position in archive
	 *
	 * @param[in] InPosition New position in archive
	 */
	virtual void					Seek( uint32 InPosition ) override;

	/**
	 * @brief Flush data
	 */
	virtual void					Flush() override;

	/**
	 * @breif Is loading archive
	 * @return True if archive loading, false if archive saving
	 */
	virtual bool					IsLoading() const;

	/**
	 * @brief Get size of archive
	 * @return Size of archive
	 */
	virtual uint32					GetSize() override;

	/**
	 * @brief Get file handle
	 * @return Pointer to file
	 */
	FORCEINLINE std::ifstream*		GetHandle() const
	{
		return file;
	}

private:
	std::ifstream*				file;			/**< Pointer to file */
};

/**
 * @ingroup WindowsPlatform
 * @brief The class for writing archive on Windows
 */
class FWindowsArchiveWriter : public FArchive
{
public:
	/**
	 * @brief Constructor
	 * @param[in] InFile Link to file
	 */
							FWindowsArchiveWriter( std::ofstream* InFile );

	/**
	 * @brief Destructor
	 */
							~FWindowsArchiveWriter();

	/**
	 * @brief Serialize data
	 *
	 * @param[in] InBuffer Pointer to buffer for serialize
	 * @param[in] InSize Size of buffer
	 */
	virtual void			Serialize( void* InBuffer, uint32 InSize ) override;

	/**
	 * @brief Get current position in archive
	 * @return Current position in archive
	 */
	virtual uint32			Tell() override;

	/**
	 * @brief Set current position in archive
	 *
	 * @param[in] InPosition New position in archive
	 */
	virtual void			Seek( uint32 InPosition ) override;

	/**
	 * @brief Flush data
	 */
	virtual void			Flush() override;

	/**
	 * @brief Is saving archive
	 * @return True if archive saving, false if archive loading
	 */
	virtual bool			IsSaving() const;

	/**
	 * @brief Get size of archive
	 * @return Size of archive
	 */
	virtual uint32			GetSize() override;

	/**
	 * @brief Get file handle
	 * @return Pointer to file
	 */
	FORCEINLINE std::ofstream* GetHandle() const
	{
		return file;
	}

private:
	std::ofstream*			file;		/**< Pointer to file */
};

#endif // !WINDOWSARCHIVE_H
