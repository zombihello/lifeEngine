#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <qfileinfo.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include "Misc/EngineGlobals.h"
#include "Logger/LoggerMacros.h"
#include "Containers/StringConv.h"
#include "Render/RenderUtils.h"
#include "System/AssetsImport.h"
#include "System/BaseEngine.h"
#include "WorldEd.h"

//
// GENERIC
//

QString FHelperAssetImporter::MakeFilterOfSupportedExtensions( uint32 InFlags /* = ET_All */ )
{
	QString				result;
	std::wstring		allSupportedFormats;

	// Texture formats
	if ( InFlags & ET_Texture2D )
	{
		const std::vector< std::wstring >&		textureExtension = FTexture2DImporter::GetSupportedExtensions();
		if ( !textureExtension.empty() )
		{
			result += "Texture Formats (";
			for ( uint32 index = 0, count = textureExtension.size(); index < count; ++index )
			{
				std::wstring		format = FString::Format( TEXT( "*.%s%s" ), textureExtension[ index ].c_str(), index + 1 < count ? TEXT( ";" ) : TEXT( "" ) );
				result += format;
				allSupportedFormats += format;
			}
			result += ");;";
		}
	}

	// Audio formats
	if ( InFlags & ET_AudioBank )
	{
		const std::vector< std::wstring >&		audioExtnesion = FAudioBankImporter::GetSupportedExtensions();
		if ( !audioExtnesion.empty() )
		{
			allSupportedFormats += !allSupportedFormats.empty() ? TEXT( ";" ) : TEXT( "" );
			result += "Audio Formats (";
			for ( uint32 index = 0, count = audioExtnesion.size(); index < count; ++index )
			{
				std::wstring		format = FString::Format( TEXT( "*.%s%s" ), audioExtnesion[ index ].c_str(), index + 1 < count ? TEXT( ";" ) : TEXT( "" ) );
				result += format;
				allSupportedFormats += format;
			}
			result += ");;";
		}
	}

	// Static mesh formats
	if ( InFlags & ET_StaticMesh )
	{
		const std::vector< std::wstring >&		staticMeshExtnesion = FStaticMeshImporter::GetSupportedExtensions();
		if ( !staticMeshExtnesion.empty() )
		{
			allSupportedFormats += !allSupportedFormats.empty() ? TEXT( ";" ) : TEXT( "" );
			result += "Static Mesh Formats (";
			for ( uint32 index = 0, count = staticMeshExtnesion.size(); index < count; ++index )
			{
				std::wstring		format = FString::Format( TEXT( "*.%s%s" ), staticMeshExtnesion[ index ].c_str(), index + 1 < count ? TEXT( ";" ) : TEXT( "" ) );
				result += format;
				allSupportedFormats += format;
			}
			result += ");;";
		}
	}

	// All supported formats
	{
		result += FString::Format( TEXT( "All supported formats (%s)" ), allSupportedFormats.c_str() );
	}

	return result;
}

FHelperAssetImporter::EImportResult FHelperAssetImporter::Import( const QString& InPath, FPackage* InPackage, TAssetHandle<FAsset>& OutAsset, std::wstring& OutError, bool InIsForceImport /* = false */ )
{
	checkMsg( InPackage, TEXT( "For import package must be valid" ) );

	TSharedPtr<FAsset>	assetRef;
	QFileInfo			fileInfo( InPath );
	QString				assetName = fileInfo.baseName();
	std::wstring		fileExtension = fileInfo.suffix().toStdWString();

	// Check on already exist asset with name in package
	if ( !InIsForceImport && InPackage->IsExist( assetName.toStdWString() ) )
	{
		return IR_AlreadyExist;
	}

	// We import texture
	if ( FHelperAssetImporter::IsSupportedExtension( fileExtension, FTexture2DImporter::GetSupportedExtensions() ) )
	{
		assetRef = FTexture2DImporter::Import( appQtAbsolutePathToEngine( InPath ), OutError );
	}

	// We import audio bank
	else if ( FHelperAssetImporter::IsSupportedExtension( fileExtension, FAudioBankImporter::GetSupportedExtensions() ) )
	{
		assetRef = FAudioBankImporter::Import( appQtAbsolutePathToEngine( InPath ), OutError );
	}

	// We import static mesh
	else if ( FHelperAssetImporter::IsSupportedExtension( fileExtension, FStaticMeshImporter::GetSupportedExtensions() ) )
	{
		assetRef = FStaticMeshImporter::Import( appQtAbsolutePathToEngine( InPath ), OutError );
	}

	// Else this is unknown asset
	else
	{
		OutError = FString::Format( TEXT( "Unknown asset type '%s' for import" ), InPath.toStdWString().c_str() );
		LE_LOG( LT_Warning, LC_Editor, OutError.c_str() );
		return IR_Error;
	}

	// Add asset to package
	if ( assetRef )
	{
		LE_LOG( LT_Log, LC_Editor, TEXT( "Imported asset '%s' from '%s' to package '%s'" ), assetRef->GetAssetName().c_str(), InPath.toStdWString().c_str(), InPackage->GetName().c_str() );		
		OutAsset	= TAssetHandle<FAsset>( assetRef, MakeSharedPtr<FAssetReference>( assetRef->GetType(), assetRef->GetGUID() ) );
		InPackage->Add( OutAsset );
	}
	else
	{
		LE_LOG( LT_Warning, LC_Editor, TEXT( "Failed import asset from '%s' to package '%s'. Error: %s" ), InPath.toStdWString().c_str(), InPackage->GetName().c_str(), OutError.c_str() );
	}

	return assetRef ? IR_Seccussed : IR_Error;
}

bool FHelperAssetImporter::Reimport( const TAssetHandle<FAsset>& InAsset, std::wstring& OutError )
{
	check( InAsset.IsAssetValid() );
	bool		bResult = false;

	// If asset already is unloaded, we must exit from method
	TSharedPtr<FAsset>		assetRef = InAsset.ToSharedPtr();
	if ( !assetRef )
	{
		OutError = TEXT( "Asset already is unloaded" );
		return false;
	}

	switch ( assetRef->GetType() )
	{
	case AT_Texture2D:		bResult = FTexture2DImporter::Reimport( assetRef, OutError ); break;
	case AT_AudioBank:		bResult = FAudioBankImporter::Reimport( assetRef, OutError ); break;
	
	//
	// Insert your asset type in this place
	//

	default:				
		OutError	= TEXT( "Unsupported asset type for reimport" ); 
		bResult		= false;
		break;
	}

	if ( bResult )
	{
		LE_LOG( LT_Log, LC_Editor, TEXT( "Reimported asset '%s' from '%s'" ), assetRef->GetAssetName().c_str(), assetRef->GetAssetSourceFile().c_str() );
	}
	else
	{
		LE_LOG( LT_Warning, LC_Editor, TEXT( "Failed reimport asset '%s' from '%s'. Error %s" ), assetRef->GetAssetName().c_str(), assetRef->GetAssetSourceFile().c_str(), OutError.c_str() );
	}

	return bResult;
}

//
// TEXTURE 2D
//

TSharedPtr<FTexture2D> FTexture2DImporter::Import( const std::wstring& InPath, std::wstring& OutError )
{
	// Getting file name from path if InName is empty
	std::wstring		filename = InPath;

	appNormalizePathSeparators( filename );
	std::size_t			pathSeparatorPos = filename.find_last_of( PATH_SEPARATOR );
	if ( pathSeparatorPos != std::string::npos )
	{
		filename.erase( 0, pathSeparatorPos + 1 );
	}

	std::size_t dotPos = filename.find_last_of( TEXT( "." ) );
	if ( dotPos != std::string::npos )
	{
		filename.erase( dotPos, filename.size() + 1 );
	}

	// Import new texture 2D
	TSharedPtr<FTexture2D>		texture2DRef = MakeSharedPtr<FTexture2D>();
	texture2DRef->SetAssetName( filename );
	texture2DRef->SetAssetSourceFile( InPath );
	return Reimport( texture2DRef, OutError ) ? texture2DRef : nullptr;
}

bool FTexture2DImporter::Reimport( const TSharedPtr<FTexture2D>& InTexture2D, std::wstring& OutError )
{
	check( InTexture2D );

	std::wstring		sourceFile = InTexture2D->GetAssetSourceFile();
	if ( sourceFile.empty() )
	{
		OutError = TEXT( "Source file is empty" );
		return false;
	}

	// Loading data from image
	int				numComponents = 0;
	uint32			sizeX = 0;
	uint32			sizeY = 0;
	void*			data = stbi_load( TCHAR_TO_ANSI( sourceFile.c_str() ), ( int* ) &sizeX, ( int* ) &sizeY, &numComponents, 4 );
	if ( !data )
	{
		OutError = FString::Format( TEXT( "Failed open file '%s'" ), sourceFile.c_str() );
		return false;
	}

	// Set new data in texture
	std::vector< byte >		tempData;
	tempData.resize( sizeX * sizeY * GPixelFormats[ PF_A8R8G8B8 ].blockBytes );
	memcpy( tempData.data(), data, tempData.size() );
	InTexture2D->SetData( PF_A8R8G8B8, sizeX, sizeY, tempData );

	// Clean up all data
	stbi_image_free( data );
	return true;
}

//
// AUDIO BANK
//

TSharedPtr<FAudioBank> FAudioBankImporter::Import( const std::wstring& InPath, std::wstring& OutError )
{
	// Getting file name from path if InName is empty
	std::wstring		filename = InPath;

	appNormalizePathSeparators( filename );
	std::size_t			pathSeparatorPos = filename.find_last_of( PATH_SEPARATOR );
	if ( pathSeparatorPos != std::string::npos )
	{
		filename.erase( 0, pathSeparatorPos + 1 );
	}

	std::size_t dotPos = filename.find_last_of( TEXT( "." ) );
	if ( dotPos != std::string::npos )
	{
		filename.erase( dotPos, filename.size() + 1 );
	}

	// Import new audio bank
	TSharedPtr<FAudioBank>		audioBankRef = MakeSharedPtr<FAudioBank>();
	audioBankRef->SetAssetName( filename );
	audioBankRef->SetAssetSourceFile( InPath );
	return Reimport( audioBankRef, OutError ) ? audioBankRef : nullptr;
}

bool FAudioBankImporter::Reimport( const TSharedPtr<FAudioBank>& InAudioBank, std::wstring& OutError )
{
	check( InAudioBank );
	InAudioBank->SetSourceOGGFile( InAudioBank->GetAssetSourceFile() );
	return true;
}

TSharedPtr<FStaticMesh> FStaticMeshImporter::Import( const std::wstring& InPath, std::wstring& OutError )
{
	// Getting file name from path if InName is empty
	std::wstring		filename = InPath;

	appNormalizePathSeparators( filename );
	std::size_t			pathSeparatorPos = filename.find_last_of( PATH_SEPARATOR );
	if ( pathSeparatorPos != std::string::npos )
	{
		filename.erase( 0, pathSeparatorPos + 1 );
	}

	std::size_t dotPos = filename.find_last_of( TEXT( "." ) );
	if ( dotPos != std::string::npos )
	{
		filename.erase( dotPos, filename.size() + 1 );
	}

	// Import new static mesh
	TSharedPtr<FStaticMesh>		staticMeshRef = MakeSharedPtr<FStaticMesh>();
	staticMeshRef->SetAssetName( filename );
	staticMeshRef->SetAssetSourceFile( InPath );
	return Reimport( staticMeshRef, OutError ) ? staticMeshRef : nullptr;
}

bool FStaticMeshImporter::Reimport( const TSharedPtr<FStaticMesh>& InStaticMesh, std::wstring& OutError )
{
	// Loading mesh with help Assimp
	check( InStaticMesh );
	Assimp::Importer		aiImport;
	const aiScene*			aiScene = aiImport.ReadFile(
		TCHAR_TO_ANSI( InStaticMesh->GetAssetSourceFile().c_str() ),
		aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights | aiProcess_Triangulate );
	if ( !aiScene )
	{
		OutError = FString::Format( TEXT( "Failed open static mesh file. Error: %s" ), ANSI_TO_TCHAR( aiImport.GetErrorString() ) );
		return false;
	}

	// Fill array meshes from Assimp scene
	FAiMeshesMap			meshes;
	ProcessNode( aiScene->mRootNode, aiScene, meshes );
	if ( meshes.empty() )
	{
		OutError = FString::Format( TEXT( "In file '%s' not found meshes" ), InStaticMesh->GetAssetSourceFile().c_str() );
		aiImport.FreeScene();
		return false;
	}

	// TODO BS yehor.pohuliaka - I'm not sure how correct this code is, it may need to be rewritten
	// Go through the material ID, take the mesh and write its vertices, and indices
	// to the shared buffer
	std::vector< FStaticMeshVertexType >		verteces;
	std::vector< uint32 >						indeces;
	std::vector< FStaticMeshSurface >			surfaces;
	std::vector< TAssetHandle<FMaterial> >		materials;
	for ( auto itRoot = meshes.begin(), itRootEnd = meshes.end(); itRoot != itRootEnd; ++itRoot )
	{
		FStaticMeshSurface							surface;
		appMemzero( &surface, sizeof( FStaticMeshSurface ) );

		surface.firstIndex = indeces.size();
		surface.materialID = materials.size();

		for ( auto itMesh = itRoot->second.begin(), itMeshEnd = itRoot->second.end(); itMesh != itMeshEnd; ++itMesh )
		{
			std::vector< FStaticMeshVertexType >	vertexBuffer;
			FStaticMeshVertexType					vertex;
			aiMesh* mesh = ( *itMesh ).mesh;
			appMemzero( &vertex, sizeof( FStaticMeshVertexType ) );

			// Prepare the vertex buffer.
			// If the vertices of the mesh do not fit into the buffer, then
			// expand it
			if ( vertexBuffer.size() < mesh->mNumVertices )
			{
				vertexBuffer.resize( vertexBuffer.size() + mesh->mNumVertices );
			}

			// Read all verteces
			for ( uint32 index = 0; index < mesh->mNumVertices; ++index )
			{
				aiVector3D		tempVector = ( *itMesh ).transformation * mesh->mVertices[ index ];
				vertex.position.x = tempVector.x;
				vertex.position.y = tempVector.y;
				vertex.position.z = tempVector.z;
				vertex.position.w = 1.f;

				tempVector = ( aiMatrix3x3 ) ( *itMesh ).transformation * mesh->mNormals[ index ];
				vertex.normal.x = tempVector.x;
				vertex.normal.y = tempVector.y;
				vertex.normal.z = tempVector.z;
				vertex.normal.w = 0.f;

				if ( mesh->mTangents )
				{
					tempVector = ( aiMatrix3x3 ) ( *itMesh ).transformation * mesh->mTangents[ index ];
					vertex.tangent.x = tempVector.x;
					vertex.tangent.y = tempVector.y;
					vertex.tangent.z = tempVector.z;
					vertex.tangent.w = 0.f;
				}

				if ( mesh->mBitangents )
				{
					tempVector = ( aiMatrix3x3 ) ( *itMesh ).transformation * mesh->mBitangents[ index ];
					vertex.binormal.x = tempVector.x;
					vertex.binormal.y = tempVector.y;
					vertex.binormal.z = tempVector.z;
					vertex.binormal.w = 0.f;
				}

				if ( mesh->mTextureCoords[ 0 ] )
				{
					tempVector = mesh->mTextureCoords[ 0 ][ index ];
					vertex.texCoord.x = tempVector.x;
					vertex.texCoord.y = tempVector.y;
				}

				vertexBuffer[ index ] = vertex;
			}

			// Read all indeces
			for ( uint32 index = 0; index < mesh->mNumFaces; ++index )
			{
				aiFace* face = &mesh->mFaces[ index ];
				for ( uint32 indexVertex = 0; indexVertex < face->mNumIndices; ++indexVertex )
				{
					uint32		index = face->mIndices[ indexVertex ];
					auto		it = find( verteces.begin(), verteces.end(), vertexBuffer[ index ] );

					// Look for the vertex index in the shared vertex buffer,
					// if not found, add the vertex to the buffer,
					// and then write its index
					if ( it == verteces.end() )
					{
						indeces.push_back( verteces.size() );
						verteces.push_back( vertexBuffer[ index ] );
					}
					else
					{
						indeces.push_back( it - verteces.begin() );
					}
				}
			}
		}

		// We process material
		if ( itRoot->first < aiScene->mNumMaterials )
		{
			materials.push_back( GEngine->GetDefaultMaterial() );
		}
		else
		{
			LE_LOG( LT_Warning, LC_General, TEXT( "Material with id %i large. Surface not created" ), itRoot->first );
			continue;
		}

		surface.numPrimitives = ( indeces.size() - surface.firstIndex ) / 3.f;		// 1 primitive = 3 indeces (triangles)
		surfaces.push_back( surface );
	}

	// Clean up all data
	aiImport.FreeScene();
	return true;
}

void FStaticMeshImporter::ProcessNode( aiNode* InNode, const aiScene* InScene, FAiMeshesMap& OutMeshes )
{
	for ( uint32 index = 0; index < InNode->mNumMeshes; ++index )
	{
		aiMesh* mesh = InScene->mMeshes[ InNode->mMeshes[ index ] ];
		OutMeshes[ mesh->mMaterialIndex ].push_back( FAiMesh( InNode->mTransformation, mesh ) );
	}

	for ( uint32 index = 0; index < InNode->mNumChildren; ++index )
	{
		ProcessNode( InNode->mChildren[ index ], InScene, OutMeshes );
	}
}

const std::vector<std::wstring>& FStaticMeshImporter::GetSupportedExtensions()
{
	// If supported extension not cached - get extensions from aiImport
	static bool								bCached = false;
	static std::vector< std::wstring >		supportedExtensions;
	if ( !bCached )
	{
		Assimp::Importer		aiImporter;
		std::string				extensionList;
		aiImporter.GetExtensionList( extensionList );
		
		// Parse string in format '*.<format>;'
		uint32		startFormat	= 0;
		for ( uint32 index = 0, count = extensionList.size(); index < count; ++index )
		{
			achar	ch = extensionList[ index ];
			if ( ch == '*' || ch == '.' )
			{
				startFormat = 0;
				continue;
			}

			if ( startFormat == 0 )
			{
				startFormat = index;
			}

			// Copy extension to array
			if ( ch == ';' )
			{
				std::string			ANSIString;
				ANSIString.resize( index - startFormat );
				memcpy( ANSIString.data(), &extensionList[ startFormat ], ANSIString.size() );
				supportedExtensions.push_back( ANSI_TO_TCHAR( ANSIString.c_str() ) );
			}
		}

		bCached = true;
	}

	return supportedExtensions;
}