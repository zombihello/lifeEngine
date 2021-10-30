#include "Math/Math.h"

const FVector		FMath::vectorZero( 0.f, 0.f, 0.f );
const FVector		FMath::vectorOne( 1.f, 1.f, 1.f );
const FQuaternion	FMath::quaternionZero( FMath::AnglesToQuaternion( 0.f, 0.f, 0.f ) );
const FMatrix		FMath::matrixIdentity( 1.f );