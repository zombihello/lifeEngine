/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PACKAGE_H
#define PACKAGE_H

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "Misc/Types.h"
#include "Misc/RefCounted.h"
#include "Misc/RefCountPtr.h"
#include "Misc/SharedPointer.h"
#include "Misc/SharedPointerInternals.h"
#include "Misc/Misc.h"
#include "Misc/Guid.h"
#include "Misc/TableOfContents.h"
#include "Misc/CoreGlobals.h"
#include "Reflection/Object.h"
#include "System/Delegate.h"
#include "System/Archive.h"
#include "System/Name.h"

/**
 * @ingroup Core
 * Reference to CPackage
 */
typedef TRefCountPtr< class CPackage >			PackageRef_t;

/**
 * @ingroup Core
 * Enumeration of type asses in package
 */
enum EAssetType
{
	AT_Unknown,								/**< Unknown asset */
	AT_FirstType,							/**< First type */

	AT_Texture2D = AT_FirstType,			/**< Texture 2D */
	AT_Material,							/**< Material */
	AT_Script,								/**< Script */
	AT_StaticMesh,							/**< Static mesh */
	AT_AudioBank,							/**< Audio bank */
	AT_PhysicsMaterial,						/**< Physics material */
	
	AT_LastType = AT_PhysicsMaterial,		/**< Last asset type for calculate count */
	AT_Count	= AT_LastType+1,			/**< Count asset types */
};

/**
 * @ingroup Core
 * Convert text to asset type
 * 
 * @param InText	Asset type in text format
 * @return Return converted asset type to enumeration
 */
FORCEINLINE EAssetType ConvertTextToAssetType( const std::wstring& InText )
{
	if ( InText == TEXT( "Texture2D" ) )
	{
		return AT_Texture2D;
	}
	else if ( InText == TEXT( "Material" ) )
	{
		return AT_Material;
	}
	else if ( InText == TEXT( "Script" ) )
	{
		return AT_Script;
	}
	else if ( InText == TEXT( "StaticMesh" ) )
	{
		return AT_StaticMesh;
	}
	else if ( InText == TEXT( "AudioBank" ) )
	{
		return AT_AudioBank;
	}
	else if ( InText == TEXT( "PhysicsMaterial" ) )
	{
		return AT_PhysicsMaterial;
	}
	return AT_Unknown;
}

/**
 * @ingroup Core
 * Convert asset type to text
 * 
 * @param InAssetType	Asset type
 * @return Return converted asset type to text
 */
FORCEINLINE std::wstring ConvertAssetTypeToText( EAssetType InAssetType )
{
	switch ( InAssetType )
	{
	case AT_Texture2D:			return TEXT( "Texture2D" );
	case AT_Material:			return TEXT( "Material" );
	case AT_Script:				return TEXT( "Script" );
	case AT_StaticMesh:			return TEXT( "StaticMesh" );
	case AT_AudioBank:			return TEXT( "AudioBank" );
	case AT_PhysicsMaterial:	return TEXT( "PhysicsMaterial" );

	case AT_Unknown:
	default:
		return TEXT( "" );
	}
}

/**
 * @ingroup Core
 * Struct reference to asset
 */
struct AssetReference
{
	/**
	 * Constructor
	 *
	 * @param[in] InType Asset type
	 * @param[in] InGUIDAsset GUID of asset
	 * @param[in] InGUIDPackage GUID of the package
	 */
	FORCEINLINE AssetReference( EAssetType InType = AT_Unknown, const CGuid& InGUIDAsset = CGuid(), const CGuid& InGUIDPackage = CGuid() )
		: type( InType ), guidAsset( InGUIDAsset ), guidPackage( InGUIDPackage )
	{}

	/**
	 * Is valid
	 * @return Return true if asset reference is valid, else return false
	 */
	FORCEINLINE bool IsValid() const
	{
		return type != AT_Unknown && guidAsset.IsValid() && guidPackage.IsValid();
	}

	EAssetType		type;				/**< Asset type */
	CGuid			guidAsset;			/**< GUID of asset */
	CGuid			guidPackage;		/**< GUID of the package */
};

/**
 * @ingroup Core
 * Asset info in package
 */
struct AssetInfo
{
	uint32						offset;		/**< Offset in archive to asset */
	uint32						size;		/**< Size data in archive */
	EAssetType					type;		/**< Asset type */
	std::wstring				name;		/**< Name of asset */
	TSharedPtr<class CAsset>	data;		/**< Pointer to asset (FMaterialRef, FTexture2DRef, etc) */
};

/**
 * @ingroup Core
 * @brief Object info in package
 */
struct ObjectInfo
{
	CName		className;		/**< Class name of this CObject. If NAME_None this is CClass */
	CName		superClassName;	/**< Super class name of this CObject. If Name_None this is CObject */
	CName		objectName;		/**< The object name */
	uint32		offset;			/**< Offset in the archive to CObject. If INVALID_ID the object not saved yet */
	uint32		size;			/**< Size data in the archive. If INVALID_ID the object export not saved yet */
	CObject*	object;			/**< Pointer to CObject */
};

/**
 * @ingroup Core
 * Asset handle for containing in TSharedPtr and TWeakPtr reference to asset and him pointer
 */
template<class ObjectType>
struct TAssetHandle
{
public:
	friend class CPackage;
	friend class CPackageManager;

	/**
	 * @brief Hash function for STL containers
	 */
	struct HashFunction
	{
		/**
		 * @brief Calculate hash of TAssetHandle
		 * @param InAssetPtr	Asset ptr
		 */
		FORCEINLINE std::size_t operator()( const TAssetHandle& InAssetPtr ) const
		{
			return Sys_MemFastHash( InAssetPtr.reference.Get(), InAssetPtr.asset.GetTypeHash() );
		}
	};

	/**
	 * @brief Constructor
	 */
	FORCEINLINE TAssetHandle()
	{}

	/**
	 * @brief Constructor
	 */
	FORCEINLINE TAssetHandle( std::nullptr_t )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InAssetPtr		Asset ptr
	 * @param InReferencePtr	Reference ptr
	 */
	FORCEINLINE TAssetHandle( const TWeakPtr<ObjectType>& InAssetPtr, const TSharedPtr<AssetReference>& InReferencePtr )
		: asset( InAssetPtr )
		, reference( InReferencePtr )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InAssetInfo		Asset info
	 */
	FORCEINLINE TAssetHandle( const AssetInfo& InAssetInfo )
		: asset( InAssetInfo.data )
		, reference( InAssetInfo.reference )
	{}

	/**
	 * @brief Constructor
	 * @param InAssetPtr	Asset ptr
	 */
	template< typename OtherType >
	FORCEINLINE TAssetHandle( const TAssetHandle<OtherType>& InAssetPtr )
		: asset( ( TWeakPtr<ObjectType> )InAssetPtr.asset )
		, reference( InAssetPtr.reference )
	{}

	/**
	 * @brief Constructor
	 * @param InAssetPtr	Asset ptr
	 */
	FORCEINLINE TAssetHandle( const TAssetHandle& InAssetPtr )
		: asset( InAssetPtr.asset )
		, reference( InAssetPtr.reference )
	{}

	/**
	 * @brief Constructor
	 * @param InAssetPtr	Asset ptr
	 */
	template< typename OtherType >
	FORCEINLINE TAssetHandle( TAssetHandle<OtherType>&& InAssetPtr )
		: asset( MoveTemp( ( TWeakPtr<ObjectType> )InAssetPtr.asset ) )
		, reference( MoveTemp( InAssetPtr.reference ) )
	{}

	/**
	 * @brief Constructor
	 * @param InAssetPtr	Asset ptr
	 */
	FORCEINLINE TAssetHandle( TAssetHandle&& InAssetPtr )
		: asset( MoveTemp( InAssetPtr.asset ) )
		, reference( MoveTemp( InAssetPtr.reference ) )
	{}

	/**
	 * @brief Overloaded operator =
	 *
	 * @param InAssetPtr	Asset ptr
	 * @return Return reference to current object
	 */
	FORCEINLINE TAssetHandle& operator=( const TAssetHandle& InAssetPtr )
	{
		asset		= InAssetPtr.asset;
		reference	= InAssetPtr.reference;
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 *
	 * @param InAssetPtr	Asset ptr
	 * @return Return reference to current object
	 */
	template< typename OtherType >
	FORCEINLINE TAssetHandle& operator=( const TAssetHandle<OtherType>& InAssetPtr )
	{
		asset		= InAssetPtr.asset;
		reference	= InAssetPtr.reference;
		return *this;
	}

	/**
	 * @brief Overloaded operator
	 *
	 * @param InAssetPtr	Asset ptr
	 * @return Return reference to current object
	 */
	template< typename OtherType >
	FORCEINLINE TAssetHandle& operator=( TAssetHandle<OtherType>&& InAssetPtr )
	{
		if ( this != ( TAssetHandle<ObjectType>* ) &InAssetPtr )
		{
			asset		= MoveTemp( InAssetPtr.asset );
			reference	= MoveTemp( InAssetPtr.reference );
		}
		return *this;
	}

	/**
	 * @brief Overloaded operator
	 *
	 * @param InAssetPtr	Asset ptr
	 * @return Return reference to current object
	 */
	FORCEINLINE TAssetHandle& operator=( TAssetHandle&& InAssetPtr )
	{
		if ( this != &InAssetPtr )
		{
			asset		= MoveTemp( InAssetPtr.asset );
			reference	= MoveTemp( InAssetPtr.reference );
		}
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 * @return Return reference to current object
	 */
	FORCEINLINE TAssetHandle& operator=( std::nullptr_t )
	{
		Reset();
		return *this;
	}

	/**
	 * @brief Overloaded operator ==
	 *
	 * @param InAssetPtr	Asset ptr
	 * @return Returning TRUE if pointers is equal, else returning FALSE
	 */
	template< typename OtherType >
	FORCEINLINE bool operator==( const TAssetHandle<OtherType>& InAssetPtr ) const
	{
		return asset == InAssetPtr.asset && reference == InAssetPtr.reference;
	}

	/**
	 * @brief Overloaded operator !=
	 *
	 * @param InAssetPtr	Asset ptr
	 * @return Returning TRUE if pointers is not equal, else returning FALSE
	 */
	template< typename OtherType >
	FORCEINLINE bool operator!=( const TAssetHandle<OtherType>& InAssetPtr ) const
	{
		return asset != InAssetPtr.asset && reference != InAssetPtr.reference;
	}

	/**
	 * @brief Overloaded operator ==
	 *
	 * @param InSharedPtr	Shared ptr
	 * @return Returning TRUE if pointers is equal, else returning FALSE
	 */
	template< typename OtherType >
	FORCEINLINE bool operator==( const TSharedPtr<OtherType>& InSharedPtr ) const
	{
		return asset == InSharedPtr;
	}

	/**
	 * @brief Overloaded operator !=
	 *
	 * @param InSharedPtr	Shared ptr
	 * @return Returning TRUE if pointers is not equal, else returning FALSE
	 */
	template< typename OtherType >
	FORCEINLINE bool operator!=( const TSharedPtr<OtherType>& InSharedPtr ) const
	{
		return asset != InSharedPtr;
	}

	/**
	 * @brief Resets this asset handle, removing a reference to the asset. If there are no other
	 * references to the asset then it will be destroyed
	 */
	FORCEINLINE void Reset()
	{
		*this = TAssetHandle<ObjectType>();
	}

	/**
	 * @brief Is valid asset handle
	 * @return Return TRUE if handle is valid
	 */
	FORCEINLINE bool IsValid() const
	{
		return reference.IsValid();
	}

	/**
	 * @brief Is valid asset
	 * @return Return TRUE if asset is loaded, else return FALSE
	 */
	FORCEINLINE bool IsAssetValid() const
	{
		return asset.IsValid();
	}

	/**
	 * @brief Get shared ptr to asset
	 * @return Return shared ptr to asset. If him is unloaded return invalid TSharedPtr
	 */
	FORCEINLINE TSharedPtr<ObjectType> ToSharedPtr() const
	{
		return asset.Pin();
	}

	/**
	 * @brief Get asset reference
	 * @return Return pointer to asset reference
	 */
	FORCEINLINE TSharedPtr<AssetReference> GetReference() const
	{
		return reference;
	}

	// Declare other actor handle types as friends as needed
	template< class OtherType > friend class TAssetHandle;

private:
	TWeakPtr<ObjectType>					asset;		/**< Pointer to asset */
	mutable TSharedPtr<AssetReference>		reference;	/**< Reference to asset for reload */
};

/**
 * @ingroup Core
 * @brief Base class for serialize assets in package
 */
class CAsset : public TSharedFromThis<CAsset>
{
public:
	friend class CPackage;

	/**
	 * @brief Typedef of dependent assets
	 */
	typedef std::unordered_set< TAssetHandle<CAsset>, TAssetHandle<CAsset>::HashFunction >		SetDependentAssets_t;

	/**
	 * @brief Constructor
	 * 
	 * @param[in] InType Asset type
	 */
	CAsset( EAssetType InType );

	/**
	 * @brief Destructor
	 */
	virtual ~CAsset();

	/**
	 * @brief Serialize
	 * 
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class CArchive& InArchive );

	/**
	 * @brief Set asset name
	 * 
	 * @param[in] InName Asset name
	 */
	void SetAssetName( const std::wstring& InName );

#if WITH_EDITOR
	/**
	 * @brief Set path to asset source file
	 * @param InPath		Path to asset source file
	 */
	FORCEINLINE void SetAssetSourceFile( const std::wstring& InPath )
	{
		if ( sourceFile != InPath )
		{
			MarkDirty();
		}
		sourceFile = InPath;
	}

	/**
	 * @brief Set this asset only for editor
	 * @param InIsOnlyEditor	Is asset only for editor
	 */
	FORCEINLINE void SetOnlyEditor( bool InIsOnlyEditor )
	{
		if ( bOnlyEditor != InIsOnlyEditor )
		{
			MarkDirty();
		}
		bOnlyEditor = InIsOnlyEditor;
	}
#endif // WITH_EDITOR

	/**
	 * @brief Is dirty asset
	 * @return Return TRUE if asset is dirty, else return FALSE
	 */
	FORCEINLINE bool IsDirty() const
	{
		return bDirty;
	}

	/**
	 * @brief Get asset type
	 * @return Return asset type
	 */
	FORCEINLINE EAssetType GetType() const
	{
		return type;
	}

	/**
	 * @brief Get GUID of asset
	 * @return Return GUID of asset
	 */
	FORCEINLINE const CGuid& GetGUID() const
	{
		return guid;
	}

	/**
	 * @brief Get name asset
	 * @return Return name asset
	 */
	FORCEINLINE const std::wstring& GetAssetName() const
	{
		return name;
	}

	/**
	 * @brief Get package
	 * @return Return package where the asset is located. If asset not located in package return nullptr
	 */
	class CPackage* GetPackage() const
	{
		return package;
	}

	/**
	 * @brief Get reference to asset
	 * @return Return struct info for reference to asset
	 */
	AssetReference GetAssetReference() const;

	/**
	 * @brief Get asset handle
	 * @return Return asset handle
	 */
	TAssetHandle<CAsset> GetAssetHandle() const;

#if WITH_EDITOR
	/**
	 * @brief Get path to asset source file
	 * @return Return path to asset source file
	 */
	FORCEINLINE const std::wstring& GetAssetSourceFile() const
	{
		return sourceFile;
	}

	/**
	 * @brief Is asset only for editor
	 * @return Return TRUE if this asset only for editor
	 */
	FORCEINLINE bool IsOnlyEditor() const
	{
		return bOnlyEditor;
	}
#endif // WITH_EDITOR

	/**
	 * @brief Get dependent assets
	 * @param OutDependentAssets	Output set of dependent assets
	 * @param InFilter				Filter of getting assets by type. If setted AT_Unknown return all types
	 */
	virtual void GetDependentAssets( SetDependentAssets_t& OutDependentAssets, EAssetType InFilter = AT_Unknown ) const;

	/**
	 * @brief Reload dependent assets
	 * @param InForce	Is need force reload assets
	 */
	virtual void ReloadDependentAssets( bool InForce = false );

protected:
	/**
	 * @brief Mark dirty asset
	 * @note When changing serializable fields, child classes should mark themselves as dirty
	 */
	void MarkDirty();

	/**
	 * @brief Fully load asset. This function called by CPackage::FullyLoad() for load ALL data
	 */
	virtual void OnFullyLoad();

private:
	bool							bDirty;				/**< Is asset is dirty */
	class CPackage*					package;			/**< The package where the asset is located */
	std::wstring					name;				/**< Name asset */
	CGuid							guid;				/**< GUID of asset */
	EAssetType						type;				/**< Asset type */
	mutable TAssetHandle<CAsset>	handle;				/**< Handle to this asset */

#if WITH_EDITOR
	std::wstring					sourceFile;			/**< Path to source file */
	bool							bOnlyEditor;		/**< Is asset only for editor */
#endif // WITH_EDITOR
};

/**
 * @ingroup Core
 * @brief Asset factory
 */
class CAssetFactory
{
public:
	/**
	 * @brief Pointer to function for create asset
	 */
	typedef TSharedPtr<CAsset>( *ConstructAssetFn_t )();

#if WITH_EDITOR
	/**
	 * @brief Enumerate result function 'ShowImportSettingsAsset' (see ShowImportSettingsAssetFn_t and CAssetFactory::ShowImportSettings)
	 */
	enum EResultShowImportSettings
	{
		RSIS_Import,		/**< Import asset */
		RSIS_ImportAll,		/**< Import all assets with current settings */
		RSIS_Cancel,		/**< Cancel import this asset */
	};

	/**
	 * @brief Pointer to function for import asset
	 */
	typedef bool( *ImportAssetFn_t )( const std::wstring& InPath, std::vector<TSharedPtr<CAsset>>& OutResult, std::wstring& OutError );

	/**
	 * @brief Pointer to function for reimport asset
	 */
	typedef bool( *ReimportAssetFn_t )( const TSharedPtr<CAsset>& InAsset, std::wstring& OutError );

	/**
	 * @brief Pointer to function for show import asset settings
	 */
	typedef void( *ShowImportSettingsAssetFn_t )( class CImGUILayer* InOwner, CThreadEvent& InEvent, EResultShowImportSettings& OutResult );

	/**
	 * @brief Struct info about asset's importer
	 */
	struct AssetImporterInfo
	{
		/**
		 * @brief Constructor
		 */
		FORCEINLINE AssetImporterInfo()
			: bValid( false )
			, importAssetFn( nullptr )
			, reimportAssetFn( nullptr )
			, showImportSettingsAssetFn( nullptr )
		{}

		/**
		 * @brief Clear
		 */
		FORCEINLINE void Clear()
		{
			bValid = false;
			importAssetFn = nullptr;
			reimportAssetFn = nullptr;
			showImportSettingsAssetFn = nullptr;
			supportedExtensions.clear();
		}

		bool							bValid;							/**< Is importer is exist */
		ImportAssetFn_t					importAssetFn;					/**< Pointer to function of import asset */
		ReimportAssetFn_t				reimportAssetFn;				/**< Pointer to function of reimport asset */
		ShowImportSettingsAssetFn_t		showImportSettingsAssetFn;		/**< Pointer to function of show import settings asset */
		std::vector<std::wstring>		supportedExtensions;			/**< Supported extensions */
	};
#endif // WITH_EDITOR

	/**
	 * @brief Constructor
	 */
	CAssetFactory();

	/**
	 * @brief Register asset type
	 *
	 * @param InFunction	Pointer to function for create asset
	 * @param InType		Asset type to create
	 */
	FORCEINLINE void Register( ConstructAssetFn_t InFunction, EAssetType InType )
	{
		constructAssetFns[InType] = InFunction;
	}

	/**
	 * @brief Unregister asset type
	 *
	 * @param InType		Asset type to create
	 */
	FORCEINLINE void UnRegister( EAssetType InType )
	{
		constructAssetFns[InType] = nullptr;
	}

#if WITH_EDITOR
	/**
	 * @brief Register importer for asset type
	 * 
	 * @param InImportFunction				Pointer to function for import asset
	 * @param InReimportFunction			Pointer to function for reimport asset
	 * @param InShowImportSettingsFunction	Pointer to function for show import settings. If it variable is null will not be used
	 * @param InSupportedExtensions			Array with supported extensions
	 * @param InType						Asset type
	 */
	FORCEINLINE void RegisterImporter( ImportAssetFn_t InImportFunction, ReimportAssetFn_t InReimportFunction, ShowImportSettingsAssetFn_t InShowImportSettingsFunction, const std::vector<std::wstring>& InSupportedExtensions, EAssetType InType )
	{
		AssetImporterInfo&		importInfo = importersInfo[InType];
		if ( importInfo.bValid )
		{
			importInfo.Clear();
		}

		importInfo.bValid						= true;
		importInfo.importAssetFn				= InImportFunction;
		importInfo.reimportAssetFn				= InReimportFunction;
		importInfo.supportedExtensions			= InSupportedExtensions;
		importInfo.showImportSettingsAssetFn	= InShowImportSettingsFunction;
	}

	/**
	 * @brief Unregister importer for asset type
	 * 
	 * @param InType		Asset type
	 */
	FORCEINLINE void UnRegisterImporter( EAssetType InType )
	{
		importersInfo[InType].Clear();
	}
#endif // WITH_EDITOR

	/**
	 * @brief Set default asset for type
	 *
	 * @param InAssetPtr	Asset
	 * @param InType		Asset type
	 */
	FORCEINLINE void SetDefault( const TAssetHandle<CAsset>& InAssetPtr, EAssetType InType )
	{
		defaultAssets[InType] = InAssetPtr;
	}

	/**
	 * @brief Create asset
	 *
	 * @param InType	Asset type to create
	 * @return Return pointer to asset if it type is registered
	 */
	FORCEINLINE TSharedPtr<CAsset> Create( EAssetType InType ) const
	{
		ConstructAssetFn_t		constructAsset = constructAssetFns[InType];
		if ( constructAsset )
		{
			return constructAsset();
		}

		return nullptr;
	}

#if WITH_EDITOR
	/**
	 * @brief Show import settings for asset type
	 * 
	 * @param InAssetType		Asset type
	 * @param InOwner			Owner ImGUI layer
	 * @param InEvent			Synchronize event, need trigger it when dialog is closed
	 * @param OutResult			Result
	 * @return Return TRUE if dialog is showed, otherwise will return FALSE
	 */
	bool ShowImportSettings( EAssetType InAssetType, class CImGUILayer* InOwner, CThreadEvent& InEvent, EResultShowImportSettings& OutResult ) const;

	/**
	 * @brief Import asset
	 * 
	 * @param InPath	Path to asset
	 * @param OutResult	Output array with imported assets
	 * @param OutError	Output error when failed import asset
	 * @return Return TRUE if asset seccussed imported, otherwise will return FALSE
	 */
	bool Import( const std::wstring& InPath, std::vector<TSharedPtr<CAsset>>& OutResult, std::wstring& OutError ) const;
	
	/**
	 * @brief Reimport asset
	 * 
	 * @param InAsset	Asset to reimport
	 * @param OutError	Output error when failed import asset
	 * @return Return TRUE when seccussed asset is reimported, otherwise will returning FALSE
	 */
	bool Reimport( const TSharedPtr<CAsset>& InAsset, std::wstring& OutError ) const;

	/**
	 * @brief Get asset type by path
	 * 
	 * @param InPath	Path to asset
	 * @return Return asset type if unknown will return AT_Unknown
	 */
	EAssetType GetAssetTypeByPath( const std::wstring& InPath ) const;

	/**
	 * @brief Get info about asset's importer
	 * 
	 * @param InType	Asset type
	 * @return Return info about asset's importer
	 */
	FORCEINLINE const AssetImporterInfo& GetImporterInfo( EAssetType InType ) const
	{
		return importersInfo[InType];
	}
#endif // WITH_EDITOR

	/**
	 * @brief Get default asset
	 *
	 * @param InType	Default asset type
	 * @return Return pointer to default asset if him exists
	 */
	FORCEINLINE TAssetHandle<CAsset> GetDefault( EAssetType InType ) const
	{
		return defaultAssets[InType];
	}

private:

#if WITH_EDITOR
	AssetImporterInfo					importersInfo[AT_Count];		/**< Importers info for each asset type */
#endif // WITH_EDITOR

	ConstructAssetFn_t					constructAssetFns[AT_Count];	/**< Array of pointers to function create asset */
	TAssetHandle<CAsset>				defaultAssets[AT_Count];		/**< Array of default assets */
};

/**
 * @ingroup Core
 * Class for help add, remove and find assets in package
 */
class CPackage : public CRefCounted
{
public:
	friend class CAsset;
	friend class CPackageManager;

	/**
	 * Destructor
	 */
	~CPackage();

	/**
	 * Save package
	 * 
	 * @param InPath Path to package
	 * @return Return true if package is saved, else false
	 */
	bool Save( const std::wstring& InPath );

	/**
	 * Add asset to package
	 * 
	 * @param[in] InAsset		Asset
	 * @param[in] OutAssetInfo	Optional. Output to this parameter asset's info if pointer is not NULL
	 */
	void Add( const TAssetHandle<CAsset>& InAsset, AssetInfo* OutAssetInfo = nullptr );

	/**
	 * @brief Add object to package
	 *
	 * @param InObject		Object
	 * @param OutObjectInfo	Optional. Output to this parameter object's info if pointer is not NULL
	 */
	void Add( CObject* InObject, ObjectInfo* OutObjectInfo = nullptr );

	/**
	 * Remove asset from package
	 * 
	 * @param InAsset			Asset to remove
	 * @param InForceUnload		Is need force unload asset if him loaded
	 * @param InIgnoreDirty		Is need ignore dirty flag in asset
	 * @return Return TRUE if asset seccussed removed from package
	 */
	FORCEINLINE bool Remove( const TAssetHandle<CAsset>& InAsset, bool InForceUnload = false, bool InIgnoreDirty = false )
	{
		if ( !InAsset.IsValid() )
		{
			return true;
		}

		return Remove( InAsset.ToSharedPtr()->GetGUID(), InForceUnload, InIgnoreDirty );
	}

	/**
	 * Remove asset from package by GUID
	 * 
	 * @param InGUID			Asset guid
	 * @param InForceUnload		Is need force unload asset if him loaded
	 * @param InIgnoreDirty		Is need ignore dirty flag in asset
	 * @return Return TRUE if asset seccussed removed from package
	 */
	bool Remove( const CGuid& InGUID, bool InForceUnload = false, bool InIgnoreDirty = false );

	/**
	 * Remove asset from package by name
	 * 
	 * @param InName			Name asset
	 * @param InForceUnload		Is need force unload asset if him loaded
	 * @param InIgnoreDirty		Is need ignore dirty flag in asset
	 * @return Return TRUE if asset seccussed removed from package
	 */
	FORCEINLINE bool Remove( const std::wstring& InName, bool InForceUnload = false, bool InIgnoreDirty = false )
	{
		auto		itAssetGUID = assetGUIDTable.find( InName );
		if ( itAssetGUID == assetGUIDTable.end() )
		{
			return true;
		}

		return Remove( itAssetGUID->second, InForceUnload, InIgnoreDirty );
	}

	/**
	 * Remove all from package
	 * 
	 * @param InForceUnload		Is need force unload assets if him loaded
	 * @param InIgnoreDirty		Is need ignore dirty flag in asset
	 * @return Return TRUE if all assets seccussed removed from package
	 */
	bool RemoveAll( bool InForceUnload = false, bool InIgnoreDirty = false );
	
	/**
	 * Unload asset
	 * 
	 * @param InAssetPtr			AssetPtr
	 * @param InForceUnload			Is need force unload (ignored shared references)
	 * @return Return TRUE if asset is unloaded
	 */
	FORCEINLINE bool UnloadAsset( const TAssetHandle<CAsset>& InAssetPtr, bool InForceUnload = false )
	{
		CGuid				assetGuid;
		{
			TSharedPtr<CAsset>	assetRef = InAssetPtr.ToSharedPtr();
			if ( !assetRef )
			{
				return true;
			}
			assetGuid = assetRef->guid;
		}

		return UnloadAsset( assetGuid, InForceUnload );
	}

	/**
	 * Unload asset
	 * 
	 * @param InGuid				Asset guid
	 * @param InForceUnload			Is need force unload (ignored shared references)
	 * @return Return TRUE if asset is unloaded
	 */
	FORCEINLINE bool UnloadAsset( const CGuid& InGuid, bool InForceUnload = false )
	{
		auto		itAsset = assetsTable.find( InGuid );
		if ( itAsset == assetsTable.end() )
		{
			return true;
		}

		AssetInfo&		assetInfo = itAsset->second;
		return UnloadAsset( assetInfo, InForceUnload );
	}

	/**
	 * Unload all assets
	 * 
	 * @param InForceUnload		Is need force unload (ignored shared references)
	 * @return Return TRUE if assets is unloaded
	 */
	FORCEINLINE bool UnloadAllAssets( bool InForceUnload = false )
	{
		return UnloadAllAssetsInternal( InForceUnload );
	}

	/**
	 * Reload asset
	 * @warning Reloaded only already loaded assets
	 * 
	 * @param InAssetPtr			AssetPtr
	 * @return Return TRUE if asset is reloaded
	 */
	FORCEINLINE bool ReloadAsset( const TAssetHandle<CAsset>& InAssetPtr )
	{
		if ( !InAssetPtr.IsValid() )
		{
			return false;
		}

		return ReloadAsset( InAssetPtr.GetReference()->guidAsset );
	}

	/**
	 * Reload asset
	 * @warning Reloaded only already loaded assets
	 *
	 * @param InGuid				Asset guid
	 * @return Return TRUE if asset is reloaded
	 */
	FORCEINLINE bool ReloadAsset( const CGuid& InGuid )
	{
		auto		itAsset = assetsTable.find( InGuid );
		if ( itAsset == assetsTable.end() )
		{
			return false;
		}

		AssetInfo&		assetInfo = itAsset->second;
		return ReloadAsset( assetInfo );
	}

	/**
	 * Reload package
	 * @warning Reloaded only package data and already loaded assets. New assets that have not been written to HDD are not deleted
	 * 
	 * @param InOnlyAsset	Is need reload only assets
	 * @return Return TRUE if asset(s) or package data is reloaded
	 */
	bool ReloadPackage( bool InOnlyAsset = false );

	/**
	 * Is exist asset with name in package
	 * 
	 * @param InName		Asset name
	 * @return Return TRUE if asset with name InName exist in package, else return FALSE
	 */
	FORCEINLINE bool IsExist( const std::wstring& InName ) const
	{
		auto		itAssetGUID = assetGUIDTable.find( InName );
		if ( itAssetGUID == assetGUIDTable.end() )
		{
			return false;
		}

		return true;
	}

	/**
	 * Is exist asset with GUID in package
	 * 
	 * @param InGUID		Asset guid
	 * @return Return TRUE if asset with guid InGUID exist in package, else return FALSE
	 */
	FORCEINLINE bool IsExist( const CGuid& InGUID ) const
	{
		auto		itAsset = assetsTable.find( InGUID );
		if ( itAsset == assetsTable.end() )
		{
			return false;
		}

		return true;
	}

	/**
	 * Find asset
	 * 
	 * @param[in] InGUID GUID of asset
	 * @return Return pointer to asset in package, if not found return nullptr
	 */
	TAssetHandle<CAsset> Find( const CGuid& InGUID );

	/**
	 * Find asset by name
	 * 
	 * @param InName Name asset
	 * @return Return pointer to asset in package, if not found return nullptr
	 */
	FORCEINLINE TAssetHandle<CAsset> Find( const std::wstring& InName )
	{
		auto		itAssetGUID = assetGUIDTable.find( InName );
		if ( itAssetGUID == assetGUIDTable.end() )
		{
			return nullptr;
		}

		return Find( itAssetGUID->second );
	}

	/**
	 * Set name of the package
	 * 
	 * @param InName New name of the package
	 */
	FORCEINLINE void SetName( const std::wstring& InName )
	{
		name = InName;
		bIsDirty = true;
		numDirtyAssets = -1;
	}

	/**
	 * Set name from path
	 * 
	 * @param InPath Path
	 */
	void SetNameFromPath( const std::wstring& InPath );

	/**
	 * Get GUID of package
	 * @return Return GUID of package
	 */
	FORCEINLINE CGuid GetGUID() const
	{
		return guid;
	}

	/**
	 * Get number loaded assets in package
	 * @retun Return number loaded assets
	 */
	FORCEINLINE uint32 GetNumLoadedAssets() const
	{
		return numLoadedAssets;
	}

	/**
	 * Get number assets in package
	 * @return Return number assets in package
	 */
	FORCEINLINE uint32 GetNumAssets() const
	{
		return assetsTable.size();
	}

	/**
	 * Is empty package
	 * @return Return true if package is empty, else return false
	 */
	FORCEINLINE bool IsEmpty() const
	{
		return GetNumAssets() == 0;
	}

	/**
	 * Is package dirty
	 * @return Return if package is dirty, else return false
	 */
	FORCEINLINE bool IsDirty() const
	{
		return bIsDirty;
	}

	/**
	 * Get path to the package from which data was last loaded
	 * @return Return path, if package is not loaded from other packages return empty string
	 */
	FORCEINLINE const std::wstring& GetFileName() const
	{
		return filename;
	}

	/**
	 * Get name of the package
	 * @return Return name of the package
	 */
	FORCEINLINE const std::wstring& GetName() const
	{
		return name;
	}

	/**
	 * Get asset info by index
	 * 
	 * @param InIndex Index of asset in package
	 * @param OutAssetInfo Output reference to asset info
	 * @param OutGuidAsset Optional output of guid asset
	 */
	FORCEINLINE void GetAssetInfo( uint32 InIndex, AssetInfo& OutAssetInfo, CGuid* OutGuidAsset = nullptr ) const
	{
		Assert( !assetsTable.empty() && InIndex >= 0 && InIndex < assetsTable.size() );
		
		uint32		index = 0;
		for ( auto itAsset = assetsTable.cbegin(), itAssetEnd = assetsTable.cend(); itAsset != itAssetEnd; ++itAsset, ++index )
		{
			if ( InIndex == index )
			{
				OutAssetInfo = itAsset->second;
				if ( OutGuidAsset )
				{
					*OutGuidAsset = itAsset->first;
				}
				break;
			}
		}
	}

	/**
	 * @brief Get pointer to asset info by index
	 * 
	 * @param InIndex		Index of asset in package
	 * @param OutAssetInfo	Output pointer to asset info
	 * @param OutGuidAsset	Optional output of guid asset
	 */
	FORCEINLINE void GetAssetInfo( uint32 InIndex, const AssetInfo*& OutAssetInfo, CGuid* OutGuidAsset = nullptr ) const
	{
		Assert( !assetsTable.empty() && InIndex >= 0 && InIndex < assetsTable.size() );

		uint32		index = 0;
		for ( auto itAsset = assetsTable.cbegin(), itAssetEnd = assetsTable.cend(); itAsset != itAssetEnd; ++itAsset, ++index )
		{
			if ( InIndex == index )
			{
				OutAssetInfo = &itAsset->second;
				if ( OutGuidAsset )
				{
					*OutGuidAsset = itAsset->first;
				}
				break;
			}
		}
	}

private:
	/**
	 * Typedef map of asset name to GUID
	 */
	typedef std::unordered_map< std::wstring, CGuid >							AssetNameToGUID_t;

	/**
	 * Typedef map of assets table
	 */
	typedef std::unordered_map< CGuid, AssetInfo, CGuid::GuidKeyFunc >			AssetTable_t;

	/**
	 * @brief Typedef map of object table
	 */
	typedef std::unordered_map<CObject*, ObjectInfo*>							ObjectTable_t;

	/**
	 * Constructor
	 *
	 * @param InName Name of the package
	 */
	CPackage( const std::wstring& InName = TEXT( "" ) );

	/**
	 * Load package
	 *
	 * @param InPath Path to package
	 * @return Return true if package is loaded, else false
	 */
	bool Load( const std::wstring& InPath );

	/**
	 * Fully load
	 * @param OutAssetArray Array of loaded asset from package
	 */
	void FullyLoad( std::vector< TAssetHandle<CAsset> >& OutAssetArray );

	/**
	 * Unload asset by asset info
	 *
	 * @param InAssetInfo		Asset info
	 * @param InForceUnload		Is need force unload (ignored shared references)
	 * @param InBroadcastEvent	Is need broadcast event OnAssetsCanDelete and OnAssetsDeleted. @note Only for editor
	 * @param InIgnoreDirty		Is need ignore dirty flag in asset
	 * @return Return TRUE if asset is unloaded
	 */
	bool UnloadAsset( AssetInfo& InAssetInfo, bool InForceUnload = false, bool InBroadcastEvent = true, bool InIgnoreDirty = false );

	/**
	 * Internal unload all assets
	 *
	 * @param InForceUnload		Is need force unload (ignored shared references)
	 * @param InIgnoreDirty		Is need ignore dirty flag in asset
	 * @return Return TRUE if assets is unloaded
	 */
	bool UnloadAllAssetsInternal( bool InForceUnload = false, bool InIgnoreDirty = false );

	/**
	 * Reload asset by asset info
	 *
	 * @param InAssetInfo		Asset info
	 * @return Return TRUE if asset is reloaded
	 */
	bool ReloadAsset( AssetInfo& InAssetInfo );

	/**
	 * Serialize package
	 * 
	 * @param InArchive Archive
	 */
	void Serialize( CArchive& InArchive );

	/**
	 * Serialize header of the package
	 * 
	 * @param InArchive		Archive
	 * @param InIsNeedSkip	Is need skip header
	 */
	void SerializeHeader( CArchive& InArchive, bool InIsNeedSkip = false );

	/**
	 * Load asset from package
	 * 
	 * @param InArchive					Archive
	 * @param InAssetGUID				Asset GUID
	 * @param InAssetInfo				Asset info
	 * @param InNeedReload				Is need reload asset if it already loaded
	 * @return Return loaded asset from package, if failed returning nullptr
	 */
	TAssetHandle<CAsset> LoadAsset( CArchive& InArchive, const CGuid& InAssetGUID, AssetInfo& InAssetInfo, bool InNeedReload = false );

	/**
	 * Update asset name in table
	 * @warning Must called from CAsset
	 * 
	 * @param InGUID		GUID of asset
	 */
	void UpdateAssetNameInTable( const CGuid& InGUID );

	/**
	 * Mark dirty asset in package
	 * @warning Must called from CAsset
	 * 
	 * @param InGUID		GUID of asset
	 */
	void MarkAssetDirty( const CGuid& InGUID );

	bool						bIsDirty;			/**< Is dirty package */
	CGuid						guid;				/**< GUID of package */
	std::wstring				filename;			/**< Path to the package from which data was last loaded */
	std::wstring				name;				/**< Package name */
	uint32						numLoadedAssets;	/**< Number loaded assets */
	uint32						numDirtyAssets;		/**< Number dirty assets in package */
	AssetNameToGUID_t			assetGUIDTable;		/**< Table for converting asset GUID to name */
	AssetTable_t				assetsTable;		/**< Table of assets in package */
	ObjectTable_t				objectTable;		/**< Table of objects in the package */
	std::vector<ObjectInfo*>	objects;			/**< Array of objects in the package */
};

/**
 * @ingroup Core
 * Class manager all packages in engine
 */
class CPackageManager
{
public:
	friend CPackage;

	/**
	 * Constructor
	 */
	CPackageManager();

	/**
	 * Init package manager
	 */
	void Init();

	/**
	 * Update package manager
	 * This method removes unused packages
	 */
	void Tick();

	/**
	 * Shutdown package manager
	 */
	void Shutdown();

	/**
	 * Find asset in package by <AssetType>'<PackageName>:<AssetName>
	 * 
	 * @param InString Reference to asset
	 * @param InType Asset type. Optional parameter, if setted return default asset in case fail
	 * @return Return finded asset. If not found returning nullptr
	 */
	TAssetHandle<CAsset> FindAsset( const std::wstring& InString, EAssetType InType = AT_Unknown );

	/**
	 * @brief Find asset in package by AssetReference
	 * 
	 * @param InAssetReference		Asset reference
	 * @return Return found asset. If not found then return NULL
	 */
	FORCEINLINE TAssetHandle<CAsset> FindAsset( const AssetReference& InAssetReference )
	{
		return FindAsset( InAssetReference.guidPackage, InAssetReference.guidAsset, InAssetReference.type );
	}

	/**
	 * Find asset in package
	 * 
	 * @param InGUIDPackage GUID of the package
	 * @param InGUIDAsset GUID of asset
	 * @param InType Asset type. Optional parameter, if setted return default asset in case fail
	 * @return Return finded asset. If not found returning nullptr
	 */
	FORCEINLINE TAssetHandle<CAsset> FindAsset( const CGuid& InGUIDPackage, const CGuid& InGUIDAsset, EAssetType InType = AT_Unknown )
	{
		std::wstring		path = g_TableOfContents.GetPackagePath( InGUIDPackage );
		if ( path.empty() )
		{
			return nullptr;
		}

		return FindAsset( path, InGUIDAsset, InType );
	}

	/**
	 * Find asset in package
	 * 
	 * @param InPath Path to the package
	 * @param InGUIDAsset GUID of asset
	 * @param InType Asset type. Optional parameter, if setted return default asset in case fail
	 */
	TAssetHandle<CAsset> FindAsset( const std::wstring& InPath, const CGuid& InGUIDAsset, EAssetType InType = AT_Unknown );

	/**
	 * Find asset in package
	 *
	 * @param InPath Path to the package
	 * @param InAsset Asset name in the package
	 * @param InType Asset type. Optional parameter, if setted return default asset in case fail
	 */
	TAssetHandle<CAsset> FindAsset( const std::wstring& InPath, const std::wstring& InAsset, EAssetType InType = AT_Unknown );

	/**
	 * Find default asset
	 * 
	 * @param InType Default asset for type
	 * @return Return finded asset. If not found returning nullptr
	 */
	TAssetHandle<CAsset> FindDefaultAsset( EAssetType InType ) const;

	/**
	 * Is default asset
	 * 
	 * @param InAsset	Asset
	 * @return Return TRUE if InAsset is default asset, else return FALSE
	 */
	FORCEINLINE bool IsDefaultAsset( const TAssetHandle<CAsset>& InAsset ) const
	{
		if ( !InAsset.IsAssetValid() )
		{
			return false;
		}
		return InAsset == FindDefaultAsset( InAsset.ToSharedPtr()->GetType() );
	}

	/**
	 * Load package
	 * 
	 * @param InPath Package path
	 * @param InCreateIfNotExist Is need create package if not exist
	 * @return Return loaded package. If not found returning nullptr
	 */
	PackageRef_t LoadPackage( const std::wstring& InPath, bool InCreateIfNotExist = false );

	/**
	 * Unload package
	 * 
	 * @param InPath			Package path
	 * @param InForceUnload		Is need force unload (ignored shared references)
	 * @return Return true if package is unloaded. If in package exist dirty asset(s) or package not found return false
	 */
	bool UnloadPackage( const std::wstring& InPath, bool InForceUnload = false );

	/**
	 * Unload all packages
	 * 
	 * @param InForceUnload		Is need force unload (ignored shared references)
	 * @return Return TRUE if all packages is unloaded. If in package exist dirty asset(s) or package not found return FALSE
	 */
	bool UnloadAllPackages( bool InForceUnload = false );

	/**
	 * Unload asset
	 *
	 * @param InAssetPtr		Asset ptr
	 * @param InForceUnload		Is need force unload (ignored shared references)
	 * @return Return TRUE if asset is unloaded
	 */
	bool UnloadAsset( const TAssetHandle<CAsset>& InAssetPtr, bool InForceUnload = false );

	/**
	 * Reload asset
	 * @warning Reloaded only already loaded assets
	 * 
	 * @param InAssetPtr		Asset ptr
	 * @return Return TRUE if asset is reloaded
	 */
	bool ReloadAsset( const TAssetHandle<CAsset>& InAssetPtr );

	/**
	 * Reload all assets in package
	 * @warning Reloaded only package data and already loaded assets. New assets that have not been written to HDD are not deleted
	 * 
	 * @param InPath			Package path
	 * @return Return TRUE if asset(s) is reloaded
	 */
	bool ReloadPackage( const std::wstring& InPath );

	/**
	 * Reload all assets in all packages
	 * @warning Reloaded only package data and already loaded assets. New assets that have not been written to HDD are not deleted
	 * 
	 * @return Return TRUE if asset(s) is reloaded
	 */
	bool ReloadAllPackages();

	/**
	 * Garbage collector of unused packages and assets
	 */
	void GarbageCollector();

	/**
	 * Is package loaded
	 * 
	 * @param InPath Package path
	 * @return Return true if package is loaded, else return false
	 */
	FORCEINLINE bool IsPackageLoaded( const std::wstring& InPath ) const
	{
		auto		itPackage = packages.find( InPath );
		return itPackage != packages.end();
	}

private:	
	/**
	 * Struct of normalized path in file system
	 */
	struct NormalizedPath
	{
		/**
		 * @brief Functions to extract the NormalizedPath as a key for std::unordered_map and std::unordered_set
		 */
		struct NormalizedPathKeyFunc
		{
			/**
			 * @brief Calculate hash of the NormalizedPath
			 *
			 * @param InPath Path
			 * @return Return hash of this NormalizedPath
			 */
			FORCEINLINE std::size_t operator()( const NormalizedPath& InPath ) const
			{
				return InPath.GetTypeHash();
			}

			/**
			 * @brief Compare NormalizedPath
			 *
			 * @param InA First NormalizedPath
			 * @param InB Second NormalizedPath
			 * @return Return true if InA and InB equal, else returning false
			 */
			FORCEINLINE bool operator()( const NormalizedPath& InA, const NormalizedPath& InB ) const
			{
				return InA.GetTypeHash() < InB.GetTypeHash();
			}
		};

		/**
		 * Constructor
		 * 
		 * @param InPath Path
		 */
		FORCEINLINE NormalizedPath( const std::wstring& InPath )
			: path( InPath )
		{
			Sys_NormalizePathSeparators( path );
		}

		/**
		 * Set new path
		 *
		 * @param InPath Path
		 */
		FORCEINLINE void Set( const std::wstring& InPath )
		{
			path = InPath;
			Sys_NormalizePathSeparators( path );
		}

		/**
		 * Convert path in string
		 * @return Return path in string
		 */
		FORCEINLINE const std::wstring& ToString() const
		{
			return path;
		}

		/**
		 * Get hash of type
		 * @return Return hash of this NormalizedPath
		 */
		FORCEINLINE uint64 GetTypeHash() const
		{
			return Sys_MemFastHash( ( const void* ) path.c_str(), path.size() );
		}

		/**
		 * Override operator for set new path
		 */
		FORCEINLINE NormalizedPath& operator=( const std::wstring& InRight )
		{
			Set( InRight );
			return *this;
		}

		/**
		 * Override operator for compare to NormalizedPath
		 */
		FORCEINLINE bool operator==( const NormalizedPath& InRight ) const
		{
			return path == InRight.path;
		}

		/**
		 * @brief Overloaded operator std::wstring
		 */
		FORCEINLINE operator std::wstring() const
		{
			return path;
		}

	private:
		std::wstring		path;			/**< Normalized path */
	};

	/**
	 * Typedef of list loaded packages
	 */
	typedef std::unordered_map< NormalizedPath, PackageRef_t, NormalizedPath::NormalizedPathKeyFunc >			PackageList_t;

	PackageList_t			packages;				/**< Opened packages */
};

/**
 * @ingroup Core
 * @brief Parse reference to asset in format <AssetType>'<PackageName>:<AssetName>
 *
 * @param InString Reference to asset
 * @param OutPackageName Package name
 * @param OutAssetName Asset name
 * @param OutAssetType Asset type
 * @return Return true if InString parsed is seccussed, else returning false
 */
bool ParseReferenceToAsset( const std::wstring& InString, std::wstring& OutPackageName, std::wstring& OutAssetName, EAssetType& OutAssetType );

/**
 * @ingroup Core
 * @brief Make reference to asset in format <AssetType>'<PackageName>:<AssetName>
 * 
 * @param InPackageName		Package name
 * @param InAssetName		Asset name
 * @param InAssetType		Asset type
 * @param OutString			Output string with reference
 * @return Return TRUE if reference created is seccussed, else returning FALSE
 */
FORCEINLINE bool MakeReferenceToAsset( const std::wstring& InPackageName, const std::wstring& InAssetName, EAssetType InAssetType, std::wstring& OutString )
{
	if ( InPackageName.empty() || InAssetName.empty() )
	{
		return false;
	}

	OutString = ConvertAssetTypeToText( InAssetType ) + TEXT( "'" ) + InPackageName + TEXT(":") + InAssetName;
	return true;
}

/**
 * @ingroup Core
 * @brief Make reference to asset in format <AssetType>'<PackageName>:<AssetName>
 * 
 * @param InAsset	Asset
 * @param OutString			Output string with reference
 * @return Return TRUE if reference created is seccussed, else returning FALSE
 */
FORCEINLINE bool MakeReferenceToAsset( const TAssetHandle<CAsset>& InAsset, std::wstring& OutString )
{
	// TODO yehor.pohuliaka - Maybe need rewrite either this function or AssetReference, because when asset was unloaded then function don't convert TAssetHandle to string reference
	if ( !InAsset.IsAssetValid() )
	{
		return false;
	}

	TSharedPtr<CAsset>		asset = InAsset.ToSharedPtr();
	if ( !asset->GetPackage() || asset->GetAssetName().empty() )
	{
		return false;
	}
	return MakeReferenceToAsset( asset->GetPackage()->GetName(), asset->GetAssetName(), asset->GetType(), OutString );
}

//
// Serialization
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, EAssetType& InValue )
{
	InArchive.Serialize( &InValue, sizeof( EAssetType ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const EAssetType& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( EAssetType ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, AssetReference& InValue )
{
	Assert( InArchive.Ver() >= VER_GUIDPackages );
	InArchive << InValue.type;

	if ( InArchive.IsLoading() && InArchive.Ver() < VER_GUIDAssets )
	{
		uint32		hash = 0;
		InArchive << hash;
		InValue.guidAsset.Set( hash, 0, 0, 0 );
	}
	else
	{
		InArchive << InValue.guidAsset;
	}

	InArchive << InValue.guidPackage;
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const AssetReference& InValue )
{
	Assert( InArchive.IsSaving() && InArchive.Ver() >= VER_GUIDPackages );
	InArchive << InValue.type;
	InArchive << InValue.guidAsset;
	InArchive << InValue.guidPackage;
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, TAssetHandle<CAsset>& InValue )
{
	if ( InArchive.IsSaving() )
	{
		TSharedPtr<CAsset>		asset = InValue.ToSharedPtr();
		InArchive << ( asset ? asset->GetAssetReference() : AssetReference() );

#if ENABLED_ASSERT
		if ( asset )
		{
			AssetReference		assetRef = asset->GetAssetReference();
			Assert( assetRef.guidAsset.IsValid() && assetRef.guidPackage.IsValid() );
		}
#endif // ENABLED_ASSERT
	}
	else
	{
		AssetReference			assetReference;
		InArchive << assetReference;
		if ( assetReference.IsValid() )
		{
			InValue = g_PackageManager->FindAsset( assetReference.guidPackage, assetReference.guidAsset, assetReference.type );
		}
		else
		{
			InValue = g_PackageManager->FindDefaultAsset( assetReference.type );
		}
	}

	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const TAssetHandle<CAsset>& InValue )
{
	Assert( InArchive.IsSaving() );
	
	TSharedPtr<CAsset>		asset = InValue.ToSharedPtr();
	InArchive << ( asset ? asset->GetAssetReference() : AssetReference() );
	return InArchive;
}

#endif // !PACKAGE_H
