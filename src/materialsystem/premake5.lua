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

project "materialsystem"
    if not buildMonolithicEngine then
        kind "SharedLib"
    else
        kind "StaticLib"
    end
    language    "C++"
    location( intermediateDir )
	
    ----------- PROJECT SETTINGS --------

    files       {
		"**.inl", 
        "**.cpp",
        "**.h",
        "**.hlsl",
        "vertexfactory.makefile",
        "../public/materialsystem/**.h",
        "../public/materialsystem/**.inl",
        "../public/core/**.cpp"
    }

    -- Enable PCH file
    pchheader       "pch_materialsystem.h"
    pchsource       "pch_materialsystem.cpp"
    includedirs     { "./", intermediateDir .. "/generated/" }

    vpaths      {
        ["src/*"]       = { "**.h", "**.inl", "**.cpp", "vertexfactory.makefile", "**.hlsl" },
        ["public/*"]    = { "../public/**.h", "../public/**.inl", "../public/**.cpp" }
    }

    -- Build commands for specific files
    filter { "files:vertexfactory.makefile" }
        buildmessage    "Generate C++ meta types for vertex factories"
        buildcommands   {
            buildDir .. binariesDir .. outputDir .. "/shadercompile " .. "-makefile \"%{file.abspath}\" -gencpp-vfs -outcpp-vfs \"" .. intermediateDir .. "/generated/materialsystem/\" -skipcompilation"
        }  
        buildoutputs    {
            intermediateDir .. "/generated/materialsystem/"
        }
    filter { "files:**.hlsl" }
        buildaction "None" 
    filter {}

    links       {
        "core",
        "stdlib",
		"interfaces"
    }

    dependson   { 
        "stdshaders",
        "shadercompile" 
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
        vpaths  { ["src/*"] = { "**.rc" } }
    filter {}

    ---------- EXCLUDES SUBPROJECT'S FILES ---------

    excludes { "stdshaders/**" }