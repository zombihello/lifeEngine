#ifndef MODEL_H
#define MODEL_H

#define COMPILING_LIBRARY
#include "../../DllGlobal.h"

////////////////
/// LIFEENGINE
///////////////
#include "ModelMesh.h"
#include "Skeleton.h"
#include "Scene3D.h"
#include "../System.h"
#include "../../Animation/AnimationManager3D.h"

//-------------------------------------------------------------------------//

class btTriangleIndexVertexArray;

//-------------------------------------------------------------------------//

namespace le
{
	//-------------------------------------------------------------------------//

	class Scene3D;
	class SceneInfoMesh;

	//-------------------------------------------------------------------------//

	class DLL_API Model
	{
	public:
		/////////////////
		/// КОНСТРУКТОР
		/////////////////
		Model( System& System );

		/////////////////
		/// ДЕСТРУКТОР
		/////////////////
		~Model();

		/////////////////////
		/// УДАЛИТЬ МОДЕЛЬ
		/////////////////////
		void DeleteModel();

		//////////////////////
		/// ЗАГРУЗКА МОДЕЛИ
		//////////////////////
		bool LoadModel( string nameModel, string route );
		void LoadModel( ModelMesh ModelMesh );

		///////////////////////////////
		/// ДОБАВИТЬ МОДЕЛЬ НА СЦЕНУ
		///////////////////////////////
		void AddToScene( Scene3D& Scene );

		////////////////////////////
		/// ЗАДАТЬ ПОЗИЦИЮ МОДЕЛИ
		///////////////////////////
		void SetPosition( Vector3f Position );

		////////////////////////////
		/// ЗАДАТЬ МАСШТАБ МОДЕЛИ
		///////////////////////////
		void SetScale( Vector3f Scale );
		void SetScale( Vector3f Scale, btTriangleIndexVertexArray* IndexVertexArrays );

		////////////////////////////
		/// ЗАДАТЬ ПОВОРОТ МОДЕЛИ
		///////////////////////////
		void SetRotate( Vector3f Rotation );
		void SetRotate( glm::quat Rotation );

		////////////////////
		/// СМЕСТИТЬ МОДЕЛЬ
		////////////////////
		void Move( Vector3f FactorMove );

		///////////////////////////
		/// МАСШТАБИРОВАТЬ МОДЕЛЬ
		//////////////////////////
		void Scale( Vector3f FactorScale );
		void Scale( Vector3f FactorScale, btTriangleIndexVertexArray* IndexVertexArrays );

		//////////////////////
		/// ПОВЕРНУТЬ МОДЕЛЬ
		//////////////////////
		void Rotate( Vector3f Rotation );
		void Rotate( glm::quat Rotation );

		//////////////////////////
		/// ОБНОВИТЬ МОДЕЛЬ
		//////////////////////////
		void UpdateModel( Shader* Shader = NULL );

		////////////////////////////////
		/// ПОЛУЧИТЬ МЕНЕДЖЕР АНИМАЦИЙ
		////////////////////////////////
		AnimationManager3D& GetAnimationManager();

		/////////////////////////////
		/// ПОЛУЧИТЬ РАЗМЕР МОДЕЛИ
		/////////////////////////////
		Vector3f GetSize();

		/////////////////////////////
		/// ПОЛУЧИТЬ ПОЗИЦИЮ МОДЕЛИ
		/////////////////////////////
		Vector3f GetPosition();

		////////////////////////////
		/// ПОЛУЧИТЬ МАСШТАБ МОДЕЛИ
		////////////////////////////
		Vector3f GetScale();

		////////////////////////////
		/// ПОЛУЧИТЬ ПОВОРОТ МОДЕЛИ
		////////////////////////////
		glm::quat GetRotate();

		////////////////////////////
		/// ПОЛУЧИТЬ ВЕРШИНЫ МОДЕЛИ
		////////////////////////////
		map<int, ModelVertex>& GetVertexs();

		///////////////////////////////////
		/// ПОЛУЧИТЬ ВЕРШИНЫ МОДЕЛИ В VBO
		///////////////////////////////////
		vector<VBO_ModelVertex>& GetVBO_Vertexs();

		//////////////////////////////////////
		/// ПОЛУЧИТЬ ВЕРШИНЫ КОЛЛИЗИИ МОДЕЛИ
		//////////////////////////////////////
		vector<float>& GetCollisionVertexs();

		///////////////////////////////////////////////////////////
		/// ПОЛУЧИТЬ ПОРЯДОК СОЕДИНЕНИЯ ВЕРШИН У КОЛЛИЗИИ МОДЕЛИ
		///////////////////////////////////////////////////////////
		vector<int>& GetCollisionIdVertexs();

		/////////////////////////
		/// ПОЛУЧИТЬ VBO МОДЕЛИ
		////////////////////////
		GLuint& GetVertexBuffer();

	private:

		GLuint									VertexBuffer;

		Vector3f								SizeModel;
		Vector3f								Position;
		Vector3f								ScaleModel;
		glm::quat								Rotation;

		glm::mat4x4								MatrixPosition;
		glm::mat4x4								MatrixRotation;
		glm::mat4x4								MatrixScale;
		glm::mat4x4								MatrixTransformation;

		Skeleton								Skeleton;
		AnimationManager3D						AnimationManager3D;

		System*									System;
		Scene3D*								Scene;

		vector<VBO_ModelVertex>					vVBO_Vertexs;
		vector<float>							vCollision_Vertexs;
		vector<int>								vCollision_IdVertexs;
		vector<GLuint>							vIndexBuffers;
		vector<GLuint>							vArrayBuffers;

		map<int, ModelVertex>					mVertexs;
		map<GLuint, SceneInfoMesh>				mRenderMesh;
	};

	//-------------------------------------------------------------------------//
}

#endif // MODEL_H