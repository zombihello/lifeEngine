//
// This is automatically generated by the tools.
// DO NOT modify this manually! Edit the corresponding .class files instead!
// Broken Singularity, All Rights Reserved.
//

// BEGIN REGISTRANTS
#ifdef WITH_REGISTRANTS
#ifndef INCLUDED_REGISTRANTS_CORE
#define INCLUDED_REGISTRANTS_CORE
static void InitRegistrants_Core()
{
	// BEGIN TABLE OF NATIVE FUNCTIONS
	static ScriptNativeFunctionLookup s_CObjectNativeFunctions[] =
	{
		MAP_NATIVE_FUNC( CObject, StackTrace )
		{ nullptr, nullptr }
	};
	// END TABLE OF NATIVE FUNCTIONS

	// BEGIN REGISTER NATIVE FUNCTIONS
	CObject::StaticAddNativeFunctions( TEXT( "CObject" ), s_CObjectNativeFunctions );
	// END REGISTER NATIVE FUNCTIONS
}
#endif // !INCLUDED_REGISTRANTS_CORE
#endif // WITH_REGISTRANTS
// END REGISTRANTS