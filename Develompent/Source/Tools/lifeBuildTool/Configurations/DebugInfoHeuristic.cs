using System;
using System.Collections.Generic;
using System.Text;

namespace lifeBuildTool
{
    class DebugInfoHeuristic
    {
        /// <summary>
        /// Should create debug info
        /// </summary>
        /// <param name="InPlatform">Platform build</param>
        /// <param name="InConfiguration">Configuration build</param>
        /// <returns>Is need creating debug info</returns>
        public static bool ShouldCreateDebugInfo( LETargetPlatform InPlatform, LETargetConfiguration InConfiguration )
        {
            switch ( InPlatform )
            {
                case LETargetPlatform.Win32:
                case LETargetPlatform.Win64:
                    switch ( InConfiguration )
                    {
                        case LETargetConfiguration.Debug:       return true;
                        case LETargetConfiguration.Release:     return false;
                        case LETargetConfiguration.Shipping:    return false;
                        default:                                return true;
                    }

                default:    return true;
            }
        }
    }
}
