/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Core/System/Application.h"
#include "Engine/Components/AudioComponent.h"

IMPLEMENT_CLASS( CAudioComponent )

/*
==================
CAudioComponent::CAudioComponent
==================
*/
CAudioComponent::CAudioComponent()
	: bIsLoop( false )
	, bIsUISound( false )
	, bIsAutoPlay( true )
	, bIsStreamable( false )
	, volume( 100.f )
	, pitch( 1.f )
	, minDistance( 1.f )
	, attenuation( 1.f )
	, source( nullptr )
	, oldSourceLocation( Math::vectorZero )
{
	UpdateAudioSourceType();
}

/*
==================
CAudioComponent::~CAudioComponent
==================
*/
CAudioComponent::~CAudioComponent()
{
	if ( source )
	{
		delete source;
	}
}

/*
==================
CAudioComponent::StaticInitializeClass
==================
*/
void CAudioComponent::StaticInitializeClass()
{
	new( staticClass, TEXT( "bIsLoop" ) )		CBoolProperty( TEXT( "Audio" ), TEXT( "Is looped" ), STRUCT_OFFSET( ThisClass, bIsLoop ), CPF_Edit );
	new( staticClass, TEXT( "bIsUISound" ) )	CBoolProperty( TEXT( "Audio" ), TEXT( "Is UI sound" ), STRUCT_OFFSET( ThisClass, bIsUISound ), CPF_Edit );
	new( staticClass, TEXT( "bIsAutoPlay" ) )	CBoolProperty( TEXT( "Audio" ), TEXT( "Is need auto play on begin play" ), STRUCT_OFFSET( ThisClass, bIsAutoPlay ), CPF_Edit );
	new( staticClass, TEXT( "bIsStreamable" ) ) CBoolProperty( TEXT( "Audio" ), TEXT( "Is streamable" ), STRUCT_OFFSET( ThisClass, bIsStreamable ), CPF_Edit );
	new( staticClass, TEXT( "Volume" ) )		CFloatProperty( TEXT( "Audio" ), TEXT( "Volume of sound" ), STRUCT_OFFSET( ThisClass, volume ), CPF_Edit );
	new( staticClass, TEXT( "Pitch" ) )			CFloatProperty( TEXT( "Audio" ), TEXT( "Pitch" ), STRUCT_OFFSET( ThisClass, pitch ), CPF_Edit );
	new( staticClass, TEXT( "Min Distance" ) )	CFloatProperty( TEXT( "Audio" ), TEXT( "Min distance" ), STRUCT_OFFSET( ThisClass, minDistance ), CPF_Edit );
	new( staticClass, TEXT( "Attenuation" ) )	CFloatProperty( TEXT( "Audio" ), TEXT( "Attenuation" ), STRUCT_OFFSET( ThisClass, attenuation ), CPF_Edit );
	new( staticClass, TEXT( "Audio Bank" ) )	CAssetProperty( TEXT( "Audio" ), TEXT( "Audio bank" ), STRUCT_OFFSET( ThisClass, bank ), CPF_Edit, AT_AudioBank );
}

#if WITH_EDITOR
/*
==================
CAudioComponent::PostEditChangeProperty
==================
*/
void CAudioComponent::PostEditChangeProperty( const PropertyChangedEvenet& InPropertyChangedEvenet )
{
	CProperty*		changedProperty = InPropertyChangedEvenet.property;
	if ( changedProperty )
	{
		static const CName		property_bIsLoop( TEXT( "bIsLoop" ) );
		static const CName		property_bIsUISound( TEXT( "bIsUISound" ) );
		static const CName		property_volume( TEXT( "Volume" ) );
		static const CName		property_pitch( TEXT( "Pitch" ) );
		static const CName		property_minDistance( TEXT( "Min Distance" ) );
		static const CName		property_attenuation( TEXT( "Attenuation" ) );
		static const CName		property_audioBank( TEXT( "Audio Bank" ) );
		static const CName		property_bIsStreamable( TEXT( "bIsStreamable" ) );

		if ( changedProperty->GetCName() == property_bIsLoop )
		{
			SetLoop( bIsLoop );
		}
		else if ( changedProperty->GetCName() == property_bIsUISound )
		{
			SetUISound( bIsUISound );
		}
		else if ( changedProperty->GetCName() == property_volume )
		{
			SetVolume( volume );
		}
		else if ( changedProperty->GetCName() == property_pitch )
		{
			SetPitch( pitch );
		}
		else if ( changedProperty->GetCName() == property_minDistance )
		{
			SetMinDistance( minDistance );
		}
		else if ( changedProperty->GetCName() == property_attenuation )
		{
			SetAttenuation( attenuation );
		}
		else if ( changedProperty->GetCName() == property_audioBank )
		{
			SetAudioBank( bank );
			if ( bank.IsAssetValid() && GetStatus() != ASS_Playing && bIsAutoPlay )
			{
				Play();
			}
		}
		else if ( changedProperty->GetCName() == property_bIsStreamable )
		{
			UpdateAudioSourceType();
		}
	}

	Super::PostEditChangeProperty( InPropertyChangedEvenet );
}
#endif // WITH_EDITOR

/*
==================
CAudioComponent::Serialize
==================
*/
void CAudioComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );	
	InArchive << bIsLoop;
	InArchive << bIsUISound;
	InArchive << bIsAutoPlay;
	InArchive << bIsStreamable;
	InArchive << volume;
	InArchive << minDistance;
	InArchive << attenuation;
	InArchive << pitch;
	InArchive << bank;

	// If we archive loading - init audio source
	if ( InArchive.IsLoading() )
	{
		// Init first location of audio source and update it type
		if ( !bIsUISound )
		{
			oldSourceLocation = GetComponentLocation();
		}
		else
		{
			oldSourceLocation = Math::vectorZero;
		}
		UpdateAudioSourceType();

#if WITH_EDITOR
		if ( CApplication::Get().GetType() == AT_Editor && bIsAutoPlay )
		{
			Play();
		}
#endif // WITH_EDITOR
	}
}

/*
==================
CAudioComponent::BeginPlay
==================
*/
void CAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	// Play sound if flag 'bIsAutoPlay' is enabled
	if ( bIsAutoPlay )
	{
		Play();
	}
}

/*
==================
CAudioComponent::TickComponent
==================
*/
void CAudioComponent::TickComponent( float InDeltaTime )
{
	Super::TickComponent( InDeltaTime );

	// If audio component is moved - update audio source location if this not UI sound
	if ( !bIsUISound )
	{
		Vector		location = GetComponentLocation();
		if ( location != oldSourceLocation )
		{
			oldSourceLocation = location;
			source->SetLocation( location );
		}
	}
}

/*
==================
CAudioComponent::UpdateAudioSourceType
==================
*/
void CAudioComponent::UpdateAudioSourceType()
{
	// If source is valid - we getting current status and free allocated memory
	EAudioSourceStatus		status = ASS_Stoped;
	if ( source )
	{
		status = source->GetStatus();
		delete source;
	}

	// If bIsStreamable is true - this audio is stream source
	if ( bIsStreamable )
	{
		source = new CAudioStreamSource();
	}
	// Else this simple audio source
	else
	{
		source = new CAudioSource();
	}

	// Reset set all values to source
	source->SetLoop( bIsLoop );
	source->SetRelativeToListener( bIsUISound );
	source->SetPitch( pitch );
	source->SetVolume( volume );
	source->SetMinDistance( minDistance );
	source->SetAttenuation( attenuation );
	source->SetAudioBank( bank );
	source->SetLocation( oldSourceLocation );

	if ( status == ASS_Playing )
	{
		source->Play();
	}
}