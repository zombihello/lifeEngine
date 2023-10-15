---
-- Copyright (C) 2022 Broken Singularity
---

function LinkPhysX()
    defines         {
        "WITH_PHYSX=1"
    }

    filter "platforms:Win64"
        local physXHome     = extPhysX .. "/physx/install/vc15win64/"

        includedirs     {
            physXHome .. "/PhysX/include/",
            physXHome .. "/PxShared/include/"
        }

        filter "configurations:Debug or DebugWithEditor"
            links       {
                physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXDEBUG.lib",
                physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXCommonDEBUG.lib",
                physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXCookingDEBUG.lib",
                physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXExtensions_staticDEBUG.lib",
                physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXFoundationDEBUG.lib",
                physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXCharacterKinematic_staticDEBUG_64.lib",
                physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXPvdSDK_staticDEBUG_64.lib",
                physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXTask_staticDEBUG_64.lib",
                physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXVehicle_staticDEBUG_64.lib"
            }

            postbuildcommands    {
                "{COPY} " .. physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXCommonDEBUG.bin " .. binariesDir .. outputDir .. "/",
                "{COPY} " .. physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXCookingDEBUG.bin " .. binariesDir .. outputDir .. "/",
                "{COPY} " .. physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXDEBUG.bin " .. binariesDir .. outputDir .. "/",
                "{COPY} " .. physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXFoundationDEBUG.bin " .. binariesDir .. outputDir .. "/"
            }
        filter "configurations:not Debug and not DebugWithEditor"
            links       {
                physXHome .. "PhysX/bin/win.x86_64.vc142.md/release/PhysX_64.lib",
                physXHome .. "PhysX/bin/win.x86_64.vc142.md/release/PhysXCommon_64.lib",
                physXHome .. "PhysX/bin/win.x86_64.vc142.md/release/PhysXCooking_64.lib",
                physXHome .. "PhysX/bin/win.x86_64.vc142.md/release/PhysXExtensions_static_64.lib",
                physXHome .. "PhysX/bin/win.x86_64.vc142.md/release/PhysXFoundation_64.lib",
                physXHome .. "PhysX/bin/win.x86_64.vc142.md/release/PhysXCharacterKinematic_static_64.lib",
                physXHome .. "PhysX/bin/win.x86_64.vc142.md/release/PhysXPvdSDK_static_64.lib",
                physXHome .. "PhysX/bin/win.x86_64.vc142.md/release/PhysXTask_static_64.lib",
                physXHome .. "PhysX/bin/win.x86_64.vc142.md/release/PhysXVehicle_static_64.lib"
            }

            postbuildcommands    {
                "{COPY} " .. physXHome .. "PhysX/bin/win.x86_64.vc142.md/debug/PhysXCommon_64.bin " .. binariesDir .. outputDir .. "/",
                "{COPY} " .. physXHome .. "PhysX/bin/win.x86_64.vc142.md/debug/PhysXCooking_64.bin " .. binariesDir .. outputDir .. "/",
                "{COPY} " .. physXHome .. "PhysX/bin/win.x86_64.vc142.md/debug/PhysX_64.bin " .. binariesDir .. outputDir .. "/",
                "{COPY} " .. physXHome .. "PhysX/bin/win.x86_64.vc142.md/debug/PhysXFoundation_64.bin " .. binariesDir .. outputDir .. "/"
            }
        filter {}
    filter {}
end