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
TGlobalResource<CLightVertexDeclaration>			g_LightVertexDeclaration;

/**
 * @ingroup Engine
 * @brief Base struct of the light instance buffer
 */
struct SBaseLightInstanceBuffer
{
	CColor		lightColor;		/**< Light color */
	float		intensivity;	/**< Intensivity */
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
	Vector		position;					/**< Position */
	float		radius;						/**< Radius */
};

/**
 * @ingroup Engine
 * @brief Struct of instance buffer for spot light
 */
template<>
struct TLightInstanceBuffer<LT_Spot> : public SBaseLightInstanceBuffer
{
	Matrix		instanceLocalToWorld;		/**< Local to World matrix for each instance */
	Vector		position;					/**< Position */
	float		radius;						/**< Radius */
	float		height;						/**< Height */
	float		cutoff;						/**< Cutoff */
	Vector		direction;					/**< Direction */
};

/**
 * @ingroup Engine
 * @brief Struct of instance buffer for directional light
 */
template<>
struct TLightInstanceBuffer<LT_Directional> : public SBaseLightInstanceBuffer
{
	Vector		direction;					/**< Direction */
};


/*
==================
CLightVertexDeclaration::InitRHI
==================
*/
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
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Point> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Point>, lightColor ),					VET_Color,	VEU_Color,				0, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Point> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Point>, intensivity ),					VET_Float1,	VEU_BlendWeight,		0, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Point> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Point>, position ),						VET_Float3,	VEU_Position,			5, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Point> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Point>, radius ),						VET_Float1,	VEU_BlendWeight,		1, true ),
#endif // USE_INSTANCING
		};
		vertexDeclarationRHI[LT_Point] = g_RHI->CreateVertexDeclaration( vertexDeclElementList );
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
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Spot> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Spot>, lightColor ),						VET_Color,	VEU_Color,				0, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Spot> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Spot>, intensivity ),					VET_Float1,	VEU_BlendWeight,		0, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Spot> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Spot>, position ),						VET_Float3,	VEU_Position,			5, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Spot> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Spot>, radius ),							VET_Float1,	VEU_BlendWeight,		1, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Spot> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Spot>, height ),							VET_Float1,	VEU_BlendWeight,		2, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Spot> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Spot>, cutoff ),							VET_Float1,	VEU_BlendWeight,		3, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Spot> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Spot>, direction ),						VET_Float3,	VEU_Normal,				0, true ),
#endif // USE_INSTANCING
		};
		vertexDeclarationRHI[LT_Spot] = g_RHI->CreateVertexDeclaration( vertexDeclElementList );
	}

	// Init vertex declaration for directional light
	{
		VertexDeclarationElementList_t		vertexDeclElementList =
		{
			SVertexElement( CLightVertexFactory::SSS_Main,		sizeof( SLightVertexType ),						STRUCT_OFFSET( SLightVertexType, position ),									VET_Float4, VEU_Position,			0 ),

#if USE_INSTANCING
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Directional> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Directional>, lightColor ),				VET_Color,	VEU_Color,				0, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Directional> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Directional>, intensivity ),				VET_Float1,	VEU_BlendWeight,		0, true ),
			SVertexElement( CLightVertexFactory::SSS_Instance,	sizeof( TLightInstanceBuffer<LT_Directional> ),	STRUCT_OFFSET( TLightInstanceBuffer<LT_Directional>, direction ),				VET_Float3,	VEU_Normal,				0, true ),
#endif // USE_INSTANCING
		};
		vertexDeclarationRHI[LT_Directional] = g_RHI->CreateVertexDeclaration( vertexDeclElementList );
	}
}

/*
==================
CLightVertexDeclaration::ReleaseRHI
==================
*/
void CLightVertexDeclaration::ReleaseRHI()
{
	vertexDeclarationRHI[LT_Point].SafeRelease();
	vertexDeclarationRHI[LT_Spot].SafeRelease();
	vertexDeclarationRHI[LT_Directional].SafeRelease();
}


/*
==================
CLightVertexShaderParameters::CLightVertexShaderParameters
==================
*/
CLightVertexShaderParameters::CLightVertexShaderParameters()
	: CGeneralVertexShaderParameters( CLightVertexFactory::staticType.SupportsInstancing() )
{}

/*
==================
CLightVertexShaderParameters::SetMesh
==================
*/
void CLightVertexShaderParameters::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	Sys_Errorf( TEXT( "CLightVertexShaderParameters::SetMesh( MeshBatch ) Not supported" ) );
}

/*
==================
CLightVertexShaderParameters::SetMesh
==================
*/
void CLightVertexShaderParameters::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CPointLightComponent>>& InLights, const class CLightVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	if ( !bSupportsInstancing )
	{
		Sys_Errorf( TEXT( "Not implemented, need add it in future" ) );
	}
	else
	{
		Assert( InVertexFactory );
		InVertexFactory->SetupInstancing( InDeviceContextRHI, InLights, InView, InNumInstances, InStartInstanceID );
	}
}

/*
==================
CLightVertexShaderParameters::SetMesh
==================
*/
void CLightVertexShaderParameters::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CSpotLightComponent>>& InLights, const class CLightVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	if ( !bSupportsInstancing )
	{
		Sys_Errorf( TEXT( "Not implemented, need add it in future" ) );
	}
	else
	{
		Assert( InVertexFactory );
		InVertexFactory->SetupInstancing( InDeviceContextRHI, InLights, InView, InNumInstances, InStartInstanceID );
	}
}

/*
==================
CLightVertexShaderParameters::SetMesh
==================
*/
void CLightVertexShaderParameters::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CDirectionalLightComponent>>& InLights, const class CLightVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	if ( !bSupportsInstancing )
	{
		Sys_Errorf( TEXT( "Not implemented, need add it in future" ) );
	}
	else
	{
		Assert( InVertexFactory );
		InVertexFactory->SetupInstancing( InDeviceContextRHI, InLights, InView, InNumInstances, InStartInstanceID );
	}
}


/*
==================
CLightVertexFactory::CLightVertexFactory
==================
*/
CLightVertexFactory::CLightVertexFactory( ELightType InLightType )
	: lightType( InLightType )
{}

/*
==================
CLightVertexFactory::InitRHI
==================
*/
void CLightVertexFactory::InitRHI()
{
	InitDeclaration( g_LightVertexDeclaration.GetVertexDeclarationRHI( lightType ) );
}

/*
==================
CLightVertexFactory::SetupInstancing
==================
*/
void CLightVertexFactory::SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	Sys_Errorf( TEXT( "CLightVertexFactory::SetupInstancing( SMeshBatch ) :: Not supported" ) );
}

/*
==================
CLightVertexFactory::SetupInstancing
==================
*/
void CLightVertexFactory::SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CPointLightComponent>>& InLights, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	Assert( lightType == LT_Point );
	Assert( InStartInstanceID < InLights.size() && InNumInstances <= InLights.size() - InStartInstanceID );

	std::vector<TLightInstanceBuffer<LT_Point>>		instanceBuffers;
	instanceBuffers.resize( InNumInstances );
	
	uint32		index = 0;
	for ( auto it = std::next( InLights.begin(), InStartInstanceID ), itEnd = InLights.end(); it != itEnd && index < InNumInstances; ++it, ++index )
	{
		TLightInstanceBuffer<LT_Point>&		instanceBuffer		= instanceBuffers[index];
		TRefCountPtr<CPointLightComponent>	pointLightComponent = *it;
		instanceBuffer.instanceLocalToWorld						= pointLightComponent->GetComponentTransform().ToMatrix();
		instanceBuffer.lightColor								= pointLightComponent->GetLightColor();
		instanceBuffer.intensivity								= pointLightComponent->GetIntensivity();
		instanceBuffer.position									= pointLightComponent->GetComponentLocation();
		instanceBuffer.radius									= pointLightComponent->GetRadius();
	}

	g_RHI->SetupInstancing( InDeviceContextRHI, SSS_Instance, instanceBuffers.data(), sizeof( TLightInstanceBuffer<LT_Point> ), InNumInstances * sizeof( TLightInstanceBuffer<LT_Point> ), InNumInstances );
}

/*
==================
CLightVertexFactory::SetupInstancing
==================
*/
void CLightVertexFactory::SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CSpotLightComponent>>& InLights, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	Assert( lightType == LT_Spot );
	Assert( InStartInstanceID < InLights.size() && InNumInstances <= InLights.size() - InStartInstanceID );

	std::vector<TLightInstanceBuffer<LT_Spot>>		instanceBuffers;
	instanceBuffers.resize( InNumInstances );

	uint32		index = 0;
	for ( auto it = std::next( InLights.begin(), InStartInstanceID ), itEnd = InLights.end(); it != itEnd && index < InNumInstances; ++it, ++index )
	{
		TLightInstanceBuffer<LT_Spot>&			instanceBuffer		= instanceBuffers[index];
		TRefCountPtr<CSpotLightComponent>		spotLightComponent	= *it;
		CTransform								spotTransform		= spotLightComponent->GetComponentTransform();
		Vector									direction			= spotTransform.GetUnitAxis( A_Z );
		spotTransform.SetRotation( SMath::LookAtQuatenrion( spotTransform.GetLocation(), spotTransform.GetLocation() + direction, spotTransform.GetUnitAxis( A_Y ), SMath::vectorUp ) );

		instanceBuffer.instanceLocalToWorld							= spotTransform.ToMatrix();
		instanceBuffer.lightColor									= spotLightComponent->GetLightColor();
		instanceBuffer.intensivity									= spotLightComponent->GetIntensivity();
		instanceBuffer.position										= spotLightComponent->GetComponentLocation();
		instanceBuffer.radius										= spotLightComponent->GetRadius();
		instanceBuffer.height										= spotLightComponent->GetHeight();
		instanceBuffer.cutoff										= spotLightComponent->GetCutoff();
		instanceBuffer.direction									= direction;
	}

	g_RHI->SetupInstancing( InDeviceContextRHI, SSS_Instance, instanceBuffers.data(), sizeof( TLightInstanceBuffer<LT_Spot> ), InNumInstances * sizeof( TLightInstanceBuffer<LT_Spot> ), InNumInstances );
}

/*
==================
CLightVertexFactory::SetupInstancing
==================
*/
void CLightVertexFactory::SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CDirectionalLightComponent>>& InLights, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	Assert( lightType == LT_Directional );
	Assert( InStartInstanceID < InLights.size() && InNumInstances <= InLights.size() - InStartInstanceID );

	std::vector<TLightInstanceBuffer<LT_Directional>>		instanceBuffers;
	instanceBuffers.resize( InNumInstances );

	uint32		index = 0;
	for ( auto it = std::next( InLights.begin(), InStartInstanceID ), itEnd = InLights.end(); it != itEnd && index < InNumInstances; ++it, ++index )
	{
		TLightInstanceBuffer<LT_Directional>&			instanceBuffer				= instanceBuffers[index];
		TRefCountPtr<CDirectionalLightComponent>		directionalLightComponent	= *it;
		instanceBuffer.lightColor													= directionalLightComponent->GetLightColor();
		instanceBuffer.intensivity													= directionalLightComponent->GetIntensivity();
		instanceBuffer.direction													= -directionalLightComponent->GetComponentTransform().GetUnitAxis( A_Z );
	}

	g_RHI->SetupInstancing( InDeviceContextRHI, SSS_Instance, instanceBuffers.data(), sizeof( TLightInstanceBuffer<LT_Directional> ), InNumInstances * sizeof( TLightInstanceBuffer<LT_Directional> ), InNumInstances );
}

/*
==================
CLightVertexFactory::GetTypeHash
==================
*/
uint64 CLightVertexFactory::GetTypeHash() const
{
	return staticType.GetHash();
}

/*
==================
CLightVertexFactory::ConstructShaderParameters
==================
*/
CVertexFactoryShaderParameters* CLightVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
	return InShaderFrequency == SF_Vertex ? new CLightVertexShaderParameters() : nullptr;
}