#include "Actors/BoxCollision.h"

IMPLEMENT_CLASS( ABoxCollision )

/*
==================
ABoxCollision::ABoxCollision
==================
*/
ABoxCollision::ABoxCollision()
{
	boxComponent = CreateComponent<CBoxComponent>( TEXT( "BoxComponent0" ) );
	collisionComponent = boxComponent;
	SetStatic( true );
}

/*
==================
ABoxCollision::StaticInitializeClass
==================
*/
void ABoxCollision::StaticInitializeClass()
{
	// Native properties
	new( staticClass, TEXT( "Box Component" ), OBJECT_Public )	CObjectProperty( CPP_PROPERTY( ThisClass, boxComponent ), TEXT( "Collision" ), TEXT( "Box component" ), CPF_Edit, CBoxComponent::StaticClass() );
}

#if WITH_EDITOR
/*
==================
ABoxCollision::GetActorIcon
==================
*/
std::wstring ABoxCollision::GetActorIcon() const
{
	return TEXT( "Engine/Editor/Icons/Actor_BoxCollision.png" );
}
#endif // WITH_EDITOR