#include "Math/Math.h"
#include "Render/IcosphereMeshBuilder.h"

/*
==================
CIcosphereMeshBuilder::CIcosphereMeshBuilder
==================
*/
CIcosphereMeshBuilder::CIcosphereMeshBuilder()
	: subdivision( 3 )
	, radius( 1.f )
	, numPrimitives( 0 )
{}

/*
==================
CIcosphereMeshBuilder::Build
==================
*/
void CIcosphereMeshBuilder::Build()
{
	const float sStep	= 0.5;         // Horizontal texture step
	const float tStep	= 0.5;         // Vertical texture step
	
	Clear();

	// Compute 12 vertices of icosahedron
	// NOTE: v0 (top), v11(bottom), v1, v6(first vert on each row) cannot be
	// shared for smooth shading (they have different texcoords)
	std::vector<Vector4D>		tmpVertices = ComputeIcosahedronVerteces();

	Vector4D	v;       // Vertex
	Vector4D	n;       // Normal
	float		scale;   // Scale factor for normalization

	// Smooth icosahedron has 14 non-shared (0 to 13) and
	// 8 shared vertices (14 to 21) (total 22 vertices)
	//  00  01  02  03  04          //
	//  /\  /\  /\  /\  /\          //
	// /  \/  \/  \/  \/  \         //
	//10--14--15--16--17--11        //
	// \  /\  /\  /\  /\  /\        //
	//  \/  \/  \/  \/  \/  \       //
	//  12--18--19--20--21--13      //
	//   \  /\  /\  /\  /\  /       //
	//    \/  \/  \/  \/  \/        //
	//    05  06  07  08  09        //
	//////////////////////////////////
	
	// Add 14 non-shared vertices first (index from 0 to 13)
	verteces.push_back( SDynamicMeshVertexType( tmpVertices[ 0 ], Vector2D( sStep, 0.f ), Vector4D( 0.f, 0.f, 1.f, 0.f ) ) );						// v0 (top)
	verteces.push_back( SDynamicMeshVertexType( tmpVertices[ 0 ], Vector2D( sStep * 3.f, 0.f ), Vector4D( 0.f, 0.f, 1.f, 0.f ) ) );				// v1
	verteces.push_back( SDynamicMeshVertexType( tmpVertices[ 0 ], Vector2D( sStep * 5.f, 0.f ), Vector4D( 0.f, 0.f, 1.f, 0.f ) ) );				// v2
	verteces.push_back( SDynamicMeshVertexType( tmpVertices[ 0 ], Vector2D( sStep * 7.f, 0.f ), Vector4D( 0.f, 0.f, 1.f, 0.f ) ) );				// v3
	verteces.push_back( SDynamicMeshVertexType( tmpVertices[ 0 ], Vector2D( sStep * 9.f, 0.f ), Vector4D( 0.f, 0.f, 1.f, 0.f ) ) );				// v4

	verteces.push_back( SDynamicMeshVertexType( tmpVertices[ 11 ], Vector2D( sStep * 2.f, tStep * 3.f ), Vector4D( 0.f, 0.f, -1.f, 0.f ) ) );		// v5 (bottom)
	verteces.push_back( SDynamicMeshVertexType( tmpVertices[ 11 ], Vector2D( sStep * 4.f, tStep * 3.f ), Vector4D( 0.f, 0.f, -1.f, 0.f ) ) );		// v6
	verteces.push_back( SDynamicMeshVertexType( tmpVertices[ 11 ], Vector2D( sStep * 6.f, tStep * 3.f ), Vector4D( 0.f, 0.f, -1.f, 0.f ) ) );		// v7
	verteces.push_back( SDynamicMeshVertexType( tmpVertices[ 11 ], Vector2D( sStep * 8.f, tStep * 3.f ), Vector4D( 0.f, 0.f, -1.f, 0.f ) ) );		// v8
	verteces.push_back( SDynamicMeshVertexType( tmpVertices[ 11 ], Vector2D( sStep * 10.f, tStep * 3.f ), Vector4D( 0.f, 0.f, -1.f, 0.f ) ) );	// v9

	v = tmpVertices[ 1 ];		// v10 (left)
	ComputeVertexNormal( v, n );
	verteces.push_back( SDynamicMeshVertexType( v, Vector2D( 0.f, tStep ), n ) );
	verteces.push_back( SDynamicMeshVertexType( v, Vector2D( sStep * 10.f, tStep ), n ) );		// v11 (right)

	v = tmpVertices[ 6 ];		// v12 (left)
	ComputeVertexNormal( v, n );
	verteces.push_back( SDynamicMeshVertexType( v, Vector2D( sStep, tStep * 2.f ), n ) );
	verteces.push_back( SDynamicMeshVertexType( v, Vector2D( sStep * 11.f, tStep * 2.f ), n ) );	// v13 (right)

	// Add 8 shared vertices to array (index from 14 to 21)
	v = tmpVertices[ 2 ];		// v14 (shared)
	ComputeVertexNormal( v, n );
	verteces.push_back( SDynamicMeshVertexType( v, Vector2D( sStep * 2.f, tStep ), n ) );
	sharedIndeces[ Vector2D( sStep * 2.f, tStep ) ] = verteces.size() - 1;

	v = tmpVertices[ 3 ];		// v15 (shared)
	ComputeVertexNormal( v, n );
	verteces.push_back( SDynamicMeshVertexType( v, Vector2D( sStep * 4.f, tStep ), n ) );
	sharedIndeces[ Vector2D( sStep * 4.f, tStep ) ] = verteces.size() - 1;

	v = tmpVertices[ 4 ];		// v16 (shared)
	ComputeVertexNormal( v, n );
	verteces.push_back( SDynamicMeshVertexType( v, Vector2D( sStep * 6.f, tStep ), n ) );
	sharedIndeces[ Vector2D( sStep * 6.f, tStep ) ] = verteces.size() - 1;

	v = tmpVertices[ 5 ];		// v17 (shared)
	ComputeVertexNormal( v, n );
	verteces.push_back( SDynamicMeshVertexType( v, Vector2D( sStep * 8.f, tStep ), n ) );
	sharedIndeces[ Vector2D( sStep * 8.f, tStep ) ] = verteces.size() - 1;

	v = tmpVertices[ 7 ];		// v18 (shared)
	ComputeVertexNormal( v, n );
	verteces.push_back( SDynamicMeshVertexType( v, Vector2D( sStep * 3.f, tStep * 2.f ), n ) );
	sharedIndeces[ Vector2D( sStep * 3.f, tStep * 2.f ) ] = verteces.size() - 1;

	v = tmpVertices[ 8 ];		// v19 (shared)
	ComputeVertexNormal( v, n );
	verteces.push_back( SDynamicMeshVertexType( v, Vector2D( sStep * 5.f, tStep * 2.f ), n ) );
	sharedIndeces[ Vector2D( sStep * 5.f, tStep * 2.f ) ] = verteces.size() - 1;

	v = tmpVertices[ 9 ];		// v20 (shared)
	ComputeVertexNormal( v, n );
	verteces.push_back( SDynamicMeshVertexType( v, Vector2D( sStep * 7.f, tStep * 2.f ), n ) );
	sharedIndeces[ Vector2D( sStep * 7.f, tStep * 2.f ) ] = verteces.size() - 1;

	v = tmpVertices[ 10 ];		// v21 (shared)
	ComputeVertexNormal( v, n );
	verteces.push_back( SDynamicMeshVertexType( v, Vector2D( sStep * 9.f, tStep * 2.f ), n ) );
	sharedIndeces[ Vector2D( sStep * 9.f, tStep * 2.f ) ] = verteces.size() - 1;

	// Build index list for icosahedron (20 triangles)
	indeces.push_back( 0 );
	indeces.push_back( 10 );		// 1st row (5 tris)
	indeces.push_back( 14 );
 
	indeces.push_back( 1 );
	indeces.push_back( 14 );		
	indeces.push_back( 15 );

	indeces.push_back( 2 );
	indeces.push_back( 15 );
	indeces.push_back( 16 );

	indeces.push_back( 3 );
	indeces.push_back( 16 );
	indeces.push_back( 17 );

	indeces.push_back( 4 );
	indeces.push_back( 17 );
	indeces.push_back( 11 );

	indeces.push_back( 10 );
	indeces.push_back( 12 );		// 2nd row (10 tris)
	indeces.push_back( 14 );
  
	indeces.push_back( 12 );
	indeces.push_back( 18 );
	indeces.push_back( 14 );

	indeces.push_back( 14 );
	indeces.push_back( 18 );
	indeces.push_back( 15 );

	indeces.push_back( 18 );
	indeces.push_back( 19 );
	indeces.push_back( 15 );

	indeces.push_back( 15 );
	indeces.push_back( 19 );
	indeces.push_back( 16 );

	indeces.push_back( 19 );
	indeces.push_back( 20 );
	indeces.push_back( 16 );

	indeces.push_back( 16 );
	indeces.push_back( 20 );
	indeces.push_back( 17 );

	indeces.push_back( 20 );
	indeces.push_back( 21 );
	indeces.push_back( 17 );

	indeces.push_back( 17 );
	indeces.push_back( 21 );
	indeces.push_back( 11 );

	indeces.push_back( 21 );
	indeces.push_back( 13 );
	indeces.push_back( 11 );

	indeces.push_back( 5 );
	indeces.push_back( 18 );		// 3rd row (5 tris)
	indeces.push_back( 12 );

	indeces.push_back( 6 );
	indeces.push_back( 19 );
	indeces.push_back( 18 );

	indeces.push_back( 7 );
	indeces.push_back( 20 );
	indeces.push_back( 19 );

	indeces.push_back( 8 );
	indeces.push_back( 21 );
	indeces.push_back( 20 );

	indeces.push_back( 9 );
	indeces.push_back( 13 );
	indeces.push_back( 21 );

	// Subdivide icosahedron
	SubdivideVertices();

	// Calc num primitives
	numPrimitives = indeces.size() / 3;
}

/*
==================
CIcosphereMeshBuilder::SubdivideVertices
==================
*/
void CIcosphereMeshBuilder::SubdivideVertices()
{
	std::vector<uint32>				tmpIndices;
	uint32							indexCount = 0;
	uint32							i1, i2, i3;				// Indices from original triangle
	const SDynamicMeshVertexType	*v1, *v2, *v3;			// Ptr to original vertices of a triangle
	Vector4D						newV1, newV2, newV3;	// New subdivided vertex positions
	Vector4D						newN1, newN2, newN3;	// New subdivided normals
	Vector2D						newT1, newT2, newT3;	// New subdivided texture coords
	uint32							newI1, newI2, newI3;	// New subdivided indices
	uint32							i = 0, j = 0;

	// Iteration for subdivision
	for ( i = 1; i <= subdivision; ++i )
	{
		// Copy prev indices
		tmpIndices = indeces;

		// Clear prev arrays
		indeces.clear();

		indexCount = tmpIndices.size();
		for ( j = 0; j < indexCount; j += 3 )
		{
			// Get 3 indices of each triangle
			i1 = tmpIndices[ j ];
			i2 = tmpIndices[ j + 1 ];
			i3 = tmpIndices[ j + 2 ];

			// Get 3 vertex attribs from prev triangle
			v1 = &verteces[ i1 ];
			v2 = &verteces[ i2 ];
			v3 = &verteces[ i3 ];

			// Get 3 new vertex attribs by spliting half on each edge
			ComputeHalfVertex( v1->position, v2->position, radius, newV1 );
			ComputeHalfVertex( v2->position, v3->position, radius, newV2 );
			ComputeHalfVertex( v1->position, v3->position, radius, newV3 );
			ComputeHalfTexCoord( v1->texCoord, v2->texCoord, newT1 );
			ComputeHalfTexCoord( v2->texCoord, v3->texCoord, newT2 );
			ComputeHalfTexCoord( v1->texCoord, v3->texCoord, newT3 );
			ComputeVertexNormal( newV1, newN1 );
			ComputeVertexNormal( newV2, newN2 );
			ComputeVertexNormal( newV3, newN3 );

			// Add new vertices to arrays
			// It will Assert if it is shared/non-shared and return index
			newI1 = AddSubVertex( newV1, newT1, newN1 );
			newI2 = AddSubVertex( newV2, newT2, newN2 );
			newI3 = AddSubVertex( newV3, newT3, newN3 );

			// Add 4 new triangle indices
			indeces.push_back( i1 );
			indeces.push_back( newI1 );
			indeces.push_back( newI3 );

			indeces.push_back( newI1 );
			indeces.push_back( i2 );
			indeces.push_back( newI2 );

			indeces.push_back( newI1 );
			indeces.push_back( newI2 );
			indeces.push_back( newI3 );

			indeces.push_back( newI3 );
			indeces.push_back( newI2 );
			indeces.push_back( i3 );
		}
	}
}

/*
==================
CIcosphereMeshBuilder::AddSubVertex
==================
*/
uint32 CIcosphereMeshBuilder::AddSubVertex( const Vector4D& InVertex, const Vector2D& InTexCoord, const Vector4D& InNormal )
{
	uint32		index = 0;     // Return value;

	// Check if is shared vertex or not first
	if ( IsSharedTexCoord( InTexCoord ) )
	{
		// Find if it does already exist in sharedIndices map using (s,t) key
		// if not in the list, add the vertex attribs to arrays and return its index
		// if exists, return the current index
		auto		it = sharedIndeces.find( InTexCoord );
		if ( it == sharedIndeces.end() )
		{
			verteces.push_back( SDynamicMeshVertexType( InVertex, InTexCoord, InNormal ) );
			index						= verteces.size() - 1;
			sharedIndeces[ InTexCoord ] = index;
		}
		else
		{
			index = it->second;
		}
	}
	// Not shared
	else
	{
		verteces.push_back( SDynamicMeshVertexType( InVertex, InTexCoord, InNormal ) );
		index		= verteces.size() - 1;
	}

	return index;
}

/*
==================
CIcosphereMeshBuilder::ComputeIcosahedronVerteces
==================
*/
std::vector<Vector4D> CIcosphereMeshBuilder::ComputeIcosahedronVerteces()
{
	const float		hAngle = PI / 180.f * 72.f;				// 72 degree = 360 / 5
	const float		vAngle = SMath::ATan( 1.0f / 2.f );		// Elevation = 26.565 degree

	std::vector<Vector4D>		vertices( 12 );		// 12 vertices
	float z, xy;									// Coords
	float hAngle1 = -PI / 2.f - hAngle / 2.f;		// Start from -126 deg at 2nd row
	float hAngle2 = -PI / 2.f;						// Start from -90 deg at 3rd row

	// The first top vertex (0, 0, radius)
	vertices[ 0 ] = Vector4D( 0.f, 0.f, radius, 1.f );

	// 10 vertices at 2nd and 3rd rows
	for ( uint32 i = 1; i <= 5; ++i )
	{
		z	= radius * SMath::Sin( vAngle );	// Elevaton
		xy	= radius * SMath::Cos( vAngle );

		vertices[ i ]		= Vector4D( xy * SMath::Cos( hAngle1 ), xy * SMath::Sin( hAngle1 ), z, 1.f );
		vertices[ i + 5 ]	= Vector4D( xy * SMath::Cos( hAngle2 ), xy * SMath::Sin( hAngle2 ), -z, 1.f );

		// Next horizontal angles
		hAngle1 += hAngle;
		hAngle2 += hAngle;
	}

	// The last bottom vertex (0, 0, -radius )
	vertices[ 11 ] = Vector4D( 0.f, 0.f, -radius, 1.f );
	return vertices;
}

/*
==================
CIcosphereMeshBuilder::ComputeFaceNormal
==================
*/
void CIcosphereMeshBuilder::ComputeFaceNormal( const Vector4D& InVert0, const Vector4D& InVert1, const Vector4D& InVert2, Vector4D& OutNormal )
{
	// Default return value (0, 0, 0)
	OutNormal = Vector4D( SMath::vectorZero, 0.f );

	// Find 2 edge vectors: InVert0-InVert1, InVert0-InVert2
	Vector4D		edge0 = InVert1 - InVert0;
	Vector4D		edge1 = InVert2 - InVert0;

	// Cross product: edge0 x edge1
	OutNormal = Vector4D( SMath::CrossVector( edge0, edge1 ), 0.f );

	// Normalize normal
	SMath::NormalizeVector( OutNormal );
}