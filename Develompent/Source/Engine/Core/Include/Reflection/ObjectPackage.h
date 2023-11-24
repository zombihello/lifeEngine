/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef REFLECTIONPACKAGE_H
#define REFLECTIONPACKAGE_H

#include <string>
#include <vector>
#include <unordered_map>

#include "Reflection/Object.h"
#include "System/Archive.h"

/**
 * @ingroup Core
 * @brief Package whom contains CObjects
 */
class CObjectPackage : public CArchive
{
public:
	/**
	 * @brief Constructor
	 */
	CObjectPackage();

	/**
	 * @brief Destructor
	 */
	~CObjectPackage();

	/**
	 * @brief Object export
	 */
	struct ObjectExport
	{
		/**
		 * @brief Constructor
		 * @param InObject	Object
		 */
		FORCEINLINE ObjectExport( CObject* InObject = nullptr )
			: dataSize( 0 )
			, object( InObject )
		{}

		/**
		 * @brief Destructor
		 */
		FORCEINLINE ~ObjectExport()
		{
			if ( object && object->HasAnyObjectFlags( OBJECT_NeedDestroy ) )
			{
				delete object;
			}
		}

		/**
		 * @brief Get object
		 * @return Return created object. If object in this object export already exist returns it
		 */
		CObject* GetObject();

		/**
		 * @brief Is valid object
		 * @return Return TRUE if object in the object export is valid, otherwise returns FALSE
		 */
		FORCEINLINE bool IsValidObject() const
		{
			return object;
		}

		CName		className;			/**< Class name for this export's CObject. If NAME_None this is CClass */
		CName		superClassName;		/**< Super class name for this export's CObject. If Name_None this is CObject */
		CName		objectName;			/**< The object name */
		uint32		dataSize;			/**< The number of bytes to serialize when saving/loading this export's CObject. If INVALID_ID the object export not saved yet */
		
	private:
		CObject*	object;				/**< The CObject represented by this export */
	};

	/**
	 * @brief Add object into the package
	 *
	 * @param InObject	Object
	 * @return Return index of a new object export
	 */
	uint32 AddObject( CObject* InObject );

	/**
	 * @brief Remove objects from the package
	 */
	void RemoveAllObjects();

	/**
	 * @brief Load package
	 * 
	 * @param InPath	Path to package
	 * @return Return TRUE if package has been successfully loaded, otherwise returns FALSE
	 */
	bool Load( const std::wstring& InPath );

	/**
	 * @brief Save package
	 *
	 * @param InPath	Path to package
	 * @return Return TRUE if package has been successfully saved, otherwise returns FALSE
	 */
	bool Save( const std::wstring& InPath );

	/**
	 * @brief Is empty
	 * @return Return TRUE if the package is empty, otherwise returns FALSE
	 */
	FORCEINLINE bool IsEmpty() const
	{
		return objectExports.empty();
	}

	/**
	 * @brief Get number of objects in the package
	 * @return Return number of objects in the package
	 */
	FORCEINLINE uint32 GetNumObjects() const
	{
		return objectExports.size();
	}

	/**
	 * @brief Get object by index
	 * 
	 * @param InIndex	Object index
	 * @return Return object in the package
	 */
	FORCEINLINE CObject* GetObject( uint32 InIndex )
	{
		Assert( InIndex >= 0 && InIndex < objectExports.size() );
		return objectExports[InIndex]->GetObject();
	}

	/**
	 * @brief Override operator << for serialize CObjects
	 * @return Return reference to self
	 */
	virtual CArchive& operator<<( class CObject*& InValue ) override;

	/**
	 * @brief Get current position in archive
	 * @return Current position in archive
	 */
	virtual uint32 Tell() override;

	/**
	 * @brief Set current position in archive
	 *
	 * @param[in] InPosition New position in archive
	 */
	virtual void Seek( uint32 InPosition ) override;

	/**
	 * @brief Flush data
	 */
	virtual void Flush() override;

	/**
	 * @brief Is saving archive
	 * @return True if archive saving, false if archive loading
	 */
	virtual bool IsSaving() const override;

	/**
	 * @breif Is loading archive
	 * @return True if archive loading, false if archive saving
	 */
	virtual bool IsLoading() const override;

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

private:
	/**
	 * @brief Serialize data
	 *
	 * @param InBuffer	Pointer to buffer for serialize
	 * @param InSize	Size of buffer
	 */
	virtual void Serialize( void* InBuffer, uint32 InSize ) override;

	CArchive*								originalArchive;	/**< Original archive to save/load data */
	std::unordered_map<CObject*, uint32>	objectExportsMap;	/**< Map of objects= exports for contains it's index */
	std::vector<ObjectExport*>				objectExports;		/**< Array of object exports */
};

#endif // !REFLECTIONPACKAGE_H