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

#include "Core/Misc/Struct.h"
#include "Core/Misc/Class.h"
#include "Core/Misc/Property.h"

IMPLEMENT_CLASS( CStruct )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CStruct )

/*
==================
CStruct::InternalFindProperty
==================
*/
CProperty* CStruct::InternalFindProperty( const CName& InName ) const
{
	for ( const CStruct* tempStruct = this; tempStruct; tempStruct = tempStruct->superStruct )
	{
		for ( uint32 index = 0, count = tempStruct->properties.size(); index < count; ++index )
		{
			if ( tempStruct->properties[index]->GetCName() == InName )
			{
				return tempStruct->properties[index];
			}
		}
	}
	return nullptr;
}

/*
==================
CStruct::GetProperties
==================
*/
void CStruct::GetProperties( std::vector<class CProperty*>& OutArrayProperties, bool InPropertiesInParents /* = true */ ) const
{
	for ( const CStruct* tempStruct = this; tempStruct; tempStruct = InPropertiesInParents ? tempStruct->superStruct : nullptr )
	{
		if ( !tempStruct->properties.empty() )
		{
			uint32		offset = OutArrayProperties.size();
			OutArrayProperties.resize( tempStruct->properties.size() + offset );
			Memory::Memcpy( OutArrayProperties.data() + offset, tempStruct->properties.data(), tempStruct->properties.size() * sizeof( CProperty* ) );
		}
	}
}

/*
==================
CStruct::GetNumProperties
==================
*/
uint32 CStruct::GetNumProperties( bool InPropertiesInParents /* = true */ ) const
{
	uint32		numProps = 0;
	for ( const CStruct* tempStruct = this; tempStruct; tempStruct = InPropertiesInParents ? tempStruct->superStruct : nullptr )
	{
		numProps += tempStruct->properties.size();
	}

	return numProps;
}

/*
==================
CStruct::AddProperty
==================
*/
void CStruct::AddProperty( CProperty* InProperty )
{
	properties.push_back( InProperty );
}