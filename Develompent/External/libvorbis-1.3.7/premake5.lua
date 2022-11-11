---
-- Copyright (C) 2022 Broken Singularity
---

function LinkVorbis()
    includedirs     {
        extVorbis .. "/include/"
    }

    filter "platforms:Win64"
        links       {
            extVorbis .. "/lib/Win64/vorbis.lib",
            extVorbis .. "/lib/Win64/vorbisenc.lib",
            extVorbis .. "/lib/Win64/vorbisfile.lib"
        }
    filter {}
end