/**
 * @file
 * @addtogroup Core Core
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/11/2021
 */

#ifndef LEBUILD_H
#define LEBUILD_H

#ifndef SHIPPING_BUILD
	#define SHIPPING_BUILD			0
#endif // !SHIPPING_BUILD

#ifndef DO_CHECK
	#define DO_CHECK				!SHIPPING_BUILD
#endif // !DO_CHECK

#endif // !LEBUILD_H