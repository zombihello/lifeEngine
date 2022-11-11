---
-- Copyright (C) 2022 Broken Singularity
---

function LinkOgg()
    includedirs     {
        extOgg .. "/include/"
    }

    filter "platforms:Win64"
        links       {
            extOgg .. "/lib/Win64/ogg.lib"
        }
    filter {}
end