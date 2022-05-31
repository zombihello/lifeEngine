#include "Render/BoundShaderStateCache.h"
#include "RHI/BaseShaderRHI.h"

/**
 * Constructor
 */
CBoundShaderStateKey::CBoundShaderStateKey( VertexDeclarationRHIParamRef_t InVertexDeclaration, VertexShaderRHIParamRef_t InVertexShader, PixelShaderRHIParamRef_t InPixelShader, HullShaderRHIParamRef_t InHullShader /*= nullptr*/, DomainShaderRHIParamRef_t InDomainShader /*= nullptr*/, GeometryShaderRHIParamRef_t InGeometryShader /*= nullptr*/ ) :
	hash( 94875494 ),
	vertexDeclaration( InVertexDeclaration ),
	vertexShader( InVertexShader ),
	pixelShader( InPixelShader ),
	hullShader( InHullShader ),
	domainShader( InDomainShader ),
	geometryShader( InGeometryShader )
{
	check( vertexDeclaration );

	hash = vertexDeclaration->GetHash( hash );
	hash = appMemFastHash( vertexShader, hash );
	hash = appMemFastHash( pixelShader, hash );
	hash = appMemFastHash( hullShader, hash );
	hash = appMemFastHash( domainShader, hash );
	hash = appMemFastHash( geometryShader, hash );
}