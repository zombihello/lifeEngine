enum EGameMode
{
	GM_Menu,
	GM_Game
};

EGameMode		GGameMode = GM_Game;

int GameInit()
{
	return 0;
}

EGameMode GetCurrentGameMode()
{
	return GGameMode;
}

void SetCurrentGameMode( EGameMode InGameMode )
{
	GGameMode = InGameMode;
}