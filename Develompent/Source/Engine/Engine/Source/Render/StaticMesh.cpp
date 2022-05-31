#include "Containers/String.h"
#include "Logger/LoggerMacros.h"
#include "System/Archive.h"
#include "Render/StaticMesh.h"
#include "Render/SceneUtils.h"

CStaticMesh::CStaticMesh()
	: CAsset( AT_StaticMesh )
{}

CStaticMesh::~CStaticMesh()
{
	// Remove all drawing policy links from scenes
	for ( auto itElement = elementDrawingPolicyMap.begin(), itElementEnd = elementDrawingPolicyMap.end(); itElement != itElementEnd; ++itElement )
	{
		for ( uint32 index = 0, count = itElement->second->drawingPolicyLinks.size(); index < count; ++index )
		{
			itElement->first.SDG->staticMeshDrawList.RemoveItem( itElement->second->drawingPolicyLinks[ index ] );
		}
	}
}

void CStaticMesh::InitRHI()
{
	// Create vertex buffer
	uint32			numVerteces = ( uint32 )verteces.Num();
	if ( numVerteces > 0 )
	{
		vertexBufferRHI = GRHI->CreateVertexBuffer( ÑString::Format( TEXT( "%s" ), GetAssetName().c_str() ).c_str(), sizeof( SStaticMeshVertexType ) * numVerteces, ( byte* )verteces.GetData(), RUF_Static );

		// Initialize vertex factory
		vertexFactory = new CStaticMeshVertexFactory();
		vertexFactory->AddVertexStream( SVertexStream{ vertexBufferRHI, sizeof( SStaticMeshVertexType ) } );		// 0 stream slot
		vertexFactory->Init();
	}

	// Create index buffer
	uint32			numIndeces = ( uint32 )indeces.Num();
	if ( numIndeces > 0 )
	{
		indexBufferRHI = GRHI->CreateIndexBuffer( ÑString::Format( TEXT( "%s" ), GetAssetName().c_str() ).c_str(), sizeof( uint32 ), sizeof( uint32 ) * numIndeces, ( byte* )indeces.GetData(), RUF_Static );
	}

	if ( !GIsEditor && !GIsCommandlet )
	{
		verteces.RemoveAllElements();
		indeces.RemoveAllElements();
	}
}

void CStaticMesh::ReleaseRHI()
{
	vertexBufferRHI.SafeRelease();
	indexBufferRHI.SafeRelease();
	vertexFactory.SafeRelease();
}

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
		LE_LOG( LT_Warning, LC_Package, TEXT( "Deprecated package version, in future must be removed supports" ) );
	}
	else
	{
		InArchive << verteces;
		InArchive << indeces;
	}

	InArchive << surfaces;
	InArchive << materials;

	if ( InArchive.IsLoading() )
	{
		BeginUpdateResource( this );
	}
}

void CStaticMesh::SetData( const std::vector<SStaticMeshVertexType>& InVerteces, const std::vector<uint32>& InIndeces, const std::vector<SStaticMeshSurface>& InSurfaces, std::vector< TAssetHandle<CMaterial> >& InMaterials )
{
	// Copy new parameters of static mesh
	verteces		= InVerteces;
	indeces			= InIndeces;
	surfaces		= InSurfaces;
	materials		= InMaterials;

	// Mark dirty all drawing policy links
	for ( auto itElement = elementDrawingPolicyMap.begin(), itElementEnd = elementDrawingPolicyMap.end(); itElement != itElementEnd; ++itElement )
	{
		itElement->second->bDirty = true;
	}

	BeginUpdateResource( this );
}

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
		for ( auto itElement = elementDrawingPolicyMap.begin(), itElementEnd = elementDrawingPolicyMap.end(); itElement != itElementEnd; ++itElement )
		{
			itElement->second->bDirty = true;
		}
	}

	materials[ InMaterialIndex ] = InNewMaterial;
}

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

TSharedPtr<CStaticMesh::SElementDrawingPolicyLink> CStaticMesh::LinkDrawList( SSceneDepthGroup& InSDG, const std::vector< TAssetHandle<CMaterial> >& InOverrideMaterials )
{
	// Make key for element drawing policy link
	SElementKeyDrawingPolicyLink	elementKey{ &InSDG, 0 };

	// Calculate override hash
	for ( uint32 index = 0, count = InOverrideMaterials.size(); index < count; ++index )
	{
		elementKey.overrideHash = appMemFastHash( index, 0 );
		elementKey.overrideHash = appMemFastHash( InOverrideMaterials[ index ].ToSharedPtr(), elementKey.overrideHash );
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
		DrawingPolicyLinkRef_t           tmpDrawPolicyLink	= new DrawingPolicyLink_t( DEC_STATIC_MESH );
		TAssetHandle<CMaterial>			material			= materials[ surface.materialID ];

		// If current material is override - use custom material
		if ( indexSurface < numOverrideMaterials )
		{
			TAssetHandle<CMaterial>			overrideMaterial = InOverrideMaterials->at( surface.materialID );
			if ( overrideMaterial.IsValid() )
			{
				material = overrideMaterial;
			}
		}

		tmpDrawPolicyLink->drawingPolicy.Init( vertexFactory, material );

		// Generate mesh batch of surface
		SMeshBatch					meshBatch;
		meshBatch.baseVertexIndex	= surface.baseVertexIndex;
		meshBatch.firstIndex		= surface.firstIndex;
		meshBatch.numPrimitives		= surface.numPrimitives;
		meshBatch.indexBufferRHI	= indexBufferRHI;
		meshBatch.primitiveType		= PT_TriangleList;
		tmpDrawPolicyLink->meshBatchList.insert( meshBatch );

		// Add to new scene draw policy link
		DrawingPolicyLinkRef_t		drawingPolicyLink = InSDG.staticMeshDrawList.AddItem( tmpDrawPolicyLink );
		check( drawingPolicyLink );
		element->drawingPolicyLinks.push_back( drawingPolicyLink );

		// Get link to mesh batch. If not founded insert new
		MeshBatchList_t::iterator        itMeshBatchLink = drawingPolicyLink->meshBatchList.find( meshBatch );
		if ( itMeshBatchLink != drawingPolicyLink->meshBatchList.end() )
		{
			element->meshBatchLinks.push_back( &( *itMeshBatchLink ) );
		}
		else
		{
			const SMeshBatch*		meshBatchLink = &( *drawingPolicyLink->meshBatchList.insert( meshBatch ).first );
			element->meshBatchLinks.push_back( meshBatchLink );
		}
	}

	return element;
}

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
	for ( uint32 index = 0, count = itElement->second->drawingPolicyLinks.size(); index < count; ++index )
	{
		InSDG.staticMeshDrawList.RemoveItem( itElement->second->drawingPolicyLinks[ index ] );
	}

	InDrawingPolicyLink.Reset();
	elementDrawingPolicyMap.erase( itElement );
}