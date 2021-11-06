/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef VERTEXFACTORY_H
#define VERTEXFACTORY_H

#include <string>
#include <vector>
#include <unordered_map>

#include "LEBuild.h"
#include "Misc/RefCounted.h"
#include "Misc/RefCountPtr.h"
#include "Render/RenderResource.h"
#include "Render/Shaders/Shader.h"
#include "RHI/BaseShaderRHI.h"
#include "RHI/BaseBufferRHI.h"
#include "RHI/TypesRHI.h"

/**
 * @ingroup Engine
 * An object used to represent the type of a vertex factory
 */
class FVertexFactoryMetaType
{
public:
	/**
	 * Constructor
	 *
	 * @param[in] InFactoryName Vertex factory name
	 * @param[in] InShaderName Vertex shader name to use this factory
	 * @param[in] InFileName File name of source vertex factory
	 */
	FVertexFactoryMetaType( const std::wstring& InFactoryName, const std::wstring& InShaderName, const std::wstring& InFileName );

	/**
	 * Get factory name
	 * @return Return name vertex factory
	 */
	FORCEINLINE const std::wstring& GetName() const
	{
		return factoryName;
	}

	/**
	 * Get shader name
	 * @return Return name vertex shader to use this factory
	 */
	FORCEINLINE const std::wstring& GetShaderName() const
	{
		return shaderName;
	}

	/**
	 * Get vertex factory hash
	 * @return Return vertex factory hash
	 */
	FORCEINLINE uint32 GetHash() const
	{
		return hash;
	}

#if WITH_EDITOR
	/**
	 * @brief Class container for storage global vertex factory types
	 */
	struct FContainerVertexFactoryMetaType
	{
		/**
		 * Typedef for vertex factory type map
		 */
		typedef std::unordered_map< uint32, FVertexFactoryMetaType* >		FVertexFactoryMap;

		/**
		 * @brief Get instance of container
		 * @return Return instance of container
		 */
		static FORCEINLINE FContainerVertexFactoryMetaType* Get()
		{
			static FContainerVertexFactoryMetaType* container = nullptr;
			if ( !container )
			{
				container = new FContainerVertexFactoryMetaType();
			}

			return container;
		}

		/**
		 * Register vertex factory type
		 *
		 * @param[in] InVertexFactoryMetaType Pointer to vertex factory meta type
		 */
		FORCEINLINE void RegisterType( FVertexFactoryMetaType* InVertexFactoryMetaType )
		{
			check( InVertexFactoryMetaType );
			vertexFactoryMetaTypes[ InVertexFactoryMetaType->hash ] = InVertexFactoryMetaType;
		}

		/**
		 * Get all registered types
		 * @return Return array vertex factory meta types
		 */
		FORCEINLINE const FVertexFactoryMap& GetRegisteredTypes() const
		{
			return vertexFactoryMetaTypes;
		}

		/**
		 * Find registered type
		 * 
		 * @param[in] InHash Vertex factory hash
		 * @return Return pointer to vertex factory meta type
		 */
		FORCEINLINE FVertexFactoryMetaType* FindRegisteredType( uint32 InHash ) const
		{
			auto		itType = vertexFactoryMetaTypes.find( InHash );
			if ( itType == vertexFactoryMetaTypes.end() )
			{
				return nullptr;
			}

			return itType->second;
		}

		/**
		 * Get number registered types
		 * @return Return number registered vertex factory meta types
		 */
		FORCEINLINE uint32 GetNumRegisteredTypes() const
		{
			return vertexFactoryMetaTypes.size();
		}

		FVertexFactoryMap			vertexFactoryMetaTypes;		/**< Array vertex factory meta types */
	};

	/**
	 * @brief Get file name of vertex factory
	 * @return Return file name of vertex factory
	 */
	FORCEINLINE const std::wstring& GetFileName() const
	{
		return sourceFilename;
	}
#endif // WITH_EDITOR

private:
	std::wstring			factoryName;		/**< Vertex factory name */
	std::wstring			shaderName;			/**< Vertex shader name */
	uint32					hash;				/**< Vertex factory hash */

#if WITH_EDITOR
	std::wstring			sourceFilename;		/**< Source file name of vertex factory */
#endif // WITH_EDITOR
};

/**
 * @ingroup Engine
 * A macro for declaring a new vertex factory type
 *
 * @warning This should be called in the class body of the new vertex factory type
 * @param[in] InFactoryClass The name of the class representing an instance of the vertex factory type
 */
#define DECLARE_VERTEX_FACTORY_TYPE( FactoryClass ) \
	public: \
		static FVertexFactoryMetaType			staticType; /**< Static type of the vertex factory */ \
		/** Get type of the vertex factory type */ \
		virtual FVertexFactoryMetaType*			GetType() const override		{ return &staticType; }

/**
 * @ingroup Engine
 * A macro to implement a vertex shader type
 *
 * @param[in] FactoryClass The name of the class representing an instance of the vertex factory type
 * @param[in] ShaderClass The name vertex shader for use factory
 * @param[in] FileName File name of source vertex factory
 */
#define IMPLEMENT_VERTEX_FACTORY_TYPE( FactoryClass, ShaderClass, FileName ) \
	FVertexFactoryMetaType			FactoryClass::staticType( \
		TEXT( #FactoryClass ), \
		TEXT( #ShaderClass ), \
		FileName );

/**
 * @ingroup Engine
 * Struct information about vertex buffer
 */
struct FVertexStream
{
	FVertexBufferRHIRef			vertexBuffer;		/**< Vertex buffer. NOTE: Need change to FVertexBuffer */
	uint32						stride;				/**< Stride */
};

/**
 * @ingroup Engine
 * Pointer to FVertexFactory
 */
typedef TRefCountPtr< class FVertexFactory >			FVertexFactoryRef;

/**
 * @ingroup Engine
 * Encapsulates a vertex data source which can be linked into a vertex shader.
 */
class FVertexFactory : public FRenderResource, public FRefCounted
{
public:
	/**
	 * Destructor
	 */
	virtual ~FVertexFactory();

	/**
	 * Initialize vertex factory
	 */
	void Init();

	/**
	 * @brief Releases the RHI resources used by this resource.
	 * Called when the resource is released.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseRHI() override;

	/**
	 * Add vertex stream
	 *
	 * @param[in] InVertexStream Vertex stream info
	 */
	FORCEINLINE void AddVertexStream( const FVertexStream& InVertexStream )
	{
		streams.push_back( InVertexStream );
	}

	/**
	 * Activates the vertex factory
	 *
	 * @param[in] InDeviceContextRHI RHI device context
	 */
	void Set( class FBaseDeviceContextRHI* InDeviceContextRHI ) const;

	/**
	 * Set shader parameters to use vertex shader
	 *
	 * @param[in] InDeviceContextRHI RHI device context
	 */
	virtual void SetShaderParameters( class FBaseDeviceContextRHI* InDeviceContextRHI );

	/**
	 * Get vertex factory type
	 * @return The vertex factory's type.
	 */
	virtual FVertexFactoryMetaType* GetType() const = 0;

	/**
	 * Get vertex shader for factory
	 * @return Return vertex shader
	 */
	FORCEINLINE FShaderRef GetShader() const
	{
		return shader;
	}

	/**
	 * Get RHI vertex declaration
	 * @return Return RHI vertex declaration, if factory not initialized return nullptr
	 */
	FORCEINLINE FVertexDeclarationRHIRef GetDeclaration() const
	{
		return declaration;
	}

	/**
	 * Initializes the vertex declaration
	 * 
	 * @warning Need call in InitRHI() method
	 * @param[in] InElements The elements of the vertex declaration
	 */
	void InitDeclaration( const FVertexDeclarationElementList& InElements );

private:
	std::vector< FVertexStream >	streams;		/**< Array vertex streams */
	FVertexDeclarationRHIRef		declaration;	/**< Vertex declaration */
	FShaderRef						shader;			/**< Vertex shader */
};

#endif // !VERTEXFACTORY_H