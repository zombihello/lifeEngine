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
        static FShaderMetaType          staticType;             /**< Static meta type of shader */ \
        /** */\
        /** Get meta type of this shader */\
        /** @return Return meta type shader */\
        /** */\
        virtual FShaderMetaType*        GetType() const override                { return &staticType; }\
        /** */\
        /** @brief Static method for create object of this shader for serialize */ \
        /** @return Return new object of this class */ \
        /** */\
		static FShader*			        ConstructSerializedInstance()           { return new ShaderClass(); } \
        /** */\
        /** @brief Static method for create object of this shader for compiling */ \
        /** @return Return new object of this class */ \
        /** */\
        static FShader*			        ConstructCompiledInstance()             { return new ShaderClass(); }

/**
 * @ingroup Engine
 * @brief A macro to implement a shader type
 * 
 * @param[in] ShaderClass The name of the class representing an instance of the shader type
 * @param[in] SourceFilename Source file name of shader
 * @param[in] FunctionName Main function name in shader
 * @param[in] Frequency Frequency of shader
 */
#if WITH_EDITOR
#define IMPLEMENT_SHADER_TYPE( ShaderClass, SourceFilename, FunctionName, Frequency ) \
	FShaderMetaType       ShaderClass::staticType( \
		TEXT( #ShaderClass ), \
		SourceFilename, \
		FunctionName, \
		Frequency, \
		ShaderClass::ConstructSerializedInstance, \
		ShaderClass::ConstructCompiledInstance, \
        ShaderClass::ShouldCache, \
        ShaderClass::ModifyCompilationEnvironment );
#else
#define IMPLEMENT_SHADER_TYPE( ShaderClass, SourceFilename, FunctionName, Frequency ) \
	FShaderMetaType       ShaderClass::staticType( \
		TEXT( #ShaderClass ), \
		SourceFilename, \
		FunctionName, \
		Frequency, \
		ShaderClass::ConstructSerializedInstance, \
		ShaderClass::ConstructCompiledInstance );
#endif // WITH_EDITOR

/**
 * @ingroup Engine
 * @brief A shader parameter's register binding
 */
class FShaderParameter
{
public:
    /**
     * @brief Bind shader parameter
     * 
     * @param InParameterMap Shader prarameter map
     * @param InParameterName Parameter name
     * @param InIsOptional Is optional parameter
     */
    void Bind( const FShaderParameterMap& InParameterMap, const tchar* InParameterName, bool InIsOptional = false );

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
 * @brief Sets the value of a vertex shader parameter
 *
 * @param InDeviceContextRHI Device context
 * @param InParameter Shader parameter
 * @param InValue Value
 * @param InElementIndex Eelement index
 */
template< class TParameterType >
FORCEINLINE void SetVertexShaderValue( class FBaseDeviceContextRHI* InDeviceContextRHI, const FShaderParameter& InParameter, const TParameterType& InValue, uint32 InElementIndex = 0 )
{
    const uint32       alignedTypeSize = Align( sizeof( TParameterType ), ShaderArrayElementAlignBytes );
    const int32        numBytesToSet = Min<int32>( sizeof( TParameterType ), InParameter.GetNumBytes() - InElementIndex * alignedTypeSize );

    if ( numBytesToSet > 0 )
    {
        GRHI->SetVertexShaderParameter(
                    InDeviceContextRHI,
                    InParameter.GetBufferIndex(),
                    InParameter.GetBaseIndex() + InElementIndex * alignedTypeSize,
                    ( uint32 )numBytesToSet,
                    &InValue );
    }
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
FORCEINLINE void SetPixelShaderValue( class FBaseDeviceContextRHI* InDeviceContextRHI, const FShaderParameter& InParameter, const TParameterType& InValue, uint32 InElementIndex = 0 )
{
    const uint32       alignedTypeSize = Align( sizeof( TParameterType ), ShaderArrayElementAlignBytes );
    const int32        numBytesToSet = Min<int32>( sizeof( TParameterType ), InParameter.GetNumBytes() - InElementIndex * alignedTypeSize );

    if ( numBytesToSet > 0 )
    {
        GRHI->SetPixelShaderParameter(
                    InDeviceContextRHI,
                    InParameter.GetBufferIndex(),
                    InParameter.GetBaseIndex() + InElementIndex * alignedTypeSize,
                    ( uint32 )numBytesToSet,
                    &InValue );
    }
}

/**
 * @ingroup Engine
 * @brief Class of meta information by shader
 */
class FShaderMetaType
{
public:
    /**
     * @brief Pointer to static method for create object of shader for serialize
     */
    typedef class FShader*        ( *FConstructSerializedInstance )();

    /**
     * @brief Pointer to static method for create object of shader for compiling
     */
    typedef class FShader*        ( *FConstructCompiledInstance )();

#if WITH_EDITOR
    /**
     * @brief Pointer to static method for check is need compile shader
     */
    typedef bool                  ( *FShouldCacheFunc )( EShaderPlatform InShaderPlatform );

    /**
     * @brief Pointer to static method for modify compilation environment of shader
     */
    typedef void                  ( *FModifyCompilationEnvironmentFunc )( EShaderPlatform InShaderPlatform, FShaderCompilerEnvironment& InEnvironment );
#endif // WITH_EDITOR

    /**
     * @brief Constructor
     * 
     * @param[in] InName Shader name
     * @param[in] InFileName File name of source
     * @param[in] InFunctionName Main function in shader
     * @param[in] InFrequency Frequency of shader
     * @param[in] InConstructSerializedInstance Pointer to static method for create object of shader for serialize
     * @param[in] InConstructCompiledInstance Pointer to static method for create object of shader for compiling
     * @param[in] InShouldCacheFunc Pointer to static method for check is need compile shader. WARNING! Only with enabled define WITH_EDITOR
     * @param[in] InModifyCompilationEnvironmentFunc Pointer to static method for modify compilation environment of shader. WARNING! Only with enabled define WITH_EDITOR
     */
    FShaderMetaType( const std::wstring& InName, const std::wstring& InFileName, const std::wstring& InFunctionName, EShaderFrequency InFrequency, FConstructSerializedInstance InConstructSerializedInstance, FConstructCompiledInstance InConstructCompiledInstance
#if WITH_EDITOR
                     , FShouldCacheFunc InShouldCacheFunc, FModifyCompilationEnvironmentFunc InModifyCompilationEnvironmentFunc
#endif // WITH_EDITOR
    );

    /**
     * @brief Constructor of copy
     * @param[in] InCopy Copy
     */
    FShaderMetaType( const FShaderMetaType& InCopy );

    /**
     * @brief Get name
     * @return Return name of shader
     */
    FORCEINLINE const std::wstring&         GetName() const
    {
        return name;
    }

    /**
     * @brief Get file name
     * @return Return file name
     */
    FORCEINLINE const std::wstring&         GetFileName() const
    {
        return fileName;
    }

    /**
     * @brief Get main function name in shader
     * @return Return main function name in shader
     */
    FORCEINLINE const std::wstring&         GetFunctionName() const
    {
        return functionName;
    }

    /**
     * @brief Get frequency of shader
     * @return Return frequency of shader
     */
    FORCEINLINE EShaderFrequency            GetFrequency() const
    {
        return frequency;
    }

    /**
     * @brief Create instance shader for serialize
     * @return Return new instance of shader
     */
    FORCEINLINE FShader*                    CreateSerializedInstace() const
    {
        check( ConstructSerializedInstance );
        return ConstructSerializedInstance();
    }

    /**
	 * @brief Create instance shader for compile
	 * @return Return new instance of shader
     */
    FORCEINLINE FShader*                    CreateCompiledInstance() const
    {
        check( ConstructCompiledInstance );
        return ConstructCompiledInstance();
    }

#if WITH_EDITOR
    /**
     * @brief Is need compile shader for platform
     * 
     * @param InShaderPlatform Shader platform
     * @return Return true if need compile shader, else returning false
     */
    FORCEINLINE bool ShouldCache( EShaderPlatform InShaderPlatform ) const
    {
        check( ShouldCacheFunc );
        return ShouldCacheFunc( InShaderPlatform );
    }

    /**
     * @brief Modify compilation environment
     *
     * @param InShaderPlatform Shader platform
     * @param InEnvironment Shader compiler environment
     */
    FORCEINLINE void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, FShaderCompilerEnvironment& InEnvironment ) const
    {
        check( ModifyCompilationEnvironmentFunc );
        ModifyCompilationEnvironmentFunc( InShaderPlatform, InEnvironment );
    }
#endif // WITH_EDITOR

private:
    std::wstring                        name;                                   /**< Name of shader */
    std::wstring                        fileName;                               /**< Source file name */
    std::wstring                        functionName;                           /**< Main function in shader */
    EShaderFrequency                    frequency;                              /**< Frequency of shader */
    FConstructSerializedInstance        ConstructSerializedInstance;            /**< Pointer to static method for create object of shader for serialize */
    FConstructCompiledInstance          ConstructCompiledInstance;              /**< Pointer to static method for create object of shader for compiling */

#if WITH_EDITOR
    FShouldCacheFunc                     ShouldCacheFunc;                        /**< Pointer to static method for check if need compile shader for platform */
    FModifyCompilationEnvironmentFunc    ModifyCompilationEnvironmentFunc;       /**< Pointer to static method for modify compilation environment */
#endif // WITH_EDITOR
};

/**
 * @ingroup Engine
 * @brief Class of management shaders
 */
class FShaderManager
{
public:
    friend FShaderMetaType;
    friend class FShaderCompiler;

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
    FShaderRef FindInstance( const std::wstring& InShaderName, uint64 InVertexFactoryHash );

    /**
     * @brief Find instance of shader
     *
     * @param[in] InVertexFactoryHash Vertex factory hash
     * @return Return reference to shader
     */
    template< typename TShaderClass >
    FORCEINLINE FShaderRef                  FindInstance( uint64 InVertexFactoryHash )
    {
        return FindInstance( TShaderClass::staticType.GetName(), InVertexFactoryHash );
    }

    /**
     * @brief Find instance of shader
     * @return Return reference to shader
     */
    template< typename TShaderClass, typename TVertexFactoryClass >
    FORCEINLINE FShaderRef                  FindInstance()
    {
        return FindInstance( TShaderClass::staticType.GetName(), TVertexFactoryClass::staticType.GetHash() );
    }

    /**
     * Find shader type by name
     *
     * @param[in] InShaderName Shader name
     * @return Return pointer to shader type, if not found return nullptr
     */
    static FORCEINLINE FShaderMetaType* FindShaderType( const std::wstring& InShaderName )
    {
        FContainerShaderTypes* container = FContainerShaderTypes::Get();
        check( container );

        auto        itShaderType = container->shaderMetaTypes.find( InShaderName );
        if ( itShaderType == container->shaderMetaTypes.end() )
        {
            return nullptr;
        }

        return itShaderType->second;
    }

private:
    /**
     * @ingroup Engine
     * Typedef shader map
     */
    typedef std::unordered_map< std::wstring, FShaderRef >     FShaderMap;

    /**
     * @ingroup Engine
     * Typedef mesh shader map
     */
    typedef std::unordered_map< uint64, FShaderMap >            FMeshShaderMap;

    /**
     * @brief Class container for storage global shader types
     */
    struct FContainerShaderTypes
    {
        /**
         * @brief Get instance of container
         * @return Return instance of container
         */
        static FORCEINLINE FContainerShaderTypes*           Get()
        {
            static FContainerShaderTypes*       container = nullptr;
            if ( !container )
            {
                container = new FContainerShaderTypes();
            }

            return container;
        }

        /**
		 * @brief Create instance of shader
		 *
		 * @param[in] InShaderName Name of shader class
		 * @return Return pointer to new object of shader class
		 */
        static FShaderRef                                   CreateShaderInstance( const tchar* InShaderName );

        std::unordered_map< std::wstring, FShaderMetaType* >        shaderMetaTypes;
    };

    /**
     * @brief Register shader type
     * @param[in] InShaderMetaType Pointer to static meta type by shader
     */
    static FORCEINLINE void                                 RegisterShaderType( FShaderMetaType* InShaderMetaType )
    {
        FContainerShaderTypes*          container = FContainerShaderTypes::Get();
        check( container && InShaderMetaType );

        container->shaderMetaTypes.insert( std::make_pair( InShaderMetaType->GetName(), InShaderMetaType ) );
    }

    /**
     * @brief Loading shader from cache
     * 
     * @param[in] InPathShaderCache Path to shader cache
     * @return Return true if shader successed loaded, else return false
     */
    bool                                                    LoadShaders( const tchar* InPathShaderCache );

    FMeshShaderMap            shaders;            /**< Map of loaded shaders */
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FShaderMetaType*& InValue )
{
	if ( InArchive.IsLoading() )
	{
		std::wstring		shaderTypeName;
		InArchive << shaderTypeName;

		if ( !shaderTypeName.empty() )
		{
            InValue = FShaderManager::FindShaderType( shaderTypeName );
		}
	}
	else
	{
		InArchive << ( InValue ? InValue->GetName() : std::wstring() );
	}

	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FShaderMetaType*& InValue )
{
	check( InArchive.IsSaving() );
    InArchive << ( InValue ? InValue->GetName() : std::wstring() );
	return InArchive;
}

#endif // !TESTSHADER_H

