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

#include "pch_shadercompile.h"
#include "tools/shadercompile/shadercompile_environment.h"

/*
==================
CShaderCompileEnvironment::CShaderCompileEnvironment
==================
*/
CShaderCompileEnvironment::CShaderCompileEnvironment( EStudioShaderType type )
	: compileFlags( 0x0 )
{
	defines.insert( std::make_pair( "VERTEX_SHADER", type == STUDIO_SHADER_VERTEX ? "1" : "0" ) );
	defines.insert( std::make_pair( "DOMAIN_SHADER", type == STUDIO_SHADER_DOMAIN ? "1" : "0" ) );
	defines.insert( std::make_pair( "HULL_SHADER", type == STUDIO_SHADER_HULL ? "1" : "0" ) );
	defines.insert( std::make_pair( "GEOMETRY_SHADER", type == STUDIO_SHADER_GEOMETRY ? "1" : "0" ) );
	defines.insert( std::make_pair( "PIXEL_SHADER", type == STUDIO_SHADER_PIXEL ? "1" : "0" ) );
	defines.insert( std::make_pair( "COMPUTE_SHADER", type == STUDIO_SHADER_COMPUTE ? "1" : "0" ) );
}

/*
==================
CShaderCompileEnvironment::CShaderCompileEnvironment
==================
*/
CShaderCompileEnvironment::CShaderCompileEnvironment( const CShaderCompileEnvironment& copy )
	: vertexFactoryFileName( copy.vertexFactoryFileName )
	, includeDirs( copy.includeDirs )
	, defines( copy.defines )
	, compileFlags( copy.compileFlags )
{}

/*
==================
CShaderCompileEnvironment::GetVertexFactoryFileName
==================
*/
const achar* CShaderCompileEnvironment::GetVertexFactoryFileName() const
{
	return vertexFactoryFileName.c_str();
}

/*
==================
CShaderCompileEnvironment::GetCompileFlags
==================
*/
uint32 CShaderCompileEnvironment::GetCompileFlags() const
{
	return compileFlags;
}

/*
==================
CShaderCompileEnvironment::GetNumIncludeDirs
==================
*/
uint32 CShaderCompileEnvironment::GetNumIncludeDirs() const
{
	return ( uint32 )includeDirs.size();
}

/*
==================
CShaderCompileEnvironment::GetNumDefines
==================
*/
uint32 CShaderCompileEnvironment::GetNumDefines() const
{
	return ( uint32 )defines.size();
}

/*
==================
CShaderCompileEnvironment::GetIncludeDir
==================
*/
const achar* CShaderCompileEnvironment::GetIncludeDir( uint32 index ) const
{
	Assert( index >= 0 && index < ( uint32 )includeDirs.size() );
	return includeDirs[index].c_str();
}

/*
==================
CShaderCompileEnvironment::GetDefine
==================
*/
void CShaderCompileEnvironment::GetDefine( uint32 index, const achar*& pName, const achar*& pDefine ) const
{
	Assert( index >= 0 && index < ( uint32 )defines.size() );

	uint32 elementID = 0;
	for ( auto it = defines.begin(), itEnd = defines.end(); it != itEnd; ++it, ++elementID )
	{
		if ( elementID == index )
		{
			pName	= it->first.c_str();
			pDefine = it->second.c_str();
			return;
		}
	}

	pName	= "";
	pDefine = "";
}