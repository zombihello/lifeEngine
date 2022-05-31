#include "Math/Math.h"
#include "Math/Rotator.h"
#include "Math/Transform.h"

const Vector		SMath::vectorZero( 0.f, 0.f, 0.f );
const Vector		SMath::vectorOne( 1.f, 1.f, 1.f );
const Quaternion	SMath::quaternionZero( SMath::AnglesToQuaternion( 0.f, 0.f, 0.f ) );
const Matrix		SMath::matrixIdentity( 1.f );
const CRotator		SMath::rotatorZero( 0.f );
const CTransform	SMath::transformZero( SMath::rotatorZero, SMath::vectorOne, SMath::vectorOne );
const Vector		SMath::vectorForward( 0.f, 0.f, 1.f );
const Vector		SMath::vectorRight( 1.f, 0.f, 0.f );
const Vector		SMath::vectorUp( 0.f, 1.f, 0.f );