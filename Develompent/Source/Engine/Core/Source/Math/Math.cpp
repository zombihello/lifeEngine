#include "Math/Math.h"
#include "Math/Transform.h"
#include "Math/Rotator.h"

const Vector		Math::vectorZero( 0.f, 0.f, 0.f );
const Vector		Math::vectorOne( 1.f, 1.f, 1.f );
const Quaternion	Math::quaternionZero( Math::AnglesToQuaternion( 0.f, 0.f, 0.f ) );
const CRotator		Math::rotatorZero( 0.f, 0.f, 0.f );
const Matrix		Math::matrixIdentity( 1.f );
const CTransform	Math::transformZero( Math::quaternionZero, Math::vectorOne, Math::vectorOne );
const Vector		Math::vectorForward( 0.f, 0.f, 1.f );
const Vector		Math::vectorRight( 1.f, 0.f, 0.f );
const Vector		Math::vectorUp( 0.f, 1.f, 0.f );