external shared class GameInfo;
external shared enum EGameMode;

class TBGameInfo : GameInfo
{
	string GetGameName()
	{
		return "TestbedGame";
	}
	
	void SetGameMode( EGameMode GameMode )
	{
		gameMode = GameMode;
	}
}

TBGameInfo GTBGameInfo;

int GameInit()
{
	GTBGameInfo.SetGameMode( GM_Game );
	return 0;
}