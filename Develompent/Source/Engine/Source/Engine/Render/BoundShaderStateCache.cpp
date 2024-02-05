/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Engine/Render/BoundShaderStateCache.h"
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
	hash = Sys_FastHash( vertexShader, hash );
	hash = Sys_FastHash( pixelShader, hash );
	hash = Sys_FastHash( hullShader, hash );
	hash = Sys_FastHash( domainShader, hash );
	hash = Sys_FastHash( geometryShader, hash );
}