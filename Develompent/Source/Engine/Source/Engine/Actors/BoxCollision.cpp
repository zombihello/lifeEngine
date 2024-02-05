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

#include "Engine/Actors/BoxCollision.h"

IMPLEMENT_CLASS( ABoxCollision )

/*
==================
ABoxCollision::ABoxCollision
==================
*/
ABoxCollision::ABoxCollision()
{
	collisionComponent = CreateComponent< CBoxComponent >( TEXT( "BoxComponent0" ) );
	boxComponent = collisionComponent;
	SetStatic( true );
}

/*
==================
ABoxCollision::StaticInitializeClass
==================
*/
void ABoxCollision::StaticInitializeClass()
{
	new( staticClass, TEXT( "Box Component" ) )	CObjectProperty( TEXT( "Collision" ), TEXT( "Box component" ), STRUCT_OFFSET( ThisClass, boxComponent ), CPF_Edit, CBoxComponent::StaticClass() );
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