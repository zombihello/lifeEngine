#include "Render/BoundShaderStateCache.h"

/**
 * Constructor
 */
FBoundShaderStateKey::FBoundShaderStateKey( FVertexDeclarationRHIParamRef InVertexDeclaration, FVertexShaderRHIParamRef InVertexShader, FPixelShaderRHIParamRef InPixelShader, FHullShaderRHIParamRef InHullShader /*= nullptr*/, FDomainShaderRHIParamRef InDomainShader /*= nullptr*/, FGeometryShaderRHIParamRef InGeometryShader /*= nullptr*/ ) :
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