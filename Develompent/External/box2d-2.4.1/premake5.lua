---
-- Copyright (C) 2022 Broken Singularity
---

function LinkBox2D()
    includedirs     {
        extBox2d .. "/install/include/"
    }

    defines         {
        "WITH_BOX2D=1"
    }

    filter "platforms:Win64"
        filter "configurations:Debug"
            links       {
                extBox2d .. "/install/lib/Win64/Debug/box2d.lib"
            }

        filter "configurations:not Debug"
            links       {
                extBox2d .. "/install/lib/Win64/Release/box2d.lib"
            }
        filter {}
    filter {}
end