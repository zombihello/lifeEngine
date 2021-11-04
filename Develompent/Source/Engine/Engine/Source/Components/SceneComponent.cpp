#include "Components/SceneComponent.h"

IMPLEMENT_CLASS( LSceneComponent )

bool LSceneComponent::IsAttachedTo( LSceneComponent* InTestComp ) const
{
	if ( InTestComp != nullptr )
	{
		for ( const LSceneComponent* Comp = this->GetAttachParent(); Comp != nullptr; Comp = Comp->GetAttachParent() )
		{
			if ( InTestComp == Comp )
			{
				return true;
			}
		}
	}
	return false;
}

void LSceneComponent::Serialize( class FArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << transform;
}

void LSceneComponent::SetupAttachment( LSceneComponent* InParent )
{
	checkMsg( InParent != this, TEXT( "Cannot attach a component to itself" ) );
	checkMsg( InParent && !InParent->IsAttachedTo( this ), TEXT( "Setting up attachment would create a cycle" ) );
	checkMsg( !attachParent, TEXT( "Need detach before attach component" ) );

	attachParent = InParent;
}