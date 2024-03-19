/**
 * @file
 * @addtogroup materialsystem materialsystem
 *
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

#ifndef VERTEXFACTORY_BASE_H
#define VERTEXFACTORY_BASE_H

#include "materialsystem/ivertexfactory.h"

/**
 * @ingroup materialsystem
 * @brief Pointer to static method for create object of a vertex factory
 */
typedef IVertexFactory* ( *CreateVertexFactoryFn_t )();

/**
 * @ingroup materialsystem
 * @brief Register vertex factory type in the system
 * 
 * @param vfType					Vertex factory type (see EVertexFactoryType)
 * @param pCreateVertexFactoryFn	Function for create vertex factory instance
 */
void RegisterVFType( EVertexFactoryType vfType, CreateVertexFactoryFn_t pCreateVertexFactoryFn );

/**
 * @ingroup materialsystem
 * @brief Macro of begin implementation vertex factory
 * 
 * @param Name	Vertex factory name
 * @param Type	Vertex factory type
 */
#define BEGIN_VERTEX_FACTORY( Name, Type ) \
	namespace Name \
	{ \
		/** Typedef of meta type for this vertex factory */ \
		typedef C_##Type##_MetaType		CVertexFactoryMetaType; \
		/** Vertex factory class */ \
		class CVertexFactory : public CBaseVertexFactory \
		{ \
			/** Get vertex factory type */ \
			virtual EVertexFactoryType GetType() const override \
			{ \
				return CVertexFactoryMetaType::GetType(); \
			} \
			/** Is supports instancing */ \
			virtual bool SupportsInstancing() const override \
			{ \
				return CVertexFactoryMetaType::SupportsInstancing(); \
			} \
			/** Get instance stream index */ \
			virtual uint32 GetInstanceStreamIndex() const override \
			{ \
				return CVertexFactoryMetaType::GetInstanceStreamIndex(); \
			}

/**
 * @ingroup materialsystem
 * @brief Macro of implementation initialize the StudioAPI resources used by this vertex factory
 */
#define VERTEXFACTORY_STUDIOAPI_INIT \
	virtual void InitStudioAPI() override

/**
 * @ingroup materialsystem
 * @brief Macro of implementation release the StudioAPI resources used by this vertex factory
 */
#define VERTEXFACTORY_STUDIOAPI_RELEASE \
	virtual void ReleaseStudioAPI() override

/**
 * @ingroup materialsystem
 * @brief Macro of implementation update the StudioAPI resources used by this vertex factory
 */
#define VERTEXFACTORY_STUDIOAPI_UPDATE \
	virtual void UpdateStudioAPI() override

/**
 * @ingroup materialsystem
 * @brief Macro of end implementation vertex factory
 */
#define END_VERTEX_FACTORY \
		}; \
		/** Register vertex factory in the system */ \
		struct RegisterVertexFactory \
		{ \
			RegisterVertexFactory() \
			{ \
				RegisterVFType( CVertexFactoryMetaType::GetType(), []() -> IVertexFactory* { return new CVertexFactory(); } ); \
			} \
		}; \
		static RegisterVertexFactory s_RegisterVertexFactory; \
	} // Namespace

/**
 * @ingroup materialsystem
 * @brief Encapsulates a vertex data source which can be linked into a vertex shader
 */
class CBaseVertexFactory : public TRefCounted<TStudioRenderResource<IVertexFactory>>
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~CBaseVertexFactory();

	/**
	 * @brief Initialize vertex factory
	 */
	virtual void Init() override;

	/**
	 * @brief Shutdown vertex factory
	 */
	virtual void Shutdown() override;
};

#endif // !VERTEXFACTORY_BASE_H