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

#include "Core/Logger/LoggerMacros.h"
#include "Core/Containers/StringConv.h"
#include "Core/Containers/String.h"
#include "Core/Misc/CoreGlobals.h"
#include "Core/System/Archive.h"
#include "Core/System/BaseFileSystem.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/Render/Texture.h"
#include "Engine/Render/RenderUtils.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseSurfaceRHI.h"

#if WITH_EDITOR
#include "Core/System/Application.h"
#include "Editor/System/GenerateMipmaps.h"
#endif // WITH_EDITOR

/*
==================
CTexture2D::CTexture2D
==================
*/
CTexture2D::CTexture2D()
	: CAsset( AT_Texture2D )
	, pixelFormat( PF_Unknown )
	, addressU( SAM_Wrap )
	, addressV( SAM_Wrap )
	, samplerFilter( SF_Point )
{}

/*
==================
CTexture2D::~CTexture2D
==================
*/
CTexture2D::~CTexture2D()
{}

/*
==================
CTexture2D::InitRHI
==================
*/
void CTexture2D::InitRHI()
{
	Assert( mipmaps.size() > 0 );
	texture = g_RHI->CreateTexture2D( CString::Format( TEXT( "%s" ), GetAssetName().c_str() ).c_str(), GetSizeX(), GetSizeY(), pixelFormat, mipmaps.size(), 0, nullptr );

	// Load all mip-levels to GPU
	for ( uint32 index = 0, count = mipmaps.size(); index < count; ++index )
	{
		const Texture2DMipMap&		mipmap				= mipmaps[index];
		CBaseDeviceContextRHI*		deviceContextRHI	= g_RHI->GetImmediateContext();
		LockedData					lockedData;
		
		g_RHI->LockTexture2D( deviceContextRHI, texture, index, true, lockedData );
		Memory::Memcpy( lockedData.data, mipmap.data.GetData(), mipmap.data.Num() );
		g_RHI->UnlockTexture2D( deviceContextRHI, texture, index, lockedData );
	}

#if WITH_EDITOR
	if ( CApplication::Get().GetType() != AT_Editor && CApplication::Get().GetType() != AT_Tool )
#endif // WITH_EDITOR
	{
		for ( uint32 index = 0, count = mipmaps.size(); index < count; ++index )
		{
			mipmaps[index].data.RemoveAllElements();
		}
	}
}

/*
==================
CTexture2D::ReleaseRHI
==================
*/
void CTexture2D::ReleaseRHI()
{
	texture.SafeRelease();
}

/*
==================
CTexture2D::SetData
==================
*/
void CTexture2D::SetData( EPixelFormat InPixelFormat, uint32 InSizeX, uint32 InSizeY, const std::vector<byte>& InData, bool InIsGenerateMipmaps /* = false */ )
{
	Texture2DMipMap	mipmap0;
	pixelFormat			= InPixelFormat;
	mipmap0.sizeX		= InSizeX;
	mipmap0.sizeY		= InSizeY;
	mipmap0.data		= InData;

	// Clear mipmaps
	mipmaps.clear();

#if WITH_EDITOR
	if ( InIsGenerateMipmaps )
	{
		Sys_GenerateMipmapsMemory( InPixelFormat, mipmap0, mipmaps );
	}
	else
#endif // WITH_EDITOR
	{		
		mipmaps.push_back( mipmap0 );
	}

	MarkDirty();
	BeginUpdateResource( this );
}

#if WITH_EDITOR
/*
==================
CTexture2D::GenerateMipmaps
==================
*/
void CTexture2D::GenerateMipmaps()
{
	Assert( !mipmaps.empty() );
	Texture2DMipMap	mipmap0 = mipmaps[0];
	mipmaps.clear();
	
	Sys_GenerateMipmapsMemory( pixelFormat, mipmap0, mipmaps );

	MarkDirty();
	BeginUpdateResource( this );
}

/*
==================
CTexture2D::GetMemorySize
==================
*/
uint64 CTexture2D::GetMemorySize() const
{
	uint64		totalSize = 0;
	for ( uint32 index = 0, count = mipmaps.size(); index < count; ++index )
	{
		totalSize += mipmaps[index].data.Num();
	}
	return totalSize;
}
#endif // WITH_EDITOR

/*
==================
CTexture2D::Serialize
==================
*/
void CTexture2D::Serialize( class CArchive& InArchive )
{
	CAsset::Serialize( InArchive );

	// Clear all mipmaps before loading
	if ( InArchive.IsLoading() )
	{
		mipmaps.clear();
	}

	if ( InArchive.Ver() < VER_Mipmaps )
	{
		Texture2DMipMap	mipmap0;
		InArchive << mipmap0.data;
		InArchive << mipmap0.sizeX;
		InArchive << mipmap0.sizeY;
		mipmaps.push_back( mipmap0 );

		std::wstring		referenceToThisAsset;
		MakeReferenceToAsset( GetAssetHandle(), referenceToThisAsset );
		Warnf( TEXT( "%s :: Deprecated package version, in future must be removed supports\n" ), referenceToThisAsset.c_str() );
	}
	else
	{
		InArchive << mipmaps;
	}

	InArchive << pixelFormat;
	InArchive << addressU;
	InArchive << addressV;
	InArchive << samplerFilter;

	// If we loading Texture2D - update render resource
	if ( InArchive.IsLoading() )
	{
		BeginUpdateResource( this );
	}
}