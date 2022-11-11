---
-- Copyright (C) 2022 Broken Singularity
---

function LinkSDL2()
    includedirs     {
        extSdl2 .. "/include/"
    }

    filter "platforms:Win64"
        links       {
            extSdl2 .. "/lib/Win64/SDL2.lib"
        }

        postbuildcommands    {
            "{COPY} " .. extSdl2 .. "/lib/Win64/SDL2.dll " .. binariesDir .. outputDir .. "/"
        }
    filter {}
end