#include "System/Viewports.h"
#include "Core.h"

FViewportConfig_Viewport::FViewportConfig_Viewport()
	: bEnabled( false )
	, bSetListenerPosition( false )
	, viewportType( LVT_Perspective )
{}

FViewportConfig_Viewport::~FViewportConfig_Viewport()
{}

FViewportConfig_Template::FViewportConfig_Template()
{}

FViewportConfig_Template::~FViewportConfig_Template()
{}

void FViewportConfig_Template::Set( EViewportConfig InViewportConfig )
{
	viewportConfig = InViewportConfig;
	switch ( viewportConfig )
	{
	case VC_2_2_Split:
		description = TEXT( "2x2 split" );

		viewportTemplates[ 0 ].bEnabled = true;
		viewportTemplates[ 0 ].viewportType = LVT_OrthoXY;

		viewportTemplates[ 1 ].bEnabled = true;
		viewportTemplates[ 1 ].viewportType = LVT_OrthoXZ;

		viewportTemplates[ 2 ].bEnabled = true;
		viewportTemplates[ 2 ].viewportType = LVT_OrthoYZ;

		viewportTemplates[ 3 ].bEnabled = true;
		viewportTemplates[ 3 ].bSetListenerPosition = true;
		viewportTemplates[ 3 ].viewportType = LVT_Perspective;
		break;

	case VC_1_2_Split:
		description = TEXT( "1x2 split" );

		viewportTemplates[ 0 ].bEnabled = true;
		viewportTemplates[ 0 ].bSetListenerPosition = true;
		viewportTemplates[ 0 ].viewportType = LVT_Perspective;

		viewportTemplates[ 1 ].bEnabled = true;
		viewportTemplates[ 1 ].viewportType = LVT_OrthoXY;

		viewportTemplates[ 2 ].bEnabled = true;
		viewportTemplates[ 2 ].viewportType = LVT_OrthoXZ;
		break;

	case VC_1_1_SplitH:
		description = TEXT( "1x1 split horizontal" );

		viewportTemplates[ 0 ].bEnabled = true;
		viewportTemplates[ 0 ].bSetListenerPosition = true;
		viewportTemplates[ 0 ].viewportType = LVT_Perspective;

		viewportTemplates[ 1 ].bEnabled = true;
		viewportTemplates[ 1 ].viewportType = LVT_OrthoXY;
		break;

	case VC_1_1_SplitV:
		description = TEXT( "1x1 split vertical" );

		viewportTemplates[ 0 ].bEnabled = true;
		viewportTemplates[ 0 ].bSetListenerPosition = true;
		viewportTemplates[ 0 ].viewportType = LVT_Perspective;

		viewportTemplates[ 1 ].bEnabled = true;
		viewportTemplates[ 1 ].viewportType = LVT_OrthoXY;
		break;

	default:
		check( false );			// Unknown viewport config
		break;
	}
}