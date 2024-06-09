/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "Core.h"
#include "Misc/JsonDocument.h"

/**
 * @ingroup Core
 * @brief Enumeration config type
 */
enum EConfigType
{
	CT_Engine,		/**< Engine config */
	CT_Game,		/**< Game config */
	CT_Input,		/**< Input config */
	CT_Editor,		/**< Editor config */
	CT_Num			/**< Number configs type */
};

/**
 * @ingroup Core
 * @brief Enumeration layer of the config
 */
enum EConfigLayer
{
	CL_Engine,		/**< Engine */
	CL_Game,		/**< Game */
	CL_User,		/**< User */
	CL_Num			/**< Number layers */
};

/**
 * @ingroup Core
 * @brief Config
 */
class CConfig
{
public:
	/**
	 * @brief Get singleton instance
	 * @return Return singleton instance
	 */
	static FORCEINLINE CConfig& Get()
	{
		static CConfig	config;
		return config;
	}

	/**
	 * @brief Initialize configs
	 */
	void Init();

	/**
	 * @brief Shutdown configs
	 */
	FORCEINLINE void Shutdown()
	{
		configs.clear();
	}

	/**
	 * @brief Get config
	 *
	 * @param InType	Config type
	 * @return Return config object
	 */
	FORCEINLINE const CJsonDocument& GetConfig( EConfigType InType ) const
	{
		std::unordered_map<EConfigType, CJsonDocument>::const_iterator		itConfig = configs.find( InType );
		if ( itConfig == configs.end() )
		{
			AssertMsg( false, TEXT( "All config types must be exist all time" ) );
		}

		return itConfig->second;
	}

	/**
	 * @brief Get config
	 *
	 * @param InType	Config type
	 * @return Return config object
	 */
	FORCEINLINE CJsonDocument& GetConfig( EConfigType InType )
	{
		std::unordered_map<EConfigType, CJsonDocument>::iterator		itConfig = configs.find( InType );
		if ( itConfig == configs.end() )
		{
			AssertMsg( false, TEXT( "All config types must be exist all time" ) );
		}

		return itConfig->second;
	}

	/**
	 * @brief Set value
	 *
	 * @param InType	Config type
	 * @param InGroup	Name of group in config
	 * @param InName	Name of value in config group
	 * @param InValue	Value
	 */
	void SetValue( EConfigType InType, const tchar* InGroup, const tchar* InName, const CJsonValue& InValue );

	/**
	 * @brief Get value
	 *
	 * @param InType	Config type
	 * @param InGroup	Name of group in config
	 * @param InName	Name of value in config group
	 * @return Return value from config, if not found returns NULL
	 */
	const CJsonValue* GetValue( EConfigType InType, const tchar* InGroup, const tchar* InName ) const;

private:
	std::unordered_map<EConfigType, CJsonDocument>		configs;		/**< Configs */
};

#endif // !CONFIG_H
