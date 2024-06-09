#include "Reflection/Class.h"
#include "Commandlets/CookPackagesCommandlet.h"

IMPLEMENT_CLASS( CCookPackagesCommandlet )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CCookPackagesCommandlet )

/*
==================
CCookPackagesCommandlet::Main
==================
*/
bool CCookPackagesCommandlet::Main( const CCommandLine& InCommandLine )
{
	g_IsCooker = true;
	g_IsCooker = false;
	AssertMsg( false, TEXT( "Need implement" ) );
	return true;
}