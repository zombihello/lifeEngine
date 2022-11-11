---
-- Copyright (C) 2022 Broken Singularity
---

function LinkTheora()
    includedirs     {
        extTheora .. "/include/"
    }

    filter "platforms:Win64"
        filter "configurations:Debug"
            links       {
                extTheora .. "/bin/Win64/libtheora_static-d.lib"
            }

        filter "configurations:not Debug"
            links       {
                extTheora .. "/bin/Win64/libtheora_static.lib"
            }
        filter {}
    filter {}
end