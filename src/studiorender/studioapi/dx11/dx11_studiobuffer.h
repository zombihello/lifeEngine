/**
 * @file
 * @addtogroup studioapi_dx11 studioapi_dx11
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef DX11_STUDIOBUFFER_H
#define DX11_STUDIOBUFFER_H

#include "core/platform.h"
#include "stdlib/template.h"

/**
 * @ingroup studioapi_dx11
 * @brief Size of the default constant buffer
 */
#define MAX_GLOBAL_CONSTANT_BUFFER_SIZE			4096

/**
 * @ingroup studioapi_dx11
 * @brief Enumeration of constant buffer slots
 * @warning These offsets must match the cbuffer register definitions in public/src/materialsystem/common.hlsl
 */
enum EConstantBufferSlotDx11
{
	CONSTBUFFERSLOT_SHADER_CONSTANTS,								/**< Global constants in a shader */
	
	// Support elements
	CONSTBUFFERSLOT_FIRST_SLOT = CONSTBUFFERSLOT_SHADER_CONSTANTS,	/**< First slot */
	CONSTBUFFERSLOT_LAST_SLOT = CONSTBUFFERSLOT_SHADER_CONSTANTS,	/**< Last slot */
	CONSTBUFFERSLOT_NUM_SLOTS										/**< Max count constant buffer slots */
};

/**
 * @ingroup studioapi_dx11
 * @brief Get maximum constant buffer size for slot
 * 
 * @param constantBufferSlot	Constant buffer slot
 * @return Return maximum constant buffer size for constantBufferSlot
 */
FORCEINLINE uint32 DX11_GetMaxConstantBufferSize( EConstantBufferSlotDx11 constantBufferSlot )
{
	static const uint32 s_ConstantBufferSizes[CONSTBUFFERSLOT_NUM_SLOTS] =
	{
		// CBs must be a multiple of 16
		Align( ( uint32 )MAX_GLOBAL_CONSTANT_BUFFER_SIZE, 16 )			// CONSTBUFFERSLOT_SHADER_CONSTANTS
	};
	static_assert( CONSTBUFFERSLOT_NUM_SLOTS == ARRAYSIZE( s_ConstantBufferSizes ), "Array size 's_ConstantBufferSizes' must be equal to CONSTBUFFERSLOT_NUM_SLOTS" );
	return s_ConstantBufferSizes[( uint32 )constantBufferSlot];
}

#endif // !DX11_STUDIOBUFFER_H