/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTOPCODES_H
#define SCRIPTOPCODES_H

#ifndef SCRIPT_OPCODE
	#define SCRIPT_OPCODE( X )		OP_##X,
	#define SCRIPT_OPCODE_ENUM
	enum EScriptOpcode
	{
#endif

	SCRIPT_OPCODE( Nop )				/**< No operation */
	SCRIPT_OPCODE( Call )				/**< Call script or native function */
	SCRIPT_OPCODE( Return )				/**< Return */
	SCRIPT_OPCODE( Num )				/**< Number of opcodes */

#ifdef SCRIPT_OPCODE
	};
	#undef SCRIPT_OPCODE_ENUM
	#undef SCRIPT_OPCODE
#endif

#endif // !SCRIPTOPCODES_H