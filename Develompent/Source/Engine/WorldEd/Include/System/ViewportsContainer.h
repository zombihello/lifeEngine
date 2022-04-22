/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef VIEWPORTSCONTAINER_H
#define VIEWPORTSCONTAINER_H

#include <wx/wx.h>
#include <wx/splitter.h>

/**
 * @ingroup WorldEd 
 * @brief Container for numerous viewports and their splitters
 */
class WxViewportsContainer : public wxWindow
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InParent		Parent window
	 * @param InID			ID
	 */
	WxViewportsContainer( wxWindow* InParent, wxWindowID InID );

	/**
	 * @brief Connects splitter sash position changed events so we can move them together
	 * @param InTopSplitter		Top wxSplitterWindow pointer
	 * @param InBottomSplitter	Bottom wxSplitterWindow pointer
	 */
	void ConnectSplitterEvents( wxSplitterWindow* InTopSplitter, wxSplitterWindow* InBottomSplitter );

	/**
	 * @brief Disconnects previously connected splitter sash position events
	 */
	void DisconnectSplitterEvents();

	/**
	 * @brief Forces the top and bottom splitter positions to match by making the bottom sash position equal the top
	 */
	void MatchSplitterPositions();

private:
	/**
	 * @brief Used to link the top and bottom splitters together so they split together
	 * @param InSplitterEvent		Splitter event
	 */
	void OnTopSplitterSashPositionChanged( wxSplitterEvent& InSplitterEvent );

	/**
	 * @brief Used to link the top and bottom splitters together so they split together
	 * @param InSplitterEvent		Splitter event
	 */
	void OnBottomSplitterSashPositionChanged( wxSplitterEvent& InSplitterEvent );

	wxSplitterWindow*			topSplitter;		/**< Top splitter */
	wxSplitterWindow*			bottomSplitter;		/**< Bottom splitter */
};

#endif // !VIEWPORTSCONTAINER_H