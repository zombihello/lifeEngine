#include "System/EditorConstraints.h"

float	CEditorConstraints::gridTranslationSizes[CEditorConstraints::ST_Num] =
{
	1.f,		// ST_1
	5.f,		// ST_5
	10.f,		// ST_10
	50.f,		// ST_50
	100.f,		// ST_100
	500.f,		// ST_500
	1000.f,		// ST_1000
	5000.f,		// ST_5000
	10000.f,	// ST_10000
};

float	CEditorConstraints::gridRotationSizes[CEditorConstraints::SR_Num] =
{
	5.f,		// SR_5
	10.f,		// SR_10
	15.f,		// SR_15
	30.f,		// SR_30
	45.f,		// SR_45
	60.f,		// SR_60
	90.f,		// SR_90
	120.f,		// SR_120
};

float	CEditorConstraints::gridScalingSizes[CEditorConstraints::SS_Num] =
{
	10.f,		// SS_10
	1.f,		// SS_1
	0.5f,		// SS_0_5
	0.25f,		// SS_0_25
	0.125f,		// SS_0_125
	0.0625f,	// SS_0_0625
	0.03125f,	// SS_0_03125
};

/*
==================
CEditorConstraints::CEditorConstraints
==================
*/
CEditorConstraints::CEditorConstraints()
	: bEnabledSnapTranslation( true )
	, bEnabledSnapRotation( true )
	, bEnabledSnapScale( false )
	, currentIdGridTranslation( ST_1 )
	, currentIdGridRotation( SR_10 )
	, currentIdGridScaling( SS_0_25 )
{}