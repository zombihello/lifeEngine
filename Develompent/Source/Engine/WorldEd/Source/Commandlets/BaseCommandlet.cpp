#include "Misc/Class.h"
#include "Commandlets/BaseCommandlet.h"
#include "Logger/LoggerMacros.h"

IMPLEMENT_CLASS( LBaseCommandlet )

bool LBaseCommandlet::ExecCommandlet( const std::wstring& InCommands, uint32 InBaseCommandIndex /*= 0*/, bool* OutResultCommand /*= nullptr*/ )
{
	std::vector< std::wstring >		tokens;
	std::vector< std::wstring >		switches;
	appParseCommandLine( InCommands.c_str(), tokens, switches );

	// If tokens more InBaseCommandIndex we creating commandlet and execute it
	if ( InBaseCommandIndex >= tokens.size() )
	{
		return false;
	}

	const std::wstring&			nameCommandlet = tokens[ InBaseCommandIndex ];
	LClass*						lclassCommandlet = LClass::StaticFindClass( nameCommandlet.c_str() );

	// If class not found try to search by added 'L' in prefix and 'Commandlet' in sufix
	if ( !lclassCommandlet )
	{
		lclassCommandlet = LClass::StaticFindClass( ( std::wstring( TEXT( "L" ) ) + nameCommandlet + TEXT( "Commandlet" ) ).c_str() );
	}

	// Create and execute commandlet
	if ( lclassCommandlet )
	{
		LBaseCommandlet*		commandlet = lclassCommandlet->CreateObject< LBaseCommandlet >();
		check( commandlet );

		bool		oldIsCommandlet = GIsCommandlet;
		GIsCommandlet = true;
		LE_LOG( LT_Log, LC_Commandlet, TEXT( "Started commandlet '%s'" ), nameCommandlet.c_str() );
		double		beginCommandletTime = appSeconds();
		bool		result = commandlet->Main( InCommands );
		double		endCommandletTime = appSeconds();
		delete commandlet;

		LE_LOG_COLOR( result ? LC_Green : LC_Red, LT_Log, LC_Commandlet, TEXT( "Result: %s" ), result ? TEXT( "seccussed" ) : TEXT( "error" ) );
		LE_LOG( LT_Log, LC_Commandlet, TEXT( "Commandlet took time %fs" ), endCommandletTime - beginCommandletTime );

		GIsCommandlet = oldIsCommandlet;
		GIsRequestingExit = true;
		if ( OutResultCommand )
		{
			*OutResultCommand = result;
		}
		return true;
	}

	return false;
}
