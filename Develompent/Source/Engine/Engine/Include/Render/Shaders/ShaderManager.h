/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <string>
#include <unordered_map>

#include "Misc/Misc.h"
#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "Shader.h"
#include "ShaderCache.h"

#if WITH_EDITOR
#include "ShaderCompiler.h"
#endif // WITH_EDITOR

/**
 * @ingroup Engine
 * @brief A macro to declare a new shader type
 * 
 * @warning This should be called in the class body of the new shader type
 * @param[in] ShaderClass The name of the class representing an instance of the shader type
 */
#define DECLARE_SHADER_TYPE( ShaderClass ) \
	public: \
        static CShaderMetaType          staticType;             /**< Static meta type of shader */ \
        /** */\
        /** Get meta type of this shader */\
        /** @return Return meta type shader */\
        /** */\
        virtual CShaderMetaType*        GetType() const override                { return &staticType; }\
        /** */\
        /** @brief Static method for create object of this shader for serialize */ \
        /** @return Return new object of this class */ \
        /** */\
		static CShader*			        ConstructSerializedInstance()           { return new ShaderClass(); } \
        /** */\
        /** @brief Static method for create object of this shader for compiling */ \
        /** @return Return new object of this class */ \
        /** */\
        static CShader*			        ConstructCompiledInstance()             { return new ShaderClass(); }

/**
 * @ingroup Engine
 * @brief A macro to implement a shader type
 * 
 * @param[in] TemplatePrefix Template prefix
 * @param[in] ShaderClass The name of the class representing an instance of the shader type
 * @param[in] SourceFilename Source file name of shader
 * @param[in] FunctionName Main function name in shader
 * @param[in] Frequency Frequency of shader
 * @param[in] Global Is global shader
 */
#if WITH_EDITOR
#define IMPLEMENT_SHADER_TYPE( TemplatePrefix, ShaderClass, SourceFilename, FunctionName, Frequency, Global ) \
	TemplatePrefix \
    CShaderMetaType       ShaderClass::staticType( \
		TEXT( #ShaderClass ), \
		SourceFilename, \
		FunctionName, \
		Frequency, \
        Global, \
		ShaderClass::ConstructSerializedInstance, \
		ShaderClass::ConstructCompiledInstance, \
        ShaderClass::ShouldCache, \
        ShaderClass::ModifyCompilationEnvironment );
#else
#define IMPLEMENT_SHADER_TYPE( TemplatePrefix, ShaderClass, SourceFilename, FunctionName, Frequency, Global ) \
	TemplatePrefix \
    CShaderMetaType       ShaderClass::staticType( \
		TEXT( #ShaderClass ), \
		SourceFilename, \
		FunctionName, \
		Frequency, \
        Global, \
		ShaderClass::ConstructSerializedInstance, \
		ShaderClass::ConstructCompiledInstance );
#endif // WITH_EDITOR

/**
 * @ingroup Engine
 * @brief A shader parameter's register binding
 */
class CShaderParameter
{
public:
    /**
     * @brief Constructor
     */
    CShaderParameter();

    /**
     * @brief Bind shader parameter
     * 
     * @param InParameterMap Shader prarameter map
     * @param InParameterName Parameter name
     * @param InIsOptional Is optional parameter
     */
    void Bind( const CShaderParameterMap& InParameterMap, const tchar* InParameterName, bool InIsOptional = false );

    /**
     * @brief Shader parameter is bounded
     * @return Return true if shader parameter is bounded, else returning false 
     */
    FORCEINLINE bool IsBound() const 
    { 
        return numBytes > 0; 
    }

    /**
     * @brief Get the buffer index
     * @return Return buffer index 
     */
	FORCEINLINE uint32 GetBufferIndex() const 
    { 
        return bufferIndex; 
    }

    /**
     * @brief Get the base index
     * @return Return base index 
     */
	FORCEINLINE uint32 GetBaseIndex() const 
    { 
        return baseIndex; 
    }

    /**
     * @brief Get the num bytes
     * @return Return number bytes of parameter 
     */
	FORCEINLINE uint32 GetNumBytes() const 
    { 
        return numBytes; 
    }

private:
    uint32      bufferIndex;     /**< Buffer index */
    uint32      baseIndex;       /**< Base index */
    uint32      numBytes;        /**< Num bytes of parameter. 0 if the parameter wasn't bound */
};

/**
 * @ingroup Engine
 * @brief A shader resource binding
 */
class CShaderResourceParameter
{
public:
    /**
     * @brief Constructor
     */
    CShaderResourceParameter();

    /**
     * @brief Bind shader parameter
     *
     * @param InParameterMap Shader prarameter map
     * @param InParameterName Parameter name
     * @param InIsOptional Is optional parameter
     */
	void Bind( const CShaderParameterMap& InParameterMap, const tchar* InParameterName, bool InIsOptional = false );

    /**
     * @brief Shader parameter is bounded
     * @return Return true if shader parameter is bounded, else returning false
     */
    FORCEINLINE bool IsBound() const 
    { 
        return numResources > 0;
    }

    /**
     * @brief Get the base index
     * @return Return base index
     */
    FORCEINLINE uint32 GetBaseIndex() const 
    { 
        return baseIndex;
    }

    /**
     * @brief Get number resources
     * @return Return number resources
     */
    FORCEINLINE uint32 GetNumResources() const 
    { 
        return numResources;
    }

    /**
     * @brief Get sampler index
     * @return Return sampler index
     */
    FORCEINLINE uint32 GetSamplerIndex() const 
    { 
        return samplerIndex;
    }

private:
	uint32 baseIndex;
	uint32 numResources;
	uint32 samplerIndex;
};

/**
 * @brief Sets the value of a vertex shader parameter
 *
 * @param InDeviceContextRHI Device context
 * @param InParameter Shader parameter
 * @param InValue Value
 * @param InElementIndex Eelement index
 */
template< class TParameterType >
FORCEINLINE void SetVertexShaderValue( class CBaseDeviceContextRHI* InDeviceContextRHI, const CShaderParameter& InParameter, const TParameterType& InValue, uint32 InElementIndex = 0 )
{
    const uint32       alignedTypeSize = Align( sizeof( TParameterType ), ShaderArrayElementAlignBytes );
    const int32        numBytesToSet = Min<int32>( alignedTypeSize, InParameter.GetNumBytes() - InElementIndex * alignedTypeSize );
    Assert( numBytesToSet >= InParameter.GetNumBytes() );

    if ( numBytesToSet > 0 )
    {
        g_RHI->SetVertexShaderParameter(
                    InDeviceContextRHI,
                    InParameter.GetBufferIndex(),
                    InParameter.GetBaseIndex() + InElementIndex * alignedTypeSize,
                    ( uint32 )numBytesToSet,
                    &InValue );
    }
}

/**
 * @brief Sets the value of a vertex shader parameter with bool type (in shaders bool type is 4 byte)
 *
 * @param InDeviceContextRHI    Device context
 * @param InParameter           Shader parameter
 * @param InValue               Value
 * @param InElementIndex        Element index
 */
template<>
FORCEINLINE void SetVertexShaderValue( class CBaseDeviceContextRHI* InDeviceContextRHI, const CShaderParameter& InParameter, const bool& InValue, uint32 InElementIndex )
{
    int32       value = InValue;
    SetVertexShaderValue( InDeviceContextRHI, InParameter, value, InElementIndex );
}

/**
 * @brief Sets the value of a pixel shader parameter
 *
 * @param InDeviceContextRHI Device context
 * @param InParameter Shader parameter
 * @param InValue Value
 * @param InElementIndex Eelement index
 */
template< class TParameterType >
FORCEINLINE void SetPixelShaderValue( class CBaseDeviceContextRHI* InDeviceContextRHI, const CShaderParameter& InParameter, const TParameterType& InValue, uint32 InElementIndex = 0 )
{
    const uint32       alignedTypeSize = Align( sizeof( TParameterType ), ShaderArrayElementAlignBytes );
    const int32        numBytesToSet = Min<int32>( sizeof( TParameterType ), InParameter.GetNumBytes() - InElementIndex * alignedTypeSize );
    Assert( numBytesToSet >= InParameter.GetNumBytes() );

    if ( numBytesToSet > 0 )
    {
        g_RHI->SetPixelShaderParameter(
                    InDeviceContextRHI,
                    InParameter.GetBufferIndex(),
                    InParameter.GetBaseIndex() + InElementIndex * alignedTypeSize,
                    ( uint32 )numBytesToSet,
                    &InValue );
    }
}

/**
 * @brief Set texture parameter
 *
 * @param InDeviceContextRHI    Device context
 * @param InParameter           Shader resource parameter
 * @param InTextureRHI          Texture 2D RHI
 * @param InElementIndex        Element index
 */
FORCEINLINE void SetTextureParameter( class CBaseDeviceContextRHI* InDeviceContextRHI, const CShaderResourceParameter& InParameter, const Texture2DRHIParamRef_t InTextureRHI, uint32 InElementIndex = 0 )
{
	if ( InParameter.IsBound() )
	{
		Assert( InElementIndex < InParameter.GetNumResources() );
        g_RHI->SetTextureParameter( InDeviceContextRHI, nullptr, InTextureRHI, InParameter.GetBaseIndex() + InElementIndex );
	}
}

/**
 * @brief Set sampler state parameter
 *
 * @param InDeviceContextRHI    Device context
 * @param InParameter           Shader resource parameter
 * @param InSamplerStateRHI     Sampler state RHI
 * @param InElementIndex        Element index
 */
FORCEINLINE void SetSamplerStateParameter( class CBaseDeviceContextRHI* InDeviceContextRHI, const CShaderResourceParameter& InParameter, const SamplerStateRHIParamRef_t InSamplerStateRHI, uint32 InElementIndex = 0 )
{
	if ( InParameter.IsBound() )
	{
        Assert( InElementIndex < InParameter.GetNumResources() );
        g_RHI->SetSamplerState( InDeviceContextRHI, nullptr, InSamplerStateRHI, InParameter.GetSamplerIndex() + InElementIndex );
	}
}

/**
 * @brief Sets the value of a pixel shader parameter with bool type (in shaders bool type is 4 byte)
 *
 * @param InDeviceContextRHI    Device context
 * @param InParameter           Shader parameter
 * @param InValue               Value
 * @param InElementIndex        Element index
 */
template<>
FORCEINLINE void SetPixelShaderValue( class CBaseDeviceContextRHI* InDeviceContextRHI, const CShaderParameter& InParameter, const bool& InValue, uint32 InElementIndex )
{
    int32       value = InValue;
    SetPixelShaderValue( InDeviceContextRHI, InParameter, value, InElementIndex );
}

/**
 * @ingroup Engine
 * @brief Class of meta information by shader
 */
class CShaderMetaType
{
public:
    /**
     * @brief Pointer to static method for create object of shader for serialize
     */
    typedef class CShader*        ( *ConstructSerializedInstanceFn_t )();

    /**
     * @brief Pointer to static method for create object of shader for compiling
     */
    typedef class CShader*        ( *ConstructCompiledInstanceFn_t )();

#if WITH_EDITOR
    /**
     * @brief Pointer to static method for Assert is need compile shader for vertex factory. If vertex factory is nullptr - checking general
     */
    typedef bool                  ( *ShouldCacheFn_t )( EShaderPlatform InShaderPlatform, CVertexFactoryMetaType* InVFMetaType );

    /**
     * @brief Pointer to static method for modify compilation environment of shader
     */
    typedef void                  ( *ModifyCompilationEnvironmentFn_t )( EShaderPlatform InShaderPlatform, ShaderCompilerEnvironment& InEnvironment );
#endif // WITH_EDITOR

    /**
     * @brief Constructor
     * 
     * @param[in] InName Shader name
     * @param[in] InFileName File name of source
     * @param[in] InFunctionName Main function in shader
     * @param[in] InFrequency Frequency of shader
     * @param[in] IsIsGlobal Shader is global?
     * @param[in] InConstructSerializedInstance Pointer to static method for create object of shader for serialize
     * @param[in] InConstructCompiledInstance Pointer to static method for create object of shader for compiling
     * @param[in] InShouldCacheFunc Pointer to static method for Assert is need compile shader. WARNING! Only with enabled define WITH_EDITOR
     * @param[in] InModifyCompilationEnvironmentFunc Pointer to static method for modify compilation environment of shader. WARNING! Only with enabled define WITH_EDITOR
     */
    CShaderMetaType( const std::wstring& InName, const std::wstring& InFileName, const std::wstring& InFunctionName, EShaderFrequency InFrequency, bool InIsGlobal, ConstructSerializedInstanceFn_t InConstructSerializedInstance, ConstructCompiledInstanceFn_t InConstructCompiledInstance
#if WITH_EDITOR
                     , ShouldCacheFn_t InShouldCacheFunc, ModifyCompilationEnvironmentFn_t InModifyCompilationEnvironmentFunc
#endif // WITH_EDITOR
    );

    /**
     * @brief Constructor of copy
     * @param[in] InCopy Copy
     */
    CShaderMetaType( const CShaderMetaType& InCopy );

    /**
     * @brief Get name
     * @return Return name of shader
     */
    FORCEINLINE const std::wstring& GetName() const
    {
        return name;
    }

    /**
     * @brief Get file name
     * @return Return file name
     */
    FORCEINLINE const std::wstring& GetFileName() const
    {
        return fileName;
    }

    /**
     * @brief Get main function name in shader
     * @return Return main function name in shader
     */
    FORCEINLINE const std::wstring& GetFunctionName() const
    {
        return functionName;
    }

    /**
     * @brief Get frequency of shader
     * @return Return frequency of shader
     */
    FORCEINLINE EShaderFrequency GetFrequency() const
    {
        return frequency;
    }

    /**
     * @brief Create instance shader for serialize
     * @return Return new instance of shader
     */
    FORCEINLINE CShader* CreateSerializedInstace() const
    {
        Assert( ConstructSerializedInstance );
        return ConstructSerializedInstance();
    }

    /**
	 * @brief Create instance shader for compile
	 * @return Return new instance of shader
     */
    FORCEINLINE CShader* CreateCompiledInstance() const
    {
        Assert( ConstructCompiledInstance );
        return ConstructCompiledInstance();
    }

#if WITH_EDITOR
    /**
     * @brief Is need compile shader for platform
     * 
     * @param InShaderPlatform Shader platform
     * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general Assert
     * @return Return true if need compile shader, else returning false
     */
    FORCEINLINE bool ShouldCache( EShaderPlatform InShaderPlatform, CVertexFactoryMetaType* InVFMetaType = nullptr ) const
    {
        Assert( ShouldCacheFunc );
        return ShouldCacheFunc( InShaderPlatform, InVFMetaType );
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
    }
#endif // WITH_EDITOR

    /**
     * @brief Is global shader
     * @return Return TRUE if shader is global, else return FALSE
     */
    FORCEINLINE bool IsGlobal() const
    {
        return bGlobal;
    }

private:
    bool                                bGlobal;                                /**< Is this shader is global */
    std::wstring                        name;                                   /**< Name of shader */
    std::wstring                        fileName;                               /**< Source file name */
    std::wstring                        functionName;                           /**< Main function in shader */
    EShaderFrequency                    frequency;                              /**< Frequency of shader */
    ConstructSerializedInstanceFn_t     ConstructSerializedInstance;            /**< Pointer to static method for create object of shader for serialize */
    ConstructCompiledInstanceFn_t       ConstructCompiledInstance;              /**< Pointer to static method for create object of shader for compiling */

#if WITH_EDITOR
    ShouldCacheFn_t                     ShouldCacheFunc;                        /**< Pointer to static method for Assert if need compile shader for platform */
    ModifyCompilationEnvironmentFn_t    ModifyCompilationEnvironmentFunc;       /**< Pointer to static method for modify compilation environment */
#endif // WITH_EDITOR
};

/**
 * @ingroup Engine
 * @brief Class of management shaders
 */
class CShaderManager
{
public:
    friend CShaderMetaType;
    friend class CShaderCompiler;

    /**
     * @brief Initialize shader manager
     */
    void Init();

    /**
     * Shutdown shader manager
     */
    void Shutdown();

    /**
     * Find instance of shader by name
     *
     * @param[in] InShaderName Shader name
     * @param[in] InVertexFactoryHash Vertex factory hash
     * @return Return reference to shader
     */
    CShader* FindInstance( const std::wstring& InShaderName, uint64 InVertexFactoryHash );

    /**
     * @brief Find instance of shader
     *
     * @param[in] InVertexFactoryHash Vertex factory hash
     * @return Return reference to shader
     */
    template< typename TShaderClass >
    FORCEINLINE TShaderClass* FindInstance( uint64 InVertexFactoryHash )
    {
        return ( TShaderClass* )FindInstance( TShaderClass::staticType.GetName(), InVertexFactoryHash );
    }

    /**
     * @brief Find instance of shader
     * @return Return reference to shader
     */
    template< typename TShaderClass, typename TVertexFactoryClass >
    FORCEINLINE TShaderClass* FindInstance()
    {
        return ( TShaderClass* )FindInstance( TShaderClass::staticType.GetName(), TVertexFactoryClass::staticType.GetHash() );
    }

    /**
     * Find shader type by name
     *
     * @param[in] InShaderName Shader name
     * @return Return pointer to shader type, if not found return nullptr
     */
    static FORCEINLINE CShaderMetaType* FindShaderType( const std::wstring& InShaderName )
    {
        ContainerShaderTypes* container = ContainerShaderTypes::Get();
        Assert( container );

        auto        itShaderType = container->shaderMetaTypes.find( InShaderName );
        if ( itShaderType == container->shaderMetaTypes.end() )
        {
            return nullptr;
        }

        return itShaderType->second;
    }

    /**
     * Get registered shader types
     * @return Return array of registered shader types
     */
    static FORCEINLINE const std::unordered_map< std::wstring, CShaderMetaType* >& GetShaderTypes()
    {
        return ContainerShaderTypes::Get()->shaderMetaTypes;
    }

    /**
     * Get shader cache filename
     * 
     * @param InShaderPlatform Shader platform
     * @return Return shader cache filename
     */
    static std::wstring GetShaderCacheFilename( EShaderPlatform InShaderPlatform );

private:
    /**
     * @ingroup Engine
     * Typedef shader map
     */
    typedef std::unordered_map< std::wstring, CShader* >        ShaderMap_t;

    /**
     * @ingroup Engine
     * Typedef mesh shader map
     */
    typedef std::unordered_map< uint64, ShaderMap_t >            MeshShaderMap_t;

    /**
     * @brief Class container for storage global shader types
     */
    struct ContainerShaderTypes
    {
        /**
         * @brief Get instance of container
         * @return Return instance of container
         */
        static FORCEINLINE ContainerShaderTypes* Get()
        {
            static ContainerShaderTypes*       container = nullptr;
            if ( !container )
            {
                container = new ContainerShaderTypes();
            }

            return container;
        }

        /**
		 * @brief Create instance of shader
		 *
		 * @param[in] InShaderName Name of shader class
		 * @return Return pointer to new object of shader class
		 */
        static CShader* CreateShaderInstance( const tchar* InShaderName );

        std::unordered_map< std::wstring, CShaderMetaType* >        shaderMetaTypes;
    };

    /**
     * @brief Register shader type
     * @param[in] InShaderMetaType Pointer to static meta type by shader
     */
    static FORCEINLINE void RegisterShaderType( CShaderMetaType* InShaderMetaType )
    {
        ContainerShaderTypes*          container = ContainerShaderTypes::Get();
        Assert( container && InShaderMetaType );

        container->shaderMetaTypes.insert( std::make_pair( InShaderMetaType->GetName(), InShaderMetaType ) );
    }

    /**
     * @brief Loading shader from cache
     * 
     * @param[in] InPathShaderCache Path to shader cache
     * @return Return true if shader successed loaded, else return false
     */
    bool LoadShaders( const tchar* InPathShaderCache );

    MeshShaderMap_t            shaders;            /**< Map of loaded shaders */
};

//
// Serialization
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, CShaderMetaType*& InValue )
{
	if ( InArchive.IsLoading() )
	{
		std::wstring		shaderTypeName;
		InArchive << shaderTypeName;

		if ( !shaderTypeName.empty() )
		{
            InValue = CShaderManager::FindShaderType( shaderTypeName );
		}
	}
	else
	{
		InArchive << ( InValue ? InValue->GetName() : std::wstring() );
	}

	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const CShaderMetaType*& InValue )
{
	Assert( InArchive.IsSaving() );
    InArchive << ( InValue ? InValue->GetName() : std::wstring() );
	return InArchive;
}

#endif // !TESTSHADER_H

