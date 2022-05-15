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
class FIcosphereMeshBuilder
{
public:
	/**
	 * @brief Constructor
	 */
	FIcosphereMeshBuilder();

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
	FORCEINLINE const std::vector< FDynamicMeshVertexType >& GetVerteces() const
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
	struct FTexCoordKeyFunc
	{
		/**
		 * @brief Calculate hash of the texture coord
		 *
		 * @param InTexCoord	Texture coord
		 * @return Return hash of this texture coord
		 */
		FORCEINLINE std::size_t operator()( const FVector2D& InTexCoord ) const
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
		FORCEINLINE bool operator()( const FVector2D& InA, const FVector2D& InB ) const
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
	std::vector<FVector4D> ComputeIcosahedronVerteces();

	/**
	 * @brief Add sub vertex
	 * 
	 * @param InVertex		Vertex position
	 * @param InTexCoord	Texture coord
	 * @param InNormal		Normal
	 * @return Return index of vertex
	 */
	uint32 AddSubVertex( const FVector4D& InVertex, const FVector2D& InTexCoord, const FVector4D& InNormal );

	/**
	 * @brief Compute face normal
	 * 
	 * @param InVert0	Vertex 0
	 * @param InVert1	Vertex 1
	 * @param InVert2	Vertex 2
	 * @param OutNormal	Computed normal
	 */
	void ComputeFaceNormal( const FVector4D& InVert0, const FVector4D& InVert1, const FVector4D& InVert2, FVector4D& OutNormal );

	/**
	 * @brief Find middle point of 2 vertices
	 * @note New vertex must be resized, so the length is equal to the given length
	 * 
	 * @param InVert0	Vertex 0
	 * @param InVert1	Vertex 1
	 * @param InLength	Length
	 * @param OutVert	Computed vertex
	 */
	FORCEINLINE void ComputeHalfVertex( const FVector4D& InVert0, const FVector4D& InVert1, float InLength, FVector4D& OutVert )
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
	FORCEINLINE void ComputeVertexNormal( const FVector4D& InVertex, FVector4D& OutNormal )
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
	FORCEINLINE float ComputeScaleForLength( const FVector4D& InVertex, float InLength ) const
	{
		return InLength / FMath::Sqrt( InVertex.x * InVertex.x + InVertex.y * InVertex.y + InVertex.z * InVertex.z );
	}

	/**
	 * @brief Find middle point of 2 vertices
	 * @note New vertex must be resized, so the length is equal to the given length
	 *
	 * @param InTexCoord0	Texture coord 0
	 * @param InTexCoord1	Texture coord 1
	 * @param OutTexCoord	Computed new texture coord
	 */
	FORCEINLINE void ComputeHalfTexCoord( const FVector2D& InTexCoord0, const FVector2D& InTexCoord1, FVector2D& OutTexCoord )
	{
		OutTexCoord = ( InTexCoord0 + InTexCoord1 ) * 0.5f;
	}

	/**
	 * @brief Is shared texture coord
	 * @param InTexCoord	Texture coord
	 * @return Return TRUE if texture coord is shared
	 */
	FORCEINLINE bool IsSharedTexCoord( const FVector2D& InTexCoord ) const
	{
		// 20 non-shared line segments
		const float			s = 0.5;  // Texture steps
		const float			t = 0.5;
		static FVector2D	segments[] = {	FVector2D( s,		0		),			FVector2D( 0,		t		),		// 00 - 05
											FVector2D( s,		0		),			FVector2D( s * 2,	t		),		// 00 - 06
											FVector2D( s * 3,	0		),			FVector2D( s * 2,	t		),		// 01 - 06
											FVector2D( s * 3,	0		),			FVector2D( s * 4,	t		),		// 01 - 07
											FVector2D( s * 5,	0		),			FVector2D( s * 4,	t		),		// 02 - 07
											FVector2D( s * 5,	0		),			FVector2D( s * 6,	t		),		// 02 - 08
											FVector2D( s * 7,	0		),			FVector2D( s * 6,	t		),		// 03 - 08
											FVector2D( s * 7,	0		),			FVector2D( s * 8,	t		),		// 03 - 09
											FVector2D( s * 9,	0		),			FVector2D( s * 8,	t		),		// 04 - 09
											FVector2D( s * 9,	0		),			FVector2D( 1,		t * 2	),		// 04 - 14
											FVector2D( 0,		t		),			FVector2D( s * 2,	1		),		// 05 - 15
											FVector2D( s * 3,	t * 2	),			FVector2D( s * 2,	1		),		// 10 - 15
											FVector2D( s * 3,	t * 2	),			FVector2D( s * 4,	1		),		// 10 - 16
											FVector2D( s * 5,	t * 2	),			FVector2D( s * 4,	1		),		// 11 - 16
											FVector2D( s * 5,	t * 2	),			FVector2D( s * 6,	1		),		// 11 - 17
											FVector2D( s * 7,	t * 2	),			FVector2D( s * 6,	1		),		// 12 - 17
											FVector2D( s * 7,	t * 2	),			FVector2D( s * 8,	1		),		// 12 - 18
											FVector2D( s * 9,	t * 2	),			FVector2D( s * 8,	1		),		// 13 - 18
											FVector2D( s * 9,	t * 2	),			FVector2D( s * 10,	1		),		// 13 - 19
											FVector2D( 1,		t * 2	),			FVector2D( s * 10,	1		) };	// 14 - 19

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
	FORCEINLINE bool IsOnLineSegment( const FVector2D& InA, const FVector2D& InB, const FVector2D& InC ) const
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
	std::vector< FDynamicMeshVertexType >							verteces;			/**< Array of verteces */
	std::vector< uint32 >											indeces;			/**< Array of indeces */
	std::unordered_map< FVector2D, uint32, FTexCoordKeyFunc >		sharedIndeces;		/**< Shared indeces */
};

#endif // !ICOSPHEREMESHBUILDER_H