![lifeEngine](EngineLogo.jpg?raw=true)

## Description
3D Game Engine with architecture like Unreal Engine

## Screenshots
World Editor
![Screenshot_WorldEd](Develompent/Docs/Screenshots/Screenshot_WorldEd.png?raw=true)

Game
![Screenshot_Game](Develompent/Docs/Screenshots/Screenshot_Game.png?raw=true)

## Building

### Windows
1. Install [Visual Studio 2019 Community](https://visualstudio.microsoft.com/downloads/)
2. Create directory in `Develompent/Source/Games/<YourGameName>` where will contain your source code of a game (for example use EleotGame dir)
3. In this directory need create dirs: `Include` (for includes), `Resources` (for .ico, .rc) and `Source` (for sources)
4. Run `Develompent/Source/GenerateSolution.bat` with parameters `-game <YourGameNameFromStep2>`
5. Open the solution created in `Develompent/Intermediate/vs2019/<YourGameNameFromStep2>.sln`
6. Build project `<YourGameNameFromStep2>`. If you select a configuration `*WithEditor`, then an editor will be built into the assembly

## Running a game
1. Copy the contents of the folder `EleotGame` to you folder `<YourGameName>`. This directory where will contain your content of a game
2. Run `Binaries/<Platform>/<YourGameName>-<Platform>-<Configuration>`. Use parameter `-editor` for launch editor

## Current status
- Not ready for any use
- See [TODO.md](TODO.md)