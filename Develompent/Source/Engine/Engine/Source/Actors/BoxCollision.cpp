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
#include "Commandlets/CookPackagesCommandlet.h"

/*
==================
ABoxCollision::InitProperties
==================
*/
bool ABoxCollision::InitProperties( const std::vector<CActorVar>& InActorVars, class CCookPackagesCommandlet* InCooker )
{
	if ( !Super::InitProperties( InActorVars, InCooker ) )
	{
		return false;
	}

	for ( uint32 index = 0, count = InActorVars.size(); index < count; ++index )
	{
		const CActorVar& actorVar = InActorVars[ index ];

		// If property is size
		if ( actorVar.GetName() == TEXT( "Size" ) )
		{
			Assert( actorVar.GetType() == AVT_Vector3D );
			boxComponent->SetSize( actorVar.GetValueVector3D() );
		}

		// If property is physics material
		if ( actorVar.GetName() == TEXT( "PhysMaterial" ) )
		{
			Assert( actorVar.GetType() == AVT_String );
			std::wstring						physMaterialName = actorVar.GetValueString();
			TAssetHandle<CPhysicsMaterial>		physMaterial = g_PackageManager->FindAsset( physMaterialName, AT_Unknown );
			if ( !physMaterial.IsAssetValid() && !InCooker->CookPhysMaterial( physMaterialName, physMaterial ) )
			{
				return false;
			}

			boxComponent->SetPhysMaterial( physMaterial );
		}
	}

	return true;
}

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