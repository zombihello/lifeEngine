/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CONCMD_H
#define CONCMD_H

#include <string>
#include <vector>

#include "System/Delegate.h"

/**
 * @ingroup Engine
 * @brief Console command
 */
class CConCmd
{
public:
	/**
	 * @brief Declare delegate of execute command
	 */
	DECLARE_DELEGATE( COnExecCmd, const std::vector<std::wstring>& /*InArgs*/ );

	/**
	 * @brief Constructor
	 * 
	 * @param InName			Name command
	 * @param InHelpText		Help text
	 * @param InExecDelegate	Execute delegate
	 */
	CConCmd( const std::wstring& InName, const std::wstring& InHelpText, const COnExecCmd::DelegateType_t& InExecDelegate );

	/**
	 * @brief Destructor
	 */
	~CConCmd();

	/**
	 * @brief Execute command
	 * @param InArgs	Array of arguments
	 */
	FORCEINLINE void Exec( const std::vector<std::wstring>& InArgs )
	{
		onExecCmd.Execute( InArgs );
	}

	/**
	 * @brief Get command name
	 * @return Return command name
	 */
	FORCEINLINE const std::wstring& GetName() const
	{
		return name;
	}

	/**
	 * @brief Get help text about this command
	 * @return Return help text about this command
	 */
	FORCEINLINE const std::wstring& GetHelpText() const
	{
		return helpText;
	}

private:
	std::wstring	name;			/**< Name */
	std::wstring	helpText;		/**< Help text */
	COnExecCmd		onExecCmd;		/**< Execute command delegate */
};

#endif // !CONCMD_H