#include "Actors/BoxCollision.h"

IMPLEMENT_CLASS( ABoxCollision )

ABoxCollision::ABoxCollision()
{
	collisionComponent = CreateComponent< CBoxComponent >( TEXT( "BoxComponent0" ) );
	boxComponent = collisionComponent;
	SetStatic( true );
}

#if WITH_EDITOR
#include "Commandlets/CookPackagesCommandlet.h"

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
			check( actorVar.GetType() == AVT_Vector3D );
			boxComponent->SetSize( actorVar.GetValueVector3D() );
		}

		// If property is physics material
		if ( actorVar.GetName() == TEXT( "PhysMaterial" ) )
		{
			check( actorVar.GetType() == AVT_String );
			std::wstring						physMaterialName = actorVar.GetValueString();
			TAssetHandle<CPhysicsMaterial>		physMaterial = GPackageManager->FindAsset( physMaterialName, AT_Unknown );
			if ( !physMaterial.IsAssetValid() && !InCooker->CookPhysMaterial( physMaterialName, physMaterial ) )
			{
				return false;
			}

			boxComponent->SetPhysMaterial( physMaterial );
		}
	}

	return true;
}

std::wstring ABoxCollision::GetActorIcon() const
{
	return TEXT( "Engine/Editor/Icons/Actor_BoxCollision.png" );
}
#endif // WITH_EDITOR