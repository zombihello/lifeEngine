#include "..\AnimationManager3D.h"
#include "..\..\System\3D\Skeleton.h"

//-------------------------------------------------------------------------//

le::AnimationManager3D::AnimationManager3D()
{
	Skeleton = NULL;
}

//-------------------------------------------------------------------------//

le::AnimationManager3D::AnimationManager3D( le::Skeleton& Skeleton )
{
	this->Skeleton = &Skeleton;
}

//-------------------------------------------------------------------------//

le::AnimationManager3D::~AnimationManager3D()
{
	mAnimations.clear();
}

//-------------------------------------------------------------------------//

void le::AnimationManager3D::Play( string nameAnimation, bool isLoop )
{
	if ( mAnimations.count( nameAnimation ) != 0 )
	{
		mAnimations[nameAnimation].Play( isLoop );
		sCurrentAnimation = nameAnimation;
	}
}

//-------------------------------------------------------------------------//

void le::AnimationManager3D::Pause()
{
	if ( sCurrentAnimation != "" )
		mAnimations[sCurrentAnimation].Pause();
}

//-------------------------------------------------------------------------//

void le::AnimationManager3D::Stop()
{
	if ( sCurrentAnimation != "" )
		mAnimations[sCurrentAnimation].Stop();
}

//-------------------------------------------------------------------------//

void le::AnimationManager3D::UpdateAnimation()
{
	if ( sCurrentAnimation != "" )
	{
		if ( Skeleton->IsInit() )
			mAnimations[sCurrentAnimation].UpdateAnimation();
	}
}

//-------------------------------------------------------------------------//

bool le::AnimationManager3D::LoadAnimations( TiXmlElement* animations )
{
	if ( Skeleton == NULL ) return false;

	if ( animations != NULL )
	{
		// ???????? ? ??????????? animation
		TiXmlElement *animation;
		animation = animations->FirstChildElement( "animation" );

		while ( animation )
		{
			float maxFrames = atof( animation->Attribute( "MaxFrames" ) );
			float fps = atof( animation->Attribute( "FrameRate" ) );

			Animation3D Anim( *Skeleton, maxFrames );
			Anim.SetFPS( fps );
			Anim.LoadAnimation( animation );

			mAnimations[animation->Attribute( "name" )] = Anim;
			animation = animation->NextSiblingElement();
		}
	}
	else
		return false;

	return true;
}

//-------------------------------------------------------------------------//

bool le::AnimationManager3D::IsPlaying()
{
	if ( sCurrentAnimation != "" )
		return mAnimations[sCurrentAnimation].IsPlaying();
	else
		return false;
}

//-------------------------------------------------------------------------//

void le::AnimationManager3D::SetSkeleton( le::Skeleton& Skeleton )
{
	this->Skeleton = &Skeleton;

	for ( auto it = mAnimations.begin(); it != mAnimations.end(); it++ )
		it->second.SetSkeleton( *this->Skeleton );
}

//-------------------------------------------------------------------------//

void le::AnimationManager3D::DeleteAnimation( string nameAnimation )
{
	if ( mAnimations.count( nameAnimation ) != 0 )
		mAnimations.erase( mAnimations.find( nameAnimation ) );
}

//-------------------------------------------------------------------------//

void le::AnimationManager3D::DeleteAllAnimations()
{
	mAnimations.clear();
}

//-------------------------------------------------------------------------//