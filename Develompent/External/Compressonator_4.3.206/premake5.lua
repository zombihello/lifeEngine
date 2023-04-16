---
-- Copyright (C) 2023 Broken Singularity
---

function LinkCompressonator()
    includedirs     {
        extCompressonator .. "/include/"
    }

    filter "platforms:Win64"
        filter "configurations:DebugWithEditor"
            links       {
                extCompressonator .. "/lib/VS2019/x64/Compressonator_MDd.lib",
				extCompressonator .. "/lib/VS2019/x64/CMP_Framework_MDd.lib"
            }

        filter "configurations:ReleaseWithEditor"
            links       {
                extCompressonator .. "/lib/VS2019/x64/Compressonator_MD.lib",
				extCompressonator .. "/lib/VS2019/x64/CMP_Framework_MD.lib"
            }
        filter {}
    filter {}
end