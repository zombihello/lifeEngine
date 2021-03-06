#ifndef CAMERA_H
#define CAMERA_H

#define COMPILING_LIBRARY
#include "../../DllGlobal.h"

////////////////
/// LIFEENGINE
///////////////
#include "../System.h"

namespace le
{
	//-------------------------------------------------------------------------//

	class DLL_API Camera
	{
	public:

		//-------------------------------------------------------------------------//

		enum TypeMove
		{
			Forward,
			Back,
			Left,
			Right
		};

		//-------------------------------------------------------------------------//

		/////////////////
		/// КОНСТРУКТОР
		/////////////////
		Camera( System& System );

		/////////////////
		/// ДЕСТРУКТОР
		/////////////////
		~Camera();

		////////////////////////
		/// ОБНОВЛЕНИЕ КАМЕРЫ
		////////////////////////
		void UpdateCamera();

		//////////////////////
		/// ПРИМЕНИТЬ КАМЕРУ
		/////////////////////
		void ApplyCamera();

		////////////////////////
		/// ПЕРЕДВИНУТЬ КАМЕРУ
		////////////////////////
		void Move( Vector3f FactorMove );
		void Move( TypeMove typeMove, float MoveSpeed );

		///////////////////////////////
		/// НАКЛОНИТЬ КАМЕРУ ПО ОСИ Z
		///////////////////////////////
		void TiltCamera( float FactorTilt );

		////////////////////////////////////
		/// ЗАДАТЬ ЧУВСТВИТЕЛЬНОСТЬ МЫШИ
		///////////////////////////////////
		void SetSensitivityMouse( float sensitivityMouse );

		///////////////////////////
		/// ЗАДАТЬ ПОЗИЦИЮ КАМЕРЫ
		///////////////////////////
		void SetPosition( Vector3f NewPosition );

		////////////////////////////////////////
		/// ЗАДАТЬ НАКЛОНЕНИЕ КАМЕРЫ ПО ОСИ Z
		///////////////////////////////////////
		void SetInclinationCamera( float InclinationCamera );

		/////////////////////////////
		/// ПОЛУЧИТЬ ПОЗИЦИЮ КАМЕРЫ
		/////////////////////////////
		Vector3f GetPosition();

		/////////////////////////////////////////
		/// ПОЛУЧИТЬ НАКЛОНЕНИЕ КАМЕРЫ ПО ОСИ Z
		/////////////////////////////////////////
		float GetInclinationCamera();

		////////////////////////////////////////////////////////////////////////////////
		/// ПОЛУЧИТЬ ВЕКТОР СМЕЩЕНИЯ (ВОЗВРАЩАЕТ СМЩЕНИЕ XYZ С УЧЕТОМ ПОВОРОТА КАМЕРЫ
		////////////////////////////////////////////////////////////////////////////////
		Vector3f GetVectorMove( TypeMove typeMove, float MoveSpeed );

		/////////////////////////////////////////
		/// ПОЛУЧИТЬ ВИДОВУЮ МАТРИЦУ
		/////////////////////////////////////////
		glm::mat4 GetViewMatrix();

	private:

		float			fSensitivityMouse;
		float			fInclinationCamera;

		System*			System;

		Vector2f		Angle;
		Vector3f		Position;
		Vector2f		CenterWindow;

		glm::mat4		ViewMatrix;
		glm::mat4		InclinationCameraMatrix;
	};

	//-------------------------------------------------------------------------//
}

#endif // CAMERA_H

