/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EDITORCONSTRAINTS_H
#define EDITORCONSTRAINTS_H

#include "Core.h"

/**
 * @ingroup WorldEd
 * @brief Editor constraints
 */
class CEditorConstraints
{
public:
	/**
	 * @brief Enumeration of grid of snap translation
	 */
	enum ESnapTranslation
	{
		ST_1,					/**< Grid 1 */
		ST_5,					/**< Grid 5 */
		ST_10,					/**< Grid 10 */
		ST_50,					/**< Grid 50 */
		ST_100,					/**< Grid 100 */
		ST_500,					/**< Grid 500 */
		ST_1000,				/**< Grid 1000 */
		ST_5000,				/**< Grid 5000 */
		ST_10000,				/**< Grid 10000 */
		ST_Num					/**< Number of snapping grid translation */
	};

	/**
	 * @brief Enumeration of grid snap rotation
	 */
	enum ESnapRotation
	{
		SR_5,					/**< Grid 5 degrees */
		SR_10,					/**< Grid 10 degrees */
		SR_15,					/**< Grid 15 degrees */
		SR_30,					/**< Grid 30 degrees */
		SR_45,					/**< Grid 45 degrees */
		SR_60,					/**< Grid 60 degrees */
		SR_90,					/**< Grid 90 degrees */
		SR_120,					/**< Grid 120 degrees */
		SR_Num					/**< Number of snapping grid rotation */
	};

	/**
	 * @brief Enumeration of grid snap scaling
	 */
	enum ESnapScale
	{
		SS_10,					/**< Grid 10 */
		SS_1,					/**< Grid 1 */
		SS_0_5,					/**< Grid 0.5 */
		SS_0_25,				/**< Grid 0.25 */
		SS_0_125,				/**< Grid 0.125 */
		SS_0_0625,				/**< Grid 0.0625 */
		SS_0_03125,				/**< Grid 0.03125 */
		SS_Num					/**< Number of snapping grid scaling */
	};

	/**
	 * @brief Constructor
	 */
	CEditorConstraints();

	/**
	 * @brief Convert enum ESnapTranslation to text
	 * 
	 * @param InGrid	Grid
	 * @return Return converted enum to text
	 */
	FORCEINLINE static const tchar* ConvSnapTranslationToText( ESnapTranslation InGrid )
	{
		switch ( InGrid )
		{
		case ST_1:		return TEXT( "1" );
		case ST_5:		return TEXT( "5" );
		case ST_10:		return TEXT( "10" );
		case ST_50:		return TEXT( "50" );
		case ST_100:	return TEXT( "100" );
		case ST_500:	return TEXT( "500" );
		case ST_1000:	return TEXT( "1000" );
		case ST_5000:	return TEXT( "5000" );
		case ST_10000:	return TEXT( "10000" );
		default:
			Assert( false );
			return TEXT( "NONE" );
		}
	}

	/**
	 * @brief Convert enum ESnapRotation to text
	 *
	 * @param InGrid	Grid
	 * @return Return converted enum to text
	 */
	FORCEINLINE static const tchar* ConvSnapRotationToText( ESnapRotation InGrid )
	{
		switch ( InGrid )
		{
		case SR_5:		return TEXT( "5" );
		case SR_10:		return TEXT( "10" );
		case SR_15:		return TEXT( "15" );
		case SR_30:		return TEXT( "30" );
		case SR_45:		return TEXT( "45" );
		case SR_60:		return TEXT( "60" );
		case SR_90:		return TEXT( "90" );
		case SR_120:	return TEXT( "120" );
		default:
			Assert( false );
			return TEXT( "NONE" );
		}
	}

	/**
	 * @brief Convert enum ESnapScale to text
	 *
	 * @param InGrid	Grid
	 * @return Return converted enum to text
	 */
	FORCEINLINE static const tchar* ConvSnapScaleToText( ESnapScale InGrid )
	{
		switch ( InGrid )
		{
		case SS_10:			return TEXT( "10" );
		case SS_1:			return TEXT( "1" );
		case SS_0_5:		return TEXT( "0.5" );
		case SS_0_25:		return TEXT( "0.25" );
		case SS_0_125:		return TEXT( "0.125" );
		case SS_0_0625:		return TEXT( "0.0625" );
		case SS_0_03125:	return TEXT( "0.03125" );
		default:
			Assert( false );
			return TEXT( "NONE" );
		}
	}

	/**
	 * @brief Set use snap translation
	 * @param InEnable	Is need use snap translation
	 */
	FORCEINLINE void UseSnapTranslation( bool InEnable )
	{
		bEnabledSnapTranslation = InEnable;
	}

	/**
	 * @brief Set use snap rotation
	 * @param InEnable	Is need use snap rotation
	 */
	FORCEINLINE void UseSnapRotation( bool InEnable )
	{
		bEnabledSnapRotation = InEnable;
	}

	/**
	 * @brief Set use snap scale
	 * @param InEnable	Is need use snap scale
	 */
	FORCEINLINE void UseSnapScale( bool InEnable )
	{
		bEnabledSnapScale = InEnable;
	}

	/**
	 * @brief Set current grid translation size
	 * @param InNewGrid		New grid size
	 */
	FORCEINLINE void SetGridTranslationSize( ESnapTranslation InNewGrid )
	{
		Assert( InNewGrid != ST_Num );
		currentIdGridTranslation = InNewGrid;
	}

	/**
	 * @brief Set current grid rotation size
	 * @param InNewGrid		New grid size
	 */
	FORCEINLINE void SetGridRotationSize( ESnapRotation InNewGrid )
	{
		Assert( InNewGrid != SR_Num );
		currentIdGridRotation = InNewGrid;
	}

	/**
	 * @brief Set current grid scaling size
	 * @param InNewGrid		New grid size
	 */
	FORCEINLINE void SetGridScalingSize( ESnapScale InNewGrid )
	{
		Assert( InNewGrid != SS_Num );
		currentIdGridScaling = InNewGrid;
	}

	/**
	 * @brief Get current grid translation size
	 * @return Return current grid translation size
	 */
	FORCEINLINE float GetGridTranslationSize() const
	{
		return gridTranslationSizes[currentIdGridTranslation];
	}

	/**
	 * @brief Get current grid rotation size
	 * @return Return current grid rotation size
	 */
	FORCEINLINE float GetGridRotationSize() const
	{
		return gridRotationSizes[currentIdGridRotation];
	}

	/**
	 * @brief Get current grid scaling size
	 * @return Return current grid scaling size
	 */
	FORCEINLINE float GetGridScalingSize() const
	{
		return gridScalingSizes[currentIdGridScaling];
	}

	/**
	 * @brief Get current ID of grid translation
	 * @return Return current ID of grid translation
	 */
	FORCEINLINE ESnapTranslation GetGridTranslationID() const
	{
		return ( ESnapTranslation )currentIdGridTranslation;
	}

	/**
	 * @brief Get current ID of grid rotation
	 * @return Return current ID of grid rotation
	 */
	FORCEINLINE ESnapRotation GetGridRotationID() const
	{
		return ( ESnapRotation )currentIdGridRotation;
	}

	/**
	 * @brief Get current ID of grid
	 * @return Return current ID of grid
	 */
	FORCEINLINE ESnapScale GetGridScalingID() const
	{
		return ( ESnapScale )currentIdGridScaling;
	}

	/**
	 * @brief Is use snap translation
	 * @return True if snap translation is using
	 */
	FORCEINLINE bool IsUseSnapTranslation() const
	{
		return bEnabledSnapTranslation;
	}

	/**
	 * @brief Is use snap rotation
	 * @return True if snap rotation is using
	 */
	FORCEINLINE bool IsUseSnapRotation() const
	{
		return bEnabledSnapRotation;
	}

	/**
	 * @brief Is use snap scale
	 * @return True if snap scale is using
	 */
	FORCEINLINE bool IsUseSnapScale() const
	{
		return bEnabledSnapScale;
	}

private:
	bool				bEnabledSnapTranslation;				/**< Is enabled snap translation */
	bool				bEnabledSnapRotation;					/**< Is enabled snap rotation */
	bool				bEnabledSnapScale;						/**< Is enabled snap scale */
	uint32				currentIdGridTranslation;				/**< Current ID of translation grid size */
	uint32				currentIdGridRotation;					/**< Current ID of rotation grid size */
	uint32				currentIdGridScaling;					/**< Current ID of scaling grid size */
	static float		gridTranslationSizes[ST_Num];			/**< Array of grid translation sizes */
	static float		gridRotationSizes[SR_Num];				/**< Array of grid rotation sizes */
	static float		gridScalingSizes[SS_Num];				/**< Array of grid scaling sizes */
};

#endif // !EDITORCONSTRAINTS_H