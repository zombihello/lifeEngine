---
-- Copyright (C) 2022 Broken Singularity
---

function LinkTmxLite()
    includedirs     {
        extTmxlite .. "/tmxlite/include/"
    }

    filter "platforms:Win64"
        filter "configurations:DebugWithEditor"
            links       {
                extTmxlite .. "/tmxlite/lib/Win64/tmxlite-s-d.lib"
            }

        filter "configurations:ReleaseWithEditor"
            links       {
                extTmxlite .. "/tmxlite/lib/Win64/tmxlite-s.lib"
            }
        filter {}
    filter {}
end