/**
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

#include "pch_stdlib.h"
#include "stdlib/math/rotator.h"
#include "stdlib/math/transform.h"
#include "stdlib/math/math.h"

const Vector		Math::vectorZero( 0.f, 0.f, 0.f );
const Vector		Math::vectorOne( 1.f, 1.f, 1.f );
const Quaternion	Math::quaternionZero( Math::AnglesToQuaternion( 0.f, 0.f, 0.f ) );
const CRotator		Math::rotatorZero( 0.f, 0.f, 0.f );
const Matrix		Math::matrixIdentity( 1.f );
const CTransform	Math::transformZero( Math::quaternionZero, Math::vectorOne, Math::vectorOne );
const Vector		Math::vectorForward( 0.f, 0.f, 1.f );
const Vector		Math::vectorRight( 1.f, 0.f, 0.f );
const Vector		Math::vectorUp( 0.f, 1.f, 0.f );