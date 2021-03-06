#include "..\..\3D\ModelMesh.h"
#include "..\..\System\MaterialManager.h"
#include <HaffmanCode.h>

//-------------------------------------------------------------------------//

le::Material::Material( )
{
	Texture = 0;

	Ambient = Color( 0.2f, 0.2f, 0.2f, 1.0f );
	Diffuse = Color( 1.0f, 1.0f, 1.0f, 1.0f );
	Specular = Color( 0.0f, 0.0f, 0.0f, 1.0f );
	Emission = Color( 0.0f, 0.0f, 0.0f, 1.0f );
	fShininess = 0.0f;
}

//-------------------------------------------------------------------------//

bool le::VBO_ModelVertex::operator==( le::VBO_ModelVertex& VBO_ModelVertex )
{
	return Position == VBO_ModelVertex.Position &&
		Normal == VBO_ModelVertex.Normal &&
		TextureCoord == VBO_ModelVertex.TextureCoord &&
		VertexColor == VBO_ModelVertex.VertexColor;
}

//-------------------------------------------------------------------------//

le::ModelMesh::~ModelMesh()
{
	Clear();
}

//-------------------------------------------------------------------------//

bool le::ModelMesh::LoadMesh( string route )
{
	TiXmlDocument LMD;

	vector<glm::vec3>		vVertexPosition;
	vector<glm::vec3>		vVertexNormal;
	vector<glm::vec2>		vTextureCoords;
	vector<Color>			vVertexColor;
	map<string, string>		mNameTexture;

	string routeTmp = route;
	routeTmp.erase( 0, routeTmp.find_last_of( '.' ) + 1 );

	if ( routeTmp == "lmd" )
	{
		// Декодируем модель
		HaffmanCode haffmanCode;
		string sModel = haffmanCode.DecompressedFromFile( route );

		if ( sModel == "ERROR" )
		{
			cout << "Error: Model [" << route << "] Not Found\n";
			return false;
		}

		LMD.Parse( sModel.c_str() );
	}
	else if ( routeTmp == "xlmd" )
	{
		if ( !LMD.LoadFile( route.c_str() ) )
		{
			cout << "Error: Model [" << route << "] Not Found\n";
			return false;
		}
	}
	else
	{
		cout << "Error: Model [" << route << "] Not Correct Format\n";
		return false;
	}

	// Работаем с контейнером model
	TiXmlElement *model;
	model = LMD.FirstChildElement( "model" );

	if ( model->Attribute( "SizeX" ) != NULL )
		SizeModel.x = atof( model->Attribute( "SizeX" ) );

	if ( model->Attribute( "SizeY" ) != NULL )
		SizeModel.y = atof( model->Attribute( "SizeY" ) );

	if ( model->Attribute( "SizeZ" ) != NULL )
		SizeModel.z = atof( model->Attribute( "SizeZ" ) );

	// Работаем с контейнером textures
	TiXmlElement *textures;
	TiXmlElement *img;

	textures = model->FirstChildElement( "textures" );

	if ( textures != NULL )
	{
		string IdTexture;
		img = textures->FirstChildElement( "image" );

		while ( img )
		{
			IdTexture = img->Attribute( "id" );

			string route = img->Attribute( "src" );
			route.erase( 0, route.find_last_of( '/' ) + 1 );

			string nameTexture = route;
			nameTexture.erase( 0, nameTexture.find_last_of( '\\' ) + 1 );

			MaterialManager::LoadGLTexture( nameTexture, route );

			mNameTexture[IdTexture] = nameTexture;
			img = img->NextSiblingElement();
		}
	}

	// Работаем с контейнером materials
	TiXmlElement *materials;
	TiXmlElement *material;

	materials = model->FirstChildElement( "materials" );

	if ( materials != NULL )
	{
		TiXmlElement *texture;
		TiXmlElement *ambient;
		TiXmlElement *diffuse;
		TiXmlElement *emission;
		TiXmlElement *specular;
		TiXmlElement *shininess;
		Color color;
		string MaterialId;

		material = materials->FirstChildElement( "material" );

		while ( material )
		{
			Material Material;
			MaterialId = material->Attribute( "id" );

			texture = material->FirstChildElement( "texture" );
			if ( texture != NULL )
				Material.Texture = MaterialManager::GetGLTexture( mNameTexture[texture->Attribute( "id" )] );

			ambient = material->FirstChildElement( "ambient" );
			if ( ambient != NULL )
			{
				color.r = atof( ambient->Attribute( "r" ) );
				color.g = atof( ambient->Attribute( "g" ) );
				color.b = atof( ambient->Attribute( "b" ) );
				color.a = atof( ambient->Attribute( "a" ) );

				Material.Ambient = color;
			}

			diffuse = material->FirstChildElement( "diffuse" );
			if ( diffuse != NULL )
			{
				color.r = atof( diffuse->Attribute( "r" ) );
				color.g = atof( diffuse->Attribute( "g" ) );
				color.b = atof( diffuse->Attribute( "b" ) );
				color.a = atof( diffuse->Attribute( "a" ) );

				Material.Diffuse = color;
			}

			emission = material->FirstChildElement( "emission" );
			if ( diffuse != NULL )
			{
				color.r = atof( emission->Attribute( "r" ) );
				color.g = atof( emission->Attribute( "g" ) );
				color.b = atof( emission->Attribute( "b" ) );
				color.a = atof( emission->Attribute( "a" ) );

				Material.Emission = color;
			}

			specular = material->FirstChildElement( "specular" );
			if ( diffuse != NULL )
			{
				color.r = atof( specular->Attribute( "r" ) );
				color.g = atof( specular->Attribute( "g" ) );
				color.b = atof( specular->Attribute( "b" ) );
				color.a = atof( specular->Attribute( "a" ) );

				Material.Specular = color;
			}

			shininess = material->FirstChildElement( "shininess" );
			if ( shininess != NULL )
				Material.fShininess = atof( shininess->Attribute( "value" ) );

			mMaterials[MaterialId] = Material;
			material = material->NextSiblingElement();
		}
	}

	
	// Работаем с контейнером geometries
	TiXmlElement *geometries;
	TiXmlElement *point;

	geometries = model->FirstChildElement( "geometries" );

	TiXmlElement *position_point;
	position_point = geometries->FirstChildElement( "position_point" );
	point = position_point->FirstChildElement( "point" );

	while ( point )
	{
		glm::vec3 Vertex;

		Vertex.x = atof( point->Attribute( "x" ) );
		Vertex.y = atof( point->Attribute( "y" ) );
		Vertex.z = atof( point->Attribute( "z" ) );

		vVertexPosition.push_back( Vertex );
		point = point->NextSiblingElement();
	}

	TiXmlElement *Normals;
	Normals = geometries->FirstChildElement( "normals" );
	point = Normals->FirstChildElement( "point" );

	while ( point )
	{
		glm::vec3 Normal;

		Normal.x = atof( point->Attribute( "x" ) );
		Normal.y = atof( point->Attribute( "y" ) );
		Normal.z = atof( point->Attribute( "z" ) );

		vVertexNormal.push_back( Normal );
		point = point->NextSiblingElement();
	}

	TiXmlElement *UV;
	UV = geometries->FirstChildElement( "UV" );
	point = UV->FirstChildElement( "point" );

	while ( point )
	{
		glm::vec2 TexCoord;

		TexCoord.x = atof( point->Attribute( "x" ) );
		TexCoord.y = atof( point->Attribute( "y" ) );

		vTextureCoords.push_back( TexCoord );
		point = point->NextSiblingElement();
	}

	TiXmlElement *VertexColors;
	VertexColors = geometries->FirstChildElement( "VertexColors" );

	if ( VertexColors != NULL )
	{
		point = VertexColors->FirstChildElement( "point" );

		while ( point )
		{
			Color VertexColor;

			VertexColor.r = atof( point->Attribute( "r" ) );
			VertexColor.g = atof( point->Attribute( "g" ) );
			VertexColor.b = atof( point->Attribute( "b" ) );
			VertexColor.a = atof( point->Attribute( "a" ) );

			vVertexColor.push_back( VertexColor );
			point = point->NextSiblingElement();
		}
	}

	TiXmlElement *polygons;
	TiXmlElement *p;

	polygons = geometries->FirstChildElement( "polygons" );
	material = polygons->FirstChildElement( "material" );

	while ( material )
	{
		p = material->FirstChildElement( "p" );
		string idMaterial = material->Attribute( "id" );
		int idVertex = 0;

		vector<unsigned int> vIdVertex;
		VBO_ModelVertex VBO_ModelVertex;
		ModelVertex ModelVertex;

		while ( p )
		{
			string sTmp = string( p->GetText() );
			istringstream iss( sTmp );

			for ( int id = 1; !iss.eof(); id++ )
			{
				string _tmp;
				iss >> _tmp;

				switch ( id )
				{
				case 1: // координаты			
					idVertex = atoi( _tmp.c_str() );
					VBO_ModelVertex.Position = vVertexPosition[idVertex];
					ModelVertex.Position = VBO_ModelVertex.Position;					
					break;

				case 2: // нормали
					VBO_ModelVertex.Normal = vVertexNormal[atoi( _tmp.c_str() )];
					ModelVertex.Normal = VBO_ModelVertex.Normal;
					break;

				case 3: // текстура
					VBO_ModelVertex.TextureCoord = vTextureCoords[atoi( _tmp.c_str() )];


					if ( vVertexColor.empty() )
					{
						bool isFind = false;
						for ( int i = 0; i < vVBO_Vertexs.size(); i++ )
						if ( vVBO_Vertexs[i] == VBO_ModelVertex )
						{
							vIdVertex.push_back( i );
							isFind = true;
							break;
						}

						if ( !isFind )
						{
							if ( mVertexs.find( idVertex ) != mVertexs.end() )
							{
								if ( mVertexs[idVertex].Position == VBO_ModelVertex.Position )
									mVertexs[idVertex].vIdVBO_Vertex.push_back( vVBO_Vertexs.size() );
							}
							else
							{
								mVertexs[idVertex] = ModelVertex;

								if ( mVertexs[idVertex].Position == VBO_ModelVertex.Position )
									mVertexs[idVertex].vIdVBO_Vertex.push_back( vVBO_Vertexs.size() );
							}

							vIdVertex.push_back( vVBO_Vertexs.size() );
							vVBO_Vertexs.push_back( VBO_ModelVertex );
						}

						id = 0;
					}

					break;

				case 4: // цвет вершины
					VBO_ModelVertex.VertexColor = vVertexColor[atoi( _tmp.c_str() )];

					bool isFind = false;
					for ( int i = 0; i < vVBO_Vertexs.size(); i++ )
					if ( vVBO_Vertexs[i] == VBO_ModelVertex )
					{
						vIdVertex.push_back( i );
						isFind = true;
						break;
					}

					if ( !isFind )
					{
						if ( mVertexs.find( idVertex ) != mVertexs.end() )
						{
							if ( mVertexs[idVertex].Position == VBO_ModelVertex.Position )
								mVertexs[idVertex].vIdVBO_Vertex.push_back( vVBO_Vertexs.size() );
						}
						else
						{
							mVertexs[idVertex] = ModelVertex;

							if ( mVertexs[idVertex].Position == VBO_ModelVertex.Position )
								mVertexs[idVertex].vIdVBO_Vertex.push_back( vVBO_Vertexs.size() );
						}

						vIdVertex.push_back( vVBO_Vertexs.size() );
						vVBO_Vertexs.push_back( VBO_ModelVertex );
					}

					id = 0;

					id = 0;
					break;
				}
			}

			p = p->NextSiblingElement();
		}

		mIdIndexs[idMaterial] = vIdVertex;
		material = material->NextSiblingElement( );
	}

	// Работаем с контейнером skeleton
	TiXmlElement *skeleton;
	skeleton = model->FirstChildElement( "skeleton" );

	if ( skeleton != NULL )
	{
		Skeleton.LoadSkeleton( skeleton );
		Skeleton.InitMesh( mVertexs, vVBO_Vertexs );
	}

	// Работаем с контейнером collision_model
	TiXmlElement *collision_model;
	collision_model = LMD.FirstChildElement( "collision_model" );

	if ( collision_model != NULL )
	{
		IsCollisionMesh = true;

		// Работаем с контейнером geometries
		geometries = collision_model->FirstChildElement( "geometries" );
		position_point = geometries->FirstChildElement( "position_point" );
		point = position_point->FirstChildElement( "point" );

		while ( point )
		{
			vCollision_Vertexs.push_back( atof( point->Attribute( "x" ) ) );
			vCollision_Vertexs.push_back( atof( point->Attribute( "y" ) ) );
			vCollision_Vertexs.push_back( atof( point->Attribute( "z" ) ) );

			point = point->NextSiblingElement( );
		}

		polygons = geometries->FirstChildElement( "polygons" );
		p = polygons->FirstChildElement( "p" );

		while ( p )
		{
			string sTmp = p->GetText( );
			istringstream iss( sTmp );

			while ( !iss.eof( ) )
			{
				string _tmp;
				iss >> _tmp;

				vCollision_IdVertexs.push_back( atoi( _tmp.c_str( ) ) );
			}

			p = p->NextSiblingElement( );
		}

		// Работаем с контейнером skeleton
		skeleton = collision_model->FirstChildElement( "skeleton" );

		if ( skeleton != NULL )
		{
			Skeleton.LoadCollisionInfo( skeleton );
			Skeleton.InitCollisionMesh( vCollision_Vertexs );
		}
	}

	// Работаем с контейнером animations
	TiXmlElement *animations;
	animations = model->FirstChildElement( "animations" );

	if ( animations != NULL )
	{
		AnimationManager3D.SetSkeleton( Skeleton );
		AnimationManager3D.LoadAnimations( animations );
	}

	return true;
}

//-------------------------------------------------------------------------//

void le::ModelMesh::Clear()
{
	vVBO_Vertexs.clear();
	mIdIndexs.clear();
	mVertexs.clear();
	mMaterials.clear();
}

//-------------------------------------------------------------------------//