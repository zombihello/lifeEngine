#include "LEBuild.h"
#include "Misc/EngineGlobals.h"
#include "Math/Math.h"
#include "RHI/BaseRHI.h"
#include "Render/VertexFactory/LightVertexFactory.h"
#include "Render/VertexFactory/GeneralVertexFactoryParams.h"
#include "Render/Scene.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( CLightVertexFactory, TEXT( "LightVertexFactory.hlsl" ), true, CLightVertexFactory::SSS_Instance )

//
// GLOBALS
//
TGlobalResource<CLightVertexDeclaration>			GLightVertexDeclaration;

/**
 * @ingroup Engine
 * @brief Base struct of the light instance buffer
 */
struct SBaseLightInstanceBuffer
{
	CColor		color;	/**< Light color */
};

/**
 * @ingroup Engine
 * @brief Empty template class of light instance buffer (don't use!)
 */
template<ELightType InLightType>
struct TLightInstanceBuffer : public SBaseLightInstanceBuffer
{
	static_assert( InLightType != LT_Unknown, "This template class cannot be used" );
};

/**
 * @ingroup Engine
 * @brief Struct of instance buffer for point light
 */
template<>
struct TLightInstanceBuffer<LT_Point> : public SBaseLightInstanceBuffer
{
	Matrix		instanceLocalToWorld;		/**< Local to World matrix for each instance */
};

/**
 * @ingroup Engine
 * @brief Struct of instance buffer for spot light
 */
template<>
struct TLightInstanceBuffer<LT_Spot> : public SBaseLightInstanceBuffer
{
	Matrix		instanceLocalToWorld;		/**< Local to World matrix for each instance */
};

/**
 * @ingroup Engine
 * @brief Struct of instance buffer for directional light
 */
template<>
struct TLightInstanceBuffer<LT_Directional> : public SBaseLightInstanceBuffer
{};

void CLightVertexDeclaration::InitRHI()
{
	// Init vertex declaration for point light
	{
		VertexDeclarationElementList_t		vertexDeclElementList =
		{
			SVertexElement( CLightVertexFactory::SSS_Main,		sizeof( SLightVertexType ),					STRUCT_OFFSET( SLightVertexType, position ),									VET_Float4, VEU_Position,			0 ),

#if USE_INSTANCING
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Point> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Point>, instanceLocalToWorld ),			VET_Float4, VEU_Position,			1, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Point> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Point>, instanceLocalToWorld ) + 16,		VET_Float4, VEU_Position,			2, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Point> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Point>, instanceLocalToWorld ) + 32,		VET_Float4, VEU_Position,			3, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Point> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Point>, instanceLocalToWorld ) + 48,		VET_Float4, VEU_Position,			4, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Point> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Point>, color ),							VET_Color,	VEU_Color,				0, true ),
#endif // USE_INSTANCING
		};
		vertexDeclarationRHI[LT_Point] = GRHI->CreateVertexDeclaration( vertexDeclElementList );
	}

	// Init vertex declaration for spot light
	{
		VertexDeclarationElementList_t		vertexDeclElementList =
		{
			SVertexElement( CLightVertexFactory::SSS_Main,		sizeof( SLightVertexType ),					STRUCT_OFFSET( SLightVertexType, position ),									VET_Float4, VEU_Position,			0 ),

#if USE_INSTANCING
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Spot> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Spot>, instanceLocalToWorld ),			VET_Float4, VEU_Position,			1, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Spot> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Spot>, instanceLocalToWorld ) + 16,		VET_Float4, VEU_Position,			2, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Spot> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Spot>, instanceLocalToWorld ) + 32,		VET_Float4, VEU_Position,			3, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Spot> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Spot>, instanceLocalToWorld ) + 48,		VET_Float4, VEU_Position,			4, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Spot> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Spot>, color ),							VET_Color,	VEU_Color,				0, true ),
#endif // USE_INSTANCING
		};
		vertexDeclarationRHI[LT_Spot] = GRHI->CreateVertexDeclaration( vertexDeclElementList );
	}

	// Init vertex declaration for directional light
	{
		VertexDeclarationElementList_t		vertexDeclElementList =
		{
			SVertexElement( CLightVertexFactory::SSS_Main,		sizeof( SLightVertexType ),						STRUCT_OFFSET( SLightVertexType, position ),									VET_Float4, VEU_Position,			0 ),

#if USE_INSTANCING
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Directional> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Directional>, color ),					VET_Color,	VEU_Color,				0, true ),
#endif // USE_INSTANCING
		};
		vertexDeclarationRHI[LT_Directional] = GRHI->CreateVertexDeclaration( vertexDeclElementList );
	}
}

void CLightVertexDeclaration::ReleaseRHI()
{
	vertexDeclarationRHI[LT_Point].SafeRelease();
	vertexDeclarationRHI[LT_Spot].SafeRelease();
	vertexDeclarationRHI[LT_Directional].SafeRelease();
}


CLightVertexShaderParameters::CLightVertexShaderParameters()
	: CGeneralVertexShaderParameters( CLightVertexFactory::staticType.SupportsInstancing() )
{}

void CLightVertexShaderParameters::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	appErrorf( TEXT( "CLightVertexShaderParameters::SetMesh( MeshBatch ) Not supported" ) );
}

void CLightVertexShaderParameters::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CPointLightComponent>>& InLights, const class CLightVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	if ( !bSupportsInstancing )
	{
		appErrorf( TEXT( "Not implemented, need add it in future" ) );
	}
	else
	{
		check( InVertexFactory );
		InVertexFactory->SetupInstancing( InDeviceContextRHI, InLights, InView, InNumInstances, InStartInstanceID );
	}
}

void CLightVertexShaderParameters::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CSpotLightComponent>>& InLights, const class CLightVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	if ( !bSupportsInstancing )
	{
		appErrorf( TEXT( "Not implemented, need add it in future" ) );
	}
	else
	{
		check( InVertexFactory );
		InVertexFactory->SetupInstancing( InDeviceContextRHI, InLights, InView, InNumInstances, InStartInstanceID );
	}
}

void CLightVertexShaderParameters::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CDirectionalLightComponent>>& InLights, const class CLightVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	if ( !bSupportsInstancing )
	{
		appErrorf( TEXT( "Not implemented, need add it in future" ) );
	}
	else
	{
		check( InVertexFactory );
		InVertexFactory->SetupInstancing( InDeviceContextRHI, InLights, InView, InNumInstances, InStartInstanceID );
	}
}


CLightVertexFactory::CLightVertexFactory( ELightType InLightType )
	: lightType( InLightType )
{}

void CLightVertexFactory::InitRHI()
{
	InitDeclaration( GLightVertexDeclaration.GetVertexDeclarationRHI( lightType ) );
}

void CLightVertexFactory::SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	appErrorf( TEXT( "CLightVertexFactory::SetupInstancing( SMeshBatch ) :: Not supported" ) );
}

void CLightVertexFactory::SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CPointLightComponent>>& InLights, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	check( lightType == LT_Point );
	check( InStartInstanceID < InLights.size() && InNumInstances <= InLights.size() - InStartInstanceID );

	std::vector<TLightInstanceBuffer<LT_Point>>		instanceBuffers;
	instanceBuffers.resize( InNumInstances );
	
	uint32		index = 0;
	for ( auto it = std::next( InLights.begin(), InStartInstanceID ), itEnd = InLights.end(); it != itEnd && index < InNumInstances; ++it, ++index )
	{
		TLightInstanceBuffer<LT_Point>&		instanceBuffer		= instanceBuffers[index];
		TRefCountPtr<CPointLightComponent>	pointLightComponent = *it;
		instanceBuffer.instanceLocalToWorld						= pointLightComponent->GetComponentTransform().ToMatrix() * SMath::ScaleMatrix( Vector(200.f,200.f,200.f) );
		instanceBuffer.color									= pointLightComponent->GetColor();
	}

	GRHI->SetupInstancing( InDeviceContextRHI, SSS_Instance, instanceBuffers.data(), sizeof( TLightInstanceBuffer<LT_Point> ), InNumInstances * sizeof( TLightInstanceBuffer<LT_Point> ), InNumInstances );
}

void CLightVertexFactory::SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CSpotLightComponent>>& InLights, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	check( lightType == LT_Spot );
	check( InStartInstanceID < InLights.size() && InNumInstances <= InLights.size() - InStartInstanceID );

	std::vector<TLightInstanceBuffer<LT_Spot>>		instanceBuffers;
	instanceBuffers.resize( InNumInstances );

	uint32		index = 0;
	for ( auto it = std::next( InLights.begin(), InStartInstanceID ), itEnd = InLights.end(); it != itEnd && index < InNumInstances; ++it, ++index )
	{
		TLightInstanceBuffer<LT_Spot>&			instanceBuffer		= instanceBuffers[index];
		TRefCountPtr<CSpotLightComponent>		spotLightComponent	= *it;
		instanceBuffer.instanceLocalToWorld							= spotLightComponent->GetComponentTransform().ToMatrix();
		instanceBuffer.color										= spotLightComponent->GetColor();
	}

	GRHI->SetupInstancing( InDeviceContextRHI, SSS_Instance, instanceBuffers.data(), sizeof( TLightInstanceBuffer<LT_Spot> ), InNumInstances * sizeof( TLightInstanceBuffer<LT_Spot> ), InNumInstances );
}

void CLightVertexFactory::SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CDirectionalLightComponent>>& InLights, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	check( lightType == LT_Directional );
	check( InStartInstanceID < InLights.size() && InNumInstances <= InLights.size() - InStartInstanceID );

	std::vector<TLightInstanceBuffer<LT_Directional>>		instanceBuffers;
	instanceBuffers.resize( InNumInstances );

	uint32		index = 0;
	for ( auto it = std::next( InLights.begin(), InStartInstanceID ), itEnd = InLights.end(); it != itEnd && index < InNumInstances; ++it, ++index )
	{
		TLightInstanceBuffer<LT_Directional>&			instanceBuffer				= instanceBuffers[index];
		TRefCountPtr<CDirectionalLightComponent>		directionalLightComponent	= *it;
		instanceBuffer.color														= directionalLightComponent->GetColor();
	}

	GRHI->SetupInstancing( InDeviceContextRHI, SSS_Instance, instanceBuffers.data(), sizeof( TLightInstanceBuffer<LT_Directional> ), InNumInstances * sizeof( TLightInstanceBuffer<LT_Directional> ), InNumInstances );
}

uint64 CLightVertexFactory::GetTypeHash() const
{
	return staticType.GetHash();
}

CVertexFactoryShaderParameters* CLightVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
	return InShaderFrequency == SF_Vertex ? new CLightVertexShaderParameters() : nullptr;
}