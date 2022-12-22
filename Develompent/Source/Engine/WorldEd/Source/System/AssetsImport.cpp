#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "System/AssetsImport.h"
#include "System/BaseFileSystem.h"
#include "Render/RenderUtils.h"
#include "WorldEd.h"

CStaticMeshImportSettingsDialog::SImportSettings		CStaticMeshImporter::importSettings;

bool CTexture2DImporter::Import( const std::wstring& InPath, std::vector<TSharedPtr<CAsset>>& OutResult, std::wstring& OutError )
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
	TSharedPtr<CTexture2D>		texture2DRef = MakeSharedPtr<CTexture2D>();
	texture2DRef->SetAssetName( filename );
	texture2DRef->SetAssetSourceFile( InPath );
	
	if ( Reimport( texture2DRef, OutError ) )
	{
		OutResult.push_back( texture2DRef );
		return true;
	}
	
	return false;
}

bool CTexture2DImporter::Reimport( const TSharedPtr<CAsset>& InTexture2D, std::wstring& OutError )
{
	TSharedPtr<CTexture2D>		texture2D = InTexture2D;
	check( texture2D );

	std::wstring		sourceFile = texture2D->GetAssetSourceFile();
	if ( sourceFile.empty() )
	{
		OutError = TEXT( "Source file is empty" );
		return false;
	}

	// Loading data from image
	int		numComponents = 0;
	uint32	sizeX	= 0;
	uint32	sizeY	= 0;
	void*	data	= stbi_load( TCHAR_TO_ANSI( sourceFile.c_str() ), ( int* )&sizeX, ( int* )&sizeY, &numComponents, 4 );
	if ( !data )
	{
		OutError = CString::Format( TEXT( "Failed open file '%s'" ), sourceFile.c_str() );
		return false;
	}

	// Set new data in texture
	std::vector<byte>		tempData;
	tempData.resize( sizeX * sizeY * GPixelFormats[PF_A8R8G8B8].blockBytes );
	memcpy( tempData.data(), data, tempData.size() );
	texture2D->SetData( PF_A8R8G8B8, sizeX, sizeY, tempData );

	// Clean up all data
	stbi_image_free( data );

	// Broadcast event of reimport/reloaded asset
	std::vector<TSharedPtr<CAsset>>		reimportedAssets{ texture2D };
	SEditorDelegates::onAssetsReloaded.Broadcast( reimportedAssets );

	return true;
}

//
// AUDIO BANK
//

bool CAudioBankImporter::Import( const std::wstring& InPath, std::vector<TSharedPtr<CAsset>>& OutResult, std::wstring& OutError )
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
	TSharedPtr<CAudioBank>		audioBankRef = MakeSharedPtr<CAudioBank>();
	audioBankRef->SetAssetName( filename );
	audioBankRef->SetAssetSourceFile( InPath );
	
	if ( Reimport( audioBankRef, OutError ) )
	{
		OutResult.push_back( audioBankRef );
		return true;
	}

	return false;
}

bool CAudioBankImporter::Reimport( const TSharedPtr<CAsset>& InAudioBank, std::wstring& OutError )
{
	TSharedPtr<CAudioBank>		audioBank = InAudioBank;
	check( audioBank );
	audioBank->SetOGGFile( audioBank->GetAssetSourceFile() );

	// Broadcast event of reimport/reloaded asset
	std::vector<TSharedPtr<CAsset>>		reimportedAssets{ InAudioBank };
	SEditorDelegates::onAssetsReloaded.Broadcast( reimportedAssets );

	return GFileSystem->IsExistFile( audioBank->GetAssetSourceFile() );
}

//
// STATIC MESH
//

bool CStaticMeshImporter::Import( const std::wstring& InPath, std::vector<TSharedPtr<CAsset>>& OutResult, std::wstring& OutError )
{
	// Parse meshes data from file
	std::vector<SMeshData>		meshes;
	if ( !ParseMeshes( InPath, meshes, OutError) )
	{
		return false;
	}

	// Combine meshes to one
	if ( importSettings.bCombineMeshes )
	{
		// Separate meshes by material id
		std::unordered_map<uint32, std::vector<uint32>>		meshesMap;		// Key - material id, Item - Index to mesh data in 'meshes'
		for ( uint32 index = 0, count = meshes.size(); index < count; ++index )
		{
			const SMeshData&	meshData = meshes[index];
			meshesMap[meshData.materialId].push_back( index );
		}

		// Combine meshes
		std::vector<SStaticMeshVertexType>		verteces;
		std::vector<uint32>						indeces;
		std::vector<SStaticMeshSurface>			surfaces;
		std::vector<TAssetHandle<CMaterial>>	materials;	
		for ( auto itMaterial = meshesMap.begin(), itMaterialEnd = meshesMap.end(); itMaterial != itMaterialEnd; ++itMaterial )
		{
			SStaticMeshSurface		surface;
			appMemzero( &surface, sizeof( SStaticMeshSurface ) );

			surface.firstIndex		= indeces.size();
			surface.materialID		= materials.size();

			for ( uint32 indexMesh = 0, countMeshes = itMaterial->second.size(); indexMesh < countMeshes; ++indexMesh )
			{
				const SMeshData&	meshData = meshes[itMaterial->second[indexMesh]];

				// Copy new verteces
				uint32		offsetVerteces = verteces.size();
				verteces.resize( verteces.size() + meshData.verteces.size() );
				memcpy( verteces.data() + offsetVerteces, meshData.verteces.data(), sizeof( SStaticMeshVertexType ) * meshData.verteces.size() );

				// Copy new indeces
				uint32		offsetIndeces = indeces.size();
				indeces.resize( indeces.size() + meshData.indeces.size() );
				for ( uint32 indexVertex = 0, count = meshData.indeces.size(); indexVertex < count; ++indexVertex )
				{
					indeces[offsetIndeces + indexVertex] = offsetVerteces + meshData.indeces[indexVertex];
				}
			}

			surface.numPrimitives = ( indeces.size() - surface.firstIndex ) / 3.f;		// 1 primitive = 3 indeces (triangles)
			
			materials.push_back( GEngine->GetDefaultMaterial() );
			surfaces.push_back( surface );
		}

		TSharedPtr<CStaticMesh>		staticMesh = MakeSharedPtr<CStaticMesh>();
		staticMesh->SetAssetName( CFilename( InPath ).GetBaseFilename() );
		staticMesh->SetAssetSourceFile( InPath );
		staticMesh->SetData( verteces, indeces, surfaces, materials );
		OutResult.push_back( staticMesh );
	}
	// Otherwise import separated meshes
	else
	{
		for ( uint32 index = 0, count = meshes.size(); index < count; ++index )
		{
			const SMeshData&			meshData	= meshes[index];
			TSharedPtr<CStaticMesh>		staticMesh	= MakeSharedPtr<CStaticMesh>();
			staticMesh->SetAssetName( meshData.name );
			staticMesh->SetAssetSourceFile( InPath + TEXT( "?" ) + meshData.name );

			std::vector<SStaticMeshSurface>			surfaces;
			std::vector<TAssetHandle<CMaterial>>	materials;
			surfaces.push_back( meshData.surface );
			materials.push_back( meshData.material );
			staticMesh->SetData( meshData.verteces, meshData.indeces, surfaces, materials );
			OutResult.push_back( staticMesh );
		}
	}

	return true;
}

void CStaticMeshImporter::ShowImportSettings( class CImGUILayer* InOwner, class CEvent* InEvent, CAssetFactory::EResultShowImportSettings& OutResult )
{
	check( InOwner && InEvent );
	TSharedPtr<CStaticMeshImportSettingsDialog>			popup = InOwner->OpenPopup<CStaticMeshImportSettingsDialog>();
	popup->OnResume().Add( [&]( CAssetFactory::EResultShowImportSettings InResult, const CStaticMeshImportSettingsDialog::SImportSettings& InImportSettings )
						   {					   
							   if ( InResult != CAssetFactory::RSIS_Cancel )
							   {
								   importSettings = InImportSettings;
							   }

							   OutResult = InResult;
							   InEvent->Trigger();
						   } );
	InEvent->Wait();
}

bool CStaticMeshImporter::Reimport( const TSharedPtr<CAsset>& InStaticMesh, std::wstring& OutError )
{
	TSharedPtr<CStaticMesh>		staticMesh = InStaticMesh;
	check( staticMesh );

	// Parse meshes data from file
	std::vector<SMeshData>		meshes;
	if ( !ParseMeshes( staticMesh->GetAssetSourceFile(), meshes, OutError ) )
	{
		return false;
	}

	check( meshes.size() == 1 );		// We support reimport only one mesh
	
	const SMeshData&						meshData = meshes[0];
	std::vector<SStaticMeshSurface>			surfaces;
	std::vector<TAssetHandle<CMaterial>>	materials;
	surfaces.push_back( meshData.surface );
	materials.push_back( meshData.material );
	staticMesh->SetData( meshData.verteces, meshData.indeces, surfaces, materials );

	// Broadcast event of reimport/reloaded asset
	std::vector< TSharedPtr<CAsset> >		reimportedAssets{ staticMesh };
	SEditorDelegates::onAssetsReloaded.Broadcast( reimportedAssets );

	return true;
}

void CStaticMeshImporter::ProcessNode( aiNode* InNode, const aiScene* InScene, AiMeshesMap_t& OutMeshes )
{
	for ( uint32 index = 0; index < InNode->mNumMeshes; ++index )
	{
		aiMesh* mesh = InScene->mMeshes[InNode->mMeshes[index]];
		OutMeshes[mesh->mMaterialIndex].push_back( SAiMesh( InNode->mTransformation, mesh ) );
	}

	for ( uint32 index = 0; index < InNode->mNumChildren; ++index )
	{
		ProcessNode( InNode->mChildren[index], InScene, OutMeshes );
	}
}

bool CStaticMeshImporter::ParseMeshes( const std::wstring& InPath, std::vector<SMeshData>& OutResult, std::wstring& OutError )
{
	// Parse path and mesh name
	std::wstring	path;
	std::wstring	meshName;
	{
		std::size_t		separatePos = InPath.find_last_of( TEXT( '?' ) );
		if ( separatePos == std::wstring::npos )
		{
			path = InPath;
		}
		else
		{
			path.resize( separatePos );
			memcpy( path.data(), InPath.data(), sizeof( std::wstring::value_type ) * path.size() );

			++separatePos;
			if ( !importSettings.bCombineMeshes && separatePos < InPath.size() )
			{
				meshName.resize( InPath.size() - separatePos );
				memcpy( meshName.data(), InPath.data() + separatePos, sizeof( std::wstring::value_type ) * meshName.size() );
			}
		}
	}

	// Loading mesh with help Assimp
	Assimp::Importer		aiImport;
	const aiScene*			aiScene = aiImport.ReadFile( TCHAR_TO_ANSI( path.c_str() ), aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights | aiProcess_JoinIdenticalVertices );
	if ( !aiScene )
	{
		OutError = CString::Format( TEXT( "Failed open static mesh file. Error: %s" ), ANSI_TO_TCHAR( aiImport.GetErrorString() ) );
		return false;
	}

	// Fill array meshes from Assimp scene
	AiMeshesMap_t			aiMeshes;
	ProcessNode( aiScene->mRootNode, aiScene, aiMeshes );
	if ( aiMeshes.empty() )
	{
		OutError = CString::Format( TEXT( "In file '%s' not found meshes" ), path.c_str() );
		aiImport.FreeScene();
		return false;
	}

	// Parse meshes
	for ( auto itMaterial = aiMeshes.begin(), itMaterialEnd = aiMeshes.end(); itMaterial != itMaterialEnd; ++itMaterial )
	{
		for ( auto itMesh = itMaterial->second.begin(), itMeshEnd = itMaterial->second.end(); itMesh != itMeshEnd; ++itMesh )
		{
			aiMesh*			mesh		= ( *itMesh ).mesh;
			std::wstring	aiName		= ANSI_TO_TCHAR( mesh->mName.C_Str() );
			if ( !meshName.empty() && aiName != meshName )
			{
				continue;
			}

			SStaticMeshVertexType	vertex;
			appMemzero( &vertex, sizeof( SStaticMeshVertexType ) );

			SMeshData		meshData;
			appMemzero( &meshData.surface, sizeof( SStaticMeshSurface ) );
			meshData.name		= aiName;
			meshData.materialId = itMaterial->first;
			
			// Prepare the vertex buffer
			meshData.verteces.resize(  mesh->mNumVertices );

			// Read all verteces
			for ( uint32 index = 0; index < mesh->mNumVertices; ++index )
			{
				aiVector3D		tempVector = ( *itMesh ).transformation * mesh->mVertices[index];
				ChangeAxisUpInVector( tempVector, importSettings.axisUp );

				vertex.position.x = tempVector.x;
				vertex.position.y = tempVector.y;
				vertex.position.z = tempVector.z;
				vertex.position.w = 1.f;

				tempVector = ( aiMatrix3x3 )( *itMesh ).transformation * mesh->mNormals[index];
				ChangeAxisUpInVector( tempVector, importSettings.axisUp );

				vertex.normal.x = tempVector.x;
				vertex.normal.y = tempVector.y;
				vertex.normal.z = tempVector.z;
				vertex.normal.w = 0.f;

				if ( mesh->mTangents )
				{
					tempVector = ( aiMatrix3x3 )( *itMesh ).transformation * mesh->mTangents[index];
					ChangeAxisUpInVector( tempVector, importSettings.axisUp );

					vertex.tangent.x = tempVector.x;
					vertex.tangent.y = tempVector.y;
					vertex.tangent.z = tempVector.z;
					vertex.tangent.w = 0.f;
				}

				if ( mesh->mBitangents )
				{
					tempVector = ( aiMatrix3x3 )( *itMesh ).transformation * mesh->mBitangents[index];
					ChangeAxisUpInVector( tempVector, importSettings.axisUp );

					vertex.binormal.x = tempVector.x;
					vertex.binormal.y = tempVector.y;
					vertex.binormal.z = tempVector.z;
					vertex.binormal.w = 0.f;
				}

				if ( mesh->mTextureCoords[0] )
				{
					tempVector = mesh->mTextureCoords[0][index];
					vertex.texCoord.x = tempVector.x;
					vertex.texCoord.y = tempVector.y;
				}

				meshData.verteces[index] = vertex;
			}

			// Read all indeces
			for ( uint32 index = 0; index < mesh->mNumFaces; ++index )
			{
				aiFace*		face = &mesh->mFaces[index];
				for ( uint32 indexVertex = 0; indexVertex < face->mNumIndices; ++indexVertex )
				{
					meshData.indeces.push_back( face->mIndices[indexVertex] );
				}
			}

			meshData.material					= GEngine->GetDefaultMaterial();
			meshData.surface.numPrimitives		= meshData.indeces.size() / 3;		// 1 primitive = 3 indeces (triangles)
			OutResult.push_back( meshData );
		}
	}

	aiImport.FreeScene();
	return true;
}

const std::vector<std::wstring>& CStaticMeshImporter::GetSupportedExtensions()
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
		uint32		startFormat = 0;
		for ( uint32 index = 0, count = extensionList.size(); index < count; ++index )
		{
			achar	ch = extensionList[index];
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
				memcpy( ANSIString.data(), &extensionList[startFormat], ANSIString.size() );
				supportedExtensions.push_back( ANSI_TO_TCHAR( ANSIString.c_str() ) );
			}
		}

		bCached = true;
	}

	return supportedExtensions;
}