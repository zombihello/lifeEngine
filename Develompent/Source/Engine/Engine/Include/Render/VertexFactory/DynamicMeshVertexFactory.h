/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef DYNAMICMESHVERTEXFACTORY_H
#define DYNAMICMESHVERTEXFACTORY_H

#include "Math/Math.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/RenderUtils.h"

 /**
  * @ingroup Engine
  * Vertex type for dynamic mesh
  */
struct FDynamicMeshVertexType
{
	FVector4D		position;		/**< Position vertex */
	FVector2D		texCoord;		/**< Texture coords */
	FVector4D		normal;			/**< Normal */
	FVector4D		tangent;		/**< Tangent */
	FVector4D		binormal;		/**< Binormal */
	FVector4D		color;			/**< Color */

	/**
	 * Constructor
	 */
	FDynamicMeshVertexType()
	{}

	/**
	 * Constructor
	 * 
	 * @param InPosition	Position
	 * @param InTexCoord	Texture coord
	 * @param InNormal		Normal
	 * @param InTangent		Tangent
	 * @param InBinormal	Binormal
	 * @param InColor		Color
	 */
	FDynamicMeshVertexType( const FVector4D& InPosition, const FVector2D& InTexCoord, const FVector4D& InNormal, const FVector4D& InTangent, const FVector4D& InBinormal, const FVector4D& InColor = FVector4D( 1.f, 1.f, 1.f, 1.f ) )
		: position( InPosition )
		, texCoord( InTexCoord )
		, normal( InNormal )
		, tangent( InTangent )
		, binormal( InBinormal )
		, color( InColor )
	{}

	/**
	 * Constructor
	 *
	 * @param InPosition	Position
	 * @param InTexCoord	Texture coord
	 * @param InNormal		Normal
	 * @param InColor		Color
	 */
	FDynamicMeshVertexType( const FVector4D& InPosition, const FVector2D& InTexCoord, const FVector4D& InNormal, const FVector4D& InColor = FVector4D( 1.f, 1.f, 1.f, 1.f ) )
		: position( InPosition )
		, texCoord( InTexCoord )
		, normal( InNormal )
		, tangent( 1.f, 0.f, 0.f, 0.f )
		, binormal( 0.f, -InPosition.y, InPosition.z, 0.f )
		, color( InColor )
	{}

	/**
	 * Overload operator ==
	 */
	bool FORCEINLINE operator==( const FDynamicMeshVertexType& InOther ) const
	{
		return position == InOther.position &&
			texCoord == InOther.texCoord &&
			normal == InOther.normal &&
			tangent == InOther.tangent &&
			binormal == InOther.binormal &&
			color == InOther.color;
	}
};

/**
 * @ingroup Engine
 * The dynamic mesh vertex declaration resource type
 */
class FDynamicMeshVertexDeclaration : public FRenderResource
{
public:
	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;

	/**
	 * @brief Releases the RHI resources used by this resource.
	 * Called when the resource is released.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseRHI() override;

	/**
	 * @brief Get vertex declaration RHI
	 * @return Return vertex declaration RHI
	 */
	FORCEINLINE FVertexDeclarationRHIRef GetVertexDeclarationRHI()
	{
		return vertexDeclarationRHI;
	}

private:
	FVertexDeclarationRHIRef		vertexDeclarationRHI;		/**< Vertex declaration RHI */
};

/**
 * @ingroup Engine
 * Global resource of dynamic mesh vertex declaration
 */
extern TGlobalResource< FDynamicMeshVertexDeclaration >			GDynamicMeshVertexDeclaration;

/**
 * @ingroup Engine
 * Vertex factory for render dynamic meshes
 */
class FDynamicMeshVertexFactory : public FVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE( FDynamicMeshVertexFactory )

public:
	enum EStreamSourceSlot
	{
		SSS_Main = 0		/**< Main vertex buffer */
	};

	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;

	/**
	 * @brief Get type hash
	 * @return Return hash of vertex factory
	 */
	virtual uint64 GetTypeHash() const override;

	/**
	 * @brief Construct vertex factory shader parameters
	 * 
	 * @param InShaderFrequency Shader frequency
	 * @return Return instance of vertex factory shader parameters
	 */
	static FVertexFactoryShaderParameters* ConstructShaderParameters( EShaderFrequency InShaderFrequency );
};

#endif // !DYNAMICMESHVERTEXFACTORY_H
