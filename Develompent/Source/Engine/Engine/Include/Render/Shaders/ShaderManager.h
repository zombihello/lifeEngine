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

#include "RHI/BaseRHI.h"
#include "Shader.h"
#include "ShaderCache.h"

/**
 * @brief A macro to declare a new shader type
 * 
 * @warning This should be called in the class body of the new shader type
 * @param[in] ShaderClass The name of the class representing an instance of the shader type
 */
#define DECLARE_SHADER_TYPE( ShaderClass ) \
	public: \
        static FShaderMetaType          staticType;             /**< Static meta type of shader */ \
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
 * @brief A macro to implement a shader type
 * 
 * @param[in] ShaderClass The name of the class representing an instance of the shader type
 * @param[in] SourceFilename Source file name of shader
 * @param[in] FunctionName Main function name in shader
 * @param[in] Frequency Frequency of shader
 */
#define IMPLEMENT_SHADER_TYPE( ShaderClass, SourceFilename, FunctionName, Frequency ) \
	FShaderMetaType       ShaderClass::staticType( \
		TEXT( #ShaderClass ), \
		SourceFilename, \
		FunctionName, \
		Frequency, \
		ShaderClass::ConstructSerializedInstance, \
		ShaderClass::ConstructCompiledInstance );

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

    /**
     * @brief Constructor
     * 
     * @param[in] InName Shader name
     * @param[in] InFileName File name of source
     * @param[in] InFunctionName Main function in shader
     * @param[in] InFrequency Frequency of shader
     * @param[in] InConstructSerializedInstance Pointer to static method for create object of shader for serialize
     * @param[in] InConstructCompiledInstance Pointer to static method for create object of shader for compiling
     */
    FShaderMetaType( const std::wstring& InName, const std::wstring& InFileName, const std::wstring& InFunctionName, EShaderFrequency InFrequency, FConstructSerializedInstance InConstructSerializedInstance, FConstructCompiledInstance InConstructCompiledInstance );

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

private:
    std::wstring                        name;                               /**< Name of shader */
    std::wstring                        fileName;                           /**< Source file name */
    std::wstring                        functionName;                       /**< Main function in shader */
    EShaderFrequency                    frequency;                          /**< Frequency of shader */
    FConstructSerializedInstance        ConstructSerializedInstance;        /**< Pointer to static method for create object of shader for serialize */
    FConstructCompiledInstance          ConstructCompiledInstance;          /**< Pointer to static method for create object of shader for compiling */
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
    void                                    Init();

    /**
     * Shutdown shader manager
     */
    void Shutdown();

    /**
     * Find instance of shader by name
     * 
     * @param[in] InShaderName Shader name
     * @return Return reference to shader
     */
    FORCEINLINE FShaderRef FindInstance( const std::wstring& InShaderName )
    {
		auto        itShader = shaders.find( InShaderName );
		if ( itShader == shaders.end() )
		{
			appErrorf( TEXT( "Shader %s not found in cache" ), InShaderName.c_str() );
			return nullptr;
		}

		return itShader->second;
    }

    /**
     * @brief Find instance of shader
     */
    template< typename TShaderClass >
    FORCEINLINE FShaderRef                  FindInstance()
    {
        return FindInstance( TShaderClass::staticType.GetName() );
    }

    /**
     * @brief Get path to shader dir
     * @return Return path to shader dir
     */
    static FORCEINLINE const tchar*         GetShaderDir()
    {
        return TEXT( "../../Engine/Shaders" );
    }

private:
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

    std::unordered_map< std::wstring, FShaderRef >            shaders;            /**< Map of loaded shaders */
};

#endif // !TESTSHADER_H

