shared enum EGameMode
{
	GM_Menu,
	GM_Game
}

shared class GameInfo
{
	GameInfo()
	{
		gameMode = GM_Menu;
	}

	string GetGameName()
	{
		return "UnknownGame";
	}
	
	EGameMode		gameMode;
}