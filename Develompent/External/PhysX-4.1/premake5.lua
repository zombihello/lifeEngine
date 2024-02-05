--[[
************************************************************
*                     This file is part of:
*                        LIFEENGINE
*            https://github.com/zombihello/lifeEngine
************************************************************
* Copyright (C) 2024 Yehor Pohuliaka.
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
]]

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

        filter "configurations:Debug*"
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
                "{COPY} " .. physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXCommonDEBUG.bin " .. binariesDir .. "/",
                "{COPY} " .. physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXCookingDEBUG.bin " .. binariesDir .. "/",
                "{COPY} " .. physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXDEBUG.bin " .. binariesDir .. "/",
                "{COPY} " .. physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXFoundationDEBUG.bin " .. binariesDir .. "/"
            }
        filter "configurations:not Debug*"
            links       {
                physXHome .. "/bin/win.x86_64.vc142.md/release/PhysX.lib",
                physXHome .. "/bin/win.x86_64.vc142.md/release/PhysXCommon.lib",
                physXHome .. "/bin/win.x86_64.vc142.md/release/PhysXCooking.lib",
                physXHome .. "/bin/win.x86_64.vc142.md/release/PhysXExtensions_static.lib",
                physXHome .. "/bin/win.x86_64.vc142.md/release/PhysXFoundation.lib",
                physXHome .. "/bin/win.x86_64.vc142.md/release/PhysXCharacterKinematic_static_64.lib",
                physXHome .. "/bin/win.x86_64.vc142.md/release/PhysXPvdSDK_static_64.lib",
                physXHome .. "/bin/win.x86_64.vc142.md/release/PhysXTask_static_64.lib",
                physXHome .. "/bin/win.x86_64.vc142.md/release/PhysXVehicle_static_64.lib"
            }

            postbuildcommands    {
                "{COPY} " .. physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXCommon.bin " .. binariesDir .. "/",
                "{COPY} " .. physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXCooking.bin " .. binariesDir .. "/",
                "{COPY} " .. physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysX.bin " .. binariesDir .. "/",
                "{COPY} " .. physXHome .. "/bin/win.x86_64.vc142.md/debug/PhysXFoundation.bin " .. binariesDir .. "/"
            }
        filter {}
    filter {}
end