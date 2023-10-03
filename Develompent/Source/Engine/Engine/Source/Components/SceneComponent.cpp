#include "Components/SceneComponent.h"
#include "Logger/LoggerMacros.h"

IMPLEMENT_CLASS( CSceneComponent )

/*
==================
CSceneComponent::CSceneComponent
==================
*/
CSceneComponent::CSceneComponent()
	: bDityComponentToWorld( true )
	, relativeLocation( Math::vectorZero )
	, relativeRotation( Math::rotatorZero )
	, relativeScale( Math::vectorOne )
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
	new( staticClass, TEXT( "Location" ) )	CVectorProperty( TEXT( "Transform" ), TEXT( "Location of the component relative to its parent" ), STRUCT_OFFSET( ThisClass, relativeLocation ), CPF_Edit );
	new( staticClass, TEXT( "Rotation" ) )	CRotatorProperty( TEXT( "Transform" ), TEXT( "Rotation of the component relative to its parent" ), STRUCT_OFFSET( ThisClass, relativeRotation ), CPF_Edit );
	new( staticClass, TEXT( "Scale" ) )		CVectorProperty( TEXT( "Transform" ), TEXT( "Non-uniform scaling of the component relative to its parent" ), STRUCT_OFFSET( ThisClass, relativeScale ), CPF_Edit, 1.f );
}

#if WITH_EDITOR
/*
==================
CSceneComponent::PostEditChangeProperty
==================
*/
void CSceneComponent::PostEditChangeProperty( const PropertyChangedEvenet& InPropertyChangedEvenet )
{
	CProperty*		changedProperty = InPropertyChangedEvenet.property;
	if ( changedProperty->GetCName() == TEXT( "Location" ) || changedProperty->GetCName() == TEXT( "Rotation" ) || changedProperty->GetCName() == TEXT( "Scale" ) )
	{
		bDityComponentToWorld = true;
	}
	Super::PostEditChangeProperty( InPropertyChangedEvenet );
}

/*
==================
CSceneComponent::DrawDebugComponent
==================
*/
void CSceneComponent::DrawDebugComponent()
{}
#endif // WITH_EDITOR

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
	if ( InArchive.IsLoading() && InArchive.Ver() < VER_UpdateTrasformSceneComponent )
	{
		CTransform		tmpTransform;
		InArchive << tmpTransform;

		relativeLocation	= tmpTransform.GetLocation();
		relativeRotation	= relativeRotationCache.QuatToRotator( tmpTransform.GetRotation() );
		relativeScale		= tmpTransform.GetScale();

		Warnf( TEXT( "Deprecated package version (0x%X). Need to re-save the package '%s', because in the future it may not open\n" ), InArchive.Ver(), InArchive.GetPath().c_str() );
		return;
	}

	InArchive << relativeLocation;
	InArchive << relativeRotation;
	InArchive << relativeScale;
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
	InParent->attachChildren.push_back( this );
}

/*
==================
CSceneComponent::Destroyed
==================
*/
void CSceneComponent::Destroyed()
{
	Super::Destroyed();
	DetachFromParent( true );
}

/*
==================
CSceneComponent::DetachFromParent
==================
*/
void CSceneComponent::DetachFromParent( bool InIsMaintainWorldPosition /* = false */ )
{
	if ( attachParent )
	{
		// Remove our component from parent's children array
		for ( uint32 index = 0, count = attachParent->attachChildren.size(); index < count; ++index )
		{
			CSceneComponent*		component = attachParent->attachChildren[index];
			if ( component == this )
			{
				attachParent->attachChildren.erase( attachParent->attachChildren.begin() + index );
				break;
			}
		}

		// If not need maintain world position then we update component to world transformation
		if ( !InIsMaintainWorldPosition )
		{
			UpdateComponentToWorld();
		}

		attachParent = nullptr;
	}
}

/*
==================
CSceneComponent::GetComponentTransform
==================
*/
const CTransform& CSceneComponent::GetComponentTransform() const
{
	if ( bDityComponentToWorld )
	{
		CSceneComponent*	thisComponent = const_cast<CSceneComponent*>( this );
		thisComponent->UpdateComponentToWorld();
	}
	return componentToWorld;
}

/*
==================
CSceneComponent::UpdateComponentToWorld
==================
*/
void CSceneComponent::UpdateComponentToWorld()
{
	// If out parent hasn't been updated before, we'll need walk up out parent attach hierarchy
	if ( attachParent && attachParent->bDityComponentToWorld )
	{
		attachParent->UpdateComponentToWorld();

		// Updating the parent may (depending on if we were already attached to parent) result in our being updated, so just return
		if ( bDityComponentToWorld )
		{
			return;
		}
	}

	// Update our component to world
	bDityComponentToWorld = false;
	if ( attachParent )
	{
		componentToWorld = GetRelativeTransform() + attachParent->GetComponentTransform();
	}
	else
	{
		componentToWorld = GetRelativeTransform();
	}

	// Update child components
	for ( uint32 index = 0, count = attachChildren.size(); index < count; ++index )
	{
		attachChildren[index]->UpdateComponentToWorld();
	}
}