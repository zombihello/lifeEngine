/**
 * @file
 * @addtogroup Engine Engine
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/19/2021
 */

#ifndef BASEVIEWPORTRHI_H
#define BASEVIEWPORTRHI_H

 /**
  * @ingroup Engine
  * @brief Base class for work with viewport RHI
  */
class BaseViewportRHI
{
public:
	/**
	 * @brief Destructor
	 */
	virtual				~BaseViewportRHI() {}

	/**
	 * @brief Presents the swap chain
	 */
	virtual void		Present() {}
};

#endif // !BASEVIEWPORTRHI_H
