---
-- Copyright (C) 2022 Broken Singularity
---

function LinkZLib()
    includedirs     {
        extZlib .. "/include/"
    }

    filter "platforms:Win64"
        filter "configurations:Debug or DebugWithEditor"
            links       {
                extZlib .. "/lib/Win64/zlibstaticd.lib"
            }

        filter "configurations:not Debug and not DebugWithEditor"
            links       {
                extZlib .. "/lib/Win64/zlibstatic.lib"
            }
        filter {}
    filter {}
end