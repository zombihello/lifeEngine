#include "Components/SceneComponent.h"

IMPLEMENT_CLASS( CSceneComponent )

CSceneComponent::CSceneComponent()
{}

CSceneComponent::~CSceneComponent()
{}

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

void CSceneComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << transform;
}

void CSceneComponent::SetupAttachment( CSceneComponent* InParent )
{
	checkMsg( InParent != this, TEXT( "Cannot attach a component to itself" ) );
	checkMsg( InParent && !InParent->IsAttachedTo( this ), TEXT( "Setting up attachment would create a cycle" ) );
	checkMsg( !attachParent, TEXT( "Need detach before attach component" ) );

	attachParent = InParent;
}