#include "Misc/ResourceIDs.h"
#include "Misc/WorldEdGlobals.h"
#include "System/WorldEdApp.h"
#include "System/EditorFrame.h"
#include "System/ViewportsContainer.h"

WxViewportsContainer::WxViewportsContainer( wxWindow* InParent, wxWindowID InID )
	: wxWindow( InParent, InID )
	, topSplitter( nullptr )
	, bottomSplitter( nullptr )
{}

void WxViewportsContainer::ConnectSplitterEvents( wxSplitterWindow* InTopSplitter, wxSplitterWindow* InBottomSplitter )
{
	topSplitter			= InTopSplitter;
	bottomSplitter		= InBottomSplitter;

	if ( topSplitter && bottomSplitter )
	{
		Connect( RID_SplitterWindow + 1, wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED, wxSplitterEventHandler( WxViewportsContainer::OnTopSplitterSashPositionChanged ) );
		Connect( RID_SplitterWindow + 2, wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED, wxSplitterEventHandler( WxViewportsContainer::OnBottomSplitterSashPositionChanged ) );
	}
	else
	{
		Disconnect( RID_SplitterWindow + 1 );
		Disconnect( RID_SplitterWindow + 2 );
	}
}

void WxViewportsContainer::DisconnectSplitterEvents()
{
	topSplitter = NULL;
	bottomSplitter = NULL;

	Disconnect( RID_SplitterWindow + 1 );
	Disconnect( RID_SplitterWindow + 2 );
}

void WxViewportsContainer::MatchSplitterPositions()
{
	if ( topSplitter && bottomSplitter )
	{
		bottomSplitter->SetSashPosition( topSplitter->GetSashPosition() );
	}
}

void WxViewportsContainer::OnTopSplitterSashPositionChanged( wxSplitterEvent& InSplitterEvent )
{
	if ( GApp->GetEditorFrame()->IsViewportsResizeTogether() )
	{
		int32		sashPos = InSplitterEvent.GetSashPosition();
		bottomSplitter->SetSashPosition( sashPos );
	}
}

void WxViewportsContainer::OnBottomSplitterSashPositionChanged( wxSplitterEvent& InSplitterEvent )
{
	if ( GApp->GetEditorFrame()->IsViewportsResizeTogether() )
	{
		int32		sashPos = InSplitterEvent.GetSashPosition();
		topSplitter->SetSashPosition( sashPos );
	}
}
