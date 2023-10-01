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
REGISTER_NAME( 1, Engine )

#ifdef REGISTERING_ENUM
		};
	#undef REGISTER_NAME
	#undef REGISTERING_ENUM
#endif // REGISTERING_ENUM