---
-- Copyright (C) 2024 Broken Singularity
---

function LinkMimalloc()
    includedirs     {
        extMimalloc .. "/include/"
    }

    filter "platforms:Win64"
        filter "configurations:Debug*"
            links       {
                extMimalloc .. "/lib/Win64/Debug/mimalloc-static.lib"
            }

        filter "configurations:not Debug*"
            links       {
                extMimalloc .. "/lib/Win64/Release/mimalloc-static.lib"
            }
        filter {}
    filter {}
end