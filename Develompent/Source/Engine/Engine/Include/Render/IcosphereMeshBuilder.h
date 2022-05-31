/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ICOSPHEREMESHBUILDER_H
#define ICOSPHEREMESHBUILDER_H

#include <vector>
#include <unordered_map>

#include "Render/VertexFactory/DynamicMeshVertexFactory.h"

 /**
  * @ingroup Engine
  * @brief Class for build icosphere
  */
class CIcosphereMeshBuilder
{
public:
	/**
	 * @brief Constructor
	 */
	CIcosphereMeshBuilder();

	/**
	 * @brief Build mesh
	 */
	void Build();

	/**
	 * @brief Clear data in mesh builder
	 */
	FORCEINLINE void Clear()
	{
		verteces.clear();
		indeces.clear();
	}

	/**
	 * @brief Set subdivision
	 * @param InSubdivision		Subdivision
	 */
	FORCEINLINE void SetSubdivision( uint32 InSubdivision )
	{
		subdivision = InSubdivision;
	}

	/**
	 * @brief Set radius
	 * @param InRadius		Radius
	 */
	FORCEINLINE void SetRadius( float InRadius )
	{
		radius = InRadius;
	}

	/**
	 * @brief Get array of verteces
	 * @return Return array of verteces
	 */
	FORCEINLINE const std::vector< SDynamicMeshVertexType >& GetVerteces() const
	{
		return verteces;
	}

	/**
	 * @brief Get array of indeces
	 * @return Return array of indeces
	 */
	FORCEINLINE const std::vector< uint32 >& GetIndeces() const
	{
		return indeces;
	}

	/**
	 * @brief Get subdivision
	 * @return Return subdivision
	 */
	FORCEINLINE uint32 GetSubdivision() const
	{
		return subdivision;
	}

	/**
	 * @brief Get radius
	 * @return Return radius
	 */
	FORCEINLINE float GetRadius() const
	{
		return radius;
	}

	/**
	 * @brief Get number of primitives
	 * @return Return number of primitives
	 */
	FORCEINLINE uint32 GetNumPrimitives() const
	{
		return numPrimitives;
	}

private:
	/**
	 * @brief Functions to extract the texture coord as a key for std::unordered_map and std::unordered_set
	 */
	struct STexCoordKeyFunc
	{
		/**
		 * @brief Calculate hash of the texture coord
		 *
		 * @param InTexCoord	Texture coord
		 * @return Return hash of this texture coord
		 */
		FORCEINLINE std::size_t operator()( const Vector2D& InTexCoord ) const
		{
			return appMemFastHash( InTexCoord );
		}

		/**
		 * @brief Compare texture coords
		 *
		 * @param InA	First texture coord
		 * @param InB	Second texture coord
		 * @return Return true if InA and InB equal, else returning false
		 */
		FORCEINLINE bool operator()( const Vector2D& InA, const Vector2D& InB ) const
		{
			return InA.x < InB.x && InA.y < InB.y;
		}
	};

	/**
	 * @brief Subdivide vertices
	 * @note Divide a trianlge (v1-v2-v3) into 4 sub triangles by adding middle vertices
	 * (newV1, newV2, newV3) and repeat N times. If subdivision=0, do nothing.
	 */
	void SubdivideVertices();

	/**
	 * @brief Compute icosahedron verteces
	 * @return Return array verteces of icosahedron
	 */
	std::vector<Vector4D> ComputeIcosahedronVerteces();

	/**
	 * @brief Add sub vertex
	 * 
	 * @param InVertex		Vertex position
	 * @param InTexCoord	Texture coord
	 * @param InNormal		Normal
	 * @return Return index of vertex
	 */
	uint32 AddSubVertex( const Vector4D& InVertex, const Vector2D& InTexCoord, const Vector4D& InNormal );

	/**
	 * @brief Compute face normal
	 * 
	 * @param InVert0	Vertex 0
	 * @param InVert1	Vertex 1
	 * @param InVert2	Vertex 2
	 * @param OutNormal	Computed normal
	 */
	void ComputeFaceNormal( const Vector4D& InVert0, const Vector4D& InVert1, const Vector4D& InVert2, Vector4D& OutNormal );

	/**
	 * @brief Find middle point of 2 vertices
	 * @note New vertex must be resized, so the length is equal to the given length
	 * 
	 * @param InVert0	Vertex 0
	 * @param InVert1	Vertex 1
	 * @param InLength	Length
	 * @param OutVert	Computed vertex
	 */
	FORCEINLINE void ComputeHalfVertex( const Vector4D& InVert0, const Vector4D& InVert1, float InLength, Vector4D& OutVert )
	{
		OutVert = InVert0 + InVert1;
		OutVert *= ComputeScaleForLength( OutVert, InLength );
		OutVert.w = 1.f;
	}

	/**
	 * @brief Compute vertex normal
	 * 
	 * @param InVertex	Vertex
	 * @param OutNormal	Computed normal
	 */
	FORCEINLINE void ComputeVertexNormal( const Vector4D& InVertex, Vector4D& OutNormal )
	{
		OutNormal = InVertex * ComputeScaleForLength( InVertex, 1.f );
	}

	/**
	 * @brief Compute scale for length
	 * Get the scale factor for vector to resize to the given length of vector
	 * 
	 * @param InVertex		Vertex
	 * @param InLength		Length
	 */
	FORCEINLINE float ComputeScaleForLength( const Vector4D& InVertex, float InLength ) const
	{
		return InLength / SMath::Sqrt( InVertex.x * InVertex.x + InVertex.y * InVertex.y + InVertex.z * InVertex.z );
	}

	/**
	 * @brief Find middle point of 2 vertices
	 * @note New vertex must be resized, so the length is equal to the given length
	 *
	 * @param InTexCoord0	Texture coord 0
	 * @param InTexCoord1	Texture coord 1
	 * @param OutTexCoord	Computed new texture coord
	 */
	FORCEINLINE void ComputeHalfTexCoord( const Vector2D& InTexCoord0, const Vector2D& InTexCoord1, Vector2D& OutTexCoord )
	{
		OutTexCoord = ( InTexCoord0 + InTexCoord1 ) * 0.5f;
	}

	/**
	 * @brief Is shared texture coord
	 * @param InTexCoord	Texture coord
	 * @return Return TRUE if texture coord is shared
	 */
	FORCEINLINE bool IsSharedTexCoord( const Vector2D& InTexCoord ) const
	{
		// 20 non-shared line segments
		const float			s = 0.5;  // Texture steps
		const float			t = 0.5;
		static Vector2D	segments[] = {	Vector2D( s,		0		),			Vector2D( 0,		t		),		// 00 - 05
										Vector2D( s,		0		),			Vector2D( s * 2,	t		),		// 00 - 06
										Vector2D( s * 3,	0		),			Vector2D( s * 2,	t		),		// 01 - 06
										Vector2D( s * 3,	0		),			Vector2D( s * 4,	t		),		// 01 - 07
										Vector2D( s * 5,	0		),			Vector2D( s * 4,	t		),		// 02 - 07
										Vector2D( s * 5,	0		),			Vector2D( s * 6,	t		),		// 02 - 08
										Vector2D( s * 7,	0		),			Vector2D( s * 6,	t		),		// 03 - 08
										Vector2D( s * 7,	0		),			Vector2D( s * 8,	t		),		// 03 - 09
										Vector2D( s * 9,	0		),			Vector2D( s * 8,	t		),		// 04 - 09
										Vector2D( s * 9,	0		),			Vector2D( 1,		t * 2	),		// 04 - 14
										Vector2D( 0,		t		),			Vector2D( s * 2,	1		),		// 05 - 15
										Vector2D( s * 3,	t * 2	),			Vector2D( s * 2,	1		),		// 10 - 15
										Vector2D( s * 3,	t * 2	),			Vector2D( s * 4,	1		),		// 10 - 16
										Vector2D( s * 5,	t * 2	),			Vector2D( s * 4,	1		),		// 11 - 16
										Vector2D( s * 5,	t * 2	),			Vector2D( s * 6,	1		),		// 11 - 17
										Vector2D( s * 7,	t * 2	),			Vector2D( s * 6,	1		),		// 12 - 17
										Vector2D( s * 7,	t * 2	),			Vector2D( s * 8,	1		),		// 12 - 18
										Vector2D( s * 9,	t * 2	),			Vector2D( s * 8,	1		),		// 13 - 18
										Vector2D( s * 9,	t * 2	),			Vector2D( s * 10,	1		),		// 13 - 19
										Vector2D( 1,		t * 2	),			Vector2D( s * 10,	1		) };	// 14 - 19

		// Check the point with all 20 line segments
		// if it is on the line segment, it is non-shared
		uint32		count = ARRAY_COUNT( segments );
		for ( uint32 i = 0, j = 1; i < count; i += 2, j += 2 )
		{
			if ( IsOnLineSegment( segments[ i ], segments[ j ], InTexCoord ) )
			{
				return false;   // Not shared
			}
		}

		return true;
	}

	/**
	 * @brief Determine a point InC is on the line segment InA-InB
	 * 
	 * @param InA	Texture coord A
	 * @param InB	Texture coord B
	 * @param InC	Texture coord C
	 */
	FORCEINLINE bool IsOnLineSegment( const Vector2D& InA, const Vector2D& InB, const Vector2D& InC ) const
	{
		const float			epsilon = 0.0001f;

		// Cross product must be 0 if InC is on the line
		float		cross = ( ( InB.x - InA.x ) * ( InC.y - InA.y ) ) - ( ( InB.y - InA.y ) * ( InC.x - InA.x ) );
		if ( cross > epsilon || cross < -epsilon )
		{
			return false;
		}

		// InC must be within InA-InB
		if ( ( InC.x > InA.x && InC.x > InB.x ) || ( InC.x < InA.x && InC.x < InB.x ) )
		{
			return false;
		}
		if ( ( InC.y > InA.y && InC.y > InB.y ) || ( InC.y < InA.y && InC.y < InB.y ) )
		{
			return false;
		}

		return true;    // All passed, it is on the line segment
	}

	uint32															subdivision;		/**< Subdivision */
	float															radius;				/**< Radius */
	uint32															numPrimitives;		/**< Number primitives in builded mesh */
	std::vector< SDynamicMeshVertexType >							verteces;			/**< Array of verteces */
	std::vector< uint32 >											indeces;			/**< Array of indeces */
	std::unordered_map< Vector2D, uint32, STexCoordKeyFunc >		sharedIndeces;		/**< Shared indeces */
};

#endif // !ICOSPHEREMESHBUILDER_H