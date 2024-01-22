/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LINKERSAVE_H
#define LINKERSAVE_H

#include "Reflection/Linker.h"

/**
 * @ingroup Core
 * @brief Handles saving LifeEngine package files
 */
class CLinkerSave : public CLinker, public CArchive
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InRoot		The top-level CObjectPackage object for the package associated with this linker
	 * @param InFilename	The name of the file for this package
	 */
	CLinkerSave( CObjectPackage* InRoot, const tchar* InFilename );

	/**
	 * @brief Destructor
	 */
	virtual ~CLinkerSave();

	/**
	 * @brief Detaches file saver and hence file handle
	 */
	void Detach();

	/**
	 * @brief Is saving archive
	 * @return True if archive saving, false if archive loading
	 */
	virtual bool IsSaving() const override;

	/**
	 * @brief Get current position in archive
	 * @return Current position in archive
	 */
	virtual uint32 Tell() override;

	/**
	 * @brief Set current position in archive
	 *
	 * @param InPosition	New position in archive
	 */
	virtual void Seek( uint32 InPosition ) override;

	/**
	 * @brief Flush data
	 */
	virtual void Flush() override;

	/**
	 * Is end of file
	 * @return Return true if end of file, else return false
	 */
	virtual bool IsEndOfFile() override;

	/**
	 * @brief Get size of archive
	 * @return Size of archive
	 */
	virtual uint32 GetSize() override;

	/**
	 * @brief Sets a flag indicating that this archive needs to filter editor-only content
	 * @param InFilterEditorOnly	Whether to filter editor-only content
	 */
	virtual void SetFilterEditorOnly( bool InFilterEditorOnly ) override;

	/**
	 * @brief Set the cooking target platform
	 * @note In build without editor do nothing
	 *
	 * @param InCookingTarget	The target platform to set
	 */
	virtual void SetCookingTarget( CBaseTargetPlatform* InCookingTarget ) override;

	/**
	 * @brief Serialize data
	 *
	 * @param InBuffer	Pointer to buffer for serialize
	 * @param InSize	Size of buffer
	 */
	virtual void Serialize( void* InBuffer, uint32 InSize ) override;

	/**
	 * @brief Override operator << for serialize CObjects
	 * @return Return reference to self
	 */
	virtual CArchive& operator<<( class CObject*& InValue ) override;

	/**
	 * @brief Get object indeces
	 * Location of the resource for a CObject is stored in the ObjectIndices array using the CObject's Index
	 * 
	 * @return Return object indeces
	 */
	FORCEINLINE std::vector<CPackageIndex>& GetObjectIndices()
	{
		return objectIndices;
	}

private:
	CArchive*						saver;			/**< The archive that actually writes the data to disk */
	std::vector<CPackageIndex>		objectIndices;	/**< Index array. Location of the resource for a CObject is stored in the ObjectIndices array using the CObject's Index */
};

#endif // !LINKERSAVE_H