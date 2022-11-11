---
-- Copyright (C) 2022 Broken Singularity
---

function LinkLuaJIT()
    includedirs     {
        extLuaJIT .. "/include/"
    }

    filter "platforms:Win64"
        links       {
            extLuaJIT .. "/lib/Win64/lua51.lib",
            extLuaJIT .. "/lib/Win64/luajit.lib"
        }
    filter {}
end