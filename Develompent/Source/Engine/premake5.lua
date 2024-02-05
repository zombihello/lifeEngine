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

project "Engine"
   kind        "StaticLib"
   language    "C++"
   location( intermediateDir )

   ----------- PROJECT SETTINGS --------

   defines				{
        "ENGINE_2D=" .. tostring( is2DEngine and 1 or 0 ),
        "USE_THEORA_CODEC=1"
    }

    includedirs         {
        "Include/",
        "../../../Engine/Shaders/"
    }

    files               {
        "Include/**.h",
        "Include/**.inl",
        "Source/**.cpp"
    }

    -- If we build the engine without editor, we will need exclude all of editor source files
    filter "configurations:not *WithEditor"
        excludes    { "Include/Editor/**.*" }
        excludes    { "Source/Editor/**.*" }
    filter {}

    ---------- PLATFORM SPECIFIC SETTINGS ---------

    -- Exclude platform specific for other platforms
    filter "platforms:not Win64"
        excludes { "**/Platform/Windows/**.*", "**/D3D11/**.*" }
    filter {}

    -- Windows
    filter "platforms:Win64"
        links { "d3d11", "d3d9", "dxgi", "dxguid", "d3dcompiler" }
    filter {}

    --------- LINK EXTERNAL LIBS -------
       
    LinkExternals()