#include <vector>
#include <unordered_map>

#include "Misc/CoreGlobals.h"
#include "Commandlets/ImportMaterialCommandlet.h"
#include "System/Archive.h"
#include "System/BaseFileSystem.h"
#include "System/Config.h"
#include "System/Package.h"
#include "Render/Material.h"
#include "Logger/LoggerMacros.h"

IMPLEMENT_CLASS( LImportMaterialCommandlet )

bool LImportMaterialCommandlet::Main( const std::wstring& InCommand )
{
	std::wstring			srcFilename;
	std::wstring			pathToDir;
	std::wstring			outputPackage;

	// Parse arguments
	{
		std::vector< std::wstring >		tokens;
		std::vector< std::wstring >		switches;
		appParseCommandLine( InCommand.c_str(), tokens, switches );

		for ( uint32 index = 0, count = ( uint32 )switches.size(); index < count; ++index )
		{
			const std::wstring&		param = switches[ index ];
			const std::wstring&		token = tokens[ index + 2 ];

			// Source filename
			if ( param == TEXT( "src" ) || param == TEXT( "source" ) )
			{
				srcFilename = token;
			}

			// Path to directory with source files
			if ( param == TEXT( "dir" ) || param == TEXT( "directory" ) )
			{
				pathToDir = token;
			}

			// Output package
			else if ( param == TEXT( "pak" ) || param == TEXT( "package" ) )
			{
				outputPackage = token;
			}
		}
	}

	checkMsg( ( srcFilename.empty() || pathToDir.empty() ) || outputPackage.empty(), TEXT( "Not entered source file or directory and destination package for saving" ) );
	checkMsg( !srcFilename.empty() || !pathToDir.empty(), TEXT( "Setted parameters '-src' and '-dir'. One must be displayed" ) );

	// Getting all source files for import
	std::vector< std::wstring >			srcFiles;
	if ( !srcFilename.empty() )
	{
		appNormalizePathSeparators( srcFilename );

		// Set path to directory
		pathToDir = srcFilename;
		std::size_t			pathSeparatorPos = pathToDir.find_last_of( PATH_SEPARATOR );
		if ( pathSeparatorPos != std::string::npos )
		{
			pathToDir.erase( pathSeparatorPos, pathToDir.size() );
		}

		// Get filename with extension
		if ( pathSeparatorPos != std::string::npos )
		{
			srcFilename.erase( 0, pathSeparatorPos + 1 );
		}
		srcFiles.push_back( srcFilename );
	}
	else
	{
		srcFiles = GFileSystem->FindFiles( pathToDir, true, false );
	}

	// Import all materials
	FPackageRef						package = GPackageManager->LoadPackage( outputPackage, true );
	std::vector< FMaterialRef >		importedMaterials;
	for ( uint32 index = 0, count = srcFiles.size(); index < count; ++index )
	{
		const std::wstring&		file = srcFiles[ index ];	
		std::wstring			filename = file;
		{
			std::size_t			dotPos		= file.find_last_of( TEXT( "." ) );
			std::wstring		extension = file;
			extension.erase( 0, dotPos + 1 );
			
			// We import materials only with extension 'LMT'
			if ( !IsSupportedExtension( extension ) )
			{
				continue;
			}

			// Getting filename
			appNormalizePathSeparators( filename );
			std::size_t			pathSeparatorPos = filename.find_last_of( PATH_SEPARATOR );
			if ( pathSeparatorPos != std::string::npos )
			{
				filename.erase( 0, pathSeparatorPos + 1 );
			}

			dotPos = filename.find_last_of( TEXT( "." ) );
			if ( dotPos != std::string::npos )
			{
				filename.erase( dotPos, filename.size() + 1 );
			}
		}

		// Import new material and add to package
		FMaterialRef		material = ImportMaterial( pathToDir + PATH_SEPARATOR + file, filename );
		check( material );
		importedMaterials.push_back( material );
		package->Add( material );

		bool	result = package->Save( outputPackage );
		check( result );
	}

	// Saving package
	return true;
}

FMaterialRef LImportMaterialCommandlet::ImportMaterial( const std::wstring& InPath, const std::wstring& InName )
{
	// Parse material in JSON format
	FConfig		lmtMaterial;
	{
		FArchive*		arMaterial = GFileSystem->CreateFileReader( InPath, AR_NoFail );
		lmtMaterial.Serialize( *arMaterial );
		delete arMaterial;
	}

	// Getting general data
	bool				bIsTwoSided		= lmtMaterial.GetValue( TEXT( "Material" ), TEXT( "IsTwoSided" ) ).GetBool();
	bool				bIsWireframe	= lmtMaterial.GetValue( TEXT( "Material" ), TEXT( "IsWireframe" ) ).GetBool();

	// Getting usage flags
	uint32				usageFlags = MU_AllMeshes;
	{
		FConfigValue	configVarUsageFlags = lmtMaterial.GetValue( TEXT( "Material" ), TEXT( "Usage" ) );
		if ( configVarUsageFlags.GetType() == FConfigValue::T_Object )
		{
			// If exist Usage object, we reset usage flags to MU_None
			usageFlags = MU_None;

			// Get usage flags
			FConfigObject		configObjUsageFlags		= configVarUsageFlags.GetObject();
			bool				bStaticMesh				= configObjUsageFlags.GetValue( TEXT( "StaticMesh" ) ).GetBool();
			bool				bSprite					= configObjUsageFlags.GetValue( TEXT( "Sprite" ) ).GetBool();

			// Combine all flags
			if ( bStaticMesh )
			{
				usageFlags |= MU_StaticMesh;
			}

			if ( bSprite )
			{
				usageFlags |= MU_Sprite;
			}
		}
		else
		{
			LE_LOG( LT_Warning, LC_Commandlet, TEXT( "Not correct type in block 'Material::Usage', must be is object. Usage flags setted to MU_AllMeshes" ) );
		}
	}

	// Getting shader types
	std::vector< FShaderMetaType* >		shaderMetaTypes;
	{
		FConfigValue	configVarShadersType = lmtMaterial.GetValue( TEXT( "Material" ), TEXT( "ShadersType" ) );
		check( configVarShadersType.GetType() == FConfigValue::T_Array );

		std::vector< FConfigValue >		configValues = configVarShadersType.GetArray();
		for ( uint32 index = 0, count = configValues.size(); index < count; ++index )
		{
			std::wstring		shaderMetaTypeName	= configValues[ index ].GetString();
			FShaderMetaType*	shaderMetaType		= GShaderManager->FindShaderType( shaderMetaTypeName );
			if ( !shaderMetaType )
			{
				LE_LOG( LT_Error, LC_Commandlet, TEXT( "Shader '%s' not found in engine" ), shaderMetaTypeName.c_str() );
				throw;
			}

			shaderMetaTypes.push_back( shaderMetaType );
		}
	}

	// Getting scalar parameters
	std::unordered_map< std::wstring, float >		scalarParameters;
	{
		FConfigValue	configVarScalarParameters = lmtMaterial.GetValue( TEXT( "Material" ), TEXT( "ScalarParameters" ) );
		if ( configVarScalarParameters.IsValid() )
		{
			check( configVarScalarParameters.GetType() == FConfigValue::T_Array );

			std::vector< FConfigValue >		configObjects = configVarScalarParameters.GetArray();
			for ( uint32 index = 0, count = configObjects.size(); index < count; ++index )
			{
				FConfigObject	configObject = configObjects[ index ].GetObject();
				std::wstring	name = configObject.GetValue( TEXT( "Name" ) ).GetString();
				float			value = configObject.GetValue( TEXT( "Value" ) ).GetNumber();
				scalarParameters.insert( std::make_pair( name, value ) );
			}
		}
	}

	// Getting texture parameters
	std::unordered_map< std::wstring, FTexture2DRef >		textureParameters;
	{
		FConfigValue	configVarTextureParameters = lmtMaterial.GetValue( TEXT( "Material" ), TEXT( "TextureParameters" ) );
		if ( configVarTextureParameters.IsValid() )
		{
			check( configVarTextureParameters.GetType() == FConfigValue::T_Array );

			std::vector< FConfigValue >		configObjects = configVarTextureParameters.GetArray();
			for ( uint32 index = 0, count = configObjects.size(); index < count; ++index )
			{
				FConfigObject		configObject = configObjects[ index ].GetObject();
				std::wstring		name = configObject.GetValue( TEXT( "Name" ) ).GetString();
				std::wstring		assetReference = configObject.GetValue( TEXT( "AssetReference" ) ).GetString();
				FTexture2DRef		texture = GPackageManager->FindAsset( assetReference, AT_Unknown );

				check( texture );
				textureParameters.insert( std::make_pair( name, texture ) );
			}
		}
	}

	// Create material and saving to package
	FMaterialRef		material = new FMaterial();
	material->SetAssetName( InName );
	material->SetTwoSided( bIsTwoSided );
	material->SetWireframe( bIsWireframe );
	material->SetUsageFlags( usageFlags );

	// Set shaders
	for ( uint32 index = 0, count = shaderMetaTypes.size(); index < count; ++index )
	{
		material->SetShader( *shaderMetaTypes[ index ] );
	}

	// Set scalar parameters
	for ( auto it = scalarParameters.begin(), itEnd = scalarParameters.end(); it != itEnd; ++it )
	{
		material->SetScalarParameterValue( it->first, it->second );
	}

	// Set texture parameters
	for ( auto it = textureParameters.begin(), itEnd = textureParameters.end(); it != itEnd; ++it )
	{
		material->SetTextureParameterValue( it->first, it->second );
	}

	return material;
}