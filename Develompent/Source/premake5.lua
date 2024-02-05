--[[
************************************************************
*                     This file is part of:
*                        LIFEENGINE
*            https://github.com/zombihello/lifeEngine
************************************************************
* Copyright (C) 2024 Yehor Pohuliaka.
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
]]

newoption {
	trigger		= "game",
	value		= "Game",
	default     = "Eleot",
	description = "Choose game for generation (need to exist next folders: './Game/<Game>Game' and './Game/<Game>Launcher')"
}

newoption {
    trigger     = "engine",
    value       = "EngineType",
    default     = "2D",
    description = "Choose engine type for generation",
    allowed = {
       { "2D",    "2D Engine" },
       { "3D",  "3D Engine" }
    }
 }

--------- GLOBAL VARIABLES -----------------	

-- Path to intermediate directory for engine and game
intermediateDir				= "%{wks.location}/"

-- Path to intermediate directory for externals
intermediateExternalDir		= intermediateDir .. "External/"

-- Output directory name
outputDir			        = "%{cfg.platform}/"

-- Path to binaries directory
binariesDir				    = "%{wks.location}/../../../Binaries/%{cfg.platform}-%{cfg.buildcfg}"

-- Path to external directory
externalDir			        = "%{wks.location}/../../External/";

-- Init game name
game			            = _OPTIONS["game"]
gameModule                  = ""
print( "Generation project for '" .. game .. "'" )

-- Engine type
if _OPTIONS["engine"] then
    if _OPTIONS["engine"] == "2D" then
        is2DEngine = true
    elseif _OPTIONS["engine"] == "3D" then
        is2DEngine = false
    end

    print( "Engine type: " .. _OPTIONS["engine"] )
else
    error( "Engine type need setted" )
end

-- Path to externals
extAssimp                       = externalDir .. "assimp-5.0.1"
extBox2d                        = externalDir .. "box2d-2.4.1"
extGlm                          = externalDir .. "glm-0.9.9.8"
extOgg                          = externalDir .. "libogg-1.3.5"
extVorbis                       = externalDir .. "libvorbis-1.3.7"
extLuaBridge                    = externalDir .. "LuaBridge-2.6"
extLuaJIT                       = externalDir .. "LuaJIT-2.1"
extOpenAL                       = externalDir .. "openal-soft-1.21.1"
extPhysX                        = externalDir .. "PhysX-4.1"
extRapidJSON                    = externalDir .. "rapidjson"
extSdl2			                = externalDir .. "SDL2-2.0.14"
extStb                          = externalDir .. "stb"
extTheora                       = externalDir .. "theora-1.2"
extZlib                         = externalDir .. "zlib"
extCompressonator				= externalDir .. "Compressonator_4.3.206"
extHalf							= externalDir .. "half-2.2.0"

-- Include modules of external libs
include( extAssimp )
include( extBox2d )
include( extGlm )
include( extOgg )
include( extVorbis )
include( extLuaBridge )
include( extLuaJIT )
include( extOpenAL )
include( extPhysX )
include( extRapidJSON )
include( extSdl2 )
include( extStb )
include( extTheora )
include( extZlib )
include( extCompressonator )
include( extHalf )

-- Projects
projEngine          = "Engine/"
projGame            = "Game/" .. game .. "Game/"
projGameLauncher    = "Game/" .. game .. "Launcher/"
projWorldEd         = "Tools/WorldEd/"

-- Link all external libs for the engine
function LinkEngineExternals()
    LinkOgg()
    LinkVorbis()
    LinkOpenAL()
    LinkLuaJIT()
    LinkLuaBridge()
    LinkRapidJSON()
    LinkGLM()
    LinkZLib()
    LinkSDL2()
    LinkTheora()
	LinkHalf()

    if is2DEngine then
        LinkBox2D()
    else
        LinkPhysX()
    end
end

-- Link all external libs for the editor
function LinkEditorExternals()
    LinkSTB()
    LinkAssmip()
	LinkCompressonator()
end

-- Link all externals
function LinkExternals()
    -- Link the engine externals
    LinkEngineExternals()

    -- Link the editor externals
    filter "configurations:*WithEditor"
        LinkEditorExternals()
    filter {}
end

-- Workspace setup
workspace "LifeEngine"
    location( "../Intermediate/" .. _ACTION .. "/" )
    configurations 	    { "Debug", "DebugWithEditor", "Release", "ReleaseWithEditor", "Shipping" }
    platforms 		    { "Win64" }
    defaultplatform	    "Win64"

    ---------------- GLOBAL SETTINGS ---------------

    characterset 		"Unicode"
    floatingpoint 		"Fast"
	vectorextensions 	"SSE2"	

    targetname( "%{prj.name}" )
    targetdir( binariesDir  )
	objdir( intermediateDir .. outputDir .. "%{prj.name}/%{cfg.buildcfg}" )

    flags       { "MultiProcessorCompile" }
    defines     { "GAMENAME=" .. "\"" .. game .. "\"" }

    --------------- PLATFORM SETTINGS --------------

    filter "platforms:Win64"
        system 			"Windows"
        architecture 	"x64"
        cppdialect 		"C++17"
        staticruntime 	"Off"
        debugdir( binariesDir )

        defines 		{
            "_WIN64",
            "_CRT_SECURE_NO_WARNINGS",		-- For remove on unsafe C functions
            "PLATFORM_64BIT=1"
        }

        buildoptions 	{ 
            "/FC", 							-- set full path of source code file when using the __FILE__ macro
            "/W1"
        }
	filter {}

    --------------- CONFIGURATION SETTINGS --------------

	filter "configurations:Debug*"
        defines 	        { "DEBUG=1",  "RELEASE=0", "SHIPPING=0" }
        symbols 	        "On"
        inlining            "Disabled"
        runtime 	        "Debug"

    filter "configurations:Release*"
        defines 	        { "NDEBUG", "DEBUG=0", "RELEASE=1", "SHIPPING=0" }
        optimize 	        "Speed"
		runtime 			"Release"
        inlining            "Auto"

    filter "configurations:Shipping"
        defines 	        { "NDEBUG", "DEBUG=0", "RELEASE=0", "SHIPPING=1" }
        optimize            "Speed"
		runtime 			"Release"
        inlining            "Auto"
    filter {}

    -- If we build the engine and a game in configuration *WithEditor then need enable macro WITH_EDITOR for specific code only for the editor
    filter "configurations:*WithEditor"
        defines     { "WITH_EDITOR=1", "WITH_IMGUI=1" }
    filter "configurations:not *WithEditor"
        defines     { "WITH_EDITOR=0", "WITH_IMGUI=0" }
    filter {}

    -------------- BUILD TYPE SETTINGS -------------
	
	-- StaticLib
	filter "kind:StaticLib"
        targetdir( intermediateDir .. outputDir .. "%{prj.name}/%{cfg.buildcfg}/Lib/" )
    filter { "kind:StaticLib", "platforms:Win64" }
        symbolspath( intermediateDir .. outputDir .. "%{prj.name}/%{cfg.buildcfg}/Lib/%{cfg.buildtarget.basename}.pdb" )

    -- SharedLib
    filter "kind:SharedLib"
        symbolspath( intermediateDir .. outputDir .. "%{prj.name}/%{cfg.buildcfg}/Lib/%{cfg.buildtarget.basename}.pdb" )
    filter {}

    --------------- PROJECTS --------------

    group "Engine/"
        include( projEngine )
    group "Game/"
        include( projGame )
        include( projGameLauncher )
    group "Tools/"
        include( projWorldEd )