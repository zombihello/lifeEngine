#include "Logger/LoggerMacros.h"
#include "Containers/String.h"
#include "Misc/Template.h"
#include "Misc/CoreGlobals.h"
#include "Reflection/Object.h"
#include "Reflection/ObjectPackage.h"
#include "Reflection/ObjectIterator.h"
#include "Reflection/Linker.h"
#include "Reflection/Class.h"
#include "Reflection/ReflectionEnvironment.h"
#include "System/BaseFileSystem.h"
#include "Components/ActorComponent.h"

IMPLEMENT_CLASS( CObjectPackage )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CObjectPackage )

/*
==================
CObjectPackage::CObjectPackage
==================
*/
CObjectPackage::CObjectPackage()
	: bDirty( false )
	, linkerLoad( nullptr )
{}