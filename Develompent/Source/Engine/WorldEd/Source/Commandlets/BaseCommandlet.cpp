#include "Reflection/ReflectionEnvironment.h"
#include "Reflection/ObjectGlobals.h"
#include "Commandlets/BaseCommandlet.h"
#include "Logger/LoggerMacros.h"

IMPLEMENT_CLASS( CBaseCommandlet )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CBaseCommandlet )

/*
==================
CBaseCommandlet::ExecCommandlet
==================
*/
bool CBaseCommandlet::ExecCommandlet( const CCommandLine& InCommandLine, bool* OutResultCommand /* = nullptr */ )
{
	std::wstring		nameCommandlet = InCommandLine.GetFirstValue( TEXT( "commandlet" ) );
	if ( nameCommandlet.empty() )
	{
		return false;
	}
	CClass*				lclassCommandlet = CReflectionEnvironment::Get().FindClass( nameCommandlet.c_str() );

	// If class not found try to search by added 'C' in prefix and 'Commandlet' in sufix
	if ( !lclassCommandlet )
	{
		lclassCommandlet = CReflectionEnvironment::Get().FindClass( ( std::wstring( TEXT( "C" ) ) + nameCommandlet + TEXT( "Commandlet" ) ).c_str() );
	}

	// Create and execute commandlet
	if ( lclassCommandlet )
	{
		CObjectPackage*			worldEdPackage = CreatePackage( nullptr, TEXT( "WorldEd" ) );
		CBaseCommandlet*		commandlet = lclassCommandlet->CreateObject<CBaseCommandlet>( worldEdPackage );
		Assert( commandlet );
		commandlet->AddToRoot();

		bool		oldIsCommandlet = g_IsCommandlet;
		g_IsCommandlet = true;
		Logf( TEXT( "\n" ) );
		Logf( TEXT( "Started commandlet '%s'\n" ), nameCommandlet.c_str() );
		double		beginCommandletTime = Sys_Seconds();
		bool		result = commandlet->Main( InCommandLine );
		double		endCommandletTime = Sys_Seconds();

		ELogColor		logColor = result ? LC_Green : LC_Red;
		g_Log->SetTextColor( logColor );
		Logf( TEXT( "\n" ) );
		Logf( TEXT( "----------------------\n" ) );
		Logf( TEXT( "Result: %s\n" ), result ? TEXT( "seccussed" ) : TEXT( "error" ) );
		Logf( TEXT( "Execution of commandlet took: %f seconds\n" ), endCommandletTime - beginCommandletTime );
		Logf( TEXT( "----------------------\n" ) );
		Logf( TEXT( "\n" ) );
		g_Log->ResetTextColor();

		g_IsCommandlet = oldIsCommandlet;
		g_IsRequestingExit = true;
		g_ShouldPauseBeforeExit = true;
		if ( OutResultCommand )
		{
			*OutResultCommand = result;
		}

		// After commandlet work we collect and purge whole garbage including this commandlet
		commandlet->RemoveFromRoot();
		CObjectGC::Get().CollectGarbage( GARBAGE_COLLECTION_KEEPFLAGS );
		return true;
	}

	return false;
}
