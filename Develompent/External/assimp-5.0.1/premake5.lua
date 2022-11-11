---
-- Copyright (C) 2022 Broken Singularity
---

function LinkAssmip()
    includedirs     {
        extAssimp .. "/include/"
    }

    filter "platforms:Win64"
        filter "configurations:Debug"
            links       {
                extAssimp .. "/lib/Win64/assimp.lib",
                extAssimp .. "/lib/Win64/IrrXML.lib",
                extAssimp .. "/lib/Win64/zlibstatic.lib"
            }

            postbuildcommands    {
                "{COPY} " .. extAssimp .. "/bin/Win64/assimpd.dll " .. binariesDir .. outputDir .. "/"
            }
        filter "configurations:not Debug"
            links       {
                extAssimp .. "/lib/Win64/assimpd.lib",
                extAssimp .. "/lib/Win64/IrrXMLd.lib",
                extAssimp .. "/lib/Win64/zlibstaticd.lib"
            }

            postbuildcommands    {
                "{COPY} " .. extAssimp .. "/bin/Win64/assimp.dll " .. binariesDir .. outputDir .. "/"
            }
        filter {}
    filter {}
end