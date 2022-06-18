#include "System/TickableObject.h"

//
// GLOBALS
//
std::vector<CTickableObject*>			CTickableObject::tickableObjects;
std::vector<CTickableObject*>			CTickableObject::renderingThreadTickableObjects;