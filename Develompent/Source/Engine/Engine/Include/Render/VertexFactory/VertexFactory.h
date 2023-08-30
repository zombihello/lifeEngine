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

#if WITH_EDITOR
#include "Render/Shaders/ShaderCompiler.h"
#endif // WITH_EDITOR

/**
 * @ingroup Engine
 * An interface to the parameter bindings for the vertex factory used by a shader
 */
class CVertexFactoryShaderParameters
{
public:
	/**
	 * @brief Destroy the CVertexFactoryShaderParameters object
	 */
	virtual ~CVertexFactoryShaderParameters() {}

	/**
	 * @brief Bind shader parameters
	 * 
	 * @param InParameterMap Shader parameter map
	 */
	virtual void Bind( const class CShaderParameterMap& InParameterMap ) = 0;

	/**
	 * @brief Set any shader data specific to this vertex factory
	 * 
	 * @param InDeviceContextRHI RHI device context
	 * @param InVertexFactory Vertex factory
	 */
	virtual void Set( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory ) const = 0;
	
	/**
	 * @brief Set the l2w transform shader
	 * 
	 * @param InDeviceContextRHI RHI device context
	 * @param InMesh Mesh data
	 * @param InVertexFactory Vertex factory
	 * @param InView Scene view
	 * @param InNumInstances Number instances
	 * @param InStartInstanceID ID of first instance
	 */
	virtual void SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct MeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const = 0;
};

/**
 * @ingroup Engine
 * An object used to represent the type of a vertex factory
 */
class CVertexFactoryMetaType
{
public:
	/**
	 * @brief Typedef of function for create shader parameters for vertex factory
	 */
	typedef CVertexFactoryShaderParameters* ( *ConstructParametersType )( EShaderFrequency InShaderFrequency );

#if WITH_EDITOR
	/**
	 * @brief Pointer to static method for Assert is need compile shader
	 */
	typedef bool ( *ShouldCacheFn_t )( EShaderPlatform InShaderPlatform );

	/**
	 * @brief Pointer to static method for modify compilation environment of shader
	 */
	typedef void ( *ModifyCompilationEnvironmentFn_t )( EShaderPlatform InShaderPlatform, ShaderCompilerEnvironment& InEnvironment );
#endif // WITH_EDITOR

	/**
	 * @brief Class container for storage global vertex factory types
	 */
	struct ContainerVertexFactoryMetaType
	{
		/**
		 * Typedef for vertex factory type map
		 */
		typedef std::unordered_map< uint64, CVertexFactoryMetaType* >		VertexFactoryMap_t;

		/**
		 * @brief Get instance of container
		 * @return Return instance of container
		 */
		static FORCEINLINE ContainerVertexFactoryMetaType* Get()
		{
			static ContainerVertexFactoryMetaType*			container = nullptr;
			if ( !container )
			{
				container = new ContainerVertexFactoryMetaType();
			}

			return container;
		}

		/**
		 * Register vertex factory type
		 *
		 * @param[in] InVertexFactoryMetaType Pointer to vertex factory meta type
		 */
		FORCEINLINE void RegisterType( CVertexFactoryMetaType* InVertexFactoryMetaType )
		{
			Assert( InVertexFactoryMetaType );
			vertexFactoryMetaTypes[ InVertexFactoryMetaType->hash ] = InVertexFactoryMetaType;
		}

		/**
		 * Get all registered types
		 * @return Return array vertex factory meta types
		 */
		FORCEINLINE const VertexFactoryMap_t& GetRegisteredTypes() const
		{
			return vertexFactoryMetaTypes;
		}

		/**
		 * Find registered type
		 *
		 * @param[in] InHash Vertex factory hash
		 * @return Return pointer to vertex factory meta type
		 */
		FORCEINLINE CVertexFactoryMetaType* FindRegisteredType( uint64 InHash ) const
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

		VertexFactoryMap_t			vertexFactoryMetaTypes;		/**< Array vertex factory meta types */
	};

	/**
	 * Constructor
	 *
	 * @param[in] InFactoryName Vertex factory name
	 * @param[in] InFileName File name of source vertex factory
	 * @param[in] InSupportsInstancing Is supported instancing
	 * @param[in] InInstanceStreamIndex Instance stream index
	 * @param[in] InConstructParameters Function for create vertex factory shader parameters
	 * @param[in] InShouldCacheFunc Pointer to static method for Assert is need compile shader. WARNING! Only with enabled define WITH_EDITOR
	 * @param[in] InModifyCompilationEnvironmentFunc Pointer to static method for modify compilation environment of shader. WARNING! Only with enabled define WITH_EDITOR
	 */
	CVertexFactoryMetaType( const std::wstring& InFactoryName, const std::wstring& InFileName, bool InSupportsInstancing, uint32 InInstanceStreamIndex, ConstructParametersType InConstructParameters
#if WITH_EDITOR
							, ShouldCacheFn_t InShouldCacheFunc, ModifyCompilationEnvironmentFn_t InModifyCompilationEnvironmentFunc
#endif // WITH_EDITOR
	);

	/**
	 * Get factory name
	 * @return Return name vertex factory
	 */
	FORCEINLINE const std::wstring& GetName() const
	{
		return factoryName;
	}

	/**
	 * Get vertex factory hash
	 * @return Return vertex factory hash
	 */
	FORCEINLINE uint64 GetHash() const
	{
		return hash;
	}

	/**
	 * @brief Create a vertex factory shader parameters
	 * 
	 * @param InShaderFrequency Shader frequency
	 * @return Return vertex factory shader parameters object
	 */
	FORCEINLINE CVertexFactoryShaderParameters* CreateShaderParameters( EShaderFrequency InShaderFrequency ) const 
	{ 
		return ( *ConstructParameters )( InShaderFrequency ); 
	}

#if WITH_EDITOR
	/**
	 * @brief Get file name of vertex factory
	 * @return Return file name of vertex factory
	 */
	FORCEINLINE const std::wstring& GetFileName() const
	{
		return sourceFilename;
	}

	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @return Return true if need compile shader, else returning false
	 */
	FORCEINLINE bool ShouldCache( EShaderPlatform InShaderPlatform ) const
	{
		Assert( ShouldCacheFunc );
		return ShouldCacheFunc( InShaderPlatform );
	}

	/**
	 * @brief Modify compilation environment
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InEnvironment Shader compiler environment
	 */
	FORCEINLINE void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, ShaderCompilerEnvironment& InEnvironment ) const
	{
		Assert( ModifyCompilationEnvironmentFunc );
		ModifyCompilationEnvironmentFunc( InShaderPlatform, InEnvironment );

		InEnvironment.difinitions.insert( std::make_pair( TEXT( "USE_INSTANCING" ), bSupportsInstancing ? TEXT( "1" ) : TEXT( "0" ) ) );
		InEnvironment.difinitions.insert( std::make_pair( TEXT( "ENABLE_HITPROXY" ),
#if ENABLE_HITPROXY
										  !g_IsCooker ? TEXT( "1" ) : TEXT( "0" )
#else
										  TEXT( "0" )
#endif // ENABLE_HITPROXY
		) );
		InEnvironment.difinitions.insert( std::make_pair( TEXT( "WITH_EDITOR" ), !g_IsCooker ? TEXT( "1" ) : TEXT( "0" ) ) );
	}
#endif // WITH_EDITOR

	/**
	 * @brief Is supports instancing
	 * @return Return true if vertex factory is supported instancing, else returning false
	 */
	FORCEINLINE bool SupportsInstancing() const
	{
		return bSupportsInstancing;
	}

	/**
	 * @brief Get instance stream index
	 * @return Return instance stream index
	 */
	FORCEINLINE uint32 GetInstanceStreamIndex() const
	{
		return instanceStreamIndex;
	}

private:
	std::wstring							factoryName;						/**< Vertex factory name */
	uint64									hash;								/**< Vertex factory hash */
	ConstructParametersType					ConstructParameters;				/**< Function of create vertex factory shader parameters */
	bool									bSupportsInstancing;				/**< Is supported instancing */
	uint32									instanceStreamIndex;				/**< Instance stream index */

#if WITH_EDITOR
	std::wstring							sourceFilename;						/**< Source file name of vertex factory */
	ShouldCacheFn_t						ShouldCacheFunc;                    /**< Pointer to static method for Assert if need compile shader for platform */
	ModifyCompilationEnvironmentFn_t		ModifyCompilationEnvironmentFunc;   /**< Pointer to static method for modify compilation environment */
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
		static CVertexFactoryMetaType			staticType; /**< Static type of the vertex factory */ \
		/** Get type of the vertex factory type */ \
		virtual CVertexFactoryMetaType*			GetType() const override		{ return &staticType; }

/**
 * @ingroup Engine
 * A macro to implement a vertex shader type
 *
 * @param[in] FactoryClass The name of the class representing an instance of the vertex factory type
 * @param[in] FileName File name of source vertex factory
 * @param[in] SupportsInstancing Is supported instancing
 * @param[in] InstanceStreamIndex Instance stream index
 */
#if WITH_EDITOR
#define IMPLEMENT_VERTEX_FACTORY_TYPE( FactoryClass, FileName, SupportsInstancing, InstanceStreamIndex ) \
	CVertexFactoryShaderParameters* Construct##FactoryClass##ShaderParameters( EShaderFrequency InShaderFrequency ) { return FactoryClass::ConstructShaderParameters( InShaderFrequency ); } \
	CVertexFactoryMetaType			FactoryClass::staticType( TEXT( #FactoryClass ), FileName, SupportsInstancing, InstanceStreamIndex, Construct##FactoryClass##ShaderParameters, FactoryClass::ShouldCache, FactoryClass::ModifyCompilationEnvironment );
#else
#define IMPLEMENT_VERTEX_FACTORY_TYPE( FactoryClass, FileName, SupportsInstancing, InstanceStreamIndex ) \
	CVertexFactoryShaderParameters* Construct##FactoryClass##ShaderParameters( EShaderFrequency InShaderFrequency ) { return FactoryClass::ConstructShaderParameters( InShaderFrequency ); } \
	CVertexFactoryMetaType			FactoryClass::staticType( TEXT( #FactoryClass ), FileName, SupportsInstancing, InstanceStreamIndex, Construct##FactoryClass##ShaderParameters );
#endif // WITH_EDITOR

/**
 * @ingroup Engine
 * Struct information about vertex buffer
 */
struct VertexStream
{
	VertexBufferRHIRef_t		vertexBuffer;		/**< Vertex buffer. NOTE: Need change to FVertexBuffer */
	uint32						stride;				/**< Stride */
};

/**
 * @ingroup Engine
 * Pointer to CVertexFactory
 */
typedef TRefCountPtr< class CVertexFactory >			VertexFactoryRef_t;

/**
 * @ingroup Engine
 * Encapsulates a vertex data source which can be linked into a vertex shader.
 */
class CVertexFactory : public CRenderResource, public CRefCounted
{
public:
	/**
	 * Destructor
	 */
	virtual ~CVertexFactory();

	/**
	 * Initialize vertex factory
	 */
	void Init();

	/**
	 * Add vertex stream
	 *
	 * @param[in] InVertexStream Vertex stream info
	 */
	FORCEINLINE void AddVertexStream( const VertexStream& InVertexStream )
	{
		streams.push_back( InVertexStream );
	}

	/**
	 * @brief Setup instancing
	 * 
	 * @param InDeviceContextRHI RHI device context
	 * @param InMesh Mesh data
	 * @param InView Scene view
	 * @param InNumInstances Number instances
	 * @param InStartInstanceID ID of first instance
	 */
	virtual void SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct MeshBatch& InMesh, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const;

	/**
	 * Activates the vertex factory
	 *
	 * @param[in] InDeviceContextRHI RHI device context
	 */
	void Set( class CBaseDeviceContextRHI* InDeviceContextRHI ) const;

	/**
	 * Set shader parameters to use vertex shader
	 *
	 * @param[in] InDeviceContextRHI RHI device context
	 */
	virtual void SetShaderParameters( class CBaseDeviceContextRHI* InDeviceContextRHI );

#if WITH_EDITOR
	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform );

	/**
	 * @brief Modify compilation environment
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InEnvironment Shader compiler environment
	 */
	static void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, ShaderCompilerEnvironment& InEnvironment );
#endif // WITH_EDITOR

	/**
	 * @brief Is supports instancing
	 * @return Return true if vertex factory is supported instancing, else returning false
	 */
	FORCEINLINE bool SupportsInstancing() const
	{
		return GetType()->SupportsInstancing();
	}

	/**
	 * @brief Get instance stream index
	 * @return Return instance stream index
	 */
	FORCEINLINE uint32 GetInstanceStreamIndex() const
	{
		return GetType()->GetInstanceStreamIndex();
	}

	/**
	 * Get vertex factory type
	 * @return The vertex factory's type.
	 */
	virtual CVertexFactoryMetaType* GetType() const = 0;

	/**
	 * @brief Get type hash
	 * @return Return hash of vertex factory
	 */
	virtual uint64 GetTypeHash() const;

	/**
	 * Get RHI vertex declaration
	 * @return Return RHI vertex declaration, if factory not initialized return nullptr
	 */
	FORCEINLINE VertexDeclarationRHIRef_t GetDeclaration() const
	{
		return declaration;
	}

	/**
	 * Initializes the vertex declaration
	 * 
	 * @warning Need call in InitRHI() method
	 * @param[in] InElements The elements of the vertex declaration
	 */
	void InitDeclaration( const VertexDeclarationElementList_t& InElements );

	/**
	 * Initializes the vertex declaration
	 *
	 * @warning Need call in InitRHI() method
	 * @param InDeclaration		Vertex declaration RHI	
	 */
	void InitDeclaration( const VertexDeclarationRHIParamRef_t InDeclaration );

protected:
	/**
	 * @brief Releases the RHI resources used by this resource.
	 * Called when the resource is released.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseRHI() override;

private:
	std::vector< VertexStream >	streams;					/**< Array vertex streams */
	VertexDeclarationRHIRef_t		declaration;				/**< Vertex declaration */
};

#endif // !VERTEXFACTORY_H