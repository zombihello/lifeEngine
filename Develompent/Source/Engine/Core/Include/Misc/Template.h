/**
 * @file
 * @addtogroup Core Core
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/15/2021
 */

#ifndef TEMPLATE_H
#define TEMPLATE_H

/**
 * @ingroup Core
 * @brief Macro for calculate count elemnts in array
 *
 * @param[in] InArray Array
 */
#define ARRAY_COUNT( InArray )				( sizeof( InArray ) / sizeof( ( InArray )[0] ) )

#endif // !TEMPLATE_H
