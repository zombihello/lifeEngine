/**
 * @file
 * @addtogroup Engine Engine
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

#ifndef BASECONTROLLER_H
#define BASECONTROLLER_H

#include "Engine/Actors/Actor.h"
#include "Core/Misc/RefCountPtr.h"

 /**
  * @ingroup Engine
  * Base class of all controllers
  */
class ABaseController : public AActor
{
	DECLARE_CLASS( ABaseController, AActor, 0, 0 )

public:
	/**
	 * @brief Constructor
	 */
	ABaseController();

	/**
	 * @brief Destructor
	 */
	virtual ~ABaseController();

	/**
	 * @brief Set controlled character
	 * @param InCharacter Character
	 */
	void SetCharacter( class ACharacter* InCharacter );

	/**
	 * @brief Get controlled character
	 * @return Return controlled character. If not exist return nullptr
	 */
	TRefCountPtr<class ACharacter> GetCharacter() const;

protected:
	TRefCountPtr<class ACharacter>		character;			/**< Character controlled by this controller */
};

#endif // !BASECONTROLLER_H