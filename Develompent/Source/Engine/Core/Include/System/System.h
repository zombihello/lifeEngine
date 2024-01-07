/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "Containers/String.h"
#include "Core.h"

/**
 * @ingroup Core
 * @brief Core system
 */
class CSystem
{
public:
	/**
	 * @brief Get singleton instance
	 * @return Return singleton instance
	 */
	static FORCEINLINE CSystem& Get()
	{
		static CSystem	system;
		return system;
	}

	/**
	 * @brief Initialize system
	 */
	void Init();

	/**
	 * @brief Shutdown system
	 */
	void Shutdown();

	/**
	 * @brief Is package extension
	 * 
	 * @param InExtension	Extension to check
	 * @return Return TRUE if this is package extension, otherwise returns FALSE
	 */
	FORCEINLINE bool IsPackageExtension( const std::wstring& InExtension ) const
	{
		std::wstring	extension = CString::ToLower( InExtension );
		for ( uint32 index = 0, count = packageExtensions.size(); index < count; ++index )
		{
			if ( packageExtensions[index] == extension )
			{
				return true;
			}
		}
		return false;
	}

	/**
	 * @brief Get list of package extensions
	 * @return Return list of package extensions
	 */
	FORCEINLINE const std::vector<std::wstring>& GetPackageExtensions() const
	{
		return packageExtensions;
	}

	/**
	 * @brief Get list of directories with packages
	 * @return Return list of directories with packages
	 */
	FORCEINLINE const std::vector<std::wstring>& GetPackagePaths() const
	{
		return packagePaths;
	}

private:
	std::vector<std::wstring>		packageExtensions;	/**< List of package extensions */
	std::vector<std::wstring>		packagePaths;		/**< List of directories with packages */
};

#endif // !SYSTEM_H