/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

// Define a message as an enumeration.
#ifndef REGISTER_NAME
	#define REGISTER_NAME( InNum, InName ) NAME_##InName = InNum,
	#define REGISTERING_ENUM
	enum EName
	{
#endif // !REGISTER_NAME

// Special zero value, meaning no name.
REGISTER_NAME( 0, None )

// Packages
REGISTER_NAME( 1, Core )
REGISTER_NAME( 2, Engine )
REGISTER_NAME( 3, Physics )
REGISTER_NAME( 4, WorldEd )

// CObject system reflection
REGISTER_NAME( 5, CObject )
REGISTER_NAME( 6, CField )
REGISTER_NAME( 7, CEnum )
REGISTER_NAME( 8, CProperty )
REGISTER_NAME( 9, CByteProperty )
REGISTER_NAME( 10, CBoolProperty )
REGISTER_NAME( 11, CIntProperty )
REGISTER_NAME( 12, CFloatProperty )
REGISTER_NAME( 13, CArrayProperty )
REGISTER_NAME( 14, CAssetProperty )
REGISTER_NAME( 15, CColorProperty )
REGISTER_NAME( 16, CObjectProperty )
REGISTER_NAME( 17, CRotatorProperty )
REGISTER_NAME( 18, CStringProperty )
REGISTER_NAME( 19, CStructProperty )
REGISTER_NAME( 20, CVectorProperty )
REGISTER_NAME( 21, CStruct )
REGISTER_NAME( 22, CClass )
REGISTER_NAME( 23, CFunction )
REGISTER_NAME( 24, CObjectPackage )
REGISTER_NAME( 25, CObjectRedirector )

#ifdef REGISTERING_ENUM
		};
	#undef REGISTER_NAME
	#undef REGISTERING_ENUM
#endif // REGISTERING_ENUM