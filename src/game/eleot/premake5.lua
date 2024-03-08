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

project "eleot"
    if not buildMonolithicEngine then
        kind "SharedLib"
    else
        kind "StaticLib"
    end
    language    "C++"
    location( intermediateDir )
	targetname	"game"
	targetdir( buildDir .. "eleot/" .. binariesDir .. outputDir )
	
    ----------- PROJECT SETTINGS --------

	defines { "ELEOT_GAME_DLL" }

    files       {
		-- Shared game code
		"../*.inl", 
        "../*.cpp",
        "../*.h",
		
		-- Eleot game code
		"**.inl", 
        "**.cpp",
        "**.h",
		
		-- Public interfaces and shared code
        "../../public/game/**.h",
        "../../public/game/**.inl",
        "../../public/core/**.cpp"
    }

    -- Enable PCH file
    pchheader       "pch_game.h"
    pchsource       "../pch_game.cpp"
    includedirs     { "./", "../" }

    vpaths      {
        ["src/*"]       = { "../*.h", "../*.inl", "../*.cpp" },
        ["src/eleot/*"]	= { "**.h", "**.inl", "**.cpp" },
		["public/*"]    = { "../public/**.h", "../public/**.inl", "../public/**.cpp" }
    }

    links       {
        "core",
        "stdlib",
		"interfaces"
    }

	----------- LINK THIRD PARTIES -----------------

    GLM.Link()

    ---------- PLATFORM SPECIFIC SETTINGS ---------

    -- Exclude platform specific for other platforms
	filter "platforms:not Win64"
        excludes { "**/platforms/windows/**.*" }
    filter {}

    -- Windows
    filter "platforms:Win64"
        files   { "**.rc" }
        vpaths  { ["src/eleot/*"] = { "**.rc" } }
    filter {}