#include "Render/BoundShaderStateCache.h"
#include "Hashing/FastHash.h"
#include "RHI/BaseShaderRHI.h"

/*
==================
CBoundShaderStateKey::CBoundShaderStateKey
==================
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
	Assert( vertexDeclaration );

	hash = vertexDeclaration->GetHash( hash );
	hash = FastHash( vertexShader, hash );
	hash = FastHash( pixelShader, hash );
	hash = FastHash( hullShader, hash );
	hash = FastHash( domainShader, hash );
	hash = FastHash( geometryShader, hash );
}