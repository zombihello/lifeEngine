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

// TODO BG yehor.pohuliaka - Need fix generate C++ code with 'TBGameInfo@ &out OutTest' params
void GameExec3( TBGameInfo@ &out OutTest ) 
{
	@OutTest = null;
}

int GameInit()
{
	return 0;
}