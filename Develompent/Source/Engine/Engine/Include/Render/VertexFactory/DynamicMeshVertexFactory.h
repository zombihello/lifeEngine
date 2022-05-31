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
struct SDynamicMeshVertexType
{
	Vector4D		position;		/**< Position vertex */
	Vector2D		texCoord;		/**< Texture coords */
	Vector4D		normal;			/**< Normal */
	Vector4D		tangent;		/**< Tangent */
	Vector4D		binormal;		/**< Binormal */
	Vector4D		color;			/**< Color */

	/**
	 * Constructor
	 */
	SDynamicMeshVertexType()
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
	SDynamicMeshVertexType( const Vector4D& InPosition, const Vector2D& InTexCoord, const Vector4D& InNormal, const Vector4D& InTangent, const Vector4D& InBinormal, const Vector4D& InColor = Vector4D( 1.f, 1.f, 1.f, 1.f ) )
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
	SDynamicMeshVertexType( const Vector4D& InPosition, const Vector2D& InTexCoord, const Vector4D& InNormal, const Vector4D& InColor = Vector4D( 1.f, 1.f, 1.f, 1.f ) )
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
	bool FORCEINLINE operator==( const SDynamicMeshVertexType& InOther ) const
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
class CDynamicMeshVertexDeclaration : public CRenderResource
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
	FORCEINLINE VertexDeclarationRHIRef_t GetVertexDeclarationRHI()
	{
		return vertexDeclarationRHI;
	}

private:
	VertexDeclarationRHIRef_t		vertexDeclarationRHI;		/**< Vertex declaration RHI */
};

/**
 * @ingroup Engine
 * Global resource of dynamic mesh vertex declaration
 */
extern TGlobalResource< CDynamicMeshVertexDeclaration >			GDynamicMeshVertexDeclaration;

/**
 * @ingroup Engine
 * Vertex factory for render dynamic meshes
 */
class CDynamicMeshVertexFactory : public CVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE( CDynamicMeshVertexFactory )

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
	static CVertexFactoryShaderParameters* ConstructShaderParameters( EShaderFrequency InShaderFrequency );
};

#endif // !DYNAMICMESHVERTEXFACTORY_H
