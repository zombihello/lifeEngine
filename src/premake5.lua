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
	value		= "game",
	default     = "eleot",
	description = "Choose game for generation (need to exist the folder './game/<game>')"
}

newoption {
    trigger     = "build-monolithic-engine",
    description = "Assemble the engine monolithically"       
}

--------- GLOBAL VARIABLES -----------------

-- Path to src folder
root                        = _MAIN_SCRIPT_DIR

-- Path to intermediate directory for the engine and a game
intermediateDir				= root .. "/intermediate/" .. _ACTION .. "/"

-- Path to intermediate directory for third parties
intermediateThirdPartyDir	= intermediateDir .. "thirdparty/"

-- Path to intermediate output directory
intermediateOutputDir       = "%{string.lower(cfg.platform)}/%{string.lower(cfg.buildcfg)}/"

-- Path to build directory
buildDir                    = root .. "/../game/"

-- Path to binaries directory
binariesDir				    = "bin/"

-- Output directory name
outputDir			        = "%{string.lower(cfg.platform)}/"

-- Path to third party directory
thirdPartyDir			    = root .. "/thirdparty/";

-- Init a game name
game			            = _OPTIONS["game"]
print( "Generation project for '" .. game .. "'" )

-- Is need assemble the engine monolithically
if _OPTIONS["build-monolithic-engine"] then
    buildMonolithicEngine = true
    print( "Assemble the engine monolithically" )
else
    buildMonolithicEngine = false
    print( "Assemble the engine modular" )
end

--------------- MODULES ---------------
launcher                    = "launcher/"
core                        = "core/"
stdlib                      = "libs/stdlib/"
appframework                = "libs/appframework/"
inputsystem                 = "inputsystem/"

--------------- THIRD PARTIES ---------
thirdParty_Mimalloc         = thirdPartyDir .. "mimalloc-2.1.2"
thirdParty_ZLib             = thirdPartyDir .. "zlib-1.2.13"
thirdParty_GLM              = thirdPartyDir .. "glm-1.0.0"
thirdParty_SDL2             = thirdPartyDir .. "SDL2-2.30.0"

include( thirdParty_Mimalloc )
include( thirdParty_ZLib )
include( thirdParty_GLM )
include( thirdParty_SDL2 )

workspace( game )
    location( root )
    configurations 	    { "Debug", "Release", "Retail" }
    platforms 		    { "Win64" }
    defaultplatform	    "Win64"

    ---------------- GLOBAL SETTINGS ---------------

    characterset 		"Unicode"
    floatingpoint 		"Fast"
	vectorextensions 	"SSE2"	

    targetdir( buildDir .. binariesDir .. outputDir )
	objdir( intermediateDir .. intermediateOutputDir .. "%{prj.name}/" )

    flags           { "MultiProcessorCompile" }
    filter { "files:public/**.cpp" }
        flags       { "NoPCH" }
    filter {}

    includedirs         {
        "public/",
        "public/libs/",
        "./"
    }

    defines 		    {
        "PLATFORM_SUBDIR=\"%{string.lower(cfg.platform)}\""
    }

    if buildMonolithicEngine then
        defines {
            "BUILD_MONOLITHIC_ENGINE"
        }
    end

    --------------- PLATFORM SETTINGS --------------

    filter "platforms:Win64"
        system 			"Windows"
        architecture 	"x64"
        cppdialect 		"C++17"
        staticruntime 	"Off"
        debugdir( buildDir .. binariesDir .. outputDir )

        defines 		{
            "_WIN64",
            "_CRT_SECURE_NO_WARNINGS",		-- For remove on unsafe C functions
            "_CRT_NONSTDC_NO_DEPRECATE",
            "PLATFORM_64BIT=1"
        }

        buildoptions 	{ 
            "/FC"     -- Set full path of source code file when using the __FILE__ macro					
        }
	filter {}

    --------------- CONFIGURATION SETTINGS --------------

	filter "configurations:Debug"
        defines 	        { "DEBUG=1",  "RELEASE=0", "RETAIL=0" }
        symbols 	        "On"
        inlining            "Disabled"
        runtime 	        "Debug"

    filter "configurations:Release"
        defines 	        { "NDEBUG", "DEBUG=0", "RELEASE=1", "RETAIL=0" }
        optimize 	        "Speed"
        runtime 			"Release"
        inlining            "Auto"

    filter "configurations:Retail"
        defines 	        { "NDEBUG", "DEBUG=0", "RELEASE=0", "RETAIL=1" }
        optimize            "Speed"
        runtime 			"Release"
        inlining            "Auto"
    filter {}

    -------------- BUILD TYPE SETTINGS -------------
	
	-- StaticLib
	filter "kind:StaticLib"
        targetdir( intermediateDir .. intermediateOutputDir .. "%{prj.name}/lib/" )
        defines { "BUILD_STATIC_LIB" }
    filter { "kind:StaticLib", "platforms:Win64" }
        symbolspath( intermediateDir .. intermediateOutputDir .. "%{prj.name}/lib/%{cfg.buildtarget.basename}.pdb" )

    -- SharedLib
    filter "kind:SharedLib"
        symbolspath( intermediateDir .. intermediateOutputDir .. "%{prj.name}/lib/%{cfg.buildtarget.basename}.pdb" )
    filter {}

    ----------------- MODULES ------------------

    group "/Engine"
        group "/Engine/Libraries"
            include( stdlib )
            include( appframework )
        include( launcher )
		include( core )
        include( inputsystem )