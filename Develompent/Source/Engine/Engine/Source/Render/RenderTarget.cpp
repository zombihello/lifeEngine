#include "Misc/EngineGlobals.h"
#include "Render/RenderTarget.h"
#include "RHI/BaseRHI.h"

CRenderTarget::CRenderTarget()
	: flags( 0 )
	, sizeX( 0 )
	, sizeY( 0 )
	, pixelFormat( PF_Unknown )
{}

void CRenderTarget::InitRHI()
{
#if !SHIPPING_BUILD
	const tchar* cDebugName = debugName.c_str();
#else
	const tchar* cDebugName = TEXT( "" );
#endif // !SHIPPING_BUILD

	check( sizeX > 0 && sizeY > 0 && pixelFormat != PF_Unknown );
	texture		= GRHI->CreateTexture2D( cDebugName, sizeX, sizeY, pixelFormat, 1, flags );
	surface		= GRHI->CreateTargetableSurface( cDebugName, sizeX, sizeY, pixelFormat, texture, flags );
}

void CRenderTarget::UpdateRHI()
{
	InitRHI();
}

void CRenderTarget::ReleaseRHI()
{
	surface.SafeRelease();
	texture.SafeRelease();
}

void CRenderTarget::Update( bool InIsDestroyed, uint32 InNewSizeX, uint32 InNewSizeY, EPixelFormat InPixelFormat, uint32 InFlags /* = 0 */, const std::wstring& InName /* = TEXT( "" ) */ )
{
	bool			bNeedUpdateRHI	= pixelFormat != InPixelFormat || sizeX != InNewSizeX || sizeY != InNewSizeY || flags != (TCF_ResolveTargetable | InFlags);

	// Do nothing if attributes is equal and don't need destroy him
	if ( !bNeedUpdateRHI && !InIsDestroyed )
	{
		return;
	}

	// Update the render target attributes
	flags			= TCF_ResolveTargetable | InFlags;
	pixelFormat		= InPixelFormat;
	sizeX			= InNewSizeX;
	sizeY			= InNewSizeY;
#if !SHIPPING_BUILD
	if ( !InName.empty() )
	{
		debugName = InName;
	}
#endif // !SHIPPING_BUILD

	// Don't reinitialize the render target RHI if him has been destroyed
	if ( InIsDestroyed )
	{
		BeginReleaseResource( this );
		if ( InIsDestroyed )
		{
			return;
		}
	}

	// Initialize the render resource if need
	if ( bNeedUpdateRHI )
	{
		BeginUpdateResource( this );
	}
}