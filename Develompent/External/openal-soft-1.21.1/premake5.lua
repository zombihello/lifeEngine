---
-- Copyright (C) 2022 Broken Singularity
---

function LinkOpenAL()
    includedirs     {
        extOpenAL .. "/include/"
    }

    filter "platforms:Win64"
        filter "configurations:Debug"
            links       {
                extOpenAL .. "/libs/Win64/OpenAL32.lib"
            }

            postbuildcommands    {
                "{COPY} " .. extOpenAL .. "/bin/Win64/OpenAL32.dll " .. binariesDir .. outputDir .. "/"
            }
        filter "configurations:not Debug"
            links       {
                extOpenAL .. "/libs/Win64/OpenAL32.lib"
            }

            postbuildcommands    {
                "{COPY} " .. extOpenAL .. "/bin/Win64/OpenAL32.dll " .. binariesDir .. outputDir .. "/"
            }
        filter {}
    filter {}
end