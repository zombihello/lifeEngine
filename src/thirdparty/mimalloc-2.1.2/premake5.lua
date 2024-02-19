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

Mimalloc = {}

function Mimalloc.Link()
    -- Set include dirs
    includedirs     {
        thirdParty_Mimalloc .. "/include/"
    }

    -- Set library name
    local   libName
    if buildMonolithicEngine then
        libName = "mimalloc-static"
    else
        libName = "mimalloc"
    end

    -- Link library and copy dlls and license
    filter "platforms:Win64"
        -- Debug
        filter "configurations:Debug"
            links { 
                thirdParty_Mimalloc .. "/lib/win64/debug/" .. libName .. ".lib" 
            }

            if not buildMonolithicEngine then
                postbuildcommands { 
                    "{COPY} " .. thirdParty_Mimalloc .. "/bin/win64/debug/" .. libName .. ".dll " .. buildDir .. binariesDir .. outputDir .. "/" 
                }
            end

        -- Release
        filter "configurations:not Debug"
            links   { 
                thirdParty_Mimalloc .. "/lib/win64/release/" .. libName .. ".lib" 
            }

            if not buildMonolithicEngine then
                postbuildcommands { 
                    "{COPY} " .. thirdParty_Mimalloc .. "/bin/win64/release/" .. libName .. ".dll " .. buildDir .. binariesDir .. outputDir .. "/" 
                }
            end
        filter {}

        Mimalloc.CopyLicense()
    filter {}
end

function Mimalloc.CopyLicense()
    postbuildcommands    {
        "{MKDIR} " .. buildDir .. "/licenses",
        "{COPY} " .. thirdParty_Mimalloc .. "/LICENSE " .. buildDir .. "/licenses",
        "{MOVE} " .. buildDir .. "/licenses/LICENSE " .. buildDir .. "/licenses/mimalloc.txt",
    }
end