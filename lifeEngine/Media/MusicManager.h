#ifndef MUSICMANAGER_H
#define MUSICMANAGER_H

#define COMPILING_LIBRARY
#include "../DllGlobal.h"

/////////////////
/// LIFEENGINE
////////////////
#include "Music.h"

namespace le
{
	//-------------------------------------------------------------------------//
	
	class DLL_API MusicManager
	{
	public:
		/////////////////
		/// КОНСТРУКТОР
		////////////////
		MusicManager( System& System );

		/////////////////
		/// ДЕСТРУКТОР
		////////////////
		~MusicManager();

		//////////////////////
		/// ЗАГРУЗИТЬ МУЗЫКУ
		/////////////////////
		void LoadMusic( string Route, string NameMusic, bool loop = false );
		void LoadMusic( string Route, string NameMusic, int CoefficientVolume, bool loop = false );

		///////////////////
		/// ИГРАТЬ МУЗЫКУ
		//////////////////
		void PlayMusic( string NameMusic );

		//////////////////////
		/// ОСТАНОВИТЬ МУЗЫКУ
		/////////////////////
		void StopMusic( string NameMusic );
		void StopMusic();

		//////////////////////
		/// ПОСТАВИТЬ МУЗЫКУ НА ПАУЗУ
		/////////////////////
		void PauseMusic( string NameMusic );
		void PauseMusic();

		////////////////////////
		/// УДАЛИТЬ ВСЮ МУЗЫКУ
		///////////////////////
		void DeleteAllMusic();

		////////////////////
		/// УДАЛИТЬ МУЗЫКУ
		///////////////////
		void DeleteMusic( string NameMusic );

		/////////////////////////
		/// ЗАГРУЖЕНА ЛИ МУЗЫКА
		/////////////////////////
		bool GetLoadedMusic( string NameMusic );

		/////////////////////
		/// ИГРАЕТ ЛИ МУЗЫКА
		/////////////////////
		bool IsPlaying( string NameMusic );
	private:
		Configuration*							Configuration;
		map<string,le::Music*>					vMusic;
		map<string,le::Music*>::iterator		Iterator;
	};
	
	//-------------------------------------------------------------------------//
}

#endif // MUSICMANAGER_H
