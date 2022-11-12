#include "Misc/Class.h"
#include "Commandlets/BaseCommandlet.h"
#include "Logger/LoggerMacros.h"

IMPLEMENT_CLASS( ÑBaseCommandlet )

bool ÑBaseCommandlet::ExecCommandlet( const CCommandLine& InCommandLine, bool* OutResultCommand /* = nullptr */ )
{
	std::wstring		nameCommandlet = InCommandLine.GetFirstValue( TEXT( "commandlet" ) );
	if ( nameCommandlet.empty() )
	{
		return false;
	}
	CClass*				lclassCommandlet = CClass::StaticFindClass( nameCommandlet.c_str() );

	// If class not found try to search by added 'C' in prefix and 'Commandlet' in sufix
	if ( !lclassCommandlet )
	{
		lclassCommandlet = CClass::StaticFindClass( ( std::wstring( TEXT( "C" ) ) + nameCommandlet + TEXT( "Commandlet" ) ).c_str() );
	}

	// Create and execute commandlet
	if ( lclassCommandlet )
	{
		ÑBaseCommandlet*		commandlet = lclassCommandlet->CreateObject< ÑBaseCommandlet >();
		check( commandlet );

		bool		oldIsCommandlet = GIsCommandlet;
		GIsCommandlet = true;
		LE_LOG( LT_Log, LC_Commandlet, TEXT( "Started commandlet '%s'" ), nameCommandlet.c_str() );
		double		beginCommandletTime = appSeconds();
		bool		result = commandlet->Main( InCommandLine );
		double		endCommandletTime = appSeconds();
		delete commandlet;

		ELogColor		logColor = result ? LC_Green : LC_Red;
		LE_LOG( LT_Log, LC_Commandlet, TEXT( "" ) );
		LE_LOG_COLOR( logColor, LT_Log, LC_Commandlet, TEXT( "----------------------" ) );
		LE_LOG_COLOR( logColor, LT_Log, LC_Commandlet, TEXT( "Result: %s" ), result ? TEXT( "seccussed" ) : TEXT( "error" ) );
		LE_LOG( LT_Log, LC_Commandlet, TEXT( "" ) );
		LE_LOG( LT_Log, LC_Commandlet, TEXT( "Execution of commandlet took: %f seconds" ), endCommandletTime - beginCommandletTime );

		GIsCommandlet = oldIsCommandlet;
		GIsRequestingExit = true;
		GShouldPauseBeforeExit = true;
		if ( OutResultCommand )
		{
			*OutResultCommand = result;
		}
		return true;
	}

	return false;
}
