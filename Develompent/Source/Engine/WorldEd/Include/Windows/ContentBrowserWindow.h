/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CONTENTBROWSERWINDOW_H
#define CONTENTBROWSERWINDOW_H

#include <string>

#include "Misc/EngineGlobals.h"
#include "Misc/SharedPointer.h"
#include "System/InputSystem.h"
#include "ImGUI/ImGUIEngine.h"
#include "Render/Texture.h"

 /**
  * @ingroup WorldEd
  * @brief Window content browser
  */
class CContentBrowserWindow : public CImGUILayer
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InName	Window name
	 */
	CContentBrowserWindow( const std::wstring& InName );

	/**
	 * @brief Init
	 */
	virtual void Init() override;

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

private:
	friend class CFileTreeNode;

	/**
	 * @brief Enumeration file type
	 */
	enum EFileNodeType
	{
		FNT_File,		/**< File */
		FNT_Folder		/**< Folder */
	};

	/**
	 * @brief File/Folder tree node
	 */
	class CFileTreeNode : public TSharedFromThis<CFileTreeNode>
	{
	public:
		/**
		 * @brief Constructor
		 * 
		 * @param InType	File type
		 * @param InName	Name node in UI
		 * @param InPath	Path to file
		 * @param InOwner	Owner
		 */
		CFileTreeNode( EFileNodeType InType, const std::wstring& InName, const std::wstring& InPath, CContentBrowserWindow* InOwner );

		/**
		 * @brief Update file tree node
		 */
		void Tick();

		/**
		 * @brief Find child node by path
		 * 
		 * @param InPath	Path to file/folder
		 * @return Return pointer to child node if him is exist, otherwise will return NULL
		 */
		FORCEINLINE TSharedPtr<CFileTreeNode> FindChild( const std::wstring& InPath ) const
		{
			for ( uint32 index = 0, count = children.size(); index < count; ++index )
			{
				if ( children[index]->GetPath() == InPath )
				{
					return children[index];
				}
			}

			return nullptr;
		}

		/**
		 * @brief Set select
		 * 
		 * @param InIsSelected			Is selected node
		 * @param InApplyToChildren		Is need apply to children
		 */
		FORCEINLINE void SetSelect( bool InIsSelected, bool InApplyToChildren = true )
		{
			// Possible select only non-root nodes
			TSharedPtr<CFileTreeNode>		selfPtr = AsShared();
			if ( selfPtr != owner->engineRoot && selfPtr != owner->gameRoot )
			{
				bSelected = InIsSelected;
			}

			if ( InApplyToChildren )
			{
				for ( uint32 index = 0, count = children.size(); index < count; ++index )
				{
					children[index]->SetSelect( InIsSelected, InApplyToChildren );
				}
			}
		}

		/**
		 * @brief Is selected node
		 * @return Return TRUE if node is selected
		 */
		FORCEINLINE bool IsSelected() const
		{
			return bSelected;
		}

		/**
		 * @brief Get type
		 * @return Return node type
		 */
		FORCEINLINE EFileNodeType GetType() const
		{
			return type;
		}

		/**
		 * @brief Get path to file
		 * @return Return path to file
		 */
		FORCEINLINE const std::wstring& GetPath() const
		{
			return path;
		}

		/**
		 * @brief Get name for UI
		 * @return Return name for UI
		 */
		FORCEINLINE const std::wstring& GetName() const
		{
			return name;
		}

		/**
		 * @brief Get array of selected nodes
		 * 
		 * @param OutSelectedNodes		Output. Array of selected nodes
		 * @param InIsIgnoreChildren	Is need ignore children node
		 */
		void GetSelectedNodes( std::vector<TSharedPtr<CFileTreeNode>>& OutSelectedNodes, bool InIsIgnoreChildren = false ) const;

	private:
		/**
		 * @brief Refresh info about file system
		 */
		void Refresh();

		/**
		 * @brief Process events
		 */
		void ProcessEvents();

		/**
		 * @brief Drag n drop handle
		 */
		void DragNDropHandle();

		/**
		 * @brief Remove child node
		 * @param InNode	Node to removing
		 */
		FORCEINLINE void RemoveChild( const TSharedPtr<CFileTreeNode>& InNode )
		{
			for ( uint32 index = 0, count = children.size(); index < count; ++index )
			{
				if ( children[index] == InNode )
				{
					InNode->parent = nullptr;
					children.erase( children.begin() + index );
					return;
				}
			}
		}

		/**
		 * @brief Remove node from parent
		 */
		FORCEINLINE void RemoveFromParent()
		{
			TSharedPtr<CFileTreeNode>	parentSharedPtr = parent.Pin();
			if ( parentSharedPtr )
			{
				parentSharedPtr->RemoveChild( AsShared() );
			}
		}

		/**
		 * @brief Set allow drop target
		 *
		 * @param InIsAllowDropTarget	Is allowed drop target
		 * @param InApplyToChildren		Is need apply to children
		 */
		FORCEINLINE void SetAllowDropTarget( bool InIsAllowDropTarget, bool InApplyToChildren = true )
		{
			bAllowDropTarget = InIsAllowDropTarget;
			if ( InApplyToChildren )
			{
				for ( uint32 index = 0, count = children.size(); index < count; ++index )
				{
					children[index]->SetAllowDropTarget( InIsAllowDropTarget, InApplyToChildren );
				}
			}
		}

		bool										bAllowDropTarget;	/**< Is allowed drop target */
		bool										bFreshed;			/**< Is freshed node */
		bool										bSelected;			/**< Is selected node */
		EFileNodeType								type;				/**< Node type */
		std::wstring								path;				/**< Full path to file */
		std::wstring								name;				/**< Node name for UI */
		CContentBrowserWindow*						owner;				/**< Content browser window who owned this node */
		TWeakPtr<CFileTreeNode>						parent;				/**< Parent node */
		std::vector<TSharedPtr<CFileTreeNode>>		children;			/**< Array of children */
	};

	/**
	 * @brief Asset node
	 */
	class CAssetNode
	{
	public:
		/**
		 * @brief Constructor
		 * 
		 * @param InAssetInfo	Asset info
		 * @param InOwner		Owner
		 */
		FORCEINLINE CAssetNode( const SAssetInfo& InAssetInfo, CContentBrowserWindow* InOwner )
			: bSelected( false )
			, info( InAssetInfo )
			, owner( InOwner )
		{}

		/**
		 * @brief Update asset node
		 */
		void Tick();

		/**
		 * @brief Set select
		 *
		 * @param InIsSelected		Is selected node
		 */
		FORCEINLINE void SetSelect( bool InIsSelected )
		{
			bSelected = InIsSelected;
		}

		/**
		 * @brief Is selected node
		 * @return Return TRUE if node is selected
		 */
		FORCEINLINE bool IsSelected() const
		{
			return bSelected;
		}

		/**
		 * @brief Get asset type
		 * @return Return asset type
		 */
		FORCEINLINE EAssetType GetType() const
		{
			return info.type;
		}

		/**
		 * @brief Get asset name
		 * @return Return asset name
		 */
		FORCEINLINE const std::wstring GetName() const
		{
			return info.name;
		}

	private:
		/**
		 * @brief Process events
		 */
		void ProcessEvents();

		bool					bSelected;	/**< Is selected node */
		SAssetInfo				info;		/**< Asset info */
		CContentBrowserWindow*	owner;		/**< Content browser window who owned this node */
	};

	/**
	 * @brief Filter info
	 */
	struct SFilterInfo
	{
		/**
		 * @brief Constructor
		 */
		FORCEINLINE SFilterInfo()
		{
			SetShowAllAssetTypes( true );
		}

		/**
		 * @brief Set show all asset types
		 * @param InIsShow	Is need show all asset types
		 */
		FORCEINLINE void SetShowAllAssetTypes( bool InIsShow )
		{
			memset( assetTypes, InIsShow, ARRAY_COUNT( assetTypes ) * sizeof( bool ) );
		}

		/**
		 * @brief Is show all asset types
		 * @return Return TRUE if need show all asset types, otherwise will return FALSE
		 */
		FORCEINLINE bool IsShowAllAssetTypes() const
		{
			bool		bNeedForAll[AT_Count];
			memset( bNeedForAll, 1, ARRAY_COUNT( bNeedForAll ) * sizeof( bool ) );
			return !memcmp( assetTypes, bNeedForAll, ARRAY_COUNT( bNeedForAll ) * sizeof( bool ) );
		}

		/**
		 * @brief Get preview filter asset type
		 * @return Return preview filter asset type
		 */
		std::string GetPreviewFilterAssetType() const;

		std::string		fileName;				/**< Filter by file name */
		std::string		assetName;				/**< Filter by asset name */
		bool			assetTypes[AT_Count];	/**< Filter by asset type */
	};

	/**
	 * @brief Draw popup menu of assets
	 */
	void DrawAssetsPopupMenu();

	/**
	 * @brief Draw popup menu of packages
	 */
	void DrawPackagesPopupMenu();

	/**
	 * @brief Set current package
	 * @param InPackage		Current package
	 */
	void SetCurrentPackage( PackageRef_t InPackage );

	/**
	 * @brief Popup action. Save a package
	 * @param InSelectedNodes		Array of selected nodes
	 */
	void PopupMenu_Package_Save( const std::vector<TSharedPtr<CFileTreeNode>>& InSelectedNodes );

	/**
	 * @brief Popup action. Open a package
	 * @param InSelectedNodes		Array of selected nodes
	 */
	void PopupMenu_Package_Open( const std::vector<TSharedPtr<CFileTreeNode>>& InSelectedNodes );

	/**
	 * @brief Popup action. Unload a package
	 * @param InSelectedNodes		Array of selected nodes
	 */
	void PopupMenu_Package_Unload( const std::vector<TSharedPtr<CFileTreeNode>>& InSelectedNodes );

	/**
	 * @brief Popup action. Reload a package
	 * @param InSelectedNodes		Array of selected nodes
	 */
	void PopupMenu_Package_Reload( const std::vector<TSharedPtr<CFileTreeNode>>& InSelectedNodes );

	/**
	 * @brief Popup action. Delete a package
	 */
	void PopupMenu_Package_Delete();

	/**
	 * @brief Popup action. Show a package in explorer
	 * @param InSelectedNodes		Array of selected nodes
	 */
	void PopupMenu_Package_ShowInExplorer( const std::vector<TSharedPtr<CFileTreeNode>>& InSelectedNodes );

	/**
	 * @brief Popup action. Rename file or folder
	 * @param InNewFilename		New filename
	 */
	void PopupMenu_Package_Rename( const std::wstring& InNewFilename );

	/**
	 * @brief Popup action. Create a folder
	 * @param InDirName		Directory name
	 */
	void PopupMenu_Package_MakeDirectory( const std::wstring& InDirName );

	/**
	 * @brief Popup action. Create a package
	 * @param InPackageName		Package name
	 */
	void PopupMenu_Package_MakePackage( const std::wstring& InPackageName );

	TSharedPtr<CFileTreeNode>	engineRoot;					/**< Engine root directory */
	TSharedPtr<CFileTreeNode>	gameRoot;					/**< Game root directory */
	TSharedPtr<CFileTreeNode>	hoveredNode;				/**< Hovered node in now time */
	PackageRef_t				package;					/**< Current package in preview */
	SFilterInfo					filterInfo;					/**< Filter info */
	float						padding;					/**< Padding in asset section */
	float						thumbnailSize;				/**< Size of thumbnail */
	TAssetHandle<CTexture2D>	assetIcons[AT_Count];		/**< Array of asset icons */
	std::vector<CAssetNode>		assets;						/**< Assets in package */
};

#endif // !CONTENTBROWSERWINDOW_H