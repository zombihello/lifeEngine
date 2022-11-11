---
-- Copyright (C) 2022 Broken Singularity
---

function LinkTmxLite()
    includedirs     {
        extTmxlite .. "/tmxlite/include/"
    }

    filter "platforms:Win64"
        filter "configurations:Debug"
            links       {
                extTmxlite .. "/tmxlite/lib/Win64/tmxlite-s-d.lib"
            }

        filter "configurations:not Debug"
            links       {
                extTmxlite .. "/tmxlite/lib/Win64/tmxlite-s.lib"
            }
        filter {}
    filter {}
end