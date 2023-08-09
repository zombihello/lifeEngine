---
-- Copyright (C) 2022 Broken Singularity
---

newoption {
	trigger		= "game",
	value		= "Game",
	default     = "EleotGame",
	description = "Choose game for generation (need to exist the folder './Games/<Game>')"
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

-- Path to binaries directory
binariesDir				    = "%{wks.location}/../../../Binaries/"

-- Output directory name
outputDir			        = "%{cfg.platform}/"

-- Output sufix target name
sufixOutputName             = "-%{cfg.platform}-%{cfg.buildcfg}"

-- Path to external directory
externalDir			        = "%{wks.location}/../../External/";

-- Init game name
game			            = _OPTIONS["game"]
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
extTmxlite                      = externalDir .. "tmxlite"
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
include( extTmxlite )
include( extZlib )
include( extCompressonator )
include( extHalf )

workspace( game )
    location( "../Intermediate/" .. _ACTION .. "/" )
    configurations 	    { "Debug", "DebugWithEditor", "Release", "ReleaseWithEditor", "Shipping" }
    platforms 		    { "Win64" }
    defaultplatform	    "Win64"

    ---------------- GLOBAL SETTINGS ---------------

    characterset 		"Unicode"
    floatingpoint 		"Fast"
	vectorextensions 	"SSE2"	

    targetname( "%{prj.name}-%{cfg.platform}-%{cfg.buildcfg}" )
    targetdir( binariesDir .. outputDir )
	objdir( intermediateDir .. outputDir .. "%{prj.name}/%{cfg.buildcfg}/" )

    flags { "MultiProcessorCompile" }

    --------------- PLATFORM SETTINGS --------------

    filter "platforms:Win64"
        system 			"Windows"
        architecture 	"x64"
        cppdialect 		"C++17"
        staticruntime 	"Off"
        debugdir( binariesDir .. outputDir )

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

	filter "configurations:Debug or DebugWithEditor"
        defines 	        { "DEBUG=1",  "RELEASE=0", "SHIPPING=0" }
        symbols 	        "On"
        inlining            "Disabled"
        runtime 	        "Debug"

    filter "configurations:Release or ReleaseWithEditor"
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

    	-------------- BUILD TYPE SETTINGS -------------
	
	-- StaticLib
	filter "kind:StaticLib"
        targetdir( intermediateDir .. outputDir .. "%{prj.name}/Lib/" )
    filter { "kind:StaticLib", "platforms:Win64" }
        symbolspath( intermediateDir .. outputDir .. "%{prj.name}/Lib/%{cfg.buildtarget.basename}.pdb" )

    -- SharedLib
    filter "kind:SharedLib"
        symbolspath( intermediateDir .. outputDir .. "%{prj.name}/Lib/%{cfg.buildtarget.basename}.pdb" )
    filter {}

    --------------- PROJECT --------------

    project( game )
        kind        "WindowedApp"
        language    "C++"
        location( intermediateDir )

        ----------- PROJECT SETTINGS --------

        defines				{
            "GAMENAME=" .. "\"" .. game .. "\"",
            "ENGINE_2D=" .. tostring( is2DEngine and 1 or 0 ),
            "USE_THEORA_CODEC=1"
        }

        includedirs         {
            "Engine/**/Include/",
            "Games/" .. game .. "/Include/",
            "../../Engine/Shaders/"
        }

        files               {
            "Engine/**/Include/**.h",
            "Engine/**/Include/**.inl",
            "Engine/**/Source/**.cpp",
            "Games/" .. game .. "/Include/**.h",
            "Games/" .. game .. "/Include/**.inl",
            "Games/" .. game .. "/Source/**.cpp",
        }

        ---------- PLATFORM SPECIFIC SETTINGS ---------

        -- Exclude platform specific for other platforms
        filter "platforms:not Win64"
            excludes { "**/Windows/**.*", "**/D3D11RHI/**.*" }
        filter {}

        -- Platform specific settings
        filter "platforms:Win64"
            files { "Games/" .. game .. "/Resources/**.rc", }
            links { "d3d11", "d3d9", "dxgi", "dxguid", "d3dcompiler" }
        filter {}

        --------- LINK EXTERNAL LIBS -------
       
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

        ------ EDITOR SETTINGS -----

        filter "configurations:*WithEditor"
            LinkSTB()
            LinkAssmip()
            LinkTmxLite()
			LinkCompressonator()
		   
            defines     { "WITH_EDITOR=1", "WITH_IMGUI=1" }

        filter "configurations:not *WithEditor"
            -- If we build engine without editor, we will need exclude all of WorldEd source files
            excludes    { "Engine/WorldEd/**.*" }
            defines     { "WITH_EDITOR=0", "WITH_IMGUI=0" }
        filter {}