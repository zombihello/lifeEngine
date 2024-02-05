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

#include <compressonator.h>
#include "Editor/System/GenerateMipmaps.h"

/*
==================
ConvertEPixelFormatToCmpFormat
==================
*/
static CMP_FORMAT ConvertEPixelFormatToCmpFormat( EPixelFormat InPixelFormat )
{
	switch ( InPixelFormat )
	{
	case PF_A8R8G8B8:			return CMP_FORMAT_ARGB_8888;
	case PF_FloatRGB:			return CMP_FORMAT_RGB_32F;
	case PF_FloatRGBA:			return CMP_FORMAT_RGBA_32F;
	case PF_R32F:				return CMP_FORMAT_R_32F;
	case PF_BC1:				return CMP_FORMAT_BC1;
	case PF_BC2:				return CMP_FORMAT_BC2;
	case PF_BC3:				return CMP_FORMAT_BC3;
	case PF_BC5:				return CMP_FORMAT_BC5;
	case PF_BC6H:				return CMP_FORMAT_BC6H;
	case PF_BC7:				return CMP_FORMAT_BC7;

	case PF_DepthStencil:
	case PF_ShadowDepth:
	case PF_FilteredShadowDepth:
	case PF_D32:
	default:
		Sys_Errorf( TEXT( "Unsupported EPixelFormat %i" ), ( uint32 )InPixelFormat );
		return CMP_FORMAT_Unknown;
	}
}

/*
==================
Sys_GenerateMipmapsMemory
==================
*/
void Sys_GenerateMipmapsMemory( EPixelFormat InPixelFormat, const Texture2DMipMap& InZeroMip, std::vector<Texture2DMipMap>& OutMipmaps, uint32 InRequestMips /*= 10*/ )
{
	CMP_MipSet cmp_MipSet;
	Sys_Memzero( &cmp_MipSet, sizeof( CMP_MipSet ) );
	CMP_ERROR	result = CMP_CreateMipSet( &cmp_MipSet, InZeroMip.sizeX, InZeroMip.sizeY, 1, CF_8bit, TT_2D );
	Assert( result == CMP_OK && ( *cmp_MipSet.m_pMipLevelTable )->m_dwLinearSize == InZeroMip.data.Num() );

	// Copy data to mipmap0
	memcpy( ( *cmp_MipSet.m_pMipLevelTable )->m_pbData, ( CMP_BYTE* )InZeroMip.data.GetData(), InZeroMip.data.Num() );

	// Generate mip levels and copy him to OutMipmaps
	CMP_GenerateMIPLevels( &cmp_MipSet, CMP_CalcMinMipSize( InZeroMip.sizeY, InZeroMip.sizeX, InRequestMips ) );
	for ( uint32 index = 0; index < cmp_MipSet.m_nMipLevels; index++ )
	{
		CMP_MipLevel*		cmp_mipLevel;
		CMP_GetMipLevel( &cmp_mipLevel, &cmp_MipSet, index, 0 );

		Texture2DMipMap	mipmap;
		mipmap.sizeX		= cmp_mipLevel->m_nWidth;
		mipmap.sizeY		= cmp_mipLevel->m_nWidth;
		mipmap.data.Resize( cmp_mipLevel->m_dwLinearSize );
		memcpy( mipmap.data.GetData(), cmp_mipLevel->m_pbData, cmp_mipLevel->m_dwLinearSize );
		OutMipmaps.push_back( mipmap );
	}

	CMP_FreeMipSet( &cmp_MipSet );
}