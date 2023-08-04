#include "Components/SceneComponent.h"

IMPLEMENT_CLASS( CSceneComponent )

/*
==================
CSceneComponent::CSceneComponent
==================
*/
CSceneComponent::CSceneComponent()
{}

/*
==================
CSceneComponent::~CSceneComponent
==================
*/
CSceneComponent::~CSceneComponent()
{}

/*
==================
CSceneComponent::StaticInitializeClass
==================
*/
void CSceneComponent::StaticInitializeClass()
{
	new CTransformProperty( staticClass, TEXT( "transform" ), TEXT( "Drawing" ), TEXT( "Transform of component" ), CPP_PROPERTY( transform ), 0 );
}

/*
==================
CSceneComponent::IsAttachedTo
==================
*/
bool CSceneComponent::IsAttachedTo( CSceneComponent* InTestComp ) const
{
	if ( InTestComp != nullptr )
	{
		for ( const CSceneComponent* Comp = this->GetAttachParent(); Comp != nullptr; Comp = Comp->GetAttachParent() )
		{
			if ( InTestComp == Comp )
			{
				return true;
			}
		}
	}
	return false;
}

/*
==================
CSceneComponent::Serialize
==================
*/
void CSceneComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << transform;
}

/*
==================
CSceneComponent::SetupAttachment
==================
*/
void CSceneComponent::SetupAttachment( CSceneComponent* InParent )
{
	AssertMsg( InParent != this, TEXT( "Cannot attach a component to itself" ) );
	AssertMsg( InParent && !InParent->IsAttachedTo( this ), TEXT( "Setting up attachment would create a cycle" ) );
	AssertMsg( !attachParent, TEXT( "Need detach before attach component" ) );

	attachParent = InParent;
}