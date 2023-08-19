#include "Containers/String.h"
#include "Logger/LoggerMacros.h"
#include "System/Archive.h"
#include "Render/Scene.h"
#include "Render/StaticMesh.h"
#include "Render/SceneUtils.h"
#include "Render/SceneHitProxyRendering.h"

/*
==================
CStaticMesh::CStaticMesh
==================
*/
CStaticMesh::CStaticMesh()
	: CAsset( AT_StaticMesh )
	, vertexFactory( new CStaticMeshVertexFactory() )
{}

/*
==================
CStaticMesh::~CStaticMesh
==================
*/
CStaticMesh::~CStaticMesh()
{
	// Remove all drawing policy links from scenes
	for ( auto itElement = elementDrawingPolicyMap.begin(), itElementEnd = elementDrawingPolicyMap.end(); itElement != itElementEnd; ++itElement )
	{
		// Mark dirty drawing policy link
		itElement->second->bDirty = true;

		// Remove items from static mesh draw list
		for ( uint32 index = 0, count = itElement->second->drawingPolicyLinks.size(); index < count; ++index )
		{
			itElement->first.SDG->staticMeshDrawList.RemoveItem( itElement->second->drawingPolicyLinks[ index ] );
		}

		// Remove items from depth draw list
		for ( uint32 index = 0, count = itElement->second->depthDrawingPolicyLinks.size(); index < count; ++index )
		{
			itElement->first.SDG->depthDrawList.RemoveItem( itElement->second->depthDrawingPolicyLinks[index] );
		}

#if ENABLE_HITPROXY
		// Remove items from hit proxy layers
		for ( uint32 index = 0, count = itElement->second->hitProxyDrawingPolicyLinks.size(); index < count; ++index )
		{
			itElement->first.SDG->hitProxyLayers[HPL_World].hitProxyDrawList.RemoveItem( itElement->second->hitProxyDrawingPolicyLinks[index] );
		}
#endif // ENABLE_HITPROXY
	}
}

/*
==================
CStaticMesh::InitRHI
==================
*/
void CStaticMesh::InitRHI()
{
	// Create vertex buffer
	uint32			numVerteces = ( uint32 )verteces.Num();
	if ( numVerteces > 0 )
	{
		vertexBufferRHI = g_RHI->CreateVertexBuffer( CString::Format( TEXT( "%s" ), GetAssetName().c_str() ).c_str(), sizeof( SStaticMeshVertexType ) * numVerteces, ( byte* )verteces.GetData(), RUF_Static );

		// Initialize vertex factory
		vertexFactory->AddVertexStream( SVertexStream{ vertexBufferRHI, sizeof( SStaticMeshVertexType ) } );		// 0 stream slot
		vertexFactory->Init();
	}

	// Create index buffer
	uint32			numIndeces = ( uint32 )indeces.Num();
	if ( numIndeces > 0 )
	{
		indexBufferRHI = g_RHI->CreateIndexBuffer( CString::Format( TEXT( "%s" ), GetAssetName().c_str() ).c_str(), sizeof( uint32 ), sizeof( uint32 ) * numIndeces, ( byte* )indeces.GetData(), RUF_Static );
	}

	if ( !g_IsEditor && !g_IsCommandlet )
	{
		verteces.RemoveAllElements();
		indeces.RemoveAllElements();
	}
}

/*
==================
CStaticMesh::ReleaseRHI
==================
*/
void CStaticMesh::ReleaseRHI()
{
	vertexBufferRHI.SafeRelease();
	indexBufferRHI.SafeRelease();
	vertexFactory->ReleaseResource();
}

/*
==================
CStaticMesh::Serialize
==================
*/
void CStaticMesh::Serialize( class CArchive& InArchive )
{
	if ( InArchive.Ver() < VER_StaticMesh )
	{
		return;
	}

	CAsset::Serialize( InArchive );

	if ( InArchive.Ver() < VER_CompressedZlib )
	{
		std::vector<SStaticMeshVertexType>		tmpVerteces;
		std::vector<uint32>						tmpIndeces;
		InArchive << tmpVerteces;
		InArchive << tmpIndeces;

		verteces = tmpVerteces;
		indeces = tmpIndeces;
		Warnf( TEXT( "Deprecated package version, in future must be removed supports\n" ) );
	}
	else
	{
		InArchive << verteces;
		InArchive << indeces;
	}

	InArchive << surfaces;
	InArchive << materials;

	if ( InArchive.Ver() < VER_BBoxInStaticMesh )
	{
		CalcBoundingBox();
	}
	else
	{
		InArchive << bbox;
	}

	if ( InArchive.IsLoading() )
	{
		// Mark dirty all drawing policy links
		MarkDirtyAllElementDrawingPolices();
		BeginUpdateResource( this );
	}
}

/*
==================
CStaticMesh::SetData
==================
*/
void CStaticMesh::SetData( const std::vector<SStaticMeshVertexType>& InVerteces, const std::vector<uint32>& InIndeces, const std::vector<SStaticMeshSurface>& InSurfaces, const std::vector< TAssetHandle<CMaterial> >& InMaterials )
{
	// Copy new parameters of static mesh
	verteces		= InVerteces;
	indeces			= InIndeces;
	surfaces		= InSurfaces;
	materials		= InMaterials;
	CalcBoundingBox();

	// Mark dirty all drawing policy links
	MarkDirtyAllElementDrawingPolices();
	BeginUpdateResource( this );
}

/*
==================
CStaticMesh::CalcBoundingBox
==================
*/
void CStaticMesh::CalcBoundingBox()
{
	// Find minimum and maximum by XYZ
	if ( verteces.Num() > 0 )
	{
		Vector		minXYZ = verteces.GetElement( 0 ).position;
		Vector		maxXYZ = verteces.GetElement( 0 ).position;

		for ( uint32 index = 0, count = verteces.Num(); index < count; ++index )
		{
			const SStaticMeshVertexType&	vertexType = verteces.GetElement( index );
			if ( minXYZ.x > vertexType.position.x )
			{
				minXYZ.x = vertexType.position.x;
			}
			if ( minXYZ.y > vertexType.position.y )
			{
				minXYZ.y = vertexType.position.y;
			}
			if ( minXYZ.z > vertexType.position.z )
			{
				minXYZ.z = vertexType.position.z;
			}

			if ( maxXYZ.x < vertexType.position.x )
			{
				maxXYZ.x = vertexType.position.x;
			}
			if ( maxXYZ.y < vertexType.position.y )
			{
				maxXYZ.y = vertexType.position.y;
			}
			if ( maxXYZ.z < vertexType.position.z )
			{
				maxXYZ.z = vertexType.position.z;
			}
		}

		bbox = CBox::BuildAABB( SMath::vectorZero, minXYZ, maxXYZ );
	}
	else
	{
		bbox.Clear();
	}
}

/*
==================
CStaticMesh::SetMaterial
==================
*/
void CStaticMesh::SetMaterial( uint32 InMaterialIndex, const TAssetHandle<CMaterial>& InNewMaterial )
{
	if ( InMaterialIndex > materials.size() )
	{
		return;
	}

	if ( materials[ InMaterialIndex ] != InNewMaterial )
	{
		MarkDirty();

		// Mark dirty all drawing policy links
		MarkDirtyAllElementDrawingPolices();
	}

	materials[ InMaterialIndex ] = InNewMaterial;
}

/*
==================
CStaticMesh::GetDependentAssets
==================
*/
void CStaticMesh::GetDependentAssets( SetDependentAssets_t& OutDependentAssets, EAssetType InFilter /* = AT_Unknown */ ) const
{
	// TODO BS yehor.pohuliaka - Need optimize it
	bool		bAllDependents	= InFilter == AT_Unknown;
	bool		bTextures		= InFilter == AT_Texture2D	|| bAllDependents;
	bool		bMaterials		= InFilter == AT_Material	|| bAllDependents;

	if ( !bAllDependents && !bTextures && !bMaterials )
	{
		return;
	}

	for ( uint32 index = 0, count = materials.size(); index < count; ++index )
	{
		TAssetHandle<CMaterial>		material = materials[ index ];
		if ( !material.IsAssetValid() )
		{
			continue;
		}

		if ( bMaterials )
		{
			OutDependentAssets.insert( material );
		}

		if ( bTextures )
		{
			TSharedPtr<CMaterial>		materialRef = material.ToSharedPtr();
			materialRef->GetDependentAssets( OutDependentAssets, AT_Texture2D );
		}
	}
}

/*
==================
CStaticMesh::ReloadDependentAssets
==================
*/
void CStaticMesh::ReloadDependentAssets( bool InForce /* = false */ )
{
	bool		bDirtyElementDrawingPolices = false;
	for ( uint32 index = 0, count = materials.size(); index < count; ++index )
	{
		TAssetHandle<CAsset>&			assetHandle = ( TAssetHandle<CAsset>& )materials[ index ];
		if ( !assetHandle.IsValid() || assetHandle.IsAssetValid() )
		{
			continue;
		}

		TSharedPtr<SAssetReference>		assetReference = assetHandle.GetReference();
		assetHandle					= g_PackageManager->FindAsset( assetReference->guidPackage, assetReference->guidAsset, assetReference->type );
		bDirtyElementDrawingPolices = true;
	}

	// Mark dirty all drawing policy links if need
	if ( bDirtyElementDrawingPolices )
	{
		MarkDirtyAllElementDrawingPolices();
	}
}

/*
==================
CStaticMesh::LinkDrawList
==================
*/
TSharedPtr<CStaticMesh::SElementDrawingPolicyLink> CStaticMesh::LinkDrawList( SSceneDepthGroup& InSDG )
{
	// Make key for element drawing policy link
	SElementKeyDrawingPolicyLink	elementKey{ &InSDG, 0 };

	// If already added drawing policy link for this scene depth group - return exist element
	{
		auto	itElement = elementDrawingPolicyMap.find( elementKey );
		if ( itElement != elementDrawingPolicyMap.end() )
		{
			return itElement->second;
		}
	}

	// Allocate new element
	TSharedPtr<SElementDrawingPolicyLink>		element = MakeDrawingPolicyLink( InSDG, 0 );

	// Add to cache and return created element
	elementDrawingPolicyMap[ elementKey ] = element;
	return element;
}

/*
==================
CStaticMesh::LinkDrawList
==================
*/
TSharedPtr<CStaticMesh::SElementDrawingPolicyLink> CStaticMesh::LinkDrawList( SSceneDepthGroup& InSDG, const std::vector< TAssetHandle<CMaterial> >& InOverrideMaterials )
{
	// Make key for element drawing policy link
	SElementKeyDrawingPolicyLink	elementKey{ &InSDG, 0 };

	// Calculate override hash
	for ( uint32 index = 0, count = InOverrideMaterials.size(); index < count; ++index )
	{
		elementKey.overrideHash = Sys_MemFastHash( index, elementKey.overrideHash );
		elementKey.overrideHash = Sys_MemFastHash( InOverrideMaterials[ index ].ToSharedPtr(), elementKey.overrideHash );
	}

	// If already added drawing policy link for this scene depth group - return exist element
	{
		auto	itElement = elementDrawingPolicyMap.find( elementKey );
		if ( itElement != elementDrawingPolicyMap.end() )
		{
			return itElement->second;
		}
	}

	// Allocate new element
	TSharedPtr<SElementDrawingPolicyLink>		element = MakeDrawingPolicyLink( InSDG, elementKey.overrideHash, ( std::vector< TAssetHandle<CMaterial> >* ) & InOverrideMaterials );

	// Add to cache and return created element
	elementDrawingPolicyMap[ elementKey ] = element;
	return element;
}

/*
==================
CStaticMesh::MakeDrawingPolicyLink
==================
*/
TSharedPtr<CStaticMesh::SElementDrawingPolicyLink> CStaticMesh::MakeDrawingPolicyLink( SSceneDepthGroup& InSDG, uint64 InOverrideHash /* = 0 */, std::vector<TAssetHandle<CMaterial>>* InOverrideMaterials /* = nullptr */ )
{
	// Allocate new element
	TSharedPtr<SElementDrawingPolicyLink>	element					= MakeSharedPtr<SElementDrawingPolicyLink>();
	uint32									numOverrideMaterials	= InOverrideMaterials ? InOverrideMaterials->size() : 0;
	element->overrideHash = InOverrideHash;

	// Generate mesh batch for surface and add to new scene draw policy link
	for ( uint32 indexSurface = 0, numSurfaces = ( uint32 )surfaces.size(); indexSurface < numSurfaces; ++indexSurface )
	{
		const SStaticMeshSurface&		surface				= surfaces[ indexSurface ];
		TAssetHandle<CMaterial>			material			= materials[ surface.materialID ];
		TSharedPtr<CMaterial>			materialRef;

		// If current material is override - use custom material
		if ( indexSurface < numOverrideMaterials )
		{
			TAssetHandle<CMaterial>		overrideMaterial = InOverrideMaterials->at( surface.materialID );
			if ( overrideMaterial.IsValid() )
			{
				material = overrideMaterial;
			}
		}
		
		// In case when reference to material is valid then get TSharedPtr to material
		if ( material.IsValid() )
		{
			materialRef = material.ToSharedPtr();
			
			// If materialRef still NULL then try load it from package
			if ( !materialRef )
			{
				materialRef = g_PackageManager->FindAsset( *material.GetReference() ).ToSharedPtr();
			}
		}
		
		// Otherwise we must use default material if materialRef is still NULL
		if ( !materialRef )
		{
			material = g_Engine->GetDefaultMaterial();
			materialRef = material.ToSharedPtr();
			Assert( materialRef );
		}

		// Generate mesh batch of surface
		SMeshBatch					meshBatch;
		meshBatch.baseVertexIndex	= surface.baseVertexIndex;
		meshBatch.firstIndex		= surface.firstIndex;
		meshBatch.numPrimitives		= surface.numPrimitives;
		meshBatch.indexBufferRHI	= indexBufferRHI;
		meshBatch.primitiveType		= PT_TriangleList;

		// Make and add to scene new static mesh drawing policy link
		const SMeshBatch*					meshBatchLink				= nullptr;
		DrawingPolicyLinkRef_t				drawingPolicyLink			= ::MakeDrawingPolicyLink<DrawingPolicyLink_t>( vertexFactory, material, meshBatch, meshBatchLink, InSDG.staticMeshDrawList, DEC_STATIC_MESH );
		element->drawingPolicyLinks.push_back( drawingPolicyLink );
		element->meshBatchLinks.push_back( meshBatchLink );

		// Make and add to scene new depth mesh drawing policy link
		if ( !materialRef->IsTranslucency() )	// TODO yehor.pohuliaka - Need implement normal translucency support in the render
		{
			DepthDrawingPolicyLinkRef_t		depthDrawingPolicyLink		= ::MakeDrawingPolicyLink<DepthDrawingPolicyLink_t>( vertexFactory, material, meshBatch, meshBatchLink, InSDG.depthDrawList, DEC_STATIC_MESH );
			element->depthDrawingPolicyLinks.push_back( depthDrawingPolicyLink );
			element->meshBatchLinks.push_back( meshBatchLink );
		}

		// Make and add to scene new hit proxy drawing policy link
#if ENABLE_HITPROXY
		HitProxyDrawingPolicyLinkRef_t		hitProxyDrawingPolicyLink	= ::MakeDrawingPolicyLink<HitProxyDrawingPolicyLink_t>( vertexFactory, material, meshBatch, meshBatchLink, InSDG.hitProxyLayers[ HPL_World ].hitProxyDrawList, DEC_STATIC_MESH );
		element->hitProxyDrawingPolicyLinks.push_back( hitProxyDrawingPolicyLink );
		element->meshBatchLinks.push_back( meshBatchLink );
#endif // ENABLE_HITPROXY
	}

	return element;
}

/*
==================
CStaticMesh::UnlinkDrawList
==================
*/
void CStaticMesh::UnlinkDrawList( SSceneDepthGroup& InSDG, TSharedPtr<SElementDrawingPolicyLink>& InDrawingPolicyLink )
{
	// If pointer is not valid, we exist
	if ( !InDrawingPolicyLink )
	{
		return;
	}

	// Find element in cache, if not found - exist
	auto	itElement = elementDrawingPolicyMap.find( SElementKeyDrawingPolicyLink{ &InSDG, InDrawingPolicyLink->overrideHash } );
	if ( itElement == elementDrawingPolicyMap.end() )
	{
		return;
	}

	// If this element in used other components, we reset only this link
	if ( InDrawingPolicyLink.GetSharedReferenceCount() > 2 )
	{
		InDrawingPolicyLink.Reset();
		return;
	}

	// Else we remove drawing policy link from SDG
	// Remove items from static mesh draw list
	for ( uint32 index = 0, count = itElement->second->drawingPolicyLinks.size(); index < count; ++index )
	{
		InSDG.staticMeshDrawList.RemoveItem( itElement->second->drawingPolicyLinks[index] );
	}

	// Remove items from depth draw list
	for ( uint32 index = 0, count = itElement->second->depthDrawingPolicyLinks.size(); index < count; ++index )
	{
		InSDG.depthDrawList.RemoveItem( itElement->second->depthDrawingPolicyLinks[index] );
	}

#if ENABLE_HITPROXY
	// Remove items from hit proxy layers
	for ( uint32 index = 0, count = itElement->second->hitProxyDrawingPolicyLinks.size(); index < count; ++index )
	{
		InSDG.hitProxyLayers[HPL_World].hitProxyDrawList.RemoveItem( itElement->second->hitProxyDrawingPolicyLinks[index] );
	}
#endif // ENABLE_HITPROXY

	InDrawingPolicyLink.Reset();
	elementDrawingPolicyMap.erase( itElement );
}